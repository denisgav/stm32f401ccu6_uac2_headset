# stm32f401ccu6_uac2_headset
Running [tinyusb UAC2 headset example](https://github.com/hathach/tinyusb/tree/master/examples/device/uac2_headset) on STM32F401.


## Hardware
- STM32F401 BlackPill board
- STLINK programmer
- I2S Microphone inmp441
- DAC PCM5102A
- LCD SSD1306
- Buttons
- Rotary encoder
- LEDs
- Resistors

## Cloning
```sh
git clone [https://github.com/denisgav/rpi_usb_sound_card.git](https://github.com/denisgav/stm32f401ccu6_uac2_headset.git)
```

## Default Pinout
![image](https://github.com/user-attachments/assets/8f8c9067-fe39-4eea-83d6-0db92e201d95)

### PCM5102a DAC

 * PCM5102A configuration inputs:
    * FLT - Filter select : Normal latency (Low) / Low latency (High)
    * DEMP - De-emphasis control for 44.1kHz sampling rate: Off (Low) / On (High)
    * XSMT - Soft mute control(1): Soft mute (Low) / soft un-mute (High)
    * FMT - Audio format selection : I2S (Low) / Left justified (High)
 * PCM5102A I2S interface:
    * SCK - System clock input
    * BCK - Audio data bit clock input
    * DIN - Audio data input
    * LRCK - Audio data word clock input

| PCM5102A I2S DAC | STM32F401 | I/F  |
| --- | --- | --- | 
| 5 V | 5 V | - |
| 3v3 |  -  | - |
| GND | GND | - |
| FLT | GND | - |
| DMP | GND | - |
| SCL | GND | - |
| BCK | PB13  | I2S2_CK |
| DIN | PB15  | I2S2_SD |
| LCK | PB12  | I2S2_WS |
| FMT | GND | - |
| XMT | 3v3 | - |

#### INMP441 Microphone

| inmp441 I2S microphone | STM32F401 | I/F | 
| --- | --- | --- | 
| VDD | 3V3 | - |
| GND | GND | - |
| SD  | PB5 | I2S3_SD |
| L/R | GND | - |
| WS  | PA15 | I2S3_WS |
| SCK | PB3 | I2S3_SCK |

GPIO pins are configurable in API by updating defines.

### LCD SSD1306
| SSD1306 | STM32F401 | I/F | 
| --- | --- | --- |
| GND | GND | - | 
| 3V3 | VCC | - | 
| SCL | PB6 | I2C1_SCL | 
| SDA | PB7 | I2C1_SDA | 

### Rotary encoder
| Rotary encoder | STM32F401 | I/F | 
| --- | --- | --- |
| GND | GND | - | 
| +   | 3V3 | - | 
| SW  | PA5 | USR_SPK_MUTE_BTN | 
| DT  | PA7 | TIM3_CH2 | 
| CLK | PA6 | TIM3_CH1 | 

### LEDs and Buttons
| item | STM32F401 | I/F | 
| --- | --- | --- |
| Mic Mute Red LED | PA1  | LED_MIC_MUTE | 
| Mic Mute button  | PA2  | USR_MIC_MUTE_BTN | 
| --- | --- | --- |
|  Play/Pause button | PB2 | USR_PLAY_PAUSE_BTN |
|  Scan Previous button | PB1 | USR_SCAN_PREV_BTN |
|  Scan Next button | PB0 | USR_SCAN_NEXT_BTN |


## Acknowledgements

To create this project, following references were used:
 * The [TinyUSB](https://github.com/hathach/tinyusb) library.
 * The [SSD1306](https://github.com/afiskon/stm32-ssd1306/tree/master) library.
 
