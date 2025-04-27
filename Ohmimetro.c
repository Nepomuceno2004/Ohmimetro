#include <stdio.h>
#include <stdlib.h>
#include "pico/stdlib.h"
#include "hardware/adc.h"
#include "hardware/i2c.h"
#include "lib/ssd1306.h"
#include "lib/font.h"
#include <math.h>

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

char cor1[10]; // Buffer para armazenar a co1
char cor2[10]; // Buffer para armazenar a cor2
char cor3[10]; // Buffer para armazenar a cor3

// Trecho para modo BOOTSEL com botão B
#include "pico/bootrom.h"
#define botaoB 6
void gpio_irq_handler(uint gpio, uint32_t events)
{
    reset_usb_boot(0, 0);
}

void corResistor(char r, char cor[], int linha)
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

void corMultiplicador(int m, char cor[])
{

    switch (m)
    {
    case 1:
        sprintf(cor, "Prata");
        break;
    case 10:
        sprintf(cor, "Dourado");
        break;
    case 100:
        sprintf(cor, "Preto");
        break;
    case 1000:
        sprintf(cor, "Marrom");
        break;
    case 10000:
        sprintf(cor, "Vermelho");
        break;
    case 100000:
        sprintf(cor, "Laranja");
        break;
    case 1000000:
        sprintf(cor, "Amarelo");
        break;
    case 10000000:
        sprintf(cor, "Verde");
        break;
    case 100000000:
        sprintf(cor, "Azul");
        break;
    case 1000000000:
        sprintf(cor, "Violeta");
        break;
    default:
        sprintf(cor, "Cor desconhecida");
        break;
    }

    return;
}

void arredondarResistor(float valor)
{
    // Valores padrão da série E24 (5% de tolerância)
    const float padroes[] = {
        1.0, 1.1, 1.2, 1.3, 1.5, 1.6, 1.8, 2.0, 2.2, 2.4,
        2.7, 3.0, 3.3, 3.6, 3.9, 4.3, 4.7, 5.1, 5.6, 6.2,
        6.8, 7.5, 8.2, 9.1};

    int n = sizeof(padroes) / sizeof(padroes[0]);

    // Normalizar valor para faixa de 1.0 a 10.0
    float multiplicador = 1.0;
    while (valor >= 10.0)
    {
        valor /= 10.0;
        multiplicador *= 10.0;
    }
    while (valor < 1.0)
    {
        valor *= 10.0;
        multiplicador /= 10.0;
    }

    // Achar o padrão mais próximo
    float melhor = padroes[0];
    float menorErro = fabs(valor - padroes[0]);
    for (int i = 1; i < n; i++)
    {
        float erro = fabs(valor - padroes[i]);
        if (erro < menorErro)
        {
            melhor = padroes[i];
            menorErro = erro;
        }
    }

    // Voltar para a escala original
    float arredondado = melhor * multiplicador;
    char str_arredondado[5];

    sprintf(str_arredondado, "%1.0f", arredondado);

    corResistor(str_arredondado[0], cor1,1);
    corResistor(str_arredondado[1], cor2,2);

    long int int_multiplicador = (long int)(multiplicador * 10);

    printf("\nMult: %d", int_multiplicador);
    corMultiplicador(int_multiplicador, cor3);

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

    char resistencia[5]; // Buffer para armazenar a string

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

        arredondarResistor(R_x);

        sprintf(resistencia, "%1.0f", R_x); // Converte o float em string
        printf("\n%1.0f", R_x);

        //  Atualiza o conteúdo do display com animações
        ssd1306_fill(&ssd, !cor); // Limpa o display

        ssd1306_rect(&ssd, 3, 3, 122, 60, cor, !cor);   // Desenha um retângulo
        ssd1306_draw_string(&ssd, "Ohmimetro", 30, 6);  // Desenha uma string
        ssd1306_line(&ssd, 3, 16, 123, 16, cor);        // Desenha uma linha
        ssd1306_draw_string(&ssd, "Resist.:", 7, 20);   // Desenha uma string
        ssd1306_draw_string(&ssd, resistencia, 80, 20); // Desenha uma string

        ssd1306_draw_string(&ssd, "Cor 1:", 7, 31); // Desenha uma string
        ssd1306_draw_string(&ssd, cor1, 57, 31);    // Desenha uma string
        ssd1306_draw_string(&ssd, "Cor 2:", 7, 42); // Desenha uma string
        ssd1306_draw_string(&ssd, cor2, 57, 42);    // Desenha uma string
        ssd1306_draw_string(&ssd, "Cor 3:", 7, 53); // Desenha uma string
        ssd1306_draw_string(&ssd, cor3, 57, 53);    // Desenha uma string

        ssd1306_send_data(&ssd); // Atualiza o display

        sleep_ms(500);
    }
}