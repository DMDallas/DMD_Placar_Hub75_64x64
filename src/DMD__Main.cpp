#include <Arduino.h>

#include "DMD_Config.h"
#include "DMD_WiFi.h"
#include "DMD_Globais.h"
#include "DMD_EEPROM.h"

// #include "DMD_OW_DS18B20.h"
// #include "DMD_OW_ATtiny.h"
#include "DMD_MQTT.h"
#include "DMD_NTP.h"
#include "DMD_WebServer.h"

#include "DMD_KeyPad.h"

#include "DMD_Matrix_Hub75.h"

#include "DMD__Placa.h"

#include "time.h"

#include "DMD_RF_RC_Switch.h"

DMD_KeyPad g_KeyPad_1;

char Key_Map[ROWS][COLS] = {
    {'S', '_', 'E'},
    {'4', '5', '>'},
    {'7', '8', '0'},
    {'4', '7', '>'}};

/*
 * Função de callback do Wifi - devem ser implementadas
 */
void onWiFiConnect(/*WiFiEventStationModeGotIP ipInfo*/) {
  DEBUG_PRINT("CB Wifi Connect - Sinal = ");  DEBUG_PRINT(String(WiFi.RSSI()) );
  DEBUG_PRINT(" (");  DEBUG_PRINT(WiFi.localIP()); DEBUG_PRINTLN(")");

  DMD_Wifi_Disable_softAP();

  g_Status_WIFI_OK = true;
}

void onWiFiDisconnected(/*WiFiEventStationModeDisconnected event_info*/) {

  if ( g_Status_WIFI_OK ) {
    String l_ap = SYSTEM_AP_USERNAME;
    // l_ap += g_Dados.NS;
    DMD_Wifi_AP(l_ap.c_str(), g_Dados.Local_PW);
  }

  // DEBUG_PRINTLN("CB Wifi - Discon");
  DEBUG_PRINTF("w");

  g_Status_WIFI_OK = false;
}

void f_Loop_Core_0( void * );
void BT_CallBack (uint8_t );

void setup() {
  // pinMode( 8, OUTPUT);
  Serial.begin(74880);
  Serial.println(F("------------------"));
  Serial.printf("ESP32 - ");
  Serial.println(SYSTEM_NAME);
  Serial.print(F("Versão: "));  Serial.println(VERSION_FW);
  Serial.print(F("Dados: "));   Serial.println(VERSION_DATA);
  Serial.println(F("------------------"));

  setenv("TZ","<GMT-3>+3",1);
  
  // Comanda a conexão prévia a rede gravada anteriormente
  DMD_Wifi_Pre_Connect();
  
  // Teste para Reset de Fábrica
  // Carregar dados da EEPROM - sempre carrega o Numero Serial
  if ( DMD_EEPROM_Check_Version(VERSION_DATA) ) {
    if ( DMD_EEPROM_ForceLoad((char *)&g_Dados, 4 + 1 + NS_SIZE ) ) {
      DEBUG_PRINTLNF("Versão do dados OK");
      if (g_Dados.reset_type == RESET_TYPE_FACTORY) {
        DEBUG_PRINTLNF("Reset de Fábrica");
        g_Dados.reset_type = RESET_TYPE_NORMAL;
        DMD_EEPROM_Save((char *)&g_Dados, sizeof(g_Dados));
      } else {
        // Carregar dados da EEPROM
        if ( DMD_EEPROM_Load((char *)&g_Dados, sizeof(g_Dados)) ) {
          DEBUG_PRINTLNF(" Dados Carregados");
        } else
          DEBUG_PRINTLNF(" Erro ao carregar dados");
      }
    }
  } else {
    DEBUG_PRINTLNF("Versão do dados diferente");
    // Zera_g_Dados();
  }

  //Inicializar NTP - Sincronismo de Data e Hora
  DMD_NTP_Init();
  
  delay(250);

  DMD_WebServer_Init();

  // g_MQTT.Init(g_Dados.MQTT_Srv_IP);

  TaskHandle_t Task_core_0;
  xTaskCreatePinnedToCore(
      f_Loop_Core_0,  /* Function to implement the task */
      "Loop_core_0",  /* Name of the task */
      30000,          /* Stack size in words */
      NULL,           /* Task input parameter */
      0,              /* Priority of the task */
      &Task_core_0,   /* Task handle. */
      0);             /* Core where the task should run */

  for (size_t i = 0; i < DMD_LOG_TECLAS; i++) {
    g_DMD_Log_Teclas[i] = 0;
  }

  g_Buzzer.Ativar(10);

  g_RC_RF.Init(DMD_RF_RC_PIN, g_Dados.RF, BT_CallBack);

  DEBUG_PRINTF(" Heap: "); DEBUG_PRINTLN(ESP.getFreeHeap());
  DEBUG_PRINTLN("-----------------------------");
}

void BT_CallBack (uint8_t f_bt) {
  DEBUG_PRINTF (" - BT="); DEBUG_PRINTLN (f_bt);

  if( g_DadoTela.Is_Idle())
    g_DadoTela.Set_Activ();

  switch (f_bt) {
    case 0:  g_DadoTela.Start();        g_Buzzer.Ativar(10); break;
    case 1:  g_DadoTela.Stop();         g_Buzzer.Ativar(10); break;
    case 2:  g_DadoTela.Start_Stop();   g_Buzzer.Ativar(10); break;
    case 10: g_DadoTela.Ponto_UP(0);    g_Buzzer.Ativar(2); break;
    case 11: g_DadoTela.Ponto_UP(0,2);  g_Buzzer.Ativar(4); break;
    case 15: g_DadoTela.Ponto_DW(0);    g_Buzzer.Ativar(6); break;
    case 20: g_DadoTela.Ponto_UP(1);    g_Buzzer.Ativar(2); break;
    case 21: g_DadoTela.Ponto_UP(1,2);  g_Buzzer.Ativar(4); break;
    case 25: g_DadoTela.Ponto_DW(1);    g_Buzzer.Ativar(6); break;
    case 50: 
      g_DadoTela.c_conta_BT++;
      g_DadoTela.c_conta_BT_seg = 5;
      DEBUG_PRINTF (" - Zerar="); DEBUG_PRINTLN (g_DadoTela.c_conta_BT);
      
      if (g_DadoTela.c_conta_BT >= 5 ){
        DEBUG_PRINTLNF (" ZERAR");
        g_DadoTela.Clear();
        g_DadoTela.c_conta_BT = 0;
        g_DadoTela.c_conta_BT_seg = 0;
        g_Buzzer.Ativar(25);
      }
      break;
    
    default:
      break;
  }

  if ( f_bt > 10 && f_bt < 50 ) { // outros botões
    g_DadoTela.c_conta_BT = 0; 
    DEBUG_PRINTF (" = bt init - reiniciado =");
  }

  Hub75_Update ( g_contador_seg, false );


  for (size_t i = 0; i < DMD_LOG_TECLAS; i++) {
    if ( g_DMD_Log_Teclas[i] == 0) {
      g_DMD_Log_Teclas[i] = (char) f_bt;
      break;
    } else {
      if ( i == (DMD_LOG_TECLAS - 1) ) { // ultimo
        for (size_t j = 1; j < DMD_LOG_TECLAS; j++) {
          g_DMD_Log_Teclas[j - 1] = g_DMD_Log_Teclas[j];
        }
        g_DMD_Log_Teclas[DMD_LOG_TECLAS - 1] = (char) f_bt;
        break;
      }
    }
  }


}

void f_Loop_Core_0( void * parameter)  {

  delay(2000);

  DMD_Matrix_HUB75_Init();

  DEBUG_PRINTF(" f_Loop_Core_0 - CORE:");
  DEBUG_PRINTLN(xPortGetCoreID());

  // Tenta conexão com a rede Wifi
  if ( !DMD_WifiConectWait15(g_Dados.WIFI_SSID , g_Dados.WIFI_PW) ){
    String l_ap = SYSTEM_AP_USERNAME;
    l_ap += g_Dados.NS;
    DMD_Wifi_AP(l_ap.c_str(), g_Dados.Local_PW);
  }

  while (1)  {
    DMD_WiFi_Loop();
    
    yield();

    DMD_WebServer_Handler();

    delay(1);
  }
}

void loop() {
  static unsigned long last_millis = 0;
  static unsigned long last_millis_fast = 0;
  unsigned long l_millis = millis();

#ifdef DEBUG_PROG
  if ( g_contador_loop == 0 ){
    DEBUG_PRINTF(" loop - CORE:");
    DEBUG_PRINTLN(xPortGetCoreID());
  }
#endif

  g_contador_loop++;
  
  // g_Tach.Loop();

  g_RC_RF.Loop();

  // -----------------------------------
  // Loop rápido - a cada 25 milisegundos
  // -----------------------------------
  if ( (l_millis - last_millis_fast) > 25 )  {
    last_millis_fast = l_millis;

    g_Buzzer.Loop();

    // g_Field.PWM_Fade_Loop();

  }

  // -----------------------------------
  // Loop normal - a cada 1 segundo
  // -----------------------------------
    if ( (l_millis - last_millis) > 1000 )  {
      g_contador_seg++;
      last_millis = l_millis;

      Hub75_Update (g_contador_seg);

      if ( (g_contador_seg & 0x3F) == 0x07) {
        DEBUG_PRINTF(" Heap: "); DEBUG_PRINTLN(ESP.getFreeHeap());
      }

    }
    // Rollover - quando o millis reinicia a contagem
    if ( l_millis < last_millis )
      last_millis = 0;

}
