/*******************************************************************************
 * Autor:   Diego Dallastella
 * Projeto: Base
 * Modulo:  Estatísticas dos módulos do sistema
 *
 * Elementos:
 *           Placa ESP8266
 *
 * Libraries:
 *
 * Pinos Utilizados
 *
  ******************************************************************************/
 /*
 * Definições do Projeto
 */
#include "DMD_Config.h"

/*
 * Inclusao de libraries
 */


/*
 * Inclusao funções e dados do projeto
 */
#include "DMD_Stat.h"

/*
 * Variável global do módulo
 */
DMD_Stat g_Stat;

/*
 * Construtora - zera todos os dados
 */
DMD_Stat::DMD_Stat () {
  Web_GET = 0;
  Web_Erro = 0;

  JSON_OK = 0;
  JSON_Erro = 0;

  // MQTT_Enviados_OK = 0;
  // MQTT_Enviados_Erro = 0;

  // ADC_Leituras = 0;

  // GET_Enviados_OK = 0;
  // GET_Enviados_Erro = 0;
  // GET_Erro_WIFI = 0;
}

/*
 * Retorna um String com as estatisticas no formato JSON
 */
void DMD_Stat::JSON(String *msg){
  *msg += "{\"Web_GET\":" + String(Web_GET) + ",";
  *msg += "\"Web_Erro\":" + String(Web_Erro) + ",";
  *msg += "\"JSON_OK\":" + String(JSON_OK) + ",";
  // *msg += "\"JSON_Erro\":" + String(JSON_Erro) + ",";
  *msg += "\"JSON_Erro\":" + String(JSON_Erro) + "}";
  // *msg += "\"MQTT_Enviados_OK\":" + String(MQTT_Enviados_OK) + ",";
  // *msg += "\"MQTT_Enviados_Erro\":" + String(MQTT_Enviados_Erro) + ",";
  // *msg += "\"ADC_Leituras\":" + String(ADC_Leituras) + ",";
  // *msg += "\"GET_Enviados_OK\":" + String(GET_Enviados_OK) + ",";
  // *msg += "\"GET_Enviados_Erro\":" + String(GET_Enviados_Erro) + ",";
  // *msg += "\"GET_Erro_WIFI\":" + String(GET_Erro_WIFI) + "}";
}
