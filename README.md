# Ohmímetro
Projeto desenvolvido para medir resistores e exibir suas cores padrão utilizando a placa BitDogLab (RP2040), display OLED (SSD1306) e matriz de LEDs WS2812.

### Funcionalidades
- Mede a resistência de um resistor conectado ao circuito.
- Calcula o valor usando o ADC do RP2040 e ajusta para o valor mais próximo da série E24 (5%).
- Identifica e exibe as três primeiras faixas de cor (1ª, 2ª e multiplicador) no display OLED.
- Representa graficamente as cores usando uma matriz de LEDs WS2812.
- Permite reiniciar o sistema no modo BOOTSEL pressionando o botão B.

### Como usar
- Conecte o resistor entre o pino ADC (GPIO28) e GND.
- Ligue o sistema (o código começa automaticamente a fazer leituras).
- O sistema irá:
  - Medir a resistência.
  - Calcular o valor comercial mais próximo.
  - Exibir no display OLED:
  - O valor da resistência em Ohms.
  - As três faixas de cor correspondentes.
  - Acender LEDs na matriz WS2812 simulando o código de cores.
  - Pressione o botão B para entrar em modo de atualização (BOOTSEL), se necessário.
- Foto do circuito:

 ![Circuito](https://i.imgur.com/NTRFv5N.png)

### Componentes Utilizados
- Placa: BitDogLab (RP2040)
- Display OLED: SSD1306, comunicação via I2C (SDA - GPIO14, SCL - GPIO15)
- Matriz de LEDs: WS2812, conectada no GPIO7
- Botão:
  - Botão B (GPIO6) - usado para reiniciar o sistema em modo BOOTSEL.
- Resistor conhecido: 10 kΩ para o divisor de tensão.
- Fontes e Bibliotecas:
  - hardware/adc.h
  - hardware/i2c.h
  - ssd1306.h
  - ws2812.pio.h
  - font.h

  
### Autor
## Matheus Nepomuceno Souza
