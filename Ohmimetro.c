#include <stdio.h>
#include <stdlib.h>
#include "pico/stdlib.h"
#include "hardware/adc.h"
#include "hardware/i2c.h"
#include "lib/ssd1306.h"
#include "lib/font.h"
#define I2C_PORT i2c1
#define I2C_SDA 14
#define I2C_SCL 15
#define endereco 0x3C
#define ADC_PIN 28 // GPIO para o ohmimetro
#define Botao_A 5  // GPIO para botão A

int R_conhecido = 10000;   // Resistor de 10k ohm
float R_x = 0.0;           // Resistor desconhecido
float ADC_VREF = 3.31;     // Tensão de referência do ADC
int ADC_RESOLUTION = 4095; // Resolução do ADC (12 bits)

// Trecho para modo BOOTSEL com botão B
#include "pico/bootrom.h"
#define botaoB 6
void gpio_irq_handler(uint gpio, uint32_t events)
{
    reset_usb_boot(0, 0);
}

void definirCor(char r, char cor[])
{

    switch (r)
    {
    case '0':
    {
        sprintf(cor, "Preto");
        break;
    }
    case '1':
    {
        sprintf(cor, "Marrom");
        break;
    }
    case '2':
    {
        sprintf(cor, "Vermelho");
        break;
    }
    case '3':
    {
        sprintf(cor, "Laranja");
        break;
    }
    case '4':
    {
        sprintf(cor, "Amarelo");
        break;
    }
    case '5':
    {
        sprintf(cor, "Verde");
        break;
    }
    case '6':
    {
        sprintf(cor, "Azul");
        break;
    }
    case '7':
    {
        sprintf(cor, "Violeta");
        break;
    }
    case '8':
    {
        sprintf(cor, "Cinza");
        break;
    }
    case '9':
    {
        sprintf(cor, "Branco");
        break;
    }

    default:
        break;

    }
    return;
}

int main()
{
    stdio_init_all();
    // Para ser utilizado o modo BOOTSEL com botão B
    gpio_init(botaoB);
    gpio_set_dir(botaoB, GPIO_IN);
    gpio_pull_up(botaoB);
    gpio_set_irq_enabled_with_callback(botaoB, GPIO_IRQ_EDGE_FALL, true, &gpio_irq_handler);
    // Aqui termina o trecho para modo BOOTSEL com botão B

    gpio_init(Botao_A);
    gpio_set_dir(Botao_A, GPIO_IN);
    gpio_pull_up(Botao_A);

    // I2C Initialisation. Using it at 400Khz.
    i2c_init(I2C_PORT, 400 * 1000);

    gpio_set_function(I2C_SDA, GPIO_FUNC_I2C);                    // Set the GPIO pin function to I2C
    gpio_set_function(I2C_SCL, GPIO_FUNC_I2C);                    // Set the GPIO pin function to I2C
    gpio_pull_up(I2C_SDA);                                        // Pull up the data line
    gpio_pull_up(I2C_SCL);                                        // Pull up the clock line
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
    char str_x[5]; // Buffer para armazenar a string
    char str_y[5]; // Buffer para armazenar a string
    char cor1[10]; // Buffer para armazenar a co1
    char cor2[10]; // Buffer para armazenar a cor2
    char cor3[10]; // Buffer para armazenar a cor3

    bool cor = true;
    while (true)
    {
        adc_select_input(2); // Seleciona o ADC para eixo X. O pino 28 como entrada analógica

        float soma = 0.0f;
        for (int i = 0; i < 500; i++)
        {
            soma += adc_read();
            sleep_ms(1);
        }
        float media = soma / 500.0f;

        // Fórmula simplificada: R_x = R_conhecido * ADC_encontrado /(ADC_RESOLUTION - adc_encontrado)
        R_x = (R_conhecido * media) / (ADC_RESOLUTION - media);

        R_x *= 1.03;
        sprintf(str_x, "%1.0f", media); // Converte o inteiro em string
        sprintf(str_y, "%1.0f", R_x);   // Converte o float em string
        printf("\n%1.0f", R_x);

        definirCor(str_y[0],cor1);
        definirCor(str_y[1],cor2);
        definirCor('3',cor3);

        //  Atualiza o conteúdo do display com animações
        ssd1306_fill(&ssd, !cor); // Limpa o display

        ssd1306_rect(&ssd, 3, 3, 122, 60, cor, !cor);  // Desenha um retângulo
        ssd1306_draw_string(&ssd, "Ohmimetro", 30, 6); // Desenha uma string
        ssd1306_line(&ssd, 3, 16, 123, 16, cor);       // Desenha uma linha
        ssd1306_draw_string(&ssd, "Resist.:", 7, 20);  // Desenha uma string
        ssd1306_draw_string(&ssd, str_y, 80, 20);      // Desenha uma string
        ssd1306_draw_string(&ssd, "Cor 1:", 7, 31);    // Desenha uma string
        ssd1306_draw_string(&ssd, cor1, 57, 31);      // Desenha uma string
        ssd1306_draw_string(&ssd, "Cor 2:", 7, 42);    // Desenha uma string
        ssd1306_draw_string(&ssd, cor2, 57, 42);    // Desenha uma string
        ssd1306_draw_string(&ssd, "Cor 3:", 7, 53);    // Desenha uma string
        ssd1306_draw_string(&ssd, cor3, 57, 53);    // Desenha uma string

        ssd1306_send_data(&ssd); // Atualiza o display

        sleep_ms(500);
    }
}