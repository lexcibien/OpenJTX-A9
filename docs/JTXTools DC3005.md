# JTXTools DC3005 (30 V / 5 A)

Capturado no DCSourceSystem, COM10, estação 1. UART 9600 baud, 8N1, LSB primeiro, sem inversão. Na captura, CH1 contém pedidos e CH0 contém respostas.
Os registradores e valores de 16 bits usam byte alto primeiro. O CRC-16 Modbus usa byte baixo primeiro.

## Registradores observados

| Endereço      | Função                           | Representação                                                             |
| ------------- | -------------------------------- | ------------------------------------------------------------------------- |
| 0x0001        | Saída                            | 0 = desligada, 1 = ligada                                                 |
| 0x0010–0x0013 | Bloco de leituras monitoradas    | A captura sem carga retornou zeros. Significado individual por confirmar. |
| 0x0020        | Limite de sobretensão (OV)       | V × 100. A interface mostrou 33,00 V e escreveu 3300.                     |
| 0x0021        | Limite de sobrecorrente (OI)     | A × 1000. A interface mostrou 5,300 A e escreveu 5300.                    |
| 0x0030        | Tensão ajustada (V)              | V × 100. A interface mostrou 12,00 V e escreveu 1200.                     |
| 0x0031        | Corrente ajustada (I)            | A × 1000. A interface mostrou 0,000 A e escreveu 0.                       |
| 0x0040        | Bloqueio (LOCK)                  | 0 = desligado, 1 = ligado                                                 |
| 0x0041        | Bipe (BEEP)                      | 0 = desligado, 1 = ligado                                                 |
| 0x0042        | Proteção de sobretensão (OVP)    | 0 = desligada, 1 = ligada                                                 |
| 0x0043        | Proteção de sobrecorrente (OCP)  | 0 = desligada, 1 = ligada                                                 |
| 0x0044        | Proteção de sobre-potência (OPP) | 0 = desligada, 1 = ligada                                                 |
| 0x0045        | Estado consultado                | Retornou 0 na captura de referência. Significado por confirmar.           |

## Escritas e respostas

Todos os 16 pedidos de escrita receberam eco byte a byte com CRC válido. Os tempos em segundos são relativos ao início de dc3005-controles.sal.

| Ação            | Pedido CH1 (s) | Resposta CH0 (s) | HEX do pedido e da resposta |
| --------------- | -------------: | ---------------: | --------------------------- |
| V               |       4.025611 |         4.050950 | `01 06 00 30 04 B0 8A B1`   |
| I               |       8.961666 |         9.000365 | `01 06 00 31 00 00 D8 05`   |
| OV              |      11.554128 |        11.590520 | `01 06 00 20 0C E4 8D 4B`   |
| OI              |      14.117148 |        14.145740 | `01 06 00 21 14 B4 D6 B7`   |
| Saída ligada    |      21.606454 |        21.621954 | `01 06 00 01 00 01 19 CA`   |
| Saída desligada |      28.368407 |        28.379544 | `01 06 00 01 00 00 D8 0A`   |
| LOCK desligado  |      42.202310 |        42.221005 | `01 06 00 40 00 00 88 1E`   |
| LOCK ligado     |      45.742772 |        45.755756 | `01 06 00 40 00 01 49 DE`   |
| BEEP desligado  |      48.787460 |        48.825582 | `01 06 00 41 00 00 D9 DE`   |
| BEEP ligado     |      51.572488 |        51.588421 | `01 06 00 41 00 01 18 1E`   |
| OVP desligado   |      54.478509 |        54.497112 | `01 06 00 42 00 00 29 DE`   |
| OVP ligado      |      57.572542 |        57.599785 | `01 06 00 42 00 01 E8 1E`   |
| OCP desligado   |      61.196085 |        61.228276 | `01 06 00 43 00 00 78 1E`   |
| OCP ligado      |      64.302160 |        64.331110 | `01 06 00 43 00 01 B9 DE`   |
| OPP desligado   |      67.190132 |        67.214159 | `01 06 00 44 00 00 C9 DF`   |
| OPP ligado      |      70.259426 |        70.284029 | `01 06 00 44 00 01 08 1F`   |

## Leituras periódicas

A função 03 envia endereço inicial e quantidade. A fonte devolve endereço da estação, função 03, contagem de bytes, valores e CRC. Exemplos observados:

| Pedido CH1              | Resposta CH0                           | Bloco                         |
| ----------------------- | -------------------------------------- | ----------------------------- |
| 01 03 00 20 00 02 C5 C1 | 01 03 04 0C E4 14 B4 B6 23             | Limites OV e OI               |
| 01 03 00 45 00 01 95 DF | 01 03 02 00 00 B8 44                   | Estado 0x0045                 |
| 01 03 00 10 00 04 45 CC | 01 03 08 00 00 00 00 00 00 00 00 95 D7 | Bloco de leituras monitoradas |
| 01 03 00 01 00 01 D5 CA | 01 03 02 00 00 B8 44                   | Saída desligada               |
| 01 03 00 40 00 02 C5 DF | 01 03 04 00 01 00 01 6A 33             | Bloqueio e bipe ligados       |
| 01 03 00 42 00 03 A5 DF | 01 03 06 00 01 00 01 00 01 8C B5       | OVP, OCP e OPP ligados        |
| 01 03 00 30 00 02 C4 04 | 01 03 04 04 B0 00 00 FA E4             | Tensão e corrente ajustadas   |

## Consultas no OpenJTX

Conectado e fora de uma sequência, o OpenJTX consulta o painel em segundo plano. O intervalo alvo entre pedidos de painel é 150 ms. A porta atende uma
transação de cada vez e prioriza ações manuais. O polling usa a função 03 em 0x0001 (saída), 0x0040–0x0041 (bloqueio e bipe),
0x0042–0x0044 (proteções) e 0x0030–0x0031 (ajustes dos encoders). Campos que têm edições pendentes são preservados até Aplicar ou reconectar.
As três curvas compartilham uma amostra periódica do bloco de medições. Um timeout mantém a porta aberta e descarta dados atrasados durante
uma pausa de recuperação. Escritas sem confirmação não são reenviadas automaticamente.

Na captura do painel físico, o pedido 01 03 00 30 00 02 C4 04 acompanhou os dois encoders. Na primeira captura, a primeira palavra caiu de 2790
para 300 (27,90 para 3,00 V) com a segunda estável em 2140. Na segunda, a primeira ficou em 300 e a segunda passou, entre outros valores, de
2140 para 10 e 4200 (2,140, 0,010 e 4,200 A).

O LOCK físico mudou a primeira palavra da resposta a 01 03 00 40 00 02 C5 DF: 01 03 04 00 00 00 00 FA 33 → 01 03 04 00 01 00 00 AB F3 →
01 03 04 00 00 00 00 FA 33.
O OCP mudou a segunda palavra da resposta a 01 03 00 42 00 03 A5 DF: [1, 0, 1] → [1, 1, 1] → [1, 0, 1].

## Medições

O DCSourceSystem consulta quatro registradores a partir de 0x0010. As 177 respostas associadas a esse pedido nas duas capturas de controles e
ciclos continham quatro palavras zero. Elas identificam o bloco candidato, mas não confirmam a ordem, a largura ou a escala dos valores.

O OpenJTX usa provisoriamente 0x0010 ÷ 1000 para tensão (V) e 0x0011 ÷ 1000 para corrente (A). A potência (W) é o produto dos dois valores da
mesma amostra. O código indica onde ajustar os índices, o formato e a escala quando houver uma leitura de referência.
O menu **Intervalo dos gráficos** define o período alvo de uma leitura compartilhada pelas três curvas: 100, 250, 500 ou 750 ms, 1 s,
ou 2, 4, 6, 8 ou 10 s. O padrão é 1 s. As medições continuam durante sequências e podem atrasar enquanto outra transação está em andamento.

## Ciclos

Com a saída desligada, o aplicativo original recusou Exec. Com a saída ligada e a tabela sem linhas habilitadas, Exec mostrou Executing e
o tráfego continuou em consulta periódica. As únicas escritas nessa captura foram as ações manuais de saída. Nenhum registrador de ciclos
foi identificado nesse cenário.

O OpenJTX executa as etapas no computador. Cada etapa escreve tensão e corrente pelos comandos confirmados, valida os ecos e espera
o tempo configurado antes da etapa seguinte. É necessário ligar a saída explicitamente. Parar ou concluir interrompe os próximos ajustes
e mantém o estado da saída. A saída pode ser desligada pelo controle dedicado.
