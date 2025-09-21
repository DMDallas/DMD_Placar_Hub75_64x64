/*******************************************************************************
 * Autor:   Diego Dallastella
 * Projeto: Bingo
 * Modulo:  Variaveis Globais
 *
 * Elementos:
 *
 * Libraries:
 *           ESP8266Wifi
 *           MQTT
 *
 * Pinos Utilizados
 *
 *
  ******************************************************************************/
#ifndef DMD_Globais_H
#define DMD_Globais_H


/*
 * Inclusao de libraries
 */
#include <Arduino.h>
//#include <ESP8266WebServer.h>

#include "DMD_Config.h"
#include "DMD_Output.h"

#include "DMD_RF_RC_Switch.h"

#define DADOS_NAME_SIZE 21
#define DADOS_ID_SIZE         DADOS_NAME_SIZE
#define DADOS_WIFI_SSID_SIZE  DADOS_NAME_SIZE
#define DADOS_WIFI_PW_SIZE    DADOS_NAME_SIZE
#define DADOS_SRV_SIZE        DADOS_NAME_SIZE
#define DADOS_LOCAL_PW_SIZE   DADOS_NAME_SIZE

#define DADOS_IP_SIZE    16
#define NEW_VERSION_SIZE 9

#define DADOS_INDEX_INVALID 0xFF

/*
 * Estruturas
 */

struct DadosEEPROMStruct {
  char version_data[4];
  char reset_type;
  char NS [NS_SIZE];

  char WIFI_SSID  [DADOS_WIFI_SSID_SIZE];
  char WIFI_PW    [DADOS_WIFI_PW_SIZE];
  char Local_PW   [DADOS_LOCAL_PW_SIZE];

  char new_version[NEW_VERSION_SIZE];
  uint8_t new_version_retry;

  char Type       [DADOS_SRV_SIZE];
  char Name       [DADOS_NAME_SIZE];
  char MQTT_Srv_IP[DADOS_IP_SIZE];

  uint16_t c_Color_Hour;
  uint16_t c_Color_Hour_Stop;
  uint16_t c_Color_Time1;
  uint16_t c_Color_Time2;
  uint16_t c_Color_Textos;

  uint16_t Dummy_1;
  uint16_t Dummy_2;
  uint16_t Dummy_3;
  uint16_t Dummy_4;

  uint16_t Tempo_Idle;
  uint8_t  Tela_ativa;
  uint16_t RF_Repeat_Time;
  
  DMD_RF_Dado RF[DMD_RC_RF_MAX_BT];
};

/*
 * Variaveis globais
 */
extern DadosEEPROMStruct g_Dados;

extern unsigned int  g_contador_seg; // Tempo de Boot 10 segundos
extern unsigned int  g_contador_loop; // Tempo de Boot 10 segundos

extern bool g_Status_WIFI_OK;
extern int  g_Status_Servidor_Counter;

extern DMD_Output g_Bz;
extern DMD_Output g_Field;
extern DMD_Output g_Rele;

extern char g_DMD_Log_Teclas[DMD_LOG_TECLAS];


#endif //DMD_Globais_H
