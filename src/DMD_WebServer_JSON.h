/*******************************************************************************
 * Autor:   Diego Dallastella
 * Projeto: WebServer JSON Específica do Projeto Home
 * Modulo:  Server - JSON
 * Versão:  1.8
 * Histórico: 1.8 - Implementada a função de remoção de Slave
 *                  Retirada das Lib - NTP
 * 
 * Elementos:
 *           Placa ESP8266
 *
 * Libraries:
 *           ArduinoJson

  ******************************************************************************/
#ifndef DMD_WebServer_JSON_H
#define DMD_WebServer_JSON_H

/*
 * Declaração da Funções
 */
void DMD_WebServer_Init_JSON();

void f_json_echo ();
void f_json_Get_Time();

void f_json_SetAP();
void f_json_wifi_detect();

void f_json_Config();
void f_json_Config_Remove ();
void f_json_Config_Int ();
void f_json_Config_Int_Topic();
void f_json_Config_Nome ();
void f_json_Config_Ip ();

void f_json_Config_Int_Color ();

void f_json_Config_NS ();
void f_json_CMD();

void f_json_Config_LocalPW ();

void f_json_file_list ();
void f_json_file_rem ();

//Estatisticas
void f_json_stat();

void f_json_Config_Debounce();

void f_json_Config_Medidor ();

void f_json_Config_PWM();

void f_json_Config_Topic ();

void f_json_Config_Topic_Temp ();

void f_json_Config_Relogio();

void f_json_Config_AqueSolar ();
void f_json_Config_Beer_Ferm ();

void f_json_RF_Add_SMW210 ();
void f_json_RF_Topic      ();
void f_json_RF_Del        ();

#endif //DMD_WebServer_JSON_H
