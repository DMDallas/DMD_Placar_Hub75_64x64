/*******************************************************************************
 * Autor:   Diego Dallastella
 * Projeto: 
 * Modulo:  
 *
 * Elementos:
 *           Placa ESP8266
 *
 * Libraries:
 *
 * Pinos Utilizados
 *
 * https://pt.aliexpress.com/item/1005005967641936.html?spm=a2g0o.order_list.order_list_main.5.4dc7caa4ZHUyLi&gatewayAdapt=glo2bra
 *
  ******************************************************************************/
#ifndef DMD_config_H
#define DMD_config_H

/*
 * Inclusao de libraries
 */
#include <Arduino.h>
#include <WiFi.h>
#include "DMD_ESP32.h"

/*
 * Definições Gerais do Projeto
 */

// Altera pinos para o Relógio do Quarto
// #define MODULO_RELOGIO_TM1637_QUARTO

// Habilita o módulo de monitoração de energia
// #define MODULO_MON_ENERGIA

// Habilita o módulo de recepção 433
// #define MODULO_RF433

/*
 * Definição dos elementos para DEBUG
 */
// Debug do Main
// #define DEBUG_PROG

#ifdef DEBUG_PROG
  // #define DMD_DEBUG_FS
  #define DMD_DEBUG_NTP
  // #define DMD_DEBUG_OTA
  #define DMD_DEBUG_MQTT
  //#define DMD_DEBUG_DADO

  #define DMD_DEBUG_BT
  
  #define DMD_DEBUG_RF

  // #define DMD_DEBUG_RELOGIO

  // Debug OneWire
  // #define DMD_DEBUG_DS1820
  // #define DMD_DEBUG_ATTINY

  #define DMD_DEBUG_MATRIX_HUB

  // #define DMD_DEBUG_ATTINY_INT
  // #define DMD_DEBUG_ATTINY_ATUADOR
  // #define DMD_DEBUG_ATTINY_PWM
  // #define DMD_DEBUG_ATTINY_MEDIDOR
  // #define DMD_DEBUG_PROMINI_ATUADOR
  // #define DMD_DEBUG_PROMINI_ENERGIA
  // #define DMD_DEBUG_RF

  // #define DMD_DEBUG_AQUEC_SOLAR
  // #define DMD_DEBUG_BEER

  #define DEBUG_PRINTLN(x)  Serial.println(x)
  #define DEBUG_PRINT(x)    Serial.print(x)
  #define DEBUG_PRINTLNF(x)  Serial.println(F(x))
  #define DEBUG_PRINTF(x)    Serial.print(F(x))
#else
  #define DEBUG_PRINTLN(x)
  #define DEBUG_PRINT(x)
  #define DEBUG_PRINTLNF(x)  
  #define DEBUG_PRINTF(x)    
#endif

#define VERSION_FW_NUN   "001" 

#ifndef DEBUG_PROG
  #define VERSION_FW_DBG "N"
#else
  #define VERSION_FW_DBG "D"
#endif

#define VERSION_FW   "VE." VERSION_FW_DBG "." VERSION_FW_NUN


#define VERSION_DATA "DE1"

#define SYSTEM_NAME "Escada"
#define DEFAULT_BOARD_NAME "ControleEscada"

/*
 * Web password
 */
// #define ENABLE_WEB_PASSWORD

/*
 * Definições ligadas ao NS
 */
#define NS_SIZE 18
#define DEFAULT_NS "00000000000000000"

/*
 * Dados da rede Wi-Fi
 */
#define CONFIG_WIFI_USERNAME "DallasHome"
#define CONFIG_WIFI_PASSWORD "11012007"

/*
 * Dados da rede Wi-Fi em modo AP
 */
#define SYSTEM_AP_USERNAME "ControleEscada"

/*
 * Usuário e Senha padrão do sistema
 */
#define DEFAULT_LOCAL_USER_NAME "bodycenter"
#define DEFAULT_LOCAL_PW   "bodycenter"

/*
 * Dados MQTT
 */
#define DEFAULT_MQTT_SRV_IP "192.168.1.55"

#define DMD_MQTT_TOPIC_BASE "DMD/Home/"
#define DMD_MQTT_TOPIC_BASE_size (sizeof(DMD_MQTT_TOPIC_BASE)-1)

#define DADOS_INT_TOPIC       12

#define TOPIC_INT ":Int"
#define TOPIC_INT_size (sizeof(TOPIC_INT)-1)
#define TOPIC_ATUADOR "Saida/"
#define TOPIC_ATUADOR_size (sizeof(TOPIC_ATUADOR)-1)
#define TOPIC_PWM "SaidaPWM/"
#define TOPIC_PWM_size (sizeof(TOPIC_PWM)-1)

#define TOPIC_TEMP "Temp/"
#define TOPIC_TEMP_size (sizeof(TOPIC_TEMP)-1)

#define TOPIC_OFF ":Off"
#define TOPIC_OFF_size (sizeof(TOPIC_OFF)-1)

#define TOPIC_TEMP_EXT "Temp_Ext"
#define TOPIC_TEMP_EXT_size (sizeof(TOPIC_TEMP_EXT)-1)

#define TOPIC_MED "Med_"

#define TOPIC_FW "DMD/FW"
#define TOPIC_FW_size (sizeof(TOPIC_FW)-1)

#define TOPIC_POT "Pot"
#define TOPIC_WH_INST "Wh_inst"
#define TOPIC_WH "Wh"

#define DADOS_TOPIC_MAX_INDEX    15

#define DMD_MQTT_INT_ORIGEM_TEMP 200

/*
 * Definições referente ao armazemento de dados na Flash (EEPROM)
 */
#define DMD_EEPROM_SIZE 512
#define DMD_EEPROM_CONFIG_START 0

#define DMD_DEBOUNCE_DEFAULT 25

/*
 * Definições da comunicação com o Servidor
 */
#define DEFAULT_TYPE "Full"

#define SRV_MAX_RETRY   3

#define FW_UPDATE_RETRY_MAX        6
#define FW_UPDATE_RETRY_SEM_UPDATE 0
#define NEW_VERSION_FW "0000.000"

/*
 * Definições Rele
 */
#define RELE_COUNTER    5
#define BUZZER_COUNTER  5

/*
 * Pinos da placa
 *              _________________________________
 *             |    ------- ---- ---- ------     |
 *             |    |     | |  | |  | |          |
 *             |    |     ---  ---  ---          |
 *             |    |                            |
 *             | RST                 (GPIO01) TX |
 *             | ADC                 (GPIO03) RX |
 *             | CH_PD              (SCL) GPIO05 |  
 *   INT ALERT | GPIO16 (XPD)       (SDA) GPIO04 |  
 *             | GPIO14 (CLK )            GPIO00 |
 *             | GPIO12 (MISO)            GPIO02 |  
 *             | GPIO13 (MOSI)       (CS) GPIO15 | OW
 *             | VCC                         GND |
 *              ---------------------------------
 *
 */
#define PIN_BUZZER     DIG_PIN_32
#define PIN_FIELD      DIG_PIN_04
#define PIN_RELE       DIG_PIN_02
#define PIN_TACH       DIG_PIN_15

#define PIN_ANALOG_FIELD  DIG_PIN_35
#define PIN_ANALOG_VIN    DIG_PIN_34
#define PIN_ANALOG_5V     DIG_PIN_39

#define DMD_GERAL_TEMPO_IDLE 300 // 5 minutos 300 segundos

#define DMD_TELA_DEFAULT_LEVEL 3

#define DMD_MAX_LEVEL         13

#define DEFAULT_STEP_DIVIDER  1000
#define DMD_SEG_P_LINHA_GRAF  60
// Pino de "interrupção" - AtTiny tem informação
#define DMD_PIN_INT    DIG_PIN_16

#define DMD_LOG_TECLAS 5

#define DMD_RF_RC_PIN 22

#define NUM_PLACARES 2

/*
 * Recepção RF 433
 */
#define PIN_RF_RX      DIG_PIN_04

#define DMD_RF_MAX_DISP 10

/*
 * Definições Modos de Reset
 */
#define RESET_TYPE_NORMAL   0x51
#define RESET_TYPE_FACTORY  0x39


// Dados
#define DMD_DADO_MED_NUM      10

#endif /* DMD_config_H */