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
/*
 * Definições do Projeto
 */
#include "DMD_Config.h"

/*
 * Inclusao de libraries
 */
// #include <ArduinoJson.h>

/*
 * Inclusao funções e dados do projeto
 */
#include "DMD_Globais.h"

#include "DMD__Placa.h"

#include "DMD_Matrix_Hub75.h"


 /*
  * Variaveis globais
  */
DadosEEPROMStruct g_Dados = {
  VERSION_DATA,           // version_data
  RESET_TYPE_FACTORY,      // reset_type
  DEFAULT_NS,             // NS

  CONFIG_WIFI_USERNAME,   // WIFI_SSID
  CONFIG_WIFI_PASSWORD,   // WIFI_PW
  DEFAULT_LOCAL_PW,       // Local_PW

  NEW_VERSION_FW,         // new_version
  FW_UPDATE_RETRY_SEM_UPDATE,// new_version_retry

  DEFAULT_TYPE,           // Type
  DEFAULT_BOARD_NAME,     // Name
  DEFAULT_MQTT_SRV_IP,    // IP padrão para o servidor MQTT

  GREEN,
  MAGENTA,
  YELLOW,
  RED,  
  0xFFFF,

  DMD_GERAL_TEMPO_IDLE,
  DMD_TELA_DEFAULT_LEVEL,
  350
};

// 0	210	230	15
// 1	200	215	15
// 2	190	200	14
// 3	180	186	14
// 4	170	172	13
// 5	160	159	13
// 6	150	146	12
// 7	140	134	11
// 8	130	123	10
// 9	120	113	9
// 10	110	104	8
// 11	100	96	96


unsigned int g_contador_seg = 0; // Mantido em 0 para não pular a primeira Leitura dos Sensores
unsigned int  g_contador_loop = 0;

bool g_Status_WIFI_OK = false;
int g_Status_Servidor_Counter = 1;

// uint8_t g_Int_Msg_Origem[ATTiny_INT_MAX_SLAVES][ATTiny_MAX_INT] ;

bool g_resetar = false;


DMD_Output g_Bz     (PIN_BUZZER);
DMD_Output g_Field  (PIN_FIELD);
DMD_Output g_Rele   (PIN_RELE);

DMD_Tach   g_Tach   (PIN_TACH);

char g_DMD_Log_Teclas[DMD_LOG_TECLAS];
