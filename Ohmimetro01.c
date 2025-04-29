/*
 * Por: Kaian de Souza Gonçalves
 *    Ohmímetro utilizando o ADC da BitDogLab
 *    Com reconhecimento automático do código de cores
 *    e exibição na matriz de LEDs
 *
 * Neste exemplo, utilizamos o ADC do RP2040 para medir a resistência de um resistor
 * desconhecido, utilizando um divisor de tensão com dois resistores.
 * O resistor conhecido é de 10k ohm e o desconhecido é o que queremos medir.
 * O programa identifica o valor comercial padrão de 5% mais próximo (E24)
 * e mostra as cores das faixas correspondentes no display OLED e na matriz de LEDs.
 */

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "pico/stdlib.h"
#include "hardware/adc.h"
#include "hardware/i2c.h"
#include "lib/ssd1306.h"
#include "lib/font.h"
#include "lib/ws2818b.h"

#define I2C_PORT i2c1
#define I2C_SDA 14
#define I2C_SCL 15
#define endereco 0x3C
#define ADC_PIN 28 // GPIO para o voltímetro
#define Botao_A 5  // GPIO para botão A

int R_conhecido = 10000;   // Resistor de 10k ohm
float R_x = 0.0;           // Resistor desconhecido
float ADC_VREF = 3.31;     // Tensão de referência do ADC
int ADC_RESOLUTION = 4095; // Resolução do ADC (12 bits)

// Definição dos valores comerciais da série E24 (tolerância de 5%)
// Cobrindo a faixa de 510Ω a 100kΩ
const float E24_values[] = {
    510, 560, 620, 680, 750, 820, 910,
    1000, 1100, 1200, 1300, 1500, 1600, 1800, 2000, 2200, 2400, 2700, 3000, 3300, 3600, 3900,
    4300, 4700, 5100, 5600, 6200, 6800, 7500, 8200, 9100,
    10000, 11000, 12000, 13000, 15000, 16000, 18000, 20000, 22000, 24000, 27000, 30000, 33000, 36000, 39000,
    43000, 47000, 51000, 56000, 62000, 68000, 75000, 82000, 91000,
    100000};

const int E24_count = sizeof(E24_values) / sizeof(float);

// Definição das cores das faixas para resistores
const char *color_names[] = {"Preto", "Marrom", "Vermelho", "Laranja", "Amarelo", "Verde", "Azul", "Violeta", "Cinza", "Branco"};

// Definição das cores em formato RGB para uso na matriz de LEDs
const uint8_t color_rgb[][3] = {
    {0, 0, 0},       // Preto
    {90, 40, 10},    // Marrom
    {200, 0, 0},     // Vermelho
    {255, 100, 0},   // Laranja
    {255, 200, 0},   // Amarelo
    {0, 180, 0},     // Verde
    {0, 0, 255},     // Azul
    {130, 0, 150},   // Violeta
    {120, 120, 120}, // Cinza
    {255, 255, 255}, // Branco
    {180, 150, 0}    // Dourado
};

// Trecho para modo BOOTSEL com botão B
#include "pico/bootrom.h"
#define botaoB 6
void gpio_irq_handler(uint gpio, uint32_t events)
{
    reset_usb_boot(0, 0);
}

// Encontra o valor comercial mais próximo da série E24
float find_closest_e24_value(float measured_resistance)
{
    float closest_value = E24_values[0];
    float min_difference = fabs(measured_resistance - closest_value);

    for (int i = 1; i < E24_count; i++)
    {
        float difference = fabs(measured_resistance - E24_values[i]);
        if (difference < min_difference)
        {
            min_difference = difference;
            closest_value = E24_values[i];
        }
    }

    return closest_value;
}

// Obtem as cores das faixas para um resistor com base no valor
void get_resistor_colors(float resistance, int *first_band, int *second_band, int *multiplier)
{
    // Se o valor for menor que 100, trabalhamos com a unidade em ohms
    // Caso contrário, normalizamos para ter dois dígitos significativos
    float normalized_value = resistance;
    *multiplier = 0;

    while (normalized_value >= 100)
    {
        normalized_value /= 10;
        (*multiplier)++;
    }

    // Primeira faixa de cor (primeiro dígito significativo)
    *first_band = (int)(normalized_value / 10);

    // Segunda faixa de cor (segundo dígito significativo)
    *second_band = (int)(normalized_value) % 10;
}

// Desenha um resistor com as cores correspondentes no display
void draw_resistor_with_colors(ssd1306_t *ssd, int first_band, int second_band, int multiplier)
{
    // Corpo do resistor
    ssd1306_rect(ssd, 32, 3, 64, 15, true, false);

    // Terminais
    ssd1306_line(ssd, 20, 10, 32, 10, true);
    ssd1306_line(ssd, 96, 10, 108, 10, true);

    // Primeira faixa de cor
    ssd1306_rect(ssd, 37, 3, 10, 15, true, true);

    // Segunda faixa de cor
    ssd1306_rect(ssd, 52, 3, 10, 15, true, true);

    // Faixa multiplicadora
    ssd1306_rect(ssd, 67, 3, 10, 15, true, true);

    // Escrevendo os nomes das cores abaixo do resistor
    char text_buffer[20];
    sprintf(text_buffer, "1:%s", color_names[first_band]);
    ssd1306_draw_string(ssd, text_buffer, 5, 22);

    sprintf(text_buffer, "2:%s", color_names[second_band]);
    ssd1306_draw_string(ssd, text_buffer, 45, 22);

    sprintf(text_buffer, "M:%s", color_names[multiplier]);
    ssd1306_draw_string(ssd, text_buffer, 85, 22);
}

// Exibe as cores do resistor na matriz de LEDs
void display_resistor_colors_on_matrix(int first_band, int second_band, int multiplier)
{
    // Limpa todos os LEDs
    clear_leds();

    // Para uma matriz 5x5, usamos uma linha para cada cor
    // Linha 5 (LEDs 20-24): Primeira faixa do resistor
    for (int i = 20; i < 25; i++)
    {
        set_led(i, color_rgb[first_band][0], color_rgb[first_band][1], color_rgb[first_band][2]);
    }

    // Linha 4 (LEDs 15-19): Segunda faixa do resistor
    for (int i = 15; i < 20; i++)
    {
        set_led(i, color_rgb[second_band][0], color_rgb[second_band][1], color_rgb[second_band][2]);
    }

    // Linha 3 (LEDs 10-14): Multiplicador do resistor
    for (int i = 10; i < 15; i++)
    {
        set_led(i, color_rgb[multiplier][0], color_rgb[multiplier][1], color_rgb[multiplier][2]);
    }

    // Atualiza a matriz de LEDs
    write_leds();
}

// Função para formatar o valor da resistência para exibição
void format_resistance_value(float resistance, char *buffer, int buffer_size)
{
    if (resistance < 1000)
    {
        // Valores menores que 1kΩ: mostrar em Ω com 1 casa decimal
        snprintf(buffer, buffer_size, "%.1f Ω", resistance);
    }
    else
    {
        // Valores maiores ou iguais a 1kΩ: mostrar em kΩ com 2 casas decimais
        snprintf(buffer, buffer_size, "%.2f kΩ", resistance / 1000.0f);
    }
}

int main()
{
    // Para ser utilizado o modo BOOTSEL com botão B
    gpio_init(botaoB);
    gpio_set_dir(botaoB, GPIO_IN);
    gpio_pull_up(botaoB);
    gpio_set_irq_enabled_with_callback(botaoB, GPIO_IRQ_EDGE_FALL, true, &gpio_irq_handler);
    // Aqui termina o trecho para modo BOOTSEL com botão B

    gpio_init(Botao_A);
    gpio_set_dir(Botao_A, GPIO_IN);
    gpio_pull_up(Botao_A);

    // Inicializar matriz de LEDs
    init_leds();
    clear_leds();
    write_leds();

    // I2C Initialisation. Using it at 400Khz.
    i2c_init(I2C_PORT, 400 * 1000);

    // Configurando I2C para display
    gpio_set_function(I2C_SDA, GPIO_FUNC_I2C);                    
    gpio_set_function(I2C_SCL, GPIO_FUNC_I2C);                    
    gpio_pull_up(I2C_SDA);                                       
    gpio_pull_up(I2C_SCL);                                        
    ssd1306_t ssd;                                                // Inicializa a estrutura do display
    ssd1306_init(&ssd, WIDTH, HEIGHT, false, endereco, I2C_PORT); // Inicializa o display
    ssd1306_config(&ssd);                                         // Configura o display
    ssd1306_send_data(&ssd);                                      // Envia os dados para o display

    // Limpa o display. O display inicia com todos os pixels apagados.
    ssd1306_fill(&ssd, false);
    ssd1306_send_data(&ssd);

    adc_init();
    adc_gpio_init(ADC_PIN); // GPIO 28 como entrada analógica

    float tensao;
    char str_r_medido[16];     // Buffer para armazenar o valor medido
    char str_r_comercial[16];  // Buffer para armazenar o valor comercial
    bool display_mode = false; // Modo de exibição: false = simples, true = avançado
    bool last_button_state = true;

    // Inicialização da matriz de LEDs com uma animação (intensidade reduzida)
    for (int i = 0; i < LED_COUNT; i++)
    {
        set_led(i, 0, 30, 0); // Verde com intensidade reduzida (reduzido de 0, 150, 0)
        write_leds();
        sleep_ms(50);
    }
    clear_leds();
    write_leds();

    while (true)
    {
        // Verificar botão A para alternar modo de exibição
        bool current_button_state = gpio_get(Botao_A);
        if (last_button_state && !current_button_state)
        {
            // Botão foi pressionado
            display_mode = !display_mode;
            sleep_ms(100); // Debounce
        }
        last_button_state = current_button_state;

        adc_select_input(2); // Seleciona o ADC para eixo X. O pino 28 como entrada analógica

        float soma = 0.0f;
        for (int i = 0; i < 500; i++)
        {
            soma += adc_read();
            sleep_ms(1);
        }
        float media = soma / 500.0f;

        // Fórmula original: R_x = R_conhecido * ADC_encontrado /(ADC_RESOLUTION - adc_encontrado)
        R_x = (R_conhecido * media) / (ADC_RESOLUTION - media);

        // Limita o valor mínimo e máximo para a faixa de resistores especificada (510Ω a 100kΩ)
        if (R_x < 510.0f)
        {
            R_x = 510.0f;
        }
        else if (R_x > 100000.0f)
        {
            R_x = 100000.0f;
        }

        // Encontra o valor comercial mais próximo
        float closest_e24 = find_closest_e24_value(R_x);

        // Determina as cores das faixas com base no valor comercial
        int first_band, second_band, multiplier;
        get_resistor_colors(closest_e24, &first_band, &second_band, &multiplier);

        // Formata as strings para exibição
        format_resistance_value(R_x, str_r_medido, sizeof(str_r_medido));
        format_resistance_value(closest_e24, str_r_comercial, sizeof(str_r_comercial));

        // Atualiza a matriz de LEDs com as cores do resistor
        display_resistor_colors_on_matrix(first_band, second_band, multiplier);

        // Atualiza o conteúdo do display
        ssd1306_fill(&ssd, false); // Limpa o display

        if (display_mode)
        {
            // Modo avançado - mostra o resistor com cores e detalhes
            draw_resistor_with_colors(&ssd, first_band, second_band, multiplier);

            // Adiciona informações sobre valores
            ssd1306_draw_string(&ssd, "Ohmimetro - BitDogLab", 10, 38); // Título
            ssd1306_draw_string(&ssd, "Medido:", 5, 48);                // Texto para valor medido
            ssd1306_draw_string(&ssd, str_r_medido, 55, 48);            // Valor medido
            ssd1306_draw_string(&ssd, "E24:", 5, 56);                   // Texto para valor comercial
            ssd1306_draw_string(&ssd, str_r_comercial, 55, 56);         // Valor comercial
        }
        else
        {
            // Modo simples - layout original com cores do resistor
            ssd1306_rect(&ssd, 3, 3, 122, 60, true, false); // Desenha um retângulo
            // ssd1306_line(&ssd, 3, 25, 123, 25, true);               // Desenha uma linha
            ssd1306_line(&ssd, 3, 37, 123, 37, true); // Desenha uma linha

            // Substituir os textos originais pelos nomes das cores
            char cor1[16], cor2[16], corM[16];
            sprintf(cor1, "1:%s", color_names[first_band]);
            sprintf(cor2, "2:%s", color_names[second_band]);
            sprintf(corM, "M:%s", color_names[multiplier]);

            ssd1306_draw_string(&ssd, cor1, 8, 6);  // Primeira cor (onde estava CEPEDI TIC37)
            ssd1306_draw_string(&ssd, cor2, 8, 16); // Segunda cor (onde estava EMBARCATECH)
            ssd1306_draw_string(&ssd, corM, 8, 26); // Multiplicador (abaixo da segunda cor)

            ssd1306_draw_string(&ssd, "ADC", 13, 41);       // Desenha uma string
            ssd1306_draw_string(&ssd, "Resisten.", 50, 41); // Desenha uma string
            ssd1306_line(&ssd, 44, 37, 44, 60, true);       // Desenha uma linha vertical

            char str_x[8], str_y[16];
            sprintf(str_x, "%1.0f", media);     // Converte o inteiro em string
            sprintf(str_y, "%s", str_r_medido); // Usa o valor formatado

            ssd1306_draw_string(&ssd, str_x, 8, 52);  // Desenha uma string
            ssd1306_draw_string(&ssd, str_y, 59, 52); // Desenha uma string
        }

        ssd1306_send_data(&ssd); // Atualiza o display
        sleep_ms(700);
    }
}