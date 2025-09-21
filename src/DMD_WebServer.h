/*******************************************************************************
 * Autor:   Diego Dallastella
 * Projeto: Home
 * Modulo:  WebServer
 * Versão:  3.0
 * Histórico: 3.0 > Incluída a função ERASE_CONFIG
 *            ...
 *            2.1 > Remoção do FS
 *            ...
 *            2.0 > retrabalhada para usar FS
 *
 * Elementos:
 *           Placa ESP8266
 *
 * Libraries:
 *           ESP8266WebServer
 *
 * Utiliza a memória interna do ESP8266
  ******************************************************************************/
#ifndef DMD_WebServer_H
#define DMD_WebServer_H

/*
 * Inclusao de libraries
 */
// #include <ESP8266WebServer.h>
#include <WebServer.h>

/*
 * Variaveis globais
 */
// extern ESP8266WebServer server;
extern WebServer server;


/*
 * Declaração da Funções
 */
void DMD_WebServer_Init ();
void DMD_WebServer_Handler ();

void f_wifi_box (String *msg, bool duplo = false);

void f_handleRoot();
void f_handleInfo();
void f_handleWifi();

void f_reset_factory();
void f_reset();
void f_eraseconfig();

void f_html_header(String *msg, int maxW);
void f_update();

void f_handleConfig();
void f_handleConfigLink();
void f_handleConfigInt();
void f_handleConfigNome();
void f_handleConfigIp();
void f_handleConfigIntColorSch();

void f_handleConfigNS();
void f_handleConfigLocalPW();

void f_json_reset ();

bool WebUpdate(const char *);
String WebUpdate_GetErro();

void f_handleConfigDebounce();

void f_handleConfigPWM();

void f_handleConfigMedidor();

void f_handleConfigTopicos();

void f_handleConfigTopicoTemp();

void f_handleConfigAquecSolar();

void f_handleConfigBeer();

void f_handleConfigRelogio();

void f_handleConfigRF();

#endif //DMD_WebServer_H
