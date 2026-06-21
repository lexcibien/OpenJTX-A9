/* MIT License

Copyright (C) 2020 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/
#pragma once

#include <QComboBox>
#include <QObject>
#include <functional>
#include <utility>

namespace ComboBoxHelper {

template <typename ValueType> struct Item
{
    Item(QString _text, const ValueType &value)
        : _text(std::move(_text))
        , _value(value)
    {
    }
    [[nodiscard]] QString text() const { return _text; }
    ValueType value() { return _value; }

private:
    QString _text;
    ValueType _value;
};

template <typename ValueType>
void setup(QComboBox *comboBox, ValueType defaultValue, const QVector<Item<ValueType>> &list, const std::function<void(ValueType)> &function = { })
{
    for (const Item<ValueType> &item : list) {
        comboBox->addItem(item._text, QVariant::fromValue(item.value));
        if (item.value == defaultValue) {
            comboBox->setCurrentIndex(comboBox->count() - 1);
        }
    }
    if (function) {
        QObject::connect(comboBox, QOverload<int>::of(&QComboBox::currentIndexChanged), [function, comboBox] {
            const ValueType value = comboBox->currentData().value<ValueType>();
            function(value);
        });
        function(defaultValue);
    }
}

template <typename ValueType, typename Target, typename Function>
void setup(QComboBox *comboBox, ValueType defaultValue, const QVector<Item<ValueType>> &list, Target *target, Function function)
{
    setup(comboBox, defaultValue, list);
    QObject::connect(comboBox, QOverload<int>::of(&QComboBox::currentIndexChanged), [function, comboBox, target] {
        const ValueType value = comboBox->currentData().value<ValueType>();
        std::invoke(function, target, value);
    });
    std::invoke(function, target, defaultValue);
}
} // namespace ComboBoxHelper
