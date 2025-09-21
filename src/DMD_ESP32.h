/*******************************************************************************
 * Autor:   Diego Dallastella
 * Projeto: Base
 * Modulo:  Definições da placa ESP32
 * Versão:  1.0
 *
 * Elementos:
 *           Placa ESP32
 *
 * Libraries:
 *
 * Pinos Utilizados
 *
 *
 *
  ******************************************************************************/
#ifndef DMD_ESP32_H
#define DMD_ESP32_H

#define DMD_ESP32
/*
 * Definições dos pinos da placa
 */

/*******************************************************************************
* Definicoes dos pinos da Placa ESP32
******************************************************************************/
/*
* Pinos Digitais
*/
#define DIG_PIN_00 0x00
//#define DIG_PIN_01 0x01  // TX
#define DIG_PIN_02 0x02
//#define DIG_PIN_03 0x03  // RX
#define DIG_PIN_04 0x04
#define DIG_PIN_05 0x05
/* Pinos utilizados para memória
  #define DIG_PIN_06 0x06
  #define DIG_PIN_07 0x07
  #define DIG_PIN_08 0x08
  #define DIG_PIN_09 0x09
  #define DIG_PIN_10 0x0A
  #define DIG_PIN_11 0x0B
*/
#define DIG_PIN_12 0x0C
#define DIG_PIN_13 0x0D
#define DIG_PIN_14 0x0E
#define DIG_PIN_15 0x0F
#define DIG_PIN_16 0x10

#define DIG_PIN_17 0x11
#define DIG_PIN_18 0x12
#define DIG_PIN_19 0x13
#define DIG_PIN_20 0x14
#define DIG_PIN_21 0x15
#define DIG_PIN_22 0x16
#define DIG_PIN_23 0x17
#define DIG_PIN_24 0x18
#define DIG_PIN_25 0x19
#define DIG_PIN_26 0x1A
#define DIG_PIN_27 0x1B
#define DIG_PIN_28 0x1C
#define DIG_PIN_29 0x1D
#define DIG_PIN_30 0x1E
#define DIG_PIN_31 0x1F
#define DIG_PIN_32 0x20
#define DIG_PIN_33 0x21
#define DIG_PIN_34 0x22  // Somente Entrada
#define DIG_PIN_35 0x23  // Somente Entrada
#define DIG_PIN_36 0x24  // Somente Entrada
// #define DIG_PIN_37 0x25
// #define DIG_PIN_38 0x26
#define DIG_PIN_39 0x27  // Somente Entrada


#define PIN_RX 1
#define PIN_TX 3

/*
 * Pino Analogico
 */
#define ADC1_CH0 DIG_PIN_36
#define ADC1_CH1 DIG_PIN_37
#define ADC1_CH2 DIG_PIN_38
#define ADC1_CH3 DIG_PIN_39
#define ADC1_CH4 DIG_PIN_32
#define ADC1_CH5 DIG_PIN_33
#define ADC1_CH6 DIG_PIN_34
#define ADC1_CH7 DIG_PIN_35
#define ADC2_CH0 DIG_PIN_04
#define ADC2_CH1 DIG_PIN_00
#define ADC2_CH2 DIG_PIN_02
#define ADC2_CH3 DIG_PIN_15
#define ADC2_CH4 DIG_PIN_13
#define ADC2_CH5 DIG_PIN_12
#define ADC2_CH6 DIG_PIN_14
#define ADC2_CH7 DIG_PIN_27
#define ADC2_CH8 DIG_PIN_25
#define ADC2_CH9 DIG_PIN_26

#define ANALOG_PIN ADC1_CH0

#define PIN_DMD_SDA 21
#define PIN_DMD_SCL 22

#endif /* DMD_ESP32_H */
