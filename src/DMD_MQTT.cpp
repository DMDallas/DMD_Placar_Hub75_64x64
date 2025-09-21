/*******************************************************************************
 * Autor:   Diego Dallastella
 * Projeto: Base
 * Modulo:  MQTT para Mosquito
 * Versão:  0.9
 *
 * Elementos:
 *           Placa ESP8266
 *
 * Libraries:
 *           AsyncMqttClient       lib_deps = AsyncMqttClient
 *
  ******************************************************************************/

#include <AsyncMqttClient.h>


#include "DMD_MQTT.h"

// #include "DMD_OW_ATtiny_Int.h"
// #include "DMD_OW_ATtiny_PWM.h"
// #include "DMD_OW_Promini_Atuador.h"
// #include "DMD_TM1637.h"
#include "DMD_EEPROM.h"

DMD_MQTT g_MQTT;

bool DMD_MQTT::Init(String f_IP) {
  D_P_MQTT_LN_F("<< MQTT >>");

  c_Srv_ip.fromString(f_IP);

  c_mqtt.setServer(c_Srv_ip, DMD_MQTT_PORT);
  //c_mqtt.setCredentials(strMqttUserName.c_str(), strMqttPassword.c_str());

  c_mqtt.onConnect(std::bind(&DMD_MQTT::onMqttConnect, this, std::placeholders::_1));
  c_mqtt.onDisconnect(std::bind(&DMD_MQTT::onMqttDisconnect, this, std::placeholders::_1));
  c_mqtt.onSubscribe(std::bind(&DMD_MQTT::onMqttSubscribe, this, std::placeholders::_1, std::placeholders::_2));
  c_mqtt.onUnsubscribe(std::bind(&DMD_MQTT::onMqttUnsubscribe, this, std::placeholders::_1));
  c_mqtt.onMessage(std::bind(&DMD_MQTT::onMqttMessage, this, std::placeholders::_1,std::placeholders::_2, std::placeholders::_3, std::placeholders::_4, std::placeholders::_5, std::placeholders::_6));
  c_mqtt.onPublish(std::bind(&DMD_MQTT::onMqttPublish, this, std::placeholders::_1));

  c_NumOfConnect = 0;
  c_NumOfDisconnect = 0;
  c_NumOfTryConnect = 0;
  c_NumOfReceived = 0;
  c_NumOfSend = 0;
  
  c_ProblemCounter = 0;

  Connect();

  c_espera_subscribe = DMD_MQTT_SUBSCRIBE_TIME + 1;

  D_P_MQTT_LN_F("<<      >>");
  return true;
}


void DMD_MQTT::Loop (unsigned int f_contador_seg, unsigned int f_num_exe = 7){

  if ( c_espera_subscribe <= DMD_MQTT_SUBSCRIBE_TIME ){
    // Aguardando resposta do servidor
    D_P_MQTT_F("Aguardando Subscribe Ack - ");
    if ( c_espera_subscribe == 0 ) {
      D_P_MQTT_LN_F("Timeout");
      // Timeout da respostado servidor
      c_mqtt.disconnect(false);
      // yield;  delay(100);  yield;  delay(100); yield;  delay(100);
      // Connect();
      c_espera_subscribe = DMD_MQTT_SUBSCRIBE_TIME + 1;
    } else {
      D_P_MQTT_LN(c_espera_subscribe);
      c_espera_subscribe--;
    }
  }

  if( ( f_contador_seg & 0xF ) == f_num_exe){ // a cada 16 segundos
    if ( !IsConnected() ) {
      D_P_MQTT_LN_F("MQTT OFF");

      Connect();

      // Em torno de 30 minutos sem conectar ao servidor
      if ( c_ProblemCounter > 120 ) {
        D_P_MQTT_F("Muitos erros de conexão seguidos. Irá resetar!");
        delay(100); yield(); delay(100);
        ESP.restart();
      }
    }
  }
}


void DMD_MQTT::onMqttConnect(bool sessionPresent) {
  String l_temp;
  l_temp.reserve(250);

  D_P_MQTT_LN_F("onMQTTConnect");

  c_NumOfConnect++;
  c_Connected = true;

  c_ProblemCounter = 0;

  // D_P_MQTT_F("Session present: ");
  // D_P_MQTT_LN(sessionPresent);

  c_Topic_Base = String(DMD_MQTT_TOPIC_BASE);

  c_Topic_Full = String(DMD_MQTT_TOPIC_BASE) + String(g_Dados.Type) + "/";
  c_Topic_Full += String(g_Dados.Name) + "/";

  D_P_MQTT_F("Base Topic ");
  D_P_MQTT_LN ( c_Topic_Base );
  
  D_P_MQTT_F("Full Topic ");
  D_P_MQTT_LN ( c_Topic_Full );

  Subscribe(c_Topic_Full.c_str());

  // if ( g_Relogio.IsOn() ) {
  //   l_temp = c_Topic_Base + String(TOPIC_TEMP_EXT);
  //   Subscribe(l_temp.c_str());
  // }

  // if ( g_Beer.IsOn() ) {
  //   l_temp = c_Topic_Full + String(DMD_MQTT_BEER_FERM_PRESET);
  //   Subscribe(l_temp.c_str());

  //   l_temp = c_Topic_Full + String(DMD_MQTT_BEER_FERM_FAST);
  //   Subscribe(l_temp.c_str());
  // }

  // for ( int i = 0 ; i < g_ATTiny_Int.getDeviceCount() ; i++ ) {
  //   if ( g_Dados.ATT_SlaveInt_Cfg[i].Topico_Int[0][0] != ':' ) {
  //     l_temp = c_Topic_Base + g_Dados.ATT_SlaveInt_Cfg[i].Topico_Int[0];
  //     Subscribe(l_temp.c_str());
  //   }

  //   if ( g_Dados.ATT_SlaveInt_Cfg[i].Topico_Int[1][0] != ':' ) {
  //     l_temp = c_Topic_Base + g_Dados.ATT_SlaveInt_Cfg[i].Topico_Int[1];
  //     Subscribe(l_temp.c_str());
  //   }

  //   if ( g_Dados.ATT_SlaveInt_Cfg[i].Topico_Int[2][0] != ':' ) {
  //     l_temp = c_Topic_Base + g_Dados.ATT_SlaveInt_Cfg[i].Topico_Int[2];
  //     Subscribe(l_temp.c_str());
  //   }
  // }

  // int l_total;
  // if ( g_ProMini_Atuador.GetDevStatus() )
  //   l_total = ProMini_MAX_ATUADOR;
  // else
  //   l_total = g_ATTiny_Atuador.getDeviceCount() * ATTiny_MAX_ATUADOR;

  // for ( int i = 0 ; i < l_total ; i++ ) {
  //   l_temp = c_Topic_Base + g_Dados.Name + "S" + String(i);
  //   Subscribe(l_temp.c_str());
  // }

  // if ( g_ProMini_Atuador.GetDevStatus() ) {
  //   for ( int i = 0 ; i < ProMini_MAX_ATUADOR ; i++ ) {
  //     l_temp = c_Topic_Full + TOPIC_ATUADOR + String(i) + "/";
  //     Subscribe(l_temp.c_str());
  //   }
  // } else {
  //   for ( int i = 0 ; i < g_ATTiny_Atuador.getDeviceCount() ; i++ ) {
  //     l_temp = c_Topic_Full + TOPIC_ATUADOR + String(i*3) + "/";
  //     Subscribe(l_temp.c_str());

  //     l_temp = c_Topic_Full + TOPIC_ATUADOR + String(i*3 + 1) + "/";
  //     Subscribe(l_temp.c_str());

  //     l_temp = c_Topic_Full + TOPIC_ATUADOR + String(i*3 + 2) + "/";
  //     Subscribe(l_temp.c_str());
  //   }
  // }

  // if ( g_ProMini_Atuador.GetDevStatus() )
  //   l_total = ProMini_MAX_PWM;
  // else
  //   l_total = g_ATTiny_PWM.getDeviceCount() * ATTiny_MAX_PWM;
    
  // for ( int i = 0 ; i < l_total ; i++ ) {
  //   l_temp = c_Topic_Base + g_Dados.Name + "P" + String(i);
  //   Subscribe(l_temp.c_str());
  // }

  c_espera_subscribe = DMD_MQTT_SUBSCRIBE_TIME;
}

void DMD_MQTT::onMqttDisconnect(AsyncMqttClientDisconnectReason reason) {
  D_P_MQTT_F("Disconnected from MQTT - "); D_P_MQTT_LN((uint8_t)reason);

  c_NumOfDisconnect++;
  c_Connected = false;
}

void DMD_MQTT::onMqttSubscribe(uint16_t packetId, uint8_t qos) {
  D_P_MQTT_F("Sub. ack pktId: "); D_P_MQTT_LN(packetId);
  // D_P_MQTT_F(" qos: ");      D_P_MQTT_LN(qos);

  c_espera_subscribe = DMD_MQTT_SUBSCRIBE_TIME + 1;

  if ( packetId == 1 ) {
    String l_msg = "";
    l_msg += "{\"IP\":\"" + WiFi.localIP().toString() + "\",";
    // switch (WiFi.status()) {
    //   case 0: l_msg += "\"WiFi\": \"Alterando\","; break;
    //   case 1: l_msg += "\"WiFi\": \"Rede não encontrada\","; break;
    //   case 3: l_msg += "\"WiFi\": \"Conectado\","; break;
    //   case 4: l_msg += "\"WiFi\": \"Senha incorreta\","; break;
    //   case 6: l_msg += "\"WiFi\": \"Modo Desligado\","; break;
    //   default: l_msg += "\"WiFi\": \"Indefinido\","; break;
    // }
    l_msg += "\"Server IP\":\"" + String(g_Dados.MQTT_Srv_IP) + "\",";
    if (IsConnected() )
      l_msg += "\"Status\":\"Conectado\",";
    else
      l_msg += "\"Status\":\"Desconectado\",";
    l_msg += "\"Con/Dis/Try\":\"" + String(c_NumOfConnect) + "/" + String(c_NumOfDisconnect) + "/" + String(c_NumOfTryConnect) + "\",";
    l_msg += "\"Leit./Env\":\"" + String(c_NumOfReceived) + "/" + String(c_NumOfSend) + "\",";
    l_msg += "\"Contadores Segundos/Loops\":\"" + String(g_contador_seg) + "/" + String(g_contador_loop) + "\",";
    l_msg += "\"Heap\":" + String(ESP.getFreeHeap()) + "}";

    PublishFull("Status", l_msg.c_str());
  }
}

void DMD_MQTT::onMqttUnsubscribe(uint16_t packetId) {
  D_P_MQTT_F("Unsub. ack pktId: ");
  D_P_MQTT_LN(packetId);
}

void DMD_MQTT::onMqttPublish(uint16_t packetId) {
  D_P_MQTT_F("Publish ack pktId: ");
  D_P_MQTT_LN(packetId);
}


void DMD_MQTT::onMqttMessage(char* topic, char* payload, AsyncMqttClientMessageProperties properties, size_t len, size_t index, size_t total) {
  D_P_MQTT_F("\nMsg topic:");   D_P_MQTT_LN(topic);
  D_P_MQTT_F("Msg:");   D_P_MQTT_LN(payload);

  c_NumOfReceived++;

  // D_P_MQTT_F("  qos: ");      D_P_MQTT(properties.qos);
  // D_P_MQTT_F("  dup: ");      D_P_MQTT(properties.dup);
  // D_P_MQTT_F("  retain: ");   D_P_MQTT(properties.retain);
  // D_P_MQTT_F("  len: ");      D_P_MQTT(len);
  // D_P_MQTT_F("  index: ");    D_P_MQTT(index);
  // D_P_MQTT_F("  total: ");    D_P_MQTT_LN(total);

  if ( strlen(topic) == c_Topic_Full.length() ) {
    // Recebido no tópico principal
    if ( (payload[0] == 'R') && (payload[1] == 'E') ) {
      // payload == RE - Reset
      PublishFull("Status", "Reset Ok");
      delay(200); yield(); delay(200); yield(); delay(200); yield(); delay(200); yield();
      ESP.restart();
    }
    if ( (payload[0] == 'S') && (payload[1] == 'T') ) {
      // payload == RE - Reset
      String l_msg = "";
      l_msg += "{\"IP\":\"" + WiFi.localIP().toString() + "\",";
      switch (WiFi.status()) {
        case 0: l_msg += "\"WiFi\": \"Alterando\","; break;
        case 1: l_msg += "\"WiFi\": \"Rede não encontrada\","; break;
        case 3: l_msg += "\"WiFi\": \"Conectado\","; break;
        case 4: l_msg += "\"WiFi\": \"Senha incorreta\","; break;
        case 6: l_msg += "\"WiFi\": \"Modo Desligado\","; break;
        default: l_msg += "\"WiFi\": \"Indefinido\","; break;
      }
      l_msg += "\"Server IP\":\"" + String(g_Dados.MQTT_Srv_IP) + "\",";
      if (IsConnected() )
        l_msg += "\"Status\":\"Conectado\",";
      else
        l_msg += "\"Status\":\"Desconectado\",";
      l_msg += "\"Con/Dis\":\"" + String(c_NumOfConnect) + "/" + String(c_NumOfDisconnect) + "\",";
      l_msg += "\"Leit./Env\":\"" + String(c_NumOfReceived) + "/" + String(c_NumOfSend) + "\",";
      l_msg += "\"Contadores Segundos/Loops\":\"" + String(g_contador_seg) + "/" + String(g_contador_loop) + "\",";
      l_msg += "\"Heap\":" + String(ESP.getFreeHeap()) + "}";

      PublishFull("Status", l_msg.c_str());
    }
  }

  if ( strlen(topic) > c_Topic_Full.length() ) {

    char * l_SubTopic = &topic[c_Topic_Full.length()];
    D_P_MQTT_F("Msg Sub topic:");   D_P_MQTT(l_SubTopic);

    // if ( strncmp(l_SubTopic,DMD_MQTT_BEER_FERM_PRESET,DMD_MQTT_BEER_FERM_PRESET_size) == 0 ) {

    //   uint16_t l_preset = 0;
    //   if ( (payload[0] == '0') && (payload[1] == '.') ){
    //     // Formato 0.XX
    //     if ( (payload[2] >= '0') && (payload[2] <= '9') && 
    //          (payload[3] >= '0') && (payload[3] <= '9') ) {
    //       l_preset = (char(payload[2]) - '0')*10 + (char(payload[3]) - '0');
    //     } else {
    //       l_preset = 255;
    //     }
    //   } else {
    //     // Formato XX
    //     for (size_t i = 0; i < len; i++) {
    //       if ( (payload[i] < '0') || (payload[i] > '9') ){
    //         break;
    //       }
    //       l_preset = l_preset*10 + (char(payload[i]) - '0');
    //     }
    //   }
    //   D_P_MQTT_F("l_preset:");   D_P_MQTT(l_preset);
  
    //   if ( l_preset < 100) {
    //     g_Beer.Set_Temp_Preset(l_preset);

    //     DMD_EEPROM_Save((char *)&g_Dados, sizeof(g_Dados));
    //   } else {
    //     D_P_MQTT_F("l_preset: erro");
    //   }
    // }

    // if ( strncmp(l_SubTopic,DMD_MQTT_BEER_FERM_FAST,DMD_MQTT_BEER_FERM_FAST_size) == 0 ) {
    //   D_P_MQTT_F(" - Fast:");

    //   if (payload[0] == 'O') {
    //     if (payload[1] == 'N')  {
    //       D_P_MQTT_F("On ");
    //       // payload == ON - Ativação
    //       g_Beer.FastCool_On();
    //     } else {
    //       D_P_MQTT_F("Off ");
    //       g_Beer.FastCool_Off();
    //     }
    //   }  
    // }
  //   if ( strncmp(l_SubTopic,TOPIC_ATUADOR,TOPIC_ATUADOR_size) == 0 ) {
  //     char l_Int = topic[c_Topic_Full.length()+TOPIC_ATUADOR_size];
  //     uint8_t l_Canal = l_Int - '0';
  //     //D_P_MQTT_F(" - Int: ");   D_P_MQTT_LN(l_Int);
  //     D_P_MQTT_F(" - Canal: ");   D_P_MQTT_LN(l_Canal);

  //     if ( (payload[0] == 'O') && (payload[1] == 'N') ) {
  //       // payload == ON - Ativação
  //       g_ATTiny_Atuador.SetOn( l_Canal / ATTiny_MAX_ATUADOR , l_Canal % ATTiny_MAX_ATUADOR );
  //       g_ProMini_Atuador.SetOn(l_Canal);
  //     } else {
  //       g_ATTiny_Atuador.SetOff( l_Canal / ATTiny_MAX_ATUADOR , l_Canal % ATTiny_MAX_ATUADOR );
  //       g_ProMini_Atuador.SetOff(l_Canal);
  //     }
  //   }

  //   if ( strncmp(l_SubTopic,TOPIC_PWM,TOPIC_PWM_size) == 0 ) {
  //     char l_Int = topic[c_Topic_Full.length()+TOPIC_PWM_size];
  //     uint8_t l_Canal = l_Int - '0';
  //     D_P_MQTT_F(" - PWM: ");   D_P_MQTT(l_Canal);
  //     D_P_MQTT_F(" - Len: ");   D_P_MQTT(len);
  //     uint16_t l_pwm = 0;
  //     for (size_t i = 0; i < len; i++) {
  //       if ( (payload[i] < '0') || (payload[i] > '9') ){
  //         break;
  //       }
  //       l_pwm = l_pwm*10 + (char(payload[i]) - '0');
  //     }
      
  //     l_pwm = l_pwm * 2.56;
  //     if ( l_pwm >= 254 )
  //       l_pwm = 255;

  //     D_P_MQTT_F(" - l_pwm: ");   D_P_MQTT(l_pwm);
  //     g_ATTiny_PWM.SetPWM(l_Canal/ATTiny_MAX_PWM, l_Canal%ATTiny_MAX_PWM, l_pwm);
  //     g_ProMini_Atuador.SetPWM(l_Canal, l_pwm);
  //   }


  }

  char * l_SubTopic = &topic[c_Topic_Base.length()];
  D_P_MQTT_F("Msg Sub topic:");   D_P_MQTT(l_SubTopic);

  // for (int l_Slave = 0 ; l_Slave < ATTiny_INT_MAX_SLAVES ; l_Slave++) {
  //   for ( int i = 0 ; i < ATTiny_MAX_INT; i++) {
  //     if ( strncmp(l_SubTopic,g_Dados.ATT_SlaveInt_Cfg[l_Slave].Topico_Int[i], DADOS_INT_TOPIC) == 0 ) {
  //       D_P_MQTT_F(" Topic Int - Slave: ");  D_P_MQTT(l_Slave);
  //       D_P_MQTT_F(" Int: ");  D_P_MQTT_LN(i);

  //       if ( g_Int_Msg_Origem[l_Slave][i] == 0 ) {
  //         if (payload[0] == 'O') {
  //           if (payload[1] == 'N')  {
  //             // payload == ON - Ativação
  //             SaidaOn(l_Slave,i);
  //             //g_ATTiny_Int.SetLedOn(l_Canal/3,l_Canal%3);
  //           } else {
  //             SaidaOff(l_Slave,i);
  //             // g_ATTiny_Int.SetLedOff(l_Canal/3,l_Canal%3);
  //           }
  //         } else {
  //           D_P_MQTT_F(" - Len: ");   D_P_MQTT(len);
  //           uint16_t l_pwm = 0;

  //           for (size_t i = 0; i < len; i++) {
  //             if ( (payload[i] < '0') || (payload[i] > '9') ){
  //               break;
  //             }
  //             l_pwm = l_pwm*10 + (char(payload[i]) - '0');
  //           }
            
  //           l_pwm = l_pwm * 2.56;
  //           if ( l_pwm >= 254 )
  //             l_pwm = 255;

  //           D_P_MQTT_F(" - l_pwm: ");   D_P_MQTT(l_pwm);
  //           SaidaPWM(l_Slave ,i , l_pwm);
  //         }
  //       } else {
  //         // Msg enviada por esta propria placa
  //         D_P_MQTT_F(" Origem do comando");
  //         g_Int_Msg_Origem[l_Slave][i] = 0;

  //       }
  //     }
  //   }
  // }

  String l_temp;

  int l_total;

  // if ( g_ProMini_Atuador.GetDevStatus() )
  //   l_total = ProMini_MAX_ATUADOR;
  // else
  //   l_total = g_ATTiny_Atuador.getDeviceCount() * ATTiny_MAX_ATUADOR;

  // for ( int i = 0 ; i < l_total ; i++ ) {
  //   l_temp = String(g_Dados.Name) + "S" + String(i);
  //   if ( strncmp(l_SubTopic,l_temp.c_str(),12) == 0 ) {
  //     uint8_t l_Canal = i;
  //     D_P_MQTT_F(" - Saida: ");   D_P_MQTT_LN(l_Canal);

  //     if ( (payload[0] == 'O') && (payload[1] == 'N') ) {
  //       // payload == ON - Ativação
  //       g_ATTiny_Atuador.SetOn( l_Canal / ATTiny_MAX_ATUADOR , l_Canal % ATTiny_MAX_ATUADOR );
  //       g_ProMini_Atuador.SetOn(l_Canal);
  //     } else {
  //       g_ATTiny_Atuador.SetOff( l_Canal / ATTiny_MAX_ATUADOR , l_Canal % ATTiny_MAX_ATUADOR );
  //       g_ProMini_Atuador.SetOff(l_Canal);
  //     }
  //   }
  // }

  // if ( g_ProMini_Atuador.GetDevStatus() )
  //   l_total = ProMini_MAX_PWM;
  // else
  //   l_total = g_ATTiny_PWM.getDeviceCount() * ATTiny_MAX_PWM;

  // for ( int i = 0 ; i < l_total ; i++ ) {
  //   l_temp = String(g_Dados.Name) + "P" + String(i);
  //   if ( strncmp(l_SubTopic,l_temp.c_str(),12) == 0 ) {
  //     uint8_t l_Canal = i;
  //     D_P_MQTT_F(" - PWM: ");   D_P_MQTT(l_Canal);
      
  //     if ( payload[0] == 'O') {
  //       uint8_t l_pwm;
  //       if ( payload[1] == 'N' ) {
  //         // l_pwm = ProMini_PWM_MAX_VALOR;
  //         l_pwm = ProMini_PWM_MAX_VALOR;
  //         if ( l_Canal < ATTiny_MAX_PWM_SISTEMA ) {
  //           if ( g_Dados.PWM_Max[l_Canal] > 0 )
  //             l_pwm = g_Dados.PWM_Max[l_Canal];
  //         } 
  //       }
  //       else
  //         l_pwm = 0;
  //       D_P_MQTT_F(" - l_pwm: ");   D_P_MQTT(l_pwm);
  //       g_ATTiny_PWM.SetPWM(l_Canal/ATTiny_MAX_PWM, l_Canal%ATTiny_MAX_PWM, l_pwm);
  //       g_ProMini_Atuador.SetPWM(l_Canal, l_pwm);

  //     } else {
  //       D_P_MQTT_F(" - Len: ");   D_P_MQTT(len);
  //       uint16_t l_pwm = 0;

  //       for (size_t i = 0; i < len; i++) {
  //         if ( (payload[i] < '0') || (payload[i] > '9') ){
  //           break;
  //         }
  //         l_pwm = l_pwm*10 + (char(payload[i]) - '0');
  //       }
        
  //       l_pwm = l_pwm * 2.56;
  //       if ( l_pwm >= 254 )
  //         l_pwm = 255;

  //       D_P_MQTT_F(" - l_pwm: ");   D_P_MQTT(l_pwm);
  //       g_ATTiny_PWM.SetPWM(l_Canal/ATTiny_MAX_PWM, l_Canal%ATTiny_MAX_PWM, l_pwm);
  //       g_ProMini_Atuador.SetPWM(l_Canal, l_pwm);
  //     }
  //   }
  // }

  // if ( g_Relogio.IsOn() ) {
  //   if ( strncmp(l_SubTopic,TOPIC_TEMP_EXT,12) == 0 ) {
  //     D_P_MQTT_F(" - TOPIC_TEMP_EXT: ");
  //     D_P_MQTT_F(" - Len: ");   D_P_MQTT(len);
  //     uint16_t l_temp = 0;

  //     for (size_t i = 0; i < len; i++) {
  //       if ( (payload[i] < '0') || (payload[i] > '9') ){
  //         break;
  //       }
  //       l_temp = l_temp*10 + (char(payload[i]) - '0');
  //     }
  //     D_P_MQTT_F(" - Temp: ");   D_P_MQTT_LN(l_temp);
  //     g_Relogio.SetTemp(1,l_temp);
  //   }
  // }

}

bool DMD_MQTT::Publish(const char * f_Sub_Topic, const char * f_msg){
  c_NumOfSend++;

  String l_Topic = c_Topic_Base + f_Sub_Topic;
  D_P_MQTT("Topic: "); D_P_MQTT_LN(l_Topic.c_str());
  D_P_MQTT("Msg: ");   D_P_MQTT_LN(f_msg);
  return c_mqtt.publish  (l_Topic.c_str(), 0, false, f_msg);
}

bool DMD_MQTT::PublishFull(const char * f_Sub_Topic, const char * f_msg){
  c_NumOfSend++;

  String l_Topic = c_Topic_Full + f_Sub_Topic;
  D_P_MQTT("Topic: "); D_P_MQTT_LN(l_Topic.c_str());
  D_P_MQTT("Msg: ");   D_P_MQTT_LN(f_msg);
  return c_mqtt.publish  (l_Topic.c_str(), 0, false, f_msg);
}

/*
 * Insere na String recebida as estatísticas no padrão JSON
 */
void DMD_MQTT::JSON (String *msg){
  *msg += "{\"Tipo\":\"MQTT\",";
  *msg += "\"Res\":\"Ok\",";
  *msg += "\"Server IP\":\"" + String(g_Dados.MQTT_Srv_IP) + "\",";
  if (IsConnected() )
    *msg += "\"Status\":\"Conectado\",";
  else
    *msg += "\"Status\":\"Desconectado\",";
  *msg += "\"Connect\":" + String(c_NumOfConnect) + ",";
  *msg += "\"Disconnect\":" + String(c_NumOfDisconnect) + ",";
  *msg += "\"TryConnect\":" + String(c_NumOfTryConnect) + ",";
  *msg += "\"Leituras\":" + String(c_NumOfReceived) + ",";
  *msg += "\"Envios\":" + String(c_NumOfSend) + "}";
}