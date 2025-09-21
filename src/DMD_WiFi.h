/*******************************************************************************
 * Autor:   Diego Dallastella
 * Projeto: Base
 * Modulo:  Wifi
 * Versão:  1.4
 *
 * Elementos:
 *           Placa ESP8266
 *
 * Libraries:
 *           Wifi
 *
  ******************************************************************************/
#ifndef DMD_WiFi_H
#define DMD_WiFi_H

/*
 * Definições gerais do Modulo
 */
// Define a quantidade de chamadas até tentar reconectar ao WiFi
#define DMD_WIFI_MAX_CONTADOR 3


#ifdef DMD_DEBUG_WIFI
  #define D_P_Wifi_LN(x)  Serial.println(x)
  #define D_P_Wifi(x)     Serial.print(x)
  #define D_P_Wifi_LN_F(x) Serial.println(F(x))
  #define D_P_Wifi_F(x)    Serial.print(F(x))
#else
  #define D_P_Wifi_LN(x)
  #define D_P_Wifi(x)
  #define D_P_Wifi_LN_F(x)
  #define D_P_Wifi_F(x)
#endif

/*
 * Declaração da Funções
 */
void DMD_Wifi_Pre_Connect();
bool DMD_WifiConectWait15(const char * ,const  char *, bool p_atual = true);
void DMD_WifiReconect (const char *,const  char *);
bool DMD_IsConected ();

void DMD_Wifi_AP (const char * ,const char * );
void DMD_Wifi_Show () ;

void DMD_WifiPrintStatus ();

void DMD_Wifi_Disable_softAP ();

struct WiFiEventStationModeGotIP {
  int i;
};

struct WiFiEventStationModeDisconnected{
  int i;
};

void onWiFiConnect(/*WiFiEventStationModeGotIP ipInfo*/) ;
void onWiFiDisconnected(/*WiFiEventStationModeDisconnected event_info*/);

void DMD_WiFi_Loop ();

void DMD_WiFi_Disconnect ();

void DMD_WiFi_On  ();
void DMD_WiFi_Off ();

void DMD_WiFi_EraseConfig ();

#endif /* DMD_WiFi_H */
