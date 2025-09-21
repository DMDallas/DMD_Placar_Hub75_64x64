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
/*
 * Definições do Projeto
 */
#include "DMD_Config.h"

/*
 * Inclusao de libraries
 */
#include "DMD_NTP.h"

#include "esp_sntp.h"
#include <time.h>             // time() ctime()

// 1577847600 = Quarta, 1 Jan 2020 00:00:00 GMT-03:00
// 1609470000 = Sexta , 1 Jan 2021 00:00:00 GMT-03:00

#define DMD_RTC_DEFAULT 1577847600 

void DMD_NTP_Callback(struct timeval *tv){
  D_P_NTP_LN_F("** NTP CB **");

  time_t f_now = time(nullptr);

  D_P_NTP(f_now); D_P_NTP(" - ");  D_P_NTP_LN( ctime(&f_now) );

  // setTime(f_now); // set para GMT não local
}

/**
 * @brief Função de inicialização do NTP, configura o cliente NTP nativo do sdk ESP8266 ou ESP32
 * 
 */
void DMD_NTP_Init(){
  D_P_NTP_LN_F("** NTP **");

  sntp_setoperatingmode(SNTP_OPMODE_POLL);
  sntp_setservername(0, "br.pool.ntp.org");
  sntp_set_time_sync_notification_cb(DMD_NTP_Callback);

  // Configura a hora inicial do sistema
  time_t rtc = DMD_RTC_DEFAULT;
  timeval tv = { rtc, 0 };
  settimeofday(&tv, nullptr);

  sntp_init();

  D_P_NTP_LN_F("** NTP **");
}