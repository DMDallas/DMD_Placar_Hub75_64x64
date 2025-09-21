/*******************************************************************************
 * Autor:   Diego Dallastella
 * Projeto: Base
 * Modulo:  MQTT para Mosquito
 * 
 * Versão:  0.1
 *
 * Elementos:
 *           Placa ESP8266
 *
 * Libraries:
 *           AsyncMqttClient       lib_deps = AsyncMqttClient
 *
  ******************************************************************************/
#ifndef DMD_MQTT_H
#define DMD_MQTT_H

/*
 * Inclusao de libraries
 */
#include <AsyncMqttClient.h>

/*
 * Definições do Projeto
 */
#include "DMD_Config.h"
#include "DMD_Globais.h"


/*
 * Definições do Módulo
 */
#define DMD_MQTT_PORT 1883

#define DMD_MQTT_SUBSCRIBE_TIME 5

/*
 * Definições gerais do Modulo
 */
#ifdef DMD_DEBUG_MQTT
  #define D_P_MQTT_LN(x)    Serial.println(x)
  #define D_P_MQTT(x)       Serial.print(x)
  #define D_P_MQTT_LN_F(x)  Serial.println(F(x))
  #define D_P_MQTT_F(x)     Serial.print(F(x))
#else
  #define D_P_MQTT_LN(x)
  #define D_P_MQTT(x)
  #define D_P_MQTT_LN_F(x)
  #define D_P_MQTT_F(x)
#endif

class DMD_MQTT {
  private:
	  AsyncMqttClient c_mqtt;
    IPAddress c_Srv_ip;
    bool      c_Connected;
    String    c_Topic_Base;
    String    c_Topic_Full;
    unsigned int c_espera_subscribe;

    // Stats
    uint32_t c_NumOfConnect;
    uint32_t c_NumOfDisconnect;
    uint32_t c_NumOfTryConnect;
    uint32_t c_NumOfReceived;
    uint32_t c_NumOfSend;

    uint16_t c_ProblemCounter;

  public:

  DMD_MQTT () {
    c_Connected = false;
  };
  
  bool Init(String ) ;

  void Loop (unsigned int, unsigned int );

  void onMqttMessage(char* , char* , AsyncMqttClientMessageProperties , size_t , size_t , size_t );

  void onMqttConnect(bool sessionPresent);

  void onMqttDisconnect(AsyncMqttClientDisconnectReason reason);

  void onMqttSubscribe(uint16_t packetId, uint8_t qos);

  void onMqttUnsubscribe(uint16_t packetId);

  void onMqttPublish(uint16_t packetId);

  bool Publish(const char * , const char * );

  bool PublishFull(const char * , const char * );

  bool IsConnected() {
    return c_Connected;
  }

  void Connect() {
    D_P_MQTT_F("MQTT Connect ");
    c_NumOfTryConnect++;
    c_ProblemCounter++;
    c_mqtt.connect();
  }

  void Subscribe(const char* f_topic) {
    D_P_MQTT_F("Sub: ");  D_P_MQTT_LN ( f_topic );
    c_mqtt.subscribe(f_topic, 0);
    delay(100);
  }

  void JSON (String *);
};

extern DMD_MQTT g_MQTT;

#endif /* DMD_MQTT_H */
