#include "PowerSupplyTab.h"
#include "qscale.h"

#include <QByteArray>
#include <QChart>
#include <QChartView>
#include <QComboBox>
#include <QDoubleSpinBox>
#include <QElapsedTimer>
#include <QFormLayout>
#include <QGroupBox>
#include <QHBoxLayout>
#include <QLabel>
#include <QLegend>
#include <QLineEdit>
#include <QLineSeries>
#include <QLocale>
#include <QMessageBox>
#include <QPushButton>
#include <QSerialPort>
#include <QSerialPortInfo>
#include <QSignalBlocker>
#include <QSpinBox>
#include <QTabWidget>
#include <QTableWidget>
#include <QTimer>
#include <QVBoxLayout>
#include <QValueAxis>
#include <QWidget>
#include <QtMath>

#include <algorithm>
#include <array>
#include <functional>
#include <optional>
#include <utility>
constexpr quint8 Station = 1;
constexpr quint8 ReadRegisters = 3;
constexpr quint8 WriteRegister = 6;
constexpr quint16 Output = 0x0001;
constexpr quint16 Voltage = 0x0030;
constexpr quint16 Current = 0x0031;
constexpr quint16 MeasurementStart = 0x0010;
constexpr quint16 MeasurementCount = 4;
constexpr double ChartWindowSeconds = 10.0;
constexpr int PanelPollIntervalMs = 150;
constexpr int ReadRecoveryMs = 150;
constexpr size_t PanelPollReadCount = 4;
constexpr std::array<std::pair<quint16, quint16>, 5> PanelReads { { { Output, 1 }, { 0x0040, 2 }, { 0x0042, 3 }, { Voltage, 2 }, { 0x0020, 2 } } };

static quint16 crc16(const QByteArray &bytes)
{
    quint16 crc = 0xFFFF;
    for (unsigned char const byte : bytes) {
        crc ^= byte;
        for (int bit = 0; bit < 8; ++bit) {
            // NOLINTNEXTLINE(bugprone-signed-bitwise)
            crc = (crc >> 1U) ^ ((crc & 1U) ? 0xA001 : 0);
        }
    }
    return crc;
}

static quint16 word(const QByteArray &bytes, qsizetype idx)
{
    // NOLINTNEXTLINE(bugprone-signed-bitwise)
    return (static_cast<quint16>(static_cast<quint8>(bytes.at(idx))) << 8) | static_cast<quint8>(bytes.at(idx + 1));
}

namespace {
struct Reading
{
    double volts;
    double amps;
};
} // namespace

static Reading decodeMeasurement(const QByteArray &frame)
{
    // Provisional map. The captured 0x0010–0x0013 response contained four zero words.
    // Change the word indices and divisors here if reference readings show a different
    // register order or resolution. If the device uses 32-bit values, combine the
    // corresponding words here and verify their order before applying the scale.
    constexpr qsizetype voltageWord = 0; // 0x0010
    constexpr qsizetype currentWord = 1; // 0x0011
    constexpr qsizetype firstDataByte = 3;
    return { .volts = word(frame, firstDataByte + (2 * voltageWord)) / 1000.0, .amps = word(frame, firstDataByte + (2 * currentWord)) / 1000.0 };
}

namespace {

struct NumericControl
{
    quint16 address { };
    int scale { };
    QDoubleSpinBox *field = nullptr;
    QPushButton *apply = nullptr;
    bool edited = false;
};

struct SwitchControl
{
    quint16 address;
    const char *name;
    QPushButton *button = nullptr;
    std::optional<bool> state;
};

struct Step
{
    quint16 voltage;
    quint16 current;
    int durationMs;
};
} // namespace

namespace {
class PowerSupplyTab final : public QWidget
{
public:
    // NOLINTNEXTLINE(readability-function-cognitive-complexity)
    explicit PowerSupplyTab(QWidget *parent)
        : QWidget(parent)
        , portList(new QComboBox)
        , measurementInterval(new QComboBox)
        , table(new QTableWidget(0, 3))
        , cycles(new QSpinBox)
        , duration(new QSpinBox)
    {
        auto *layout = new QVBoxLayout(this);
        auto *connection = new QHBoxLayout;

        auto *refresh = new QPushButton(tr("Atualizar portas"));
        connectButton = new QPushButton(tr("Conectar"));
        refreshStateButton = new QPushButton(tr("Consultar estados"));
        status = new QLabel(tr("Desconectada"));
        status->setWordWrap(true);
        connection->addWidget(portList);
        connection->addWidget(refresh);
        connection->addWidget(connectButton);
        connection->addWidget(refreshStateButton);

        for (const int interval : { 25, 50, 75, 100, 500, 1000, 5000, 6000, 8000, 10000 }) {
            measurementInterval->addItem(interval < 1000 ? tr("%1 ms").arg(interval) : tr("%1 s").arg(interval / 1000), interval);
        }
        measurementInterval->setCurrentIndex(measurementInterval->findData(100));
        connection->addWidget(new QLabel(tr("Intervalo dos gráficos")));
        connection->addWidget(measurementInterval);
        connect(measurementInterval, &QComboBox::currentIndexChanged, this, [this] {
            if (measurementClock.isValid()) {
                nextMeasurement = measurementClock.elapsed() + measurementInterval->currentData().toInt();
            }
            schedulePanelPoll();
        });
        connection->addWidget(status, 1);
        layout->addLayout(connection);

        auto *pages = new QTabWidget;
        auto *commandPage = new QWidget;
        auto *commandLayout = new QVBoxLayout(commandPage);
        pages->addTab(commandPage, tr("Comandos"));
        pages->addTab(createMonitoringPage(), tr("Medições"));
        layout->addWidget(pages, 1);

        auto *controls = new QHBoxLayout;
        auto *settings = new QGroupBox(tr("Ajustes"));
        auto *form = new QFormLayout(settings);
        addNumeric(form, numeric.at(0), tr("Tensão"), tr(" V"), 32.000, 2);
        addNumeric(form, numeric.at(1), tr("Corrente"), tr(" A"), 5.200, 3);
        addNumeric(form, numeric.at(2), tr("Limite de sobretensão"), tr(" V"), 33, 2);
        addNumeric(form, numeric.at(3), tr("Limite de sobrecorrente"), tr(" A"), 5.3, 3);
        auto *switches = new QGroupBox(tr("Saída e proteções"));
        auto *switchLayout = new QVBoxLayout(switches);
        for (auto &control : toggles) {
            control.button = new QPushButton;
            switchLayout->addWidget(control.button);
            connect(control.button, &QPushButton::clicked, this, [this, &control] {
                refreshForAction([this, &control] {
                    if (!control.state) {
                        finishAction(tr("A fonte retornou um estado desconhecido."));
                        return;
                    }
                    const bool enable = !*control.state;
                    if (control.address == Output && enable
                        && QMessageBox::question(
                               this, tr("Ligar saída"), tr("Ligar a saída da fonte?"), QMessageBox::Yes | QMessageBox::No, QMessageBox::No)
                            != QMessageBox::Yes) {
                        finishAction(tr("Estados atualizados"));
                        return;
                    }
                    if (!actionInProgress || !serial.isOpen()) {
                        return;
                    }
                    if (control.address == Output && !enable) {
                        stopSequence();
                    }
                    writeAndRefresh(control.address, enable);
                });
            });
        }
        controls->addWidget(settings, 1);
        controls->addWidget(switches, 1);
        commandLayout->addLayout(controls);

        auto *program = new QGroupBox(tr("Ciclos"));
        auto *programLayout = new QVBoxLayout(program);

        table->setHorizontalHeaderLabels({ tr("Tensão (V)"), tr("Corrente (A)"), tr("Duração (ms)") });
        table->setEditTriggers(QAbstractItemView::NoEditTriggers);
        table->setSelectionBehavior(QAbstractItemView::SelectRows);
        table->setSelectionMode(QAbstractItemView::SingleSelection);
        table->setMaximumHeight(145);
        programLayout->addWidget(table);
        auto *editor = new QHBoxLayout;

        duration->setRange(100, 3600000);
        duration->setValue(1000);
        duration->setSuffix(tr(" ms"));
        addStep = new QPushButton(tr("Adicionar ajustes à sequência"));
        removeStep = new QPushButton(tr("Remover etapa"));
        editor->addWidget(duration);
        editor->addWidget(addStep);
        editor->addWidget(removeStep);
        editor->addStretch();
        programLayout->addLayout(editor);
        auto *runner = new QHBoxLayout;

        cycles->setRange(1, 9999);
        cycles->setValue(10);
        run = new QPushButton(tr("Executar"));
        elapsed = new QLabel(tr("Tempo: 00:00:00"));
        runner->addWidget(new QLabel(tr("Repetições")));
        runner->addWidget(cycles);
        runner->addWidget(run);
        runner->addWidget(elapsed);
        runner->addStretch();
        programLayout->addLayout(runner);
        commandLayout->addWidget(program);
        commandLayout->addStretch();

        timeout.setSingleShot(true);
        panelPoll.setSingleShot(true);
        panelPoll.setTimerType(Qt::PreciseTimer);
        connect(&panelPoll, &QTimer::timeout, this, [this] { pollPanel(); });
        stepTimer.setSingleShot(true);
        runtimeTimer.setInterval(250);
        connect(refresh, &QPushButton::clicked, this, [this] { refreshPorts(); });
        connect(connectButton, &QPushButton::clicked, this, [this] { toggleConnection(); });
        connect(refreshStateButton, &QPushButton::clicked, this,
            [this] { refreshForAction([this] { readMeasurements(tr("Estados e medições atualizados")); }); });
        connect(&serial, &QSerialPort::readyRead, this, [this] { receive(); });
        connect(&serial, &QSerialPort::errorOccurred, this, [this](QSerialPort::SerialPortError error) {
            if (serial.isOpen() && error == QSerialPort::ResourceError) {
                disconnectSupply(serial.errorString());
            }
        });
        connect(&timeout, &QTimer::timeout, this, [this] { recoverFromTimeout(); });
        connect(&stepTimer, &QTimer::timeout, this, [this] { advanceSequence(); });
        connect(&runtimeTimer, &QTimer::timeout, this, [this] { showRuntime(); });
        connect(addStep, &QPushButton::clicked, this, [this] {
            const auto *const volts = numeric.at(0).field;
            const auto *const amps = numeric.at(1).field;
            steps.append({ .voltage = encode(numeric.at(0)), .current = encode(numeric.at(1)), .durationMs = duration->value() });
            const int row = table->rowCount();
            table->insertRow(row);
            table->setItem(row, 0, new QTableWidgetItem(volts->cleanText()));
            table->setItem(row, 1, new QTableWidgetItem(amps->cleanText()));
            table->setItem(row, 2, new QTableWidgetItem(QString::number(duration->value())));
            updateControls();
        });
        connect(removeStep, &QPushButton::clicked, this, [this] {
            const int row = table->currentRow();
            if (row >= 0) {
                steps.removeAt(row);
                table->removeRow(row);
                updateControls();
            }
        });
        connect(run, &QPushButton::clicked, this, [this] {
            if (running) {
                stopSequence();
                return;
            }
            refreshForAction([this] {
                if (!toggles.at(0).state.value_or(false)) {
                    finishAction(tr("Ligue a saída antes de executar a sequência."));
                    return;
                }
                running = true;
                stepIndex = 0;
                remainingCycles = cycles->value();
                runtime.start();
                showRuntime();
                runtimeTimer.start();
                finishAction(tr("Sequência em execução"));
                startStep();
            });
        });

        refreshPorts();
        updateControls();
    }

private:
    struct Request
    {
        quint8 function;
        quint16 address;
        quint16 value;
        QByteArray frame;
        std::function<void()> onSuccess;
    };

    static quint16 encode(const NumericControl &control) { return static_cast<quint16>(qRound(control.field->value() * control.scale)); }

    void addNumeric(QFormLayout *form, NumericControl &control, const QString &name, const QString &unit, double maximum, int decimals)
    {
        auto *row = new QHBoxLayout;
        control.field = new QDoubleSpinBox;
        control.field->setDecimals(decimals);
        control.field->setRange(0, maximum);
        control.field->setSuffix(unit);
        control.apply = new QPushButton(tr("Aplicar"));
        row->addWidget(control.field);
        row->addWidget(control.apply);
        form->addRow(name, row);
        connect(control.field->findChild<QLineEdit *>(), &QLineEdit::textEdited, this, [&control] { control.edited = true; });
        connect(control.field, &QDoubleSpinBox::valueChanged, this, [&control] { control.edited = true; });
        connect(control.apply, &QPushButton::clicked, this, [this, &control] {
            const auto value = encode(control);
            refreshForAction([this, &control, value] { writeAndRefresh(control.address, value); });
        });
    }

    void refreshPorts()
    {
        const auto previous = portList->currentData();
        portList->clear();
        for (const auto &port : QSerialPortInfo::availablePorts()) {
            portList->addItem(port.portName(), port.systemLocation());
        }
        const int index = portList->findData(previous);
        if (index >= 0) {
            portList->setCurrentIndex(index);
        }
    }

    void toggleConnection()
    {
        if (serial.isOpen()) {
            disconnectSupply(tr("Desconectada"));
            return;
        }
        if (portList->currentIndex() < 0) {
            status->setText(tr("Selecione uma porta serial."));
            return;
        }
        serial.setPortName(portList->currentData().toString());
        serial.setBaudRate(QSerialPort::Baud9600);
        serial.setDataBits(QSerialPort::Data8);
        serial.setParity(QSerialPort::NoParity);
        serial.setStopBits(QSerialPort::OneStop);
        serial.setFlowControl(QSerialPort::NoFlowControl);
        if (!serial.open(QIODevice::ReadWrite)) {
            status->setText(serial.errorString());
            return;
        }
        measurementClock.start();
        resetChartHistory();
        nextPanel = PanelPollIntervalMs;
        refreshForAction([this] { readMeasurements(tr("Conectada")); }, true);
    }

    void disconnectSupply(const QString &reason)
    {
        const QString &message = reason;
        timeout.stop();
        panelPoll.stop();
        queuedAction = { };
        recovering = false;
        backgroundTimeout = false;
        backgroundRead = false;
        pollIndex = 0;
        actionInProgress = false;
        ++connectionGeneration;
        pending.reset();
        rx.clear();
        serial.close();
        voltageGauge.scale->setValue(0);
        currentGauge.scale->setValue(0);
        voltageGauge.label->setText(tr("— V"));
        currentGauge.label->setText(tr("— A"));
        for (auto &control : toggles) {
            control.state.reset();
        }
        for (auto &control : numeric) {
            control.edited = false;
        }
        stopSequence();
        status->setText(message);
        updateControls();
    }

    void refreshForAction(std::function<void()> done, bool includeSettings = false)
    {
        if (!serial.isOpen() || recovering || actionInProgress || (pending && !backgroundRead)) {
            return;
        }
        panelPoll.stop();
        actionInProgress = true;
        status->setText(tr("Consultando a fonte…"));
        updateControls();

        if (backgroundRead) {
            queuedAction = [this, includeSettings, done = std::move(done)] mutable { readState(0, includeSettings, std::move(done)); };
        } else {
            readState(0, includeSettings, std::move(done));
        }
    }

    void readState(size_t index, bool includeSettings, std::function<void()> done)
    {
        if (index == (includeSettings ? PanelReads.size() : 3)) {
            done();
            return;
        }
        const auto [address, count] = PanelReads.at(index);
        request(ReadRegisters, address, count, [this, index, includeSettings, done = std::move(done)] {
            afterFrame([this, index, includeSettings, done] { readState(index + 1, includeSettings, done); });
        });
    }

    void schedulePanelPoll()
    {
        if (!serial.isOpen() || recovering || actionInProgress || pending) {
            panelPoll.stop();
            return;
        }
        const auto due = running ? nextMeasurement : std::min(nextPanel, nextMeasurement);
        const auto delay = std::max<qint64>(10, due - measurementClock.elapsed());
        panelPoll.start(static_cast<int>(delay));
    }

    void pollPanel()
    {
        if (!serial.isOpen() || recovering || actionInProgress || pending) {
            return;
        }

        const auto now = measurementClock.elapsed();
        if (now < nextMeasurement && (running || now < nextPanel)) {
            schedulePanelPoll();
            return;
        }

        backgroundRead = true;
        if (now >= nextMeasurement && (running || nextMeasurement <= nextPanel || now < nextPanel)) {
            request(ReadRegisters, MeasurementStart, MeasurementCount, [this] { finishPanelRead(); });
        } else {
            const auto [address, count] = PanelReads.at(pollIndex);
            pollIndex = (pollIndex + 1) % PanelPollReadCount;
            nextPanel = now + PanelPollIntervalMs;
            request(ReadRegisters, address, count, [this] { finishPanelRead(); });
        }
    }

    void finishPanelRead()
    {
        backgroundRead = false;
        if (auto next = std::exchange(queuedAction, { })) {
            afterFrame(std::move(next));
        } else if (const auto address = std::exchange(deferredSequenceWrite, std::nullopt)) {
            afterFrame([this, address, generation = sequenceGeneration] {
                if (generation == sequenceGeneration) {
                    sequenceWrite(*address);
                }
            });
        }
    }

    void recoverFromTimeout()
    {
        if (!pending) {
            return;
        }
        const bool wasBackground = backgroundRead;
        const bool wasWrite = pending->function == WriteRegister;
        pending.reset();
        rx.clear();
        recovering = true;
        panelPoll.stop();
        if (!wasBackground) {
            stopSequence();
            status->setText(wasWrite ? tr("A fonte não confirmou a escrita. Confira o estado antes de repetir.")
                                     : tr("A fonte não respondeu à consulta. A porta continua aberta."));
        } else if (!actionInProgress) {
            backgroundTimeout = true;
            status->setText(tr("Resposta atrasada. Retomando as leituras…"));
        }
        // RTU read responses carry no register address. Discard late bytes
        // during a quiet period before issuing a request for another block.
        QTimer::singleShot(ReadRecoveryMs, this, [this, wasBackground, generation = connectionGeneration] {
            if (!serial.isOpen() || generation != connectionGeneration) {
                return;
            }
            recovering = false;
            if (wasBackground) {
                finishPanelRead();
            } else {
                actionInProgress = false;
            }
            schedulePanelPoll();
            updateControls();
        });
        updateControls();
    }

    void afterFrame(std::function<void()> next)
    {
        QTimer::singleShot(10, this, [this, next = std::move(next), generation = connectionGeneration] {
            if (serial.isOpen() && generation == connectionGeneration) {
                next();
            }
        });
    }

    void finishAction(const QString &message)
    {
        actionInProgress = false;
        backgroundTimeout = false;
        status->setText(message);
        updateControls();
        if (const auto address = std::exchange(deferredSequenceWrite, std::nullopt)) {
            sequenceWrite(*address);
        }
        schedulePanelPoll();
    }

    void writeAndRefresh(quint16 address, quint16 value)
    {
        request(WriteRegister, address, value, [this, address, value] {
            for (auto &control : numeric) {
                if (control.address == address && encode(control) == value) {
                    control.edited = false;
                }
            }
            afterFrame([this] { readState(0, false, [this] { readMeasurements(tr("Comando confirmado. Estados e medições atualizados.")); }); });
        });
    }

    void request(quint8 function, quint16 address, quint16 value, std::function<void()> onSuccess)
    {
        if (recovering || pending || !serial.isOpen()) {
            return;
        }
        QByteArray frame;
        frame.append(static_cast<char>(Station));
        frame.append(static_cast<char>(function));
        frame.append(static_cast<char>(address >> 8U));
        frame.append(static_cast<char>(address & 0xFFU));
        frame.append(static_cast<char>(value >> 8U));
        frame.append(static_cast<char>(value & 0xFFU));
        const auto crc = crc16(frame);
        frame.append(static_cast<char>(crc & 0xFFU));
        frame.append(static_cast<char>(crc >> 8U));
        if (function == ReadRegisters && address == MeasurementStart) {
            measurementRequestGeneration = chartGeneration;
            nextMeasurement = measurementClock.elapsed() + measurementInterval->currentData().toInt();
        }
        pending = Request { .function = function, .address = address, .value = value, .frame = frame, .onSuccess = std::move(onSuccess) };
        if (serial.write(frame) != frame.size()) {
            disconnectSupply(serial.errorString());
            return;
        }
        timeout.start(500);
        updateControls();
    }

    // NOLINTNEXTLINE(readability-function-cognitive-complexity)
    void receive()
    {
        const auto newBytes = serial.readAll();
        if (!pending) {
            return;
        }
        rx += newBytes;
        const Request sent = *pending;
        while (rx.size() >= 2) {
            const auto function = static_cast<quint8>(rx.at(1));
            const bool exception = function == (sent.function | 0x80U);
            if (static_cast<quint8>(rx.at(0)) != Station || (!exception && function != sent.function)) {
                rx.remove(0, 1);
                continue;
            }
            // NOLINTNEXTLINE(readability-avoid-nested-conditional-operator)
            const int length = exception ? 5 : function == WriteRegister ? 8 : 5 + (2 * sent.value);
            if (!exception && function == ReadRegisters && rx.size() >= 3 && static_cast<quint8>(rx.at(2)) != 2 * sent.value) {
                rx.remove(0, 1);
                continue;
            }
            if (rx.size() < length) {
                return;
            }
            const auto frame = rx.first(length);
            if (crc16(frame) || (!exception && function == WriteRegister && frame != sent.frame)) {
                rx.remove(0, 1);
                continue;
            }
            rx.clear();
            pending.reset();
            timeout.stop();
            if (exception && backgroundRead) {
                for (auto &control : toggles) {
                    if (control.address >= sent.address && control.address < sent.address + sent.value) {
                        control.state.reset();
                    }
                }
                if (!actionInProgress) {
                    status->setText(tr("A fonte recusou uma consulta do painel físico."));
                }
                finishPanelRead();
            } else if (exception && sent.function == ReadRegisters && sent.address == MeasurementStart) {
                finishAction(tr("Estados atualizados. A fonte recusou a leitura de medições."));
            } else if (exception) {
                stopSequence();
                for (auto &control : toggles) {
                    control.state.reset();
                }
                finishAction(tr("A fonte recusou o comando (exceção %1).").arg(static_cast<int>(static_cast<quint8>(frame.at(2)))));
            } else {
                if (function == ReadRegisters && sent.address == MeasurementStart) {
                    if (measurementRequestGeneration == chartGeneration) {
                        showMeasurement(decodeMeasurement(frame));
                    }
                } else if (function == ReadRegisters) {
                    for (quint16 index = 0; index < sent.value; ++index) {
                        updateValue(sent.address + index, word(frame, 3 + (2 * index)));
                    }
                }
                if (backgroundTimeout && backgroundRead && !actionInProgress) {
                    backgroundTimeout = false;
                    status->setText(tr("Conectada"));
                }
                sent.onSuccess();
            }
            schedulePanelPoll();
            updateControls();
            return;
        }
    }

    struct Gauge
    {
        QScale *scale = nullptr;
        QLabel *label = nullptr;
    };

    struct Plot
    {
        QLineSeries *series = nullptr;
        QValueAxis *time = nullptr;
        QValueAxis *value = nullptr;
        double minimumSpan = 1;
    };

    QWidget *createMonitoringPage()
    {
        auto *page = new QWidget;
        auto *layout = new QVBoxLayout(page);
        auto *note = new QLabel(tr("Amostras contínuas. Mapeamento de tensão e corrente provisório."));
        note->setWordWrap(true);
        auto *toolbar = new QHBoxLayout;
        toolbar->addWidget(note, 1);
        auto *reset = new QPushButton(tr("Reiniciar gráficos"));
        connect(reset, &QPushButton::clicked, this, [this] {
            resetChartHistory();
            schedulePanelPoll();
        });
        toolbar->addWidget(reset);
        layout->addLayout(toolbar);

        auto *content = new QHBoxLayout;
        auto *gauges = new QVBoxLayout;
        gauges->addWidget(createGauge(tr("Tensão medida"), tr("— V"), 30, 5, voltageGauge));
        gauges->addWidget(createGauge(tr("Corrente medida"), tr("— A"), 5, 1, currentGauge));
        gauges->addStretch();
        content->addLayout(gauges, 1);

        auto *charts = new QVBoxLayout;
        charts->addWidget(createPlot(tr("Tensão (V)"), QColor("#2475ba"), 0.1, plots.at(0)));
        charts->addWidget(createPlot(tr("Corrente (A)"), QColor("#387d45"), 0.1, plots.at(1)));
        charts->addWidget(createPlot(tr("Potência (W)"), QColor("#ad5c26"), 1, plots.at(2)));
        content->addLayout(charts, 3);
        layout->addLayout(content, 1);
        return page;
    }

    static QGroupBox *createGauge(const QString &title, const QString &initial, double maximum, double step, Gauge &gauge)
    {
        auto *box = new QGroupBox(title);
        auto *layout = new QVBoxLayout(box);
        gauge.scale = new QScale;
        gauge.scale->setRange(0, maximum);
        gauge.scale->setMajorStepSize(step);
        gauge.scale->setOrientations(Qt::Horizontal);
        gauge.scale->setMinimumSize(150, 85);
        layout->addWidget(gauge.scale);
        gauge.label = new QLabel(initial);
        gauge.label->setAlignment(Qt::AlignCenter);
        layout->addWidget(gauge.label);
        return box;
    }

    static QChartView *createPlot(const QString &title, const QColor &color, double span, Plot &plot)
    {
        auto *chart = new QChart;
        chart->legend()->hide();
        chart->setMargins(QMargins(4, 2, 6, 2));
        plot.series = new QLineSeries;
        plot.series->setColor(color);
        plot.series->setPointsVisible(true);
        chart->addSeries(plot.series);
        plot.time = new QValueAxis;
        plot.time->setTitleText(tr("Tempo (s)"));
        plot.time->setRange(0, ChartWindowSeconds);
        plot.value = new QValueAxis;
        plot.value->setTitleText(title);
        plot.value->setRange(0, span);
        plot.minimumSpan = span;
        chart->addAxis(plot.time, Qt::AlignBottom);
        chart->addAxis(plot.value, Qt::AlignLeft);
        plot.series->attachAxis(plot.time);
        plot.series->attachAxis(plot.value);
        auto *view = new QChartView(chart);
        view->setRenderHint(QPainter::Antialiasing);
        view->setMinimumHeight(140);
        return view;
    }

    void readMeasurements(const QString &message)
    {
        request(ReadRegisters, MeasurementStart, MeasurementCount, [this, message] { finishAction(message); });
    }

    void resetChartHistory()
    {
        ++chartGeneration;
        chartOrigin = measurementClock.isValid() ? measurementClock.elapsed() : 0;
        nextMeasurement = chartOrigin;
        for (const auto &plot : plots) {
            plot.series->clear();
            plot.time->setRange(0, ChartWindowSeconds);
            plot.value->setRange(0, plot.minimumSpan);
        }
    }

    void showMeasurement(const Reading &reading)
    {
        voltageGauge.scale->setValue(reading.volts);
        currentGauge.scale->setValue(reading.amps);
        voltageGauge.label->setText(tr("%1 V").arg(locale().toString(reading.volts, 'f', 3)));
        currentGauge.label->setText(tr("%1 A").arg(locale().toString(reading.amps, 'f', 3)));

        const double seconds = static_cast<double>(measurementClock.elapsed() - chartOrigin) / 1000.0;
        const double end = std::max(ChartWindowSeconds, seconds);
        const double start = end - ChartWindowSeconds;
        // Power is calculated from simultaneous voltage/current samples. If a
        // dedicated power register is verified, decode it in decodeMeasurement
        // and use that value here instead of the product.
        const std::array values { reading.volts, reading.amps, reading.volts * reading.amps };
        for (size_t index = 0; index < plots.size(); ++index) {
            const auto &plot = plots.at(index);
            plot.series->append(seconds, values.at(index));
            const auto points = plot.series->points();
            const auto firstVisible
                = std::lower_bound(points.cbegin(), points.cend(), start, [](const QPointF &point, double left) { return point.x() < left; });
            // Keep one point outside the viewport so a segment crossing its left edge stays visible.
            const auto firstRetained = firstVisible == points.cbegin() ? firstVisible : firstVisible - 1;
            const auto removeCount = firstRetained - points.cbegin();
            if (removeCount > 0) {
                plot.series->removePoints(0, static_cast<int>(removeCount));
            }
            plot.time->setRange(start, end);

            double maximum = plot.minimumSpan;
            for (auto point = firstRetained; point != points.cend(); ++point) {
                maximum = std::max(maximum, point->y() * 1.1);
            }
            plot.value->setRange(0, maximum);
        }
    }

    void updateValue(quint16 address, quint16 value)
    {
        for (auto &control : toggles) {
            if (control.address == address) {
                control.state = value == 0 || value == 1 ? std::optional<bool>(value == 1) : std::nullopt;
            }
        }
        if (address == Output && value != 1 && running) {
            stopSequence();
        }
        for (auto const &control : numeric) {
            if (control.address == address && !control.edited) {
                // Reads track physical encoders while leaving pending user edits intact.
                const QSignalBlocker blocker(control.field);
                if (encode(control) != value) {
                    control.field->setValue(static_cast<double>(value) / control.scale);
                }
            }
        }
    }

    [[nodiscard]] bool canRun() const
    {
        return serial.isOpen() && !recovering && (!pending || backgroundRead) && !actionInProgress && !steps.isEmpty();
    }

    void updateControls()
    {
        const bool open = serial.isOpen();
        const bool ready = open && !recovering && (!pending || backgroundRead) && !actionInProgress;
        refreshStateButton->setEnabled(ready);
        portList->setEnabled(!open);
        connectButton->setText(open ? tr("Desconectar") : tr("Conectar"));
        for (const auto &control : numeric) {
            control.apply->setEnabled(ready && !running);
        }
        for (const auto &control : toggles) {
            // NOLINTNEXTLINE(readability-avoid-nested-conditional-operator)
            const auto state = !control.state ? tr("desconhecido") : *control.state ? tr("ligado") : tr("desligado");
            control.button->setText(tr("%1 (%2)").arg(tr(control.name), state));
            control.button->setEnabled(ready);
        }
        addStep->setEnabled(!running);
        removeStep->setEnabled(!running && !steps.isEmpty());
        cycles->setEnabled(!running);
        run->setEnabled(running || canRun());
        run->setText(running ? tr("Parar") : tr("Executar"));
        run->setToolTip(!running && !canRun() ? tr("Conecte a fonte e adicione uma etapa.") : QString());
    }

    void startStep() { sequenceWrite(Voltage); }

    void sequenceWrite(quint16 address)
    {
        if (!running) {
            return;
        }
        if (actionInProgress || backgroundRead || recovering) {
            deferredSequenceWrite = address;
            return;
        }
        const auto &step = steps.at(stepIndex);
        request(WriteRegister, address, address == Voltage ? step.voltage : step.current, [this, address, generation = sequenceGeneration] {
            if (!running || generation != sequenceGeneration) {
                return;
            }
            if (address == Voltage) {
                afterFrame([this, generation] {
                    if (generation == sequenceGeneration) {
                        sequenceWrite(Current);
                    }
                });
            } else {
                stepTimer.start(steps.at(stepIndex).durationMs);
            }
        });
    }

    void advanceSequence()
    {
        ++stepIndex;
        if (stepIndex == steps.size()) {
            stepIndex = 0;
            if (--remainingCycles == 0) {
                stopSequence();
                return;
            }
        }
        startStep();
    }

    void showRuntime()
    {
        const auto seconds = runtime.elapsed() / 1000;
        elapsed->setText(tr("Tempo: %1:%2:%3")
                .arg(seconds / 3600, 2, 10, QLatin1Char('0'))
                .arg(seconds / 60 % 60, 2, 10, QLatin1Char('0'))
                .arg(seconds % 60, 2, 10, QLatin1Char('0')));
    }

    void stopSequence()
    {
        if (running) {
            showRuntime();
        }
        running = false;
        ++sequenceGeneration;
        deferredSequenceWrite.reset();
        stepTimer.stop();
        runtimeTimer.stop();
        updateControls();
        schedulePanelPoll();
    }

    QSerialPort serial;
    QTimer timeout;
    QTimer panelPoll;
    QTimer stepTimer;
    QTimer runtimeTimer;
    QElapsedTimer runtime;
    QElapsedTimer measurementClock;
    Gauge voltageGauge;
    Gauge currentGauge;
    std::array<Plot, 3> plots;
    QByteArray rx;
    std::optional<Request> pending;
    unsigned connectionGeneration = 0;
    bool actionInProgress = false;
    bool backgroundRead = false;
    bool recovering = false;
    bool backgroundTimeout = false;
    qint64 nextPanel = 0;
    qint64 nextMeasurement = 0;
    qint64 chartOrigin = 0;
    unsigned chartGeneration = 0;
    unsigned measurementRequestGeneration = 0;
    size_t pollIndex = 0;
    std::function<void()> queuedAction;
    std::optional<quint16> deferredSequenceWrite;
    QComboBox *portList;
    QComboBox *measurementInterval;
    QPushButton *connectButton;
    QPushButton *refreshStateButton;
    QLabel *status;
    QLabel *elapsed;
    QTableWidget *table;
    QSpinBox *cycles;
    QSpinBox *duration;
    QPushButton *run;
    QPushButton *addStep;
    QPushButton *removeStep;
    QList<Step> steps;
    qsizetype stepIndex = 0;
    int remainingCycles = 0;
    unsigned sequenceGeneration = 0;
    bool running = false;
    std::array<NumericControl, 4> numeric {
        {
            { .address = Voltage, .scale = 100 },
            { .address = Current, .scale = 1000 },
            { .address = 0x0020, .scale = 100 },
            { .address = 0x0021, .scale = 1000 },
        },
    };
    std::array<SwitchControl, 6> toggles {
        {
            { .address = Output, .name = "Saída" },
            { .address = 0x0040, .name = "Bloqueio" },
            { .address = 0x0041, .name = "Bipe" },
            { .address = 0x0042, .name = "OVP" },
            { .address = 0x0043, .name = "OCP" },
            { .address = 0x0044, .name = "OPP" },
        },
    };
};
} // namespace

QWidget *createPowerSupplyTab(QWidget *parent) { return new PowerSupplyTab(parent); }
