/*******************************************************************************
 * Autor:   Diego Dallastella
 * Projeto: Base
 * Modulo:  NTP
 * Versão:  2.0
 * Histórico  > 2.0 - Alterado para utilização do cliente NTP nativo do SDK
 *            > 1.3 - Alterada a lib utilizada para o NTP
 *
 * Fonte: https://github.com/espressif/esp-idf/blob/master/examples/protocols/sntp/main/sntp_example_main.c
 *        https://docs.espressif.com/projects/esp-idf/en/latest/esp32/api-reference/system/system_time.html
 * 
 * Elementos:
 *           Placa ESP32
 *
 * Libraries:
 *
  ******************************************************************************/
#ifndef DMD_NTP_H
#define DMD_NTP_H

/*
 * Definições gerais do Modulo
 */
#ifdef DMD_DEBUG_NTP
  #define D_P_NTP_LN(x)  Serial.println(x)
  #define D_P_NTP(x)     Serial.print(x)
  #define D_P_NTP_LN_F(x) Serial.println(F(x))
  #define D_P_NTP_F(x)    Serial.print(F(x))
#else
  #define D_P_NTP_LN(x)
  #define D_P_NTP(x)
  #define D_P_NTP_LN_F(x)
  #define D_P_NTP_F(x)
#endif

/*
 * Declaração da Funções
 */
void DMD_NTP_Init();

#endif // DMD_NTP_H
