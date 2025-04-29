# Ohmímetro com Reconhecimento Automático do Código de Cores de Resistores - RP2040 (BitDogLab)

## Descrição

Este projeto foi desenvolvido para a placa BitDogLab, utilizando o microcontrolador RP2040. O objetivo é criar um ohmímetro capaz de medir a resistência de componentes e identificar automaticamente o código de cores correspondente ao valor padrão mais próximo da série E24 (tolerância de 5%).

O sistema realiza as seguintes funções:
1. Leitura da resistência de um resistor conectado ao circuito divisor de tensão
2. Cálculo do valor da resistência utilizando o ADC do RP2040
3. Identificação do valor comercial padrão E24 (5% de tolerância) mais próximo
4. Determinação das cores das três primeiras faixas (1ª, 2ª e multiplicador)
5. Exibição no display OLED SSD1306:
   - Representação gráfica do resistor com as cores correspondentes
   - Valor medido da resistência
   - Valor comercial padrão mais próximo

A faixa de resistores suportada é de 510Ω até 100kΩ, todos pertencentes à série E24 (tolerância de 5%).

## Montagem do Circuito

O circuito utiliza um divisor de tensão com um resistor conhecido de 10kΩ e o resistor a ser medido. O ponto médio entre os dois resistores é conectado ao pino ADC (GPIO 28) do RP2040.

### Conexões:
- Resistor conhecido (10kΩ): entre 3.3V e o ponto médio
- Resistor desconhecido (a medir): entre o ponto médio e GND
- Ponto médio: conectado ao GPIO 28 (ADC)
- Display OLED SSD1306: conectado às portas I2C (GPIO 14 e 15)

## Observações
- Implementação do modo BOOTSEL por botão externo (Botão B - GPIO 6). Isto facilita a gravação no desenvolvimento do programa. Quando o projeto for finalizado, pode-se retirar esta função.
- Para resistores fora da faixa suportada, o sistema pode apresentar medidas imprecisas.
