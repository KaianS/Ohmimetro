# Ohmímetro com Reconhecimento Automático do Código de Cores de Resistores - RP2040 (BitDogLab)

## Descrição

Este projeto foi desenvolvido para a placa BitDogLab, utilizando o microcontrolador RP2040. O objetivo é criar um ohmímetro capaz de medir a resistência de componentes e identificar automaticamente o código de cores correspondente ao valor padrão mais próximo da série E24 (tolerância de 5%).

## Funcionalidades

O sistema realiza as seguintes funções:
1. Leitura da resistência de um resistor conectado ao circuito divisor de tensão
2. Cálculo do valor da resistência utilizando o ADC do RP2040
3. Identificação do valor comercial padrão E24 (5% de tolerância) mais próximo
4. Determinação das cores das três primeiras faixas (1ª, 2ª e multiplicador)
5. Exibição no display OLED SSD1306:
   - Modo Simples: Exibição direta das cores e valores
   - Modo Avançado: Representação gráfica do resistor com as cores correspondentes
6. Visualização das cores na matriz de LEDs RGB 5x5
7. Alternância entre modos de exibição através do Botão A

## Especificações Técnicas

- **Faixa de Medição**: 510Ω até 100kΩ
- **Série de Resistores**: E24 (tolerância de 5%)
- **Resolução ADC**: 12 bits (4095 níveis)
- **Tensão de Referência**: 3.31V
- **Resistor de Referência**: 10kΩ

## Montagem do Circuito

### Componentes Necessários
- 1x Placa BitDogLab (RP2040)
- 1x Display OLED SSD1306
- 1x Matriz de LEDs RGB WS2812B 5x5
- 1x Resistor 10kΩ (referência)
- 2x Botões de pressão
- Jumpers e conectores

### Conexões
- **Divisor de Tensão**:
  - Resistor conhecido (10kΩ): entre 3.3V e o ponto médio
  - Resistor desconhecido (a medir): entre o ponto médio e GND
  - Ponto médio: conectado ao GPIO 28 (ADC)
- **Interface**:
  - Display OLED SSD1306: I2C (GPIO 14 - SDA, GPIO 15 - SCL)
  - Matriz de LEDs: GPIO configurado para WS2812B
  - Botão A (Modo): GPIO 5
  - Botão B (BOOTSEL): GPIO 6

## Características do Software

- **Modos de Exibição**:
  - Modo Simples: Exibe cores e valores numéricos em layout básico
  - Modo Avançado: Mostra representação gráfica do resistor com cores
- **Processamento de Medidas**:
  - Média de 500 leituras para maior precisão
  - Atualização a cada 700ms
  - Normalização automática de valores (Ω/kΩ)
- **Feedback Visual**:
  - Display OLED para informações detalhadas
  - Matriz LED para visualização rápida das cores

## Vídeo Demonstrativo

[![Vídeo do Ohmímetro em Funcionamento](link_para_thumbnail)](link_para_seu_video)

> **Nota**: Substitua `link_para_thumbnail` pelo link da imagem de prévia do vídeo e `link_para_seu_video` pelo link do vídeo demonstrativo.

## Observações e Limitações

- O sistema possui proteção contra valores fora da faixa (510Ω - 100kΩ)
- Implementação do modo BOOTSEL por botão externo (Botão B - GPIO 6) para facilitar o desenvolvimento
- A precisão da medição pode variar dependendo da qualidade do resistor de referência
- Para resistores fora da faixa suportada, o sistema pode apresentar medidas imprecisas

## Autor

Por: Kaian de Souza Gonçalves

## Licença

Este projeto é distribuído sob a licença [Inserir tipo de licença]. Veja o arquivo `LICENSE` para mais detalhes.
