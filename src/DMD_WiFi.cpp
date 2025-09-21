/*******************************************************************************
 * Autor:   Diego Dallastella
 * Projeto: Base
 * Modulo:  Wifi
 * Versão:  1.4
 *
 * Elementos:
 *           Placa ESP32
 *
 * Libraries:
 *           Wifi
 *
  ******************************************************************************/

/*
 * Definições do Projeto
 */
#include "DMD_Config.h"

/*
 * Inclusao de libraries
 */
#include "DMD_WiFi.h"
#include <DNSServer.h>

#include "DMD_Globais.h"

#include "esp_wifi.h"


int g_Wifi_contador = 0;

#define DNS_PORT 53
DNSServer dnsServer;

//IPAddress apIP(192, 168, 4, 1);
IPAddress apIP(8, 8, 8, 8);
IPAddress netMsk(255, 255, 255, 0);

/*
 *
 */
void WiFiEvent(WiFiEvent_t event) {
    D_P_Wifi_F("[WiFi] "); D_P_Wifi_LN(event);

    switch (event) {
        case SYSTEM_EVENT_WIFI_READY: 
            D_P_Wifi_LN_F("WiFi interface ready");
            break;
        case SYSTEM_EVENT_SCAN_DONE:
            D_P_Wifi_LN_F("Completed scan for access points");
            break;
        case SYSTEM_EVENT_STA_START:
            D_P_Wifi_LN_F("WiFi client started");
            break;
        case SYSTEM_EVENT_STA_STOP:
            D_P_Wifi_LN_F("WiFi clients stopped");
            break;
        case SYSTEM_EVENT_STA_CONNECTED:
            D_P_Wifi_LN_F("Connected to access point");
            break;
        case SYSTEM_EVENT_STA_DISCONNECTED:
            D_P_Wifi_LN_F("Disconnected from WiFi access point");
            onWiFiDisconnected();
            break;
        case SYSTEM_EVENT_STA_AUTHMODE_CHANGE:
            D_P_Wifi_LN_F("Authentication mode of access point has changed");
            break;
        case SYSTEM_EVENT_STA_GOT_IP:
            //D_P_Wifi_F("Obtained IP address: ");
            //D_P_Wifi_LN(WiFi.localIP());
            onWiFiConnect();
            break;
        case SYSTEM_EVENT_STA_LOST_IP:
            D_P_Wifi_LN_F("Lost IP address and IP address is reset to 0");
            break;
        case SYSTEM_EVENT_STA_WPS_ER_SUCCESS:
            D_P_Wifi_LN_F("WiFi Protected Setup (WPS): succeeded in enrollee mode");
            break;
        case SYSTEM_EVENT_STA_WPS_ER_FAILED:
            D_P_Wifi_LN_F("WiFi Protected Setup (WPS): failed in enrollee mode");
            break;
        case SYSTEM_EVENT_STA_WPS_ER_TIMEOUT:
            D_P_Wifi_LN_F("WiFi Protected Setup (WPS): timeout in enrollee mode");
            break;
        case SYSTEM_EVENT_STA_WPS_ER_PIN:
            D_P_Wifi_LN_F("WiFi Protected Setup (WPS): pin code in enrollee mode");
            break;
        case SYSTEM_EVENT_AP_START:
            D_P_Wifi_LN_F("WiFi access point started");
            break;
        case SYSTEM_EVENT_AP_STOP:
            D_P_Wifi_LN_F("WiFi access point  stopped");
            break;
        case SYSTEM_EVENT_AP_STACONNECTED:
            D_P_Wifi_LN_F("Client connected");
            break;
        case SYSTEM_EVENT_AP_STADISCONNECTED:
            D_P_Wifi_LN_F("Client disconnected");
            break;
        case SYSTEM_EVENT_AP_STAIPASSIGNED:
            D_P_Wifi_LN_F("Assigned IP address to client");
            break;
        case SYSTEM_EVENT_AP_PROBEREQRECVED:
            D_P_Wifi_LN_F("Received probe request");
            break;
        case SYSTEM_EVENT_GOT_IP6:
            D_P_Wifi_LN_F("IPv6 is preferred");
            break;
        case SYSTEM_EVENT_ETH_START:
            D_P_Wifi_LN_F("Ethernet started");
            break;
        case SYSTEM_EVENT_ETH_STOP:
            D_P_Wifi_LN_F("Ethernet stopped");
            break;
        case SYSTEM_EVENT_ETH_CONNECTED:
            D_P_Wifi_LN_F("Ethernet connected");
            break;
        case SYSTEM_EVENT_ETH_DISCONNECTED:
            D_P_Wifi_LN_F("Ethernet disconnected");
            break;
        case SYSTEM_EVENT_ETH_GOT_IP:
            D_P_Wifi_LN_F("Obtained IP address");
            break;
        default: break;
  }
}

void DMD_Wifi_Pre_Connect () {
  Serial.print("Modo Wifi: "); Serial.print(WiFi.getMode());

  if ( WiFi.getMode() == WIFI_MODE_NULL ) {
    Serial.print("Null");
    DMD_WiFi_On ();
    delay(30);
  }

  WiFi.onEvent(WiFiEvent);

  WiFi.setAutoConnect(false);

  /* Caso o sistema esteja travando quando um cliente é conectado no AP é necessário colocar 
   * um delay(100) na linha 336 do arquivo WiFiGeneric.cpp.
   * esp_err_t WiFiGenericClass::_eventCallback(void *arg, system_event_t *event, wifi_prov_event_t *prov_event)
   *   if(event->event_id < 26) {
   *       log_d("Event: %d - %s", event->event_id, system_event_names[event->event_id]);
   *       // DMD
   *       delay(100);
   *       // DMD
   *   }
   */
  WiFi.softAPConfig(apIP, apIP, netMsk);

  dnsServer.setErrorReplyCode(DNSReplyCode::NoError);
  dnsServer.start(DNS_PORT, "*", apIP);

  DEBUG_PRINTLNF("Conectando a rede anterior");
  
  WiFi.enableSTA(true);

  if ( WiFi.begin() == WL_CONNECT_FAILED) {
    DEBUG_PRINTLNF("WiFi Pre - Begin Erro");
  }
}

/*
 *
 */
bool DMD_WifiConectWait15 (const char * p_ssid, const char * p_pass, bool p_atual) {

  g_Wifi_contador = 0;

  if ( p_atual ) {
    // Espera pela conexão por 5 segundos
    for ( int i = 0; i < 10 ; i++) {
      if (WiFi.status() == WL_CONNECTED) {
        DMD_Wifi_Disable_softAP();
        DEBUG_PRINTF("Rede ESP:"); DEBUG_PRINT(WiFi.SSID());
        DEBUG_PRINTF(" Rede Gradava:"); DEBUG_PRINT(p_ssid);
        if ( strncmp( WiFi.SSID().c_str(), p_ssid, DADOS_WIFI_SSID_SIZE ) != 0 ) {
          DEBUG_PRINTF("Rede Wifi Alterada");
          break;
        } else {
          DEBUG_PRINTF("Rede Wifi OK");
          return true;
        }
      }
      delay(500);
      DEBUG_PRINTF("+");
    }
  }

  WiFi.persistent(true);
  WiFi.begin(p_ssid , p_pass);

  D_P_Wifi_F("Conectando a rede: ");
  D_P_Wifi_LN(p_ssid);

  // WiFi.begin(p_ssid , p_pass);

  // Espera pela conexão por 15 segundos
  for ( int i = 0; i < 30 ; i++) {
    if (WiFi.status() == WL_CONNECTED) {
      DMD_Wifi_Disable_softAP();
      return true;
    }
    delay(500);
    DEBUG_PRINTF(".");
  }
  return false;
}

void DMD_Wifi_AP (const char * p_ssid, const char * p_pass) {
  // Desabilita o AP Local
  // WiFi.mode(WIFI_MODE_APSTA);

  WiFi.enableSTA(false);

  WiFi.softAP(p_ssid , p_pass);
//  WiFi.softAPConfig(apIP, apIP, netMsk);
  DEBUG_PRINTF("SoftAP IP address: ");
  DEBUG_PRINTLN(WiFi.softAPIP()); 
  
}

void DMD_Wifi_Show () {
   WiFi.printDiag(Serial);
}

/*
 *
 */
void DMD_Wifi_Disable_softAP () {
  // Desabilita o AP Local
  // WiFi.mode(WIFI_STA);
  // WiFi.softAPdisconnect(true);

  WiFi.enableAP(false);
}

/*
 * Reconecta se estiver desconectado
 */
void DMD_WifiPrintStatus () {
  switch (WiFi.status()) {
    case WL_IDLE_STATUS:
      D_P_Wifi_LN_F(" WIFI - WL_IDLE_STATUS");
      break;

    case WL_NO_SSID_AVAIL:
      D_P_Wifi_LN_F(" WIFI - WL_NO_SSID_AVAIL");
      break;
    case WL_CONNECT_FAILED:
      D_P_Wifi_LN_F(" WIFI - WL_CONNECT_FAILED");
      break;
    case WL_CONNECTION_LOST:
      D_P_Wifi_LN_F(" WIFI - WL_CONNECTION_LOST");
      break;
    case WL_DISCONNECTED:
      D_P_Wifi_LN_F(" WIFI - WL_DISCONNECTED");
      break;
    case WL_CONNECTED:
      D_P_Wifi_LN_F(" WIFI - WL_CONNECTED");
      break;
    case WL_SCAN_COMPLETED:
      D_P_Wifi_LN_F(" WIFI - WL_SCAN_COMPLETED");
      break;
    default:
      D_P_Wifi_LN_F(" WIFI - default");
      break;
  }
}

/*
 * Reconecta se estiver desconectado
 * Sugerido executar a cada 1 minuto aproximadamente
 */
void DMD_WifiReconect (const char * p_ssid, const char * p_pass) {

  DMD_WifiPrintStatus();

  switch (WiFi.status()) {
    case WL_IDLE_STATUS:
      WiFi.begin(p_ssid , p_pass);
      break;

    case WL_NO_SSID_AVAIL:
    case WL_CONNECT_FAILED:
    case WL_CONNECTION_LOST:
    case WL_DISCONNECTED:
    default:
      D_P_Wifi_F(" WL reconect"); D_P_Wifi(g_Wifi_contador);
        // WiFi.begin(p_ssid , p_pass);
      // Tenta conectar a cada 5 chamadas da função
      if (g_Wifi_contador >= ( DMD_WIFI_MAX_CONTADOR + 1 ) )
          g_Wifi_contador = 0;
      
      if (g_Wifi_contador == DMD_WIFI_MAX_CONTADOR){
        D_P_Wifi_LN_F(" WL STA enable");
        WiFi.enableSTA(true);
        WiFi.begin(p_ssid , p_pass);
      } else {
        WiFi.disconnect(true);
        WiFi.enableSTA(false);

      }

      g_Wifi_contador++;
      break;

    case WL_CONNECTED:
    case WL_SCAN_COMPLETED:
    // default:
      // DMD_Wifi_Disable_softAP();
      break;
  }

/*
  if( WiFi.status() == WL_IDLE_STATUS ) {
    WiFi.begin(p_ssid , p_pass);
  }*/
}

/*
 * Returna TRUE se estiver conectado
 */
bool DMD_IsConected () {
  if( WiFi.status() == WL_CONNECTED )
    return true;
    
  return false;
}


void DMD_WiFi_Loop () {
  //DNS
  dnsServer.processNextRequest();
}

void DMD_WiFi_Disconnect () {
  WiFi.disconnect();
};

void DMD_WiFi_On(){
  // WiFi.setSleep(false);
  // WiFi.mode(WIFI_STA);

  WiFi.enableSTA(true);
}

void DMD_WiFi_Off(){
  WiFi.disconnect(true);
  WiFi.mode(WIFI_OFF);
  // WiFi.setSleep(true);
}


void DMD_WiFi_EraseConfig (){
  esp_wifi_restore();
}