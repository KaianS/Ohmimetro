# Guia de Montagem do Ohmímetro com Reconhecimento de Código de Cores

Este guia detalha como montar fisicamente o circuito do ohmímetro na sua placa BitDogLab, utilizando uma protoboard e jumpers.

## Componentes Necessários

1. Placa BitDogLab com RP2040
2. Protoboard (pequena ou média)
3. Resistor de 10kΩ (resistor de referência fixo)
4. Resistores para teste (na faixa de 510Ω a 100kΩ)
5. Jumpers (fios de conexão)

## Preparação e Segurança

Antes de iniciar a montagem:

1. **DESCONECTE a placa BitDogLab de qualquer fonte de alimentação**
2. Organize os componentes em uma superfície limpa e não condutora
3. Verifique se nenhum dos componentes está danificado
4. Tenha cuidado para não tocar nos componentes eletrônicos com as mãos úmidas

## Passos para Montagem

### Passo 1: Posicionar a Placa e a Protoboard

1. Coloque a placa BitDogLab e a protoboard lado a lado em uma superfície estável
2. Certifique-se de que o display OLED esteja visível e acessível

### Passo 2: Conexões da Placa à Protoboard

1. Usando jumpers, conecte:
   - Pino 3.3V da BitDogLab → Terminal positivo (linha superior) da protoboard
   - Pino GND da BitDogLab → Terminal negativo (linha inferior) da protoboard
   - Pino GPIO 28 (ADC) da BitDogLab → Ponto médio na protoboard (onde os dois resistores se encontrarão)

### Passo 3: Montar o Divisor de Tensão

1. Coloque o resistor de referência de 10kΩ na protoboard:
   - Uma extremidade conectada à linha de 3.3V
   - Outra extremidade conectada ao ponto médio (onde está o GPIO 28)

2. Deixe espaço para o resistor a ser medido:
   - Preparar uma posição onde uma extremidade ficará conectada ao ponto médio
   - A outra extremidade será conectada à linha de GND

```
    3.3V (VCC) -----+
                     |
                    [10kΩ] Resistor Fixo
                     |
                     +------ GPIO 28 (ADC)
                     |
                [Rx] Resistor a ser medido
                     |
    GND -------------+
```

### Passo 4: Como Realizar a Medição

1. Para medir um resistor:
   - DESLIGUE a placa (desconecte o USB)
   - Insira o resistor a ser medido no local destinado (Rx)
   - Certifique-se de que as conexões estão firmes
   - LIGUE a placa novamente (conecte o USB)
   - Aguarde a leitura no display OLED

### Passo 5: Testando Diferentes Resistores

1. Para cada novo resistor a ser testado:
   - DESLIGUE a placa (desconecte o USB)
   - Remova o resistor anterior
   - Insira o novo resistor
   - LIGUE a placa novamente (conecte o USB)

## Cuidados e Dicas

1. **NUNCA conecte ou desconecte componentes com a placa ligada**
2. **NUNCA conecte resistores com valores inferiores a 330Ω diretamente** (pode causar sobrecarga)
3. Mantenha as conexões firmes para evitar leituras oscilantes
4. Se o display mostrar valores muito inconsistentes, verifique todas as conexões
5. O sistema é calibrado para a faixa de 510Ω a 100kΩ; valores fora dessa faixa podem ter precisão reduzida

## Possíveis Problemas e Soluções

| Problema | Possível Causa | Solução |
|----------|----------------|---------|
| Display não liga | Conexão I2C incorreta | Verifique os fios conectados aos pinos GPIO 14 e 15 |
| Valor "0" ou muito baixo | Resistor não conectado ou curto-circuito | Verifique a posição do resistor na protoboard |
| Valor muito alto ou "inf" | Conexão interrompida | Verifique se o resistor está bem conectado |
| Leitura instável | Contato ruim | Assegure que todos os jumpers e o resistor estão bem encaixados |

## Limitações do Sistema

1. A precisão da medida depende da qualidade do ADC e do resistor de referência
2. A tolerância do resistor de referência (10kΩ) afeta todas as medições
3. Resistores de valores muito altos (>100kΩ) ou muito baixos (<510Ω) podem apresentar medidas imprecisas

## Após o Uso

1. Desligue a placa desconectando o USB
2. Remova o resistor de teste
3. Guarde os componentes em local seco 