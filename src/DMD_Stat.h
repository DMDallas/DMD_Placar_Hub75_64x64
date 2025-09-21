/*******************************************************************************
 * Autor:   Diego Dallastella
 * Projeto: Base
 * Modulo:  Estatisticas
 *
 * Elementos:
 *           Placa ESP8266
 *
 * Libraries:
 *           OneWire
 *
 * Pinos Utilizados
 *
  ******************************************************************************/
#ifndef DMD_Stat_H
#define DMD_Stat_H

/*
 * Inclusao de libraries
 */
#include <Arduino.h>

#include "DMD_Stat.h"

/*
 * Estruturas
 */
class DMD_Stat {
public:
  int Web_GET;
  int Web_Erro;

  int JSON_OK;
  int JSON_Erro;

  // int MQTT_Enviados_OK;
  // int MQTT_Enviados_Erro;

  // int ADC_Leituras;

  // int GET_Enviados_OK;
  // int GET_Enviados_Erro;
  // int GET_Erro_WIFI;

  public:
    DMD_Stat ();

    void JSON (String *);
};

/*
 * Variaveis globais
 */

extern DMD_Stat g_Stat;

#endif //DMD_Stat_H
