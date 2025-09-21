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
/*
 * Definições do Projeto
 */
#include "DMD_Config.h"

/*
 * Inclusao de libraries
 */
// #include <ESP8266WebServer.h> Incluida no .h para que sejam replicadas definições - incluir somente o DMD_*.H
// #include <ESP8266WebServer.h>
#include <ArduinoJson.h>

/*
 * Inclusao funções e dados do projeto
 */
#include "DMD_EEPROM.h"
#include "DMD_WiFi.h"
#include "DMD_Stat.h"
#include "DMD_Globais.h"
#include "DMD_WebServer.h"
#include "DMD_WebServer_JSON.h"
// #include "DMD_HTTP_Update.h"
// #include "DMD_OW_ATtiny_Int.h"
#include "DMD_MQTT.h"
// #include "DMD_TM1637.h"

// #include "DMD_OW_ATtiny_Medidor.h"
// #include "DMD_OW_DS18B20.h"


/*
 * Ajax JSON - Inicializa as funções
 *  Registra a funções na classe Server
 */
void DMD_WebServer_Init_JSON(){
  // Respostas JSON
  server.on("/json_echo", f_json_echo);
  server.on("/json_Get_Time", f_json_Get_Time);

  server.on("/json_wifi", f_json_SetAP);
  server.on("/json_wifi_detect", f_json_wifi_detect);

  server.on("/json_config", f_json_Config);
  // server.on("/json_config_rem", f_json_Config_Remove);
  // server.on("/json_config_int", f_json_Config_Int);
  // server.on("/json_config_int_topic", f_json_Config_Int_Topic);
  server.on("/json_config_nome", f_json_Config_Nome);
  server.on("/json_config_ip", f_json_Config_Ip);
  // server.on("/json_cmd", f_json_CMD);

  // server.on("/json_config_int_color_sch", f_json_Config_Int_Color);

  // server.on("/json_config_debounce", f_json_Config_Debounce);
  // server.on("/json_config_pwm", f_json_Config_PWM);

  // server.on("/json_config_medidor", f_json_Config_Medidor);

  server.on("/json_config_local_pw", f_json_Config_LocalPW);

  // server.on("/json_config_topic", f_json_Config_Topic);

  // server.on("/json_config_topico_temp", f_json_Config_Topic_Temp);

  // server.on("/json_config_relogio", f_json_Config_Relogio);
  
  // server.on("/json_config_aquec_solar", f_json_Config_AqueSolar);  

  // server.on("/json_config_beer_ferm", f_json_Config_Beer_Ferm);  

  // server.on("/json_rf_topico",      f_json_RF_Topic);
  // server.on("/json_rf_del",         f_json_RF_Del);
  // server.on("/json_rf_add_smw210",  f_json_RF_Add_SMW210);

  // if ( Check_NS() ) {
  //   server.on("/json_config_ns", f_json_Config_NS);
  // }

  server.on("/json_reset", f_json_reset);

  //Estatisticas
  server.on("/json_Stat", f_json_stat);
}

/*
 * Ajax JSON - Responde com o dados recebido
 */
void f_json_echo (){
  DEBUG_PRINTF("WJ - echo - ");
  String msg = "{ \"Status\":\"Recedibo\"";
  for (uint8_t i=0; i<server.args(); i++){
    DEBUG_PRINTLN(" Arg -> "+ server.argName(i) + ":"+ server.arg(i));
    msg += "\",\"" + server.argName(i) + "\":"+ server.arg(i);
  }
  msg += "}";
  server.send(200, "application/json", msg);
  g_Stat.JSON_OK++;
}

/*
 * Ajax JSON - Envia dados de Umidade do Solo
 */
void f_json_Get_Time (){
  DEBUG_PRINTF("WJ - Get Time");

  String msg = "{\"Contador\":\""+ String(g_contador_seg) + "\"}";

  server.send(200, "application/json", msg);
  g_Stat.JSON_OK++;
}

/*
 * Ajax JSON - Configura WiFi
 */
void f_json_SetAP (){
  DEBUG_PRINTF("WJ - Set AP - ");

  bool c_ok = false;
  
  String msg = "{\"Status\":";

  //for (uint8_t i=0; i<server.args(); i++){
  if( server.args() == 1 ) {
    int i = 0;
    DEBUG_PRINTLN(" Arg -> "+ server.argName(i) + " : "+ server.arg(i));
    if(server.argName(i) == "plain") {
      DynamicJsonDocument root(200);
      DeserializationError error = deserializeJson( root, server.arg(i));
      if (error) {
        msg += "\"Formato dos dados incorretos.\""; DEBUG_PRINTLN("Erro Parsing");
        return;
      }
      DEBUG_PRINTLNF("JSON parsing OK!");
      if (  measureJson(root["c_R"])>3 ){
        if (measureJson(root["c_R_PW"])>9){
          strncpy(g_Dados.WIFI_SSID, root["c_R"],DADOS_WIFI_SSID_SIZE);
          strncpy(g_Dados.WIFI_PW, root["c_R_PW"],DADOS_WIFI_PW_SIZE);
          DEBUG_PRINTF(" Nova rede: "); DEBUG_PRINTLN(g_Dados.WIFI_SSID);
          DEBUG_PRINTF(" Novo Pass: "); DEBUG_PRINTLN(g_Dados.WIFI_PW);
          msg += "\"Rede "+String(g_Dados.WIFI_SSID)+" gravada com sucesso.\""; DEBUG_PRINTLNF("OK");
          c_ok = true;
        } else{
          msg += "\"Senha muito curta.\""; DEBUG_PRINTLNF("Erro PW");
        }
      } else {
        msg += "\"Nome da rede muito curta.\""; DEBUG_PRINTLNF("Erro Rede");
      }
    } else {
        msg += "\"Formato dos dados incorretos!\""; DEBUG_PRINTLNF("Erro dado");
    }
  } else {
        msg += "\"Dados enviados incorretos.\""; DEBUG_PRINTLNF("Erro json");
  }

  if ( c_ok ) {
    msg += ",\"Res\":\"Ok\"";
    g_Stat.JSON_OK++;
    DEBUG_PRINTLNF("OK");
  }
  else {
    msg += ",\"Res\":\"Erro\"";
    DEBUG_PRINTLNF("Erro");
    g_Stat.JSON_Erro++;
  }

  msg += "}";

  DEBUG_PRINTLN(msg);

  server.send(200, "application/json", msg);

  if ( c_ok ) {
    DMD_EEPROM_Save((char *)&g_Dados, sizeof(g_Dados));
  }
}


/*
 * Ajax JSON - Envia as valores das estatísticas
 */
void f_json_stat() {
  DEBUG_PRINTLN("WJ - Stat");

  String msg;
  msg += "{ \"Nome\":\""+ String(g_Dados.Name);
  msg += "\",\"Versão\":\""+ String(VERSION_FW)+" ("+ String(VERSION_DATA)+")\"";
  msg += "\",\"FreeHeap\":\""+ String(ESP.getFreeHeap()) + ")\"";
  msg += ",\"MQTT\":"; g_MQTT.JSON(&msg);
  // msg += ",\"ATtiny\":"; g_ATTiny_Slave.JSON(&msg);
  // msg += ",\"ATtinyInt\":"; g_ATTiny_Int.JSON(&msg);
  // msg += ",\"Promini\":"; g_ProMini_Atuador.JSON(&msg);
  msg += ",\"Stats\":";  g_Stat.JSON(&msg);
  msg += "}";
  server.send(200, "application/json", msg);
  g_Stat.JSON_OK++;
}

void f_json_wifi_detect(){
  DEBUG_PRINT("WJ - Wifi");

  int n = WiFi.scanNetworks();
  
  String msg = "{\"Total\":"+ String(n);
  if ( n > 0 ) {
    msg += ",\"Redes\":[{\"Nome\":\""+ WiFi.SSID(0) +"\",\"Sinal\":\""+ String(WiFi.RSSI(0))+"\"";
    for (int i = 1; i < n; ++i) {
      msg += "},{\"Nome\":\""+ WiFi.SSID(i) +"\",\"Sinal\":\""+ String(WiFi.RSSI(i))+"\"";
    }
    msg += "}]";
  }

  msg += "}";

  DEBUG_PRINT(msg);

  server.send(200, "application/json", msg);
  g_Stat.JSON_OK++;
}

/*
 * Ajax JSON - Configura 
 */
void f_json_Config (){
  DEBUG_PRINT("WJ - Config - ");

  bool c_ok = false;
  
  String msg = "{\"Status\":";

  //for (uint8_t i=0; i<server.args(); i++){
  if( server.args() == 1 ) {
    int i = 0;
    DEBUG_PRINTLN(" Arg -> "+ server.argName(i) + " : "+ server.arg(i));
    if(server.argName(i) == "plain") {
      DynamicJsonDocument root(300);
      DeserializationError error = deserializeJson( root, server.arg(i));
      if (error) {
        msg += "\"Formato dos dados incorretos.\""; DEBUG_PRINTLN("Erro Parsing");
        return;
      }
      DEBUG_PRINTLN("JSON parsing OK!");
      if (  measureJson(root["c_R"])>3 ){
        if (measureJson(root["c_R_PW"])>9){
          if (measureJson(root["c_w_Em"])>9){
            if (measureJson(root["c_w_N"])>3){
              strncpy(g_Dados.WIFI_SSID, root["c_R"],DADOS_WIFI_SSID_SIZE);
              strncpy(g_Dados.WIFI_PW, root["c_R_PW"],DADOS_WIFI_PW_SIZE);
              DEBUG_PRINT(" Nova rede: "); DEBUG_PRINTLN(g_Dados.WIFI_SSID);
              DEBUG_PRINT(" Novo Pass: "); DEBUG_PRINTLN(g_Dados.WIFI_PW);
              msg += "\"Rede "+String(g_Dados.WIFI_SSID)+" gravada com sucesso.\""; DEBUG_PRINTLN("OK");

              strncpy(g_Dados.Name, root["c_w_N"],DADOS_NAME_SIZE);
              // strcpy(g_Dados.Latitude, root["c_w_La"]);
              // strcpy(g_Dados.Longitude, root["c_w_Lo"]);
              // strcpy(g_Dados.Email, root["c_w_Em"]);
              //g_Dados.relay_counter[0] = root["c_w_T"];
              // DEBUG_PRINT(" Latitude: "); DEBUG_PRINTLN(g_Dados.Latitude);
              // DEBUG_PRINT(" Logitude: "); DEBUG_PRINTLN(g_Dados.Longitude);            
              // DEBUG_PRINT(" Email: "); DEBUG_PRINTLN(g_Dados.Email);

              c_ok = true;
            } else {
              msg += "\"Nome muito curto.\""; DEBUG_PRINTLN("Erro Nome");
            }
          } else {
            msg += "\"Email muito curto.\""; DEBUG_PRINTLN("Erro Email");
          }
        } else{
          msg += "\"Senha muito curta.\""; DEBUG_PRINTLN("Erro PW");
        }
      } else {
        msg += "\"Nome da rede muito curta.\""; DEBUG_PRINTLN("Erro Rede");
      }
    } else {
        msg += "\"Formato dos dados incorretos!\""; DEBUG_PRINTLN("Erro dado");
    }
  } else {
        msg += "\"Dados enviados incorretos.\""; DEBUG_PRINTLN("Erro json");
  }

  if ( c_ok ) {
    msg += ",\"Res\":\"Ok\"";
    g_Stat.JSON_OK++;
    DEBUG_PRINTLN("OK");
  }
  else {
    msg += ",\"Res\":\"Erro\"";
    DEBUG_PRINTLN("Erro");
    g_Stat.JSON_Erro++;
  }

  msg += "}";

  DEBUG_PRINTLN(msg);

  server.send(200, "application/json", msg);

  if ( c_ok ) {
    DMD_EEPROM_Save((char *)&g_Dados, sizeof(g_Dados));
    //DMD_WifiConectWait15(g_Dados.AP_ssid , g_Dados.AP_pass);
  }
}

/*
 * Ajax JSON - Configura tempo dos Reles
 */
void f_json_Config_Remove () {
  DEBUG_PRINT("WJ - Config Remove - ");

  bool c_ok = false;
  int l_tipo , l_rem;
  
  String msg = "{\"Status\":\"";

  //for (uint8_t i=0; i<server.args(); i++){
  if( server.args() == 1 ) {
    int i = 0;
    DEBUG_PRINTLN(" Arg -> "+ server.argName(i) + " : "+ server.arg(i));
    if(server.argName(i) == "plain") {
      DynamicJsonDocument root(150);
      DeserializationError error = deserializeJson( root, server.arg(i));
      if (error) {
        msg += "\"Formato dos dados incorretos.\""; DEBUG_PRINTLN("Erro Parsing");
        return;
      }
      DEBUG_PRINTLN("JSON parsing OK!");
      if ( root.containsKey("c_Tipo") && root.containsKey("c_REM") ){
        l_tipo = root["c_Tipo"];
        l_rem =  root["c_REM"];
        switch (l_tipo) {
          // case 0:   // Interruptor
          //   msg += "Interruptor " + String(l_rem) + " removido.";
          //   DEBUG_PRINT("Apagar dados interruptor: ");DEBUG_PRINTLN(l_rem);
          //   g_ATTiny_Int.RemoveSlave(l_rem);
          //   break;
          // case 1:   // Atuador
          //   msg += "Atuador " + String(l_rem) + " removido.";
          //   DEBUG_PRINT("Apagar dados Atuador: ");    DEBUG_PRINTLN(l_rem);
          //   g_ATTiny_Atuador.RemoveSlave(l_rem);
          //   break;
          // case 2:   // PWM
          //   msg += "Atuador PWM " + String(l_rem) + " removido.";
          //   DEBUG_PRINT("Apagar dados PWM: ");        DEBUG_PRINTLN(l_rem);
          //   g_ATTiny_PWM.RemoveSlave(l_rem);
          //   break;
          // case 3:   // Medidor
          //   msg += "Atuador Medidor " + String(l_rem) + " removido.";
          //   DEBUG_PRINT("Apagar dados PWM: ");        DEBUG_PRINTLN(l_rem);
          //   g_ATTiny_Med.RemoveSlave(l_rem);
          //   break;
          // case 6:   // Remove DS18b20
          //   if ( g_OW_DS.RemoveSlaves() ) {
          //     msg += "DS18b20 Apagados.";
          //     DEBUG_PRINT("DS18b20 Apagados");
          //   } else {
          //     msg += "Erro ao apagar DS18b20.";
          //     DEBUG_PRINT("DS18b20 Apagar erro");
          //   }
          //   break;
          // case 7:   // Interruptor
          //   if ( g_ATTiny_Int.CreateVirtualInt() ) {
          //     msg += "Interruptor Virtual criado.";
          //     DEBUG_PRINT("Criado Int Virtual");
          //   } else {
          //     msg += "Erro ao criar Interruptor Virtual.";
          //     DEBUG_PRINT("Erro criar Int Virtual");
          //   }
          //   break;
          // case 8:   // ProMini
          //   msg += "Atuador Promini removido.";
          //   DEBUG_PRINT("Apagar dados Promini");
          //   g_ProMini_Atuador.RemoveSlave();
          //   break;
          // case 9:   // Realizar busca
          //   msg += "Busca de novos Slaves realizada.";
          //   DEBUG_PRINT("Buscar Slaves");
          //   g_ATTiny_Slave.Search();
          //   g_OW_DS.Search();
          //   break;
        default:
            DEBUG_PRINT("Tipo inválido");
            break;
        }
      c_ok = true;
      } else {
        msg += "\"Formato dos dados incorretos!\""; DEBUG_PRINTLN("Erro dado");
      }
    } else {
        msg += "\"Dados enviados incorretos.\""; DEBUG_PRINTLN("Erro json");
    }
  }

  if ( c_ok ) {
    msg += "\",\"Res\":\"Ok\"";
    g_Stat.JSON_OK++;
    DEBUG_PRINTLN("OK");
  }
  else {
    msg += "\",\"Res\":\"Erro\"";
    DEBUG_PRINTLN("Erro");
    g_Stat.JSON_Erro++;
  }

  msg += "}";

  DEBUG_PRINTLN(msg);

  server.send(200, "application/json", msg);

  if ( c_ok ) {
    DMD_EEPROM_Save((char *)&g_Dados, sizeof(g_Dados));
    //DMD_WifiConectWait15(g_Dados.AP_ssid , g_Dados.AP_pass);
  }
}

/*
 * Ajax JSON - Configura o esquemas de cores usado nos interruptores
 */
// void f_json_Config_Int_Color () {
//   DEBUG_PRINT("WJ - Config Int Color - ");

//   bool c_ok = false;
//   int  l_sch;
  
//   String msg = "{\"Status\":\"";

//   //for (uint8_t i=0; i<server.args(); i++){
//   if( server.args() == 1 ) {
//     int i = 0;
//     DEBUG_PRINTLN(" Arg -> "+ server.argName(i) + " : "+ server.arg(i));
//     if(server.argName(i) == "plain") {
//       DynamicJsonDocument root(150);
//       DeserializationError error = deserializeJson( root, server.arg(i));
//       if (error) {
//         msg += "\"Formato dos dados incorretos.\""; DEBUG_PRINTLN("Erro Parsing");
//         return;
//       }
//       DEBUG_PRINTLN("JSON parsing OK!");
//       if ( root.containsKey("c_C") ){
//         l_sch = root["c_C"];
//         if ( l_sch <= 7 ) {
//           if ( g_ATTiny_Int.SetLedColorScheme(l_sch) ) {
//             g_Dados.ColorScheme = l_sch;
//             c_ok = true;
//             msg += "\"Esquema de cor alterado com sucesso!\""; DEBUG_PRINTLN("SCH OK");
//           } else {
//             msg += "\"Erro ao alterer o esquema em ao menos um Slave!\""; DEBUG_PRINTLN("SCH ERRO");
//           }
//         } else {
//           if (l_sch == 10) {
//             g_ATTiny_Int.SetDia(false);
//             DEBUG_PRINTLN("Set Noite");
//           } else {
//             g_ATTiny_Int.SetDia(true);
//             DEBUG_PRINTLN("Set Dia");
//           }
//         }

//       } else {
//         msg += "\"Formato dos dados incorretos!\""; DEBUG_PRINTLN("Erro dado");
//       }
//     } else {
//         msg += "\"Dados enviados incorretos.\""; DEBUG_PRINTLN("Erro json");
//     }
//   }

//   if ( c_ok ) {
//     msg += "\",\"Res\":\"Ok\"";
//     g_Stat.JSON_OK++;
//     DEBUG_PRINTLN("OK");
//   }
//   else {
//     msg += "\",\"Res\":\"Erro\"";
//     DEBUG_PRINTLN("Erro");
//     g_Stat.JSON_Erro++;
//   }

//   msg += "}";

//   DEBUG_PRINTLN(msg);

//   server.send(200, "application/json", msg);

//   if ( c_ok ) {
//     DMD_EEPROM_Save((char *)&g_Dados, sizeof(g_Dados));
//     //DMD_WifiConectWait15(g_Dados.AP_ssid , g_Dados.AP_pass);
//   }
// }


/*
 * Ajax JSON - Configura as saidas ativadas pelo Interruptor
 */
// void f_json_Config_Int () {
//   DEBUG_PRINT("WJ - Config Int - ");

//   bool c_ok = false;
//   int l_dev, l_Int , l_saida;
  
//   String msg = "{\"Status\":\"";

//   //for (uint8_t i=0; i<server.args(); i++){
//   if( server.args() == 1 ) {
//     int i = 0;
//     DEBUG_PRINTLN(" Arg -> "+ server.argName(i) + " : "+ server.arg(i));
//     if(server.argName(i) == "plain") {
//       DynamicJsonDocument root(150);
//       DeserializationError error = deserializeJson( root, server.arg(i));
//       if (error) {
//         msg += "\"Formato dos dados incorretos.\""; DEBUG_PRINTLN("Erro Parsing");
//         return;
//       }
//       DEBUG_PRINTLN("JSON parsing OK!");
//       if ( root.containsKey("c_D") && root.containsKey("c_I") ){
//         l_dev = root["c_D"];
//         l_Int = root["c_I"];
//         if ( root.containsKey("c_S") ) {
//           l_saida =  root["c_S"];

//           if ( l_dev < ATTiny_INT_MAX_SLAVES && l_Int < ATTiny_MAX_INT && l_saida < ATTiny_MAX_ATUADOR_SISTEMA ) {
//             if ( g_Dados.ATT_SlaveInt_Cfg[l_dev].AtivarSaida[l_Int][l_saida] )
//               g_Dados.ATT_SlaveInt_Cfg[l_dev].AtivarSaida[l_Int][l_saida] = false;
//             else 
//               g_Dados.ATT_SlaveInt_Cfg[l_dev].AtivarSaida[l_Int][l_saida] = true;
//             c_ok = true;
//           }
//         }
//         if ( root.containsKey("c_PU") ) {
//           l_saida =  root["c_PU"];
//           if ( l_dev < ATTiny_INT_MAX_SLAVES && l_Int < ATTiny_MAX_INT && l_saida < ATTiny_MAX_PWM_SISTEMA ) {
//             if ( g_Dados.ATT_SlaveInt_Cfg[l_dev].AtivarPWM_UP[l_Int][l_saida] )
//               g_Dados.ATT_SlaveInt_Cfg[l_dev].AtivarPWM_UP[l_Int][l_saida] = false;
//             else 
//               g_Dados.ATT_SlaveInt_Cfg[l_dev].AtivarPWM_UP[l_Int][l_saida] = true;
//             c_ok = true;
//           }          
//         }
//       } else {
//         msg += "\"Formato dos dados incorretos!\""; DEBUG_PRINTLN("Erro dado");
//       }
//     } else {
//         msg += "\"Dados enviados incorretos.\""; DEBUG_PRINTLN("Erro json");
//     }
//   }

//   if ( c_ok ) {
//     msg += "\",\"Res\":\"Ok\"";
//     g_Stat.JSON_OK++;
//     DEBUG_PRINTLN("OK");
//   }
//   else {
//     msg += "\",\"Res\":\"Erro\"";
//     DEBUG_PRINTLN("Erro");
//     g_Stat.JSON_Erro++;
//   }

//   msg += "}";

//   DEBUG_PRINTLN(msg);

//   server.send(200, "application/json", msg);

//   if ( c_ok ) {
//     DMD_EEPROM_Save((char *)&g_Dados, sizeof(g_Dados));
//     //DMD_WifiConectWait15(g_Dados.AP_ssid , g_Dados.AP_pass);
//   }
// }

/*
 * Ajax JSON - Resete da placa
 */
void f_json_reset () {
  DEBUG_PRINT("WJ - Reset - ");

  bool c_ok = false;
  
  String msg;
  msg.reserve (500);

  msg = "{\"Status\":\"";

  //for (uint8_t i=0; i<server.args(); i++){
  if( server.args() == 1 ) {
    int i = 0;
    DEBUG_PRINTLN(" Arg -> "+ server.argName(i) + " : "+ server.arg(i));
    if(server.argName(i) == "plain") {
      DynamicJsonDocument root(150);
      DeserializationError error = deserializeJson( root, server.arg(i));
      if (error) {
        msg += "\"Formato dos dados incorretos.\""; DEBUG_PRINTLN("Erro Parsing");
        return;
      }
      DEBUG_PRINTLN("JSON parsing OK!");
      if (  measureJson(root["c_S"]) > 1 ){
        if ( root["c_S"] == 157 ) {
          DEBUG_PRINT("Codigo OK ");
          c_ok = true;
          msg += "Sistema será reiniciado. ";
        } else {
           msg += "Erro ao solicitar o reset da placa. ";
        }
      }
    } else {
        msg += "\"Formato dos dados incorretos!\""; DEBUG_PRINTLN("Erro dado");
    }
  } else {
        msg += "\"Dados enviados incorretos.\""; DEBUG_PRINTLN("Erro json");
  }

  if ( c_ok ) {
    msg += "\",\"Res\":\"Ok\"";
    g_Stat.JSON_OK++;
    DEBUG_PRINTLN("OK");
  }
  else {
    msg += "\",\"Res\":\"Erro\"";
    DEBUG_PRINTLN("Erro");
    g_Stat.JSON_Erro++;
  }

  msg += "}";

  DEBUG_PRINTLN(msg);

  server.send(200, "application/json", msg);

  if ( c_ok ) {
    delay(500);
    ESP.restart();
  }
}


// void f_json_Config_Int_Topic () {
//   DEBUG_PRINT("WJ - Config Int Topic - ");

//   bool c_ok = false;
//   //int temp;
  
//   String msg = "{\"Status\":\"";
//   msg.reserve(300);

//   //for (uint8_t i=0; i<server.args(); i++){
//   if( server.args() == 1 ) {
//     int i = 0;
//     DEBUG_PRINTLN(" Arg -> "+ server.argName(i) + " : "+ server.arg(i));
//     if(server.argName(i) == "plain") {
//       DynamicJsonDocument root(150);
//       DeserializationError error = deserializeJson( root, server.arg(i));
//       if (error) {
//         msg += "Formato dos dados incorretos."; DEBUG_PRINTLN("Erro Parsing");
//         return;
//       }

//       if (  measureJson(root["c_No"]) > 3 /*&&
//             measureJson(root["c_D" ]) > 1 &&
//             measureJson(root["c_I" ]) > 1 */){
//         if (  measureJson(root["c_No"]) < (DADOS_INT_TOPIC+2) ){
//           int l_device = root["c_D" ];
//           int l_int = root["c_I" ];
//           strncpy(g_Dados.ATT_SlaveInt_Cfg[l_device].Topico_Int[l_int], root["c_No"],DADOS_INT_TOPIC);
//           msg += "Topico alterado com sucesso.";
//           c_ok = true;
//         } else {
//           msg += "Nome muito longo."; DEBUG_PRINTLNF("Erro - Longo");
//         }
//       } else {
//         msg += "Nome muito curto."; DEBUG_PRINTLNF("Erro - Curto");
//       }
//     } else {
//         msg += "Formato dos dados incorretos!"; DEBUG_PRINTLN("Erro dado");
//     }
//   } else {
//         msg += "Dados enviados incorretos."; DEBUG_PRINTLN("Erro json");
//   }

//   if ( c_ok ) {
//     msg += "\",\"Res\":\"Ok\"";
//     g_Stat.JSON_OK++;
//     DEBUG_PRINTLN("OK");
//   }
//   else {
//     msg += "\",\"Res\":\"Erro\"";
//     DEBUG_PRINTLN("Erro");
//     g_Stat.JSON_Erro++;
//   }
//   msg += "}";

//   DEBUG_PRINTLN(msg);

//   server.send(200, "application/json", msg);

//   if ( c_ok ) {
//     DMD_EEPROM_Save((char *)&g_Dados, sizeof(g_Dados));
//   }
// }


void f_json_Config_Nome () {
  DEBUG_PRINT("WJ - Config Nome - ");

  bool c_ok = false;
  //int temp;
  
  String msg = "{\"Status\":\"";
  msg.reserve(300);

  //for (uint8_t i=0; i<server.args(); i++){
  if( server.args() == 1 ) {
    int i = 0;
    DEBUG_PRINTLN(" Arg -> "+ server.argName(i) + " : "+ server.arg(i));
    if(server.argName(i) == "plain") {
      DynamicJsonDocument root(150);
      DeserializationError error = deserializeJson( root, server.arg(i));
      if (error) {
        msg += "\"Formato dos dados incorretos.\""; DEBUG_PRINTLN("Erro Parsing");
        return;
      }
      if (  measureJson(root["c_No"])>3 ){
        if (  measureJson(root["c_No"]) < (DADOS_NAME_SIZE+2) ){
          strncpy(g_Dados.Name, root["c_No"],DADOS_NAME_SIZE);
          msg += "Nome gravado com sucesso.";
          c_ok = true;
        } else {
          msg += "\"Nome muito longo.\""; DEBUG_PRINTLNF("Erro - Longo");
        }
      } else {
        msg += "\"Nome muito curto.\""; DEBUG_PRINTLNF("Erro - Curto");
      }
    } else {
        msg += "\"Formato dos dados incorretos!\""; DEBUG_PRINTLN("Erro dado");
    }
  } else {
        msg += "\"Dados enviados incorretos.\""; DEBUG_PRINTLN("Erro json");
  }

  if ( c_ok ) {
    msg += "\",\"Res\":\"Ok\"";
    g_Stat.JSON_OK++;
    DEBUG_PRINTLN("OK");
  }
  else {
    msg += "\",\"Res\":\"Erro\"";
    DEBUG_PRINTLN("Erro");
    g_Stat.JSON_Erro++;
  }
  msg += "}";

  DEBUG_PRINTLN(msg);

  server.send(200, "application/json", msg);

  if ( c_ok ) {
    DMD_EEPROM_Save((char *)&g_Dados, sizeof(g_Dados));
  }
}


void f_json_Config_Ip () {
  DEBUG_PRINT("WJ - Config Ip - ");

  bool c_ok = false;
  //int temp;
  
  String msg = "{\"Status\":\"";
  msg.reserve(300);

  //for (uint8_t i=0; i<server.args(); i++){
  if( server.args() == 1 ) {
    int i = 0;
    DEBUG_PRINTLN(" Arg -> "+ server.argName(i) + " : "+ server.arg(i));
    if(server.argName(i) == "plain") {
      DynamicJsonDocument root(150);
      DeserializationError error = deserializeJson( root, server.arg(i));
      if (error) {
        msg += "\"Formato dos dados incorretos.\""; DEBUG_PRINTLN("Erro Parsing");
        return;
      }
      if (  measureJson(root["c_IP"])>7 ){
        if (  measureJson(root["c_IP"]) < (DADOS_NAME_SIZE+2) ){
          strncpy(g_Dados.MQTT_Srv_IP, root["c_IP"],DADOS_IP_SIZE);
          msg += "Ip gravado com sucesso.";
          c_ok = true;
        } else {
          msg += "\"IP muito longo.\""; DEBUG_PRINTLNF("Erro - Longo");
        }
      } else {
        msg += "\"IP muito curto.\""; DEBUG_PRINTLNF("Erro - Curto");
      }
    } else {
        msg += "\"Formato dos dados incorretos!\""; DEBUG_PRINTLN("Erro dado");
    }
  } else {
        msg += "\"Dados enviados incorretos.\""; DEBUG_PRINTLN("Erro json");
  }

  if ( c_ok ) {
    msg += "\",\"Res\":\"Ok\"";
    g_Stat.JSON_OK++;
    DEBUG_PRINTLN("OK");
  }
  else {
    msg += "\",\"Res\":\"Erro\"";
    DEBUG_PRINTLN("Erro");
    g_Stat.JSON_Erro++;
  }
  msg += "}";

  DEBUG_PRINTLN(msg);

  server.send(200, "application/json", msg);

  if ( c_ok ) {
    DMD_EEPROM_Save((char *)&g_Dados, sizeof(g_Dados));
  }
}


/*
 * Ajax JSON - Configura NS
 */
void f_json_Config_NS () {
  DEBUG_PRINT("WJ - Config NS - ");

  bool c_ok = false;
  //int temp;
  
  String msg = "{\"Status\":\"";
  msg.reserve(300);

  //for (uint8_t i=0; i<server.args(); i++){
  if( server.args() == 1 ) {
    int i = 0;
    DEBUG_PRINTLN(" Arg -> "+ server.argName(i) + " : "+ server.arg(i));
    if(server.argName(i) == "plain") {
      DynamicJsonDocument root(150);
      DeserializationError error = deserializeJson( root, server.arg(i));
      if (error) {
        msg += "\"Formato dos dados incorretos.\""; DEBUG_PRINTLN("Erro Parsing");
        return;
      }
      DEBUG_PRINT("measureJson(root[c_NS]) - ");
      DEBUG_PRINTLN(measureJson(root["c_NS"]));
      if ( measureJson(root["c_NS"]) == (NS_SIZE + 2) ){
        //temp =  root["c_NS"] ;
        //if ( temp >= 5 ) {
          DEBUG_PRINT("NS = ");
          strncpy(g_Dados.NS, root["c_NS"],NS_SIZE);
          //g_Dados.NS_gravado = true;
          msg += "Numero Serial gravado com sucesso.";
          //msg += "\",\"mac\":\"" + WiFi.macAddress() + "\"";
          c_ok = true;

        //} else {
        //  msg += "Erro ao gravar o Numero Serial.";
        //}
      } else {
        msg += "Numero Serial inválido.";
      }
    } else {
        msg += "\"Formato dos dados incorretos!\""; DEBUG_PRINTLN("Erro dado");
    }
  } else {
        msg += "\"Dados enviados incorretos.\""; DEBUG_PRINTLN("Erro json");
  }

  if ( c_ok ) {
    msg += "\",\"Res\":\"Ok\"";
    g_Stat.JSON_OK++;
    DEBUG_PRINTLN("OK");
  }
  else {
    msg += "\",\"Res\":\"Erro\"";
    DEBUG_PRINTLN("Erro");
    g_Stat.JSON_Erro++;
  }
  msg += "}";

  DEBUG_PRINTLN(msg);

  server.send(200, "application/json", msg);

  if ( c_ok ) {
    DMD_EEPROM_Save((char *)&g_Dados, sizeof(g_Dados));
  }
}

/*
 * Ajax JSON - Comandos
 */
void SaidaOn (uint8_t, int8_t);
void SaidaOff (uint8_t, int8_t);

// void f_json_CMD () {
//   DEBUG_PRINT("WJ - CMD - ");

//   bool c_ok = false;
//   int l_dev, l_Int;
  
//   String msg = "{\"Status\":\"";

//   //for (uint8_t i=0; i<server.args(); i++){
//   if( server.args() == 1 ) {
//     int i = 0;
//     DEBUG_PRINTLN(" Arg -> "+ server.argName(i) + " : "+ server.arg(i));
//     if(server.argName(i) == "plain") {
//       DynamicJsonDocument root(150);
//       DeserializationError error = deserializeJson( root, server.arg(i));
//       if (error) {
//         msg += "\"Formato dos dados incorretos.\""; DEBUG_PRINTLN("Erro Parsing");
//         return;
//       }
//       DEBUG_PRINTLN("JSON parsing OK!");
//       if ( root.containsKey("c_D") && root.containsKey("c_I") ) {
//         l_dev = root["c_D"];
//         l_Int = root["c_I"];
//         DEBUG_PRINTF("l_dev "); DEBUG_PRINT(l_dev);
//         DEBUG_PRINTF(" l_Int "); DEBUG_PRINTLN(l_Int);

//         if ( l_dev < ATTiny_INT_MAX_SLAVES && l_Int < ATTiny_MAX_INT ) {
//           if ( g_ATTiny_Int.Read_Int_Status(l_dev,l_Int) ) {
//             //String l_temp = "Int/" + String(3*l_dev+l_Int) + '/';
//             //g_MQTT.Publish(l_temp.c_str(), "OFF");
//             DEBUG_PRINTLNF(" Off");
//             SaidaOff(l_dev,l_Int);
//             msg += "\"Saida Desativada\"";
//             c_ok = true;
//           } else {
//             //String l_temp = "Int/" + String(3*l_dev+l_Int) + '/';
//             //g_MQTT.Publish(l_temp.c_str(), "ON");
//             DEBUG_PRINTLNF(" On");
//             SaidaOn(l_dev,l_Int);
//             msg += "\"Saida Ativada\""; 
//             c_ok = true;
//           }          
//         }
//         if ( l_dev >= 10 && l_dev <= 19 && l_Int < ATTiny_MAX_ATUADOR ) {
//           // Ativaçao do Slave Promini
//           l_dev = l_dev - 10;
//           DEBUG_PRINTLNF(" Atuador");
//           if ( g_ATTiny_Atuador.GetStatus(l_dev, l_Int) ) {
//             //String l_temp = "Int/" + String(3*l_dev+l_Int) + '/';
//             //g_MQTT.Publish(l_temp.c_str(), "OFF");
//             DEBUG_PRINTLNF(" Off");
//             g_ATTiny_Atuador.SetOff(l_dev,l_Int);
//             msg += "\"Saida Desativada\"";;
//             c_ok = true;
//           } else {
//             //String l_temp = "Int/" + String(3*l_dev+l_Int) + '/';
//             //g_MQTT.Publish(l_temp.c_str(), "ON");
//             DEBUG_PRINTLNF(" On");
//             g_ATTiny_Atuador.SetOn(l_dev,l_Int);
//             msg += "\"Saida Ativada\""; 
//             c_ok = true;
//           }          
//         }


//         if ( l_dev == 20 && l_Int < ProMini_MAX_ATUADOR ) {
//           // Ativaçao do Slave Promini
//           DEBUG_PRINTLNF(" ProMini");
//           if ( g_ProMini_Atuador.GetStatus(l_Int) ) {
//             //String l_temp = "Int/" + String(3*l_dev+l_Int) + '/';
//             //g_MQTT.Publish(l_temp.c_str(), "OFF");
//             DEBUG_PRINTLNF(" Off");
//             g_ProMini_Atuador.SetOff(l_Int);
//             msg += "\"Saida Desativada\"";;
//             c_ok = true;
//           } else {
//             //String l_temp = "Int/" + String(3*l_dev+l_Int) + '/';
//             //g_MQTT.Publish(l_temp.c_str(), "ON");
//             DEBUG_PRINTLNF(" On");
//             g_ProMini_Atuador.SetOn(l_Int);
//             msg += "\"Saida Ativada\"";
//             c_ok = true;
//           }          
//         }
//       }
//     } else {
//         msg += "\"Formato dos dados incorretos!\""; DEBUG_PRINTLN("Erro dado");
//     }
//   } else {
//         msg += "\"Dados enviados incorretos.\""; DEBUG_PRINTLN("Erro json");
//   }

//   if ( c_ok ) {
//     msg += "\",\"Res\":\"Ok\"";
//     g_Stat.JSON_OK++;
//     DEBUG_PRINTLN("OK");
//   }
//   else {
//     msg += "\",\"Res\":\"Erro\"";
//     DEBUG_PRINTLN("Erro");
//     g_Stat.JSON_Erro++;
//   }
//   msg += "}";

//   DEBUG_PRINTLN(msg);

//   server.send(200, "application/json", msg);

//   // if ( c_ok ) {
//   //   DMD_EEPROM_Save((char *)&g_Dados, sizeof(g_Dados));
//   // }
// }

/*
 * Ajax JSON - Configura Local PW
 */
void f_json_Config_LocalPW () {
  DEBUG_PRINT("WJ - Config Local PW - ");

  bool c_ok = false;
  
  String msg = "{\"Status\":\"";
  msg.reserve(300);

  //for (uint8_t i=0; i<server.args(); i++){
  if( server.args() == 1 ) {
    int i = 0;
    DEBUG_PRINTLN(" Arg -> "+ server.argName(i) + " : "+ server.arg(i));
    if(server.argName(i) == "plain") {
      DynamicJsonDocument root(150);
      DeserializationError error = deserializeJson( root, server.arg(i));
      if (error) {
        msg += "\"Formato dos dados incorretos.\""; DEBUG_PRINTLN("Erro Parsing");
        return;
      }
      DEBUG_PRINT("measureJson(root[c_PW]) - ");
      DEBUG_PRINTLN(measureJson(root["c_PW"]));
      if ( measureJson(root["c_PW"]) >= 10 ){
        DEBUG_PRINT("Atual: ");
        DEBUG_PRINT(g_Dados.Local_PW);
        strncpy(g_Dados.Local_PW, root["c_PW"],DADOS_LOCAL_PW_SIZE);
        DEBUG_PRINT("Nova: ");
        DEBUG_PRINT(g_Dados.Local_PW);
        msg += "Senha local alterada com sucesso gravado com sucesso.";
        c_ok = true;
      } else {
        msg += "Senha local inválida.";
      }
    } else {
        msg += "\"Formato dos dados incorretos!\""; DEBUG_PRINTLN("Erro dado");
    }
  } else {
        msg += "\"Dados enviados incorretos.\""; DEBUG_PRINTLN("Erro json");
  }

  if ( c_ok ) {
    msg += "\",\"Res\":\"Ok\"";
    g_Stat.JSON_OK++;
    DEBUG_PRINTLN(" OK");
  }
  else {
    msg += "\",\"Res\":\"Erro\"";
    DEBUG_PRINTLN(" Erro");
    g_Stat.JSON_Erro++;
  }
  msg += "}";

  DEBUG_PRINTLN(msg);

  server.send(200, "application/json", msg);

  if ( c_ok ) {
    DMD_EEPROM_Save((char *)&g_Dados, sizeof(g_Dados));
  }
}

// void f_json_Config_Debounce () {
//   DEBUG_PRINT("WJ - Config Debounce - ");

//   bool c_ok = false;
//   //int temp;
  
//   String msg = "{\"Status\":\"";
//   msg.reserve(300);

//   //for (uint8_t i=0; i<server.args(); i++){
//   if( server.args() == 1 ) {
//     int i = 0;
//     DEBUG_PRINTLN(" Arg -> "+ server.argName(i) + " : "+ server.arg(i));
//     if(server.argName(i) == "plain") {
//       DynamicJsonDocument root(150);
//       DeserializationError error = deserializeJson( root, server.arg(i));
//       if (error) {
//         msg += "\"Formato dos dados incorretos.\""; DEBUG_PRINTLN("Erro Parsing");
//         return;
//       }
//       if (  measureJson(root["c_No"])>3 ){
//         DEBUG_PRINTLNF("OK");
//         if ( g_ATTiny_Int.Set_Debounce(root["c_No"]) ) {
//           msg += "Tempo alterado com sucesso.";
//           g_Dados.Debounce = root["c_No"];
//           msg += "Tempo alterado com sucesso para ";
//           msg += g_Dados.Debounce;
//           c_ok = true;
//         } else {
//           msg += "Erro na comunicação com Int.";
//         }
//       } else {
//         msg += "\"Nome muito curto.\""; DEBUG_PRINTLNF("Erro - Curto");
//       }
//     } else {
//         msg += "\"Formato dos dados incorretos!\""; DEBUG_PRINTLN("Erro dado");
//     }
//   } else {
//         msg += "\"Dados enviados incorretos.\""; DEBUG_PRINTLN("Erro json");
//   }

//   if ( c_ok ) {
//     msg += "\",\"Res\":\"Ok\"";
//     g_Stat.JSON_OK++;
//     DEBUG_PRINTLN("OK");
//   }
//   else {
//     msg += "\",\"Res\":\"Erro\"";
//     DEBUG_PRINTLN("Erro");
//     g_Stat.JSON_Erro++;
//   }
//   msg += "}";

//   DEBUG_PRINTLN(msg);

//   server.send(200, "application/json", msg);

//   if ( c_ok ) {
//     DMD_EEPROM_Save((char *)&g_Dados, sizeof(g_Dados));
//   }
// }



/*
 * Ajax JSON - Configura o percentual referente ao medidores
 */
// void f_json_Config_Medidor () {
//   DEBUG_PRINT("WJ - Medidor - ");

//   bool c_ok = false;
//   uint16_t l_dev, l_tipo, l_valor;
  
//   String msg = "{\"Status\":\"";

//   //for (uint8_t i=0; i<server.args(); i++){
//   if( server.args() == 1 ) {
//     int i = 0;
//     DEBUG_PRINTLN(" Arg -> "+ server.argName(i) + " : "+ server.arg(i));
//     if(server.argName(i) == "plain") {
//       DynamicJsonDocument root(150);
//       DeserializationError error = deserializeJson( root, server.arg(i));
//       if (error) {
//         msg += "\"Formato dos dados incorretos.\""; DEBUG_PRINTLN("Erro Parsing");
//         return;
//       }
//       DEBUG_PRINTLN("JSON parsing OK!");
//       if ( root.containsKey("c_T") && root.containsKey("c_D") && root.containsKey("c_V")){
//         l_dev = root["c_D"];  DEBUG_PRINT("c_D ");  DEBUG_PRINT(l_dev);
//         l_tipo= root["c_T"];  DEBUG_PRINT("c_T ");  DEBUG_PRINT(l_tipo);
//         l_valor = root["c_V"];DEBUG_PRINT("c_V ");  DEBUG_PRINT(l_valor);

//         if ( l_dev >= g_ATTiny_Med.getDeviceCount() ) {
//           msg += "\"Device inválido.\""; DEBUG_PRINTLN("Erro device");
//           return;
//         }

//         switch (l_tipo) {
//           case 1: // Minimo
//             g_Dados.ATT_SlaveMed[l_dev].Minimo = l_valor;
//             msg += "Valor Minimo para " + String(l_dev) + " gravado."; DEBUG_PRINTLN("Min ok");
//             c_ok = true;
//             g_ATTiny_Med.SetPercentMinMax(l_dev, g_Dados.ATT_SlaveMed[l_dev].Minimo, g_Dados.ATT_SlaveMed[l_dev].Maximo);
//             break;
//           case 2: // Máximo
//             g_Dados.ATT_SlaveMed[l_dev].Maximo = l_valor;
//             msg += "Valor Máximo para " + String(l_dev) + " gravado."; DEBUG_PRINTLN("Max ok");
//             c_ok = true;
//             g_ATTiny_Med.SetPercentMinMax(l_dev, g_Dados.ATT_SlaveMed[l_dev].Minimo, g_Dados.ATT_SlaveMed[l_dev].Maximo);
//             break;
//           case 3: // Tópico
//             if ( l_valor >= DADOS_TOPIC_MAX_INDEX) {
//               msg += "\"Tópico inválido!\""; DEBUG_PRINTLN("Erro tópico");
//               break;
//             }
//             g_Dados.ATT_SlaveMed[l_dev].Topico_Index = l_valor;
//             msg += "Tópico do slave " + String(l_dev) + " alterado para tópico " + String(l_valor) ; DEBUG_PRINTLN("Tópico alterado");
//             c_ok = true;
//             break;
//           default:
//             break;
//         } 

//       } else {
//         msg += "\"Formato dos dados incorretos!\""; DEBUG_PRINTLN("Erro dado");
//       }
//     } else {
//         msg += "\"Dados enviados incorretos.\""; DEBUG_PRINTLN("Erro json");
//     }
//   }

//   if ( c_ok ) {
//     msg += "\",\"Res\":\"Ok\"";
//     g_Stat.JSON_OK++;
//     DEBUG_PRINTLN("OK");
//   }
//   else {
//     msg += "\",\"Res\":\"Erro\"";
//     DEBUG_PRINTLN("Erro");
//     g_Stat.JSON_Erro++;
//   }

//   msg += "}";

//   DEBUG_PRINTLN(msg);

//   server.send(200, "application/json", msg);

//   if ( c_ok ) {
//     DMD_EEPROM_Save((char *)&g_Dados, sizeof(g_Dados));
//     //DMD_WifiConectWait15(g_Dados.AP_ssid , g_Dados.AP_pass);
//   }
// }



/*
 * Ajax JSON - Configura o valor máximo das saidas PWM
 */
// void f_json_Config_PWM () {
//   DEBUG_PRINT("WJ - PWM - ");

//   bool c_ok = false;
//   uint16_t l_dev, l_tipo, l_valor;
  
//   String msg = "{\"Status\":\"";

//   //for (uint8_t i=0; i<server.args(); i++){
//   if( server.args() == 1 ) {
//     int i = 0;
//     DEBUG_PRINTLN(" Arg -> "+ server.argName(i) + " : "+ server.arg(i));
//     if(server.argName(i) == "plain") {
//       DynamicJsonDocument root(150);
//       DeserializationError error = deserializeJson( root, server.arg(i));
//       if (error) {
//         msg += "\"Formato dos dados incorretos.\""; DEBUG_PRINTLN("Erro Parsing");
//         return;
//       }
//       DEBUG_PRINTLN("JSON parsing OK!");
//       if ( root.containsKey("c_T") && root.containsKey("c_D") && root.containsKey("c_V")){
//         l_tipo= root["c_T"];  DEBUG_PRINT("c_T ");  DEBUG_PRINT(l_tipo);
//         l_dev = root["c_D"];  DEBUG_PRINT("c_D ");  DEBUG_PRINT(l_dev);
//         l_valor = root["c_V"];DEBUG_PRINT("c_V ");  DEBUG_PRINT(l_valor);

//         if ( l_dev >= ATTiny_MAX_PWM_SISTEMA ) {
//           msg += "\"Saida inválida.\""; DEBUG_PRINTLN("Erro Saida");
//           return;
//         }

//         switch (l_tipo) {
//           case 1: // Geral
//             g_Dados.PWM_Max[l_dev] = l_valor;
//             msg += "Valor PWM "+String(l_dev)+" ("+ String(l_valor)+") gravado."; DEBUG_PRINTLN("PWM ok");
//             c_ok = true;
//             break;
//           default:
//             break;
//         } 

//       } else {
//         msg += "\"Formato dos dados incorretos!\""; DEBUG_PRINTLN("Erro dado");
//       }
//     } else {
//         msg += "\"Dados enviados incorretos.\""; DEBUG_PRINTLN("Erro json");
//     }
//   }

//   if ( c_ok ) {
//     msg += "\",\"Res\":\"Ok\"";
//     g_Stat.JSON_OK++;
//     DEBUG_PRINTLN("OK");
//   }
//   else {
//     msg += "\",\"Res\":\"Erro\"";
//     DEBUG_PRINTLN("Erro");
//     g_Stat.JSON_Erro++;
//   }

//   msg += "}";

//   DEBUG_PRINTLN(msg);

//   server.send(200, "application/json", msg);

//   if ( c_ok ) {
//     DMD_EEPROM_Save((char *)&g_Dados, sizeof(g_Dados));
//     //DMD_WifiConectWait15(g_Dados.AP_ssid , g_Dados.AP_pass);
//   }
// }


// void f_json_Config_Topic () {
//   DEBUG_PRINT("WJ - Config Temp Topic - ");

//   bool c_ok = false;
//   //int temp;
  
//   String msg = "{\"Status\":\"";
//   msg.reserve(300);

//   //for (uint8_t i=0; i<server.args(); i++){
//   if( server.args() == 1 ) {
//     int i = 0;
//     DEBUG_PRINTLN(" Arg -> "+ server.argName(i) + " : "+ server.arg(i));
//     if(server.argName(i) == "plain") {
//       DynamicJsonDocument root(150);
//       DeserializationError error = deserializeJson( root, server.arg(i));
//       if (error) {
//         msg += "Formato dos dados incorretos."; DEBUG_PRINTLN("Erro Parsing");
//         return;
//       }

//       if ( root.containsKey("c_No") && root.containsKey("c_D") ){
//         int l_device = root["c_D" ];
//         if ( l_device == 57 ) {
//           // Limpar todos os Tópicos
//           for ( uint8_t i = 0; i < DADOS_TOPIC_MAX_INDEX ; i++ ){
//               strncpy(g_Dados.Topicos[i], TOPIC_OFF, TOPIC_OFF_size);
//               msg += "Topico alterado com sucesso.";
//               c_ok = true;
//           }
//         } else {
//           if (  measureJson(root["c_No"]) < (DADOS_INT_TOPIC+2) ){
//             if ( l_device <= DADOS_TOPIC_MAX_INDEX ) {
//               strncpy(g_Dados.Topicos[l_device], root["c_No"],DADOS_INT_TOPIC);
//               msg += "Topico alterado com sucesso.";
//               c_ok = true;
//             } else {
//               msg += "Device inválido."; DEBUG_PRINTLNF(" Erro - Device");
//             }
//           } else {
//             msg += "Nome muito longo."; DEBUG_PRINTLNF("Erro - Longo");
//           }
//         }
//       } else {
//         msg += "Nome muito curto."; DEBUG_PRINTLNF("Erro - Curto");
//       }
//     } else {
//         msg += "Formato dos dados incorretos!"; DEBUG_PRINTLN("Erro dado");
//     }
//   } else {
//         msg += "Dados enviados incorretos."; DEBUG_PRINTLN("Erro json");
//   }

//   if ( c_ok ) {
//     msg += "\",\"Res\":\"Ok\"";
//     g_Stat.JSON_OK++;
//     DEBUG_PRINTLN("OK");
//   }
//   else {
//     msg += "\",\"Res\":\"Erro\"";
//     DEBUG_PRINTLN("Erro");
//     g_Stat.JSON_Erro++;
//   }
//   msg += "}";

//   DEBUG_PRINTLN(msg);

//   server.send(200, "application/json", msg);

//   if ( c_ok ) {
//     DMD_EEPROM_Save((char *)&g_Dados, sizeof(g_Dados));
//   }
// }


// void f_json_Config_Relogio () {
//   DEBUG_PRINT("WJ - Config Relogio - ");

//   bool c_ok = false;
//   //int temp;
  
//   String msg = "{\"Status\":\"";
//   msg.reserve(300);

//   //for (uint8_t i=0; i<server.args(); i++){
//   if( server.args() == 1 ) {
//     int i = 0;
//     DEBUG_PRINTLN(" Arg -> "+ server.argName(i) + " : "+ server.arg(i));
//     if(server.argName(i) == "plain") {
//       DynamicJsonDocument root(150);
//       DeserializationError error = deserializeJson( root, server.arg(i));
//       if (error) {
//         msg += "Formato dos dados incorretos."; DEBUG_PRINTLN("Erro Parsing");
//         return;
//       }

//       if ( root.containsKey("c_D") ){
//         int l_cmd = root["c_D"];
//         uint8_t l_dado;
//         if ( root.containsKey("c_No") ) {
//           l_dado = root["c_No"];
//         } else {
//           l_dado = 0;
//         }
//         switch (l_cmd) {
//           case 6:   // Habilitar Relogio
//             g_Dados.Relogio_Brilho = 0x30;
//             g_Relogio.On();
//             msg += "Ativar relógio.";
//             c_ok = true;
//             break;
//           case 5:   // Desabilitar Relogio
//             g_Dados.Relogio_Brilho = 0xFF;
//             g_Relogio.Off();
//             msg += "Desativar relógio.";
//             c_ok = true;
//             break;
//           case 1:   // Fade Low
//             DEBUG_PRINT("dado "); DEBUG_PRINTLN(l_dado);
//             DEBUG_PRINT("Brilho Pre "); DEBUG_PRINTLN(g_Dados.Relogio_Brilho);
//             g_Dados.Relogio_Brilho = g_Dados.Relogio_Brilho & 0xF0;
//             DEBUG_PRINT("Brilho Low "); DEBUG_PRINTLN(g_Dados.Relogio_Brilho);
//             g_Dados.Relogio_Brilho = (g_Dados.Relogio_Brilho) | ( l_dado & 0x0F);
//             DEBUG_PRINT("Brilho Low "); DEBUG_PRINTLN(g_Dados.Relogio_Brilho);
//             g_Relogio.Set_Fade(g_Dados.Relogio_Brilho);
//             msg += "Fade Low alterado.";
//             c_ok = true;
//             break;
//           case 2:   // Fade High
//             DEBUG_PRINT("dado "); DEBUG_PRINTLN(l_dado);
//             DEBUG_PRINT("Brilho Pre "); DEBUG_PRINTLN(g_Dados.Relogio_Brilho);
//             g_Dados.Relogio_Brilho = g_Dados.Relogio_Brilho & 0x0F;
//             DEBUG_PRINT("Brilho High "); DEBUG_PRINTLN(g_Dados.Relogio_Brilho);
//             g_Dados.Relogio_Brilho = (g_Dados.Relogio_Brilho) | ( (l_dado << 4 ) & 0xF0);
//             DEBUG_PRINT("Fade High"); DEBUG_PRINTLN(g_Dados.Relogio_Brilho);
//             g_Relogio.Set_Fade(g_Dados.Relogio_Brilho);
//             msg += "Fade High alterado.";
//             c_ok = true;
//             break;
//           case 3:   // Hora Dia
//             DEBUG_PRINT("dado "); DEBUG_PRINTLN(l_dado);
//             g_Dados.Relogio_Hora = g_Dados.Relogio_Hora & 0xF0;
//             DEBUG_PRINT("Hora dia "); DEBUG_PRINTLN(g_Dados.Relogio_Hora);
//             g_Dados.Relogio_Hora = (g_Dados.Relogio_Hora) | ( l_dado & 0x0F);
//             DEBUG_PRINT("Fade dia"); DEBUG_PRINTLN(g_Dados.Relogio_Hora);
//             g_Relogio.Set_Time(g_Dados.Relogio_Hora);
//             msg += "Hora dia alterada.";
//             c_ok = true;
//             break;
//           case 4:   // Hora Noite
//             DEBUG_PRINT("dado "); DEBUG_PRINTLN(l_dado);
//             g_Dados.Relogio_Hora = g_Dados.Relogio_Hora & 0x0F;
//             DEBUG_PRINT("Hora noite "); DEBUG_PRINTLN(g_Dados.Relogio_Hora);
//             g_Dados.Relogio_Hora = (g_Dados.Relogio_Hora) | ( (l_dado << 4 ) & 0xF0);
//             DEBUG_PRINT("Fade noite"); DEBUG_PRINTLN(g_Dados.Relogio_Hora);
//             g_Relogio.Set_Time(g_Dados.Relogio_Hora);
//             msg += "Hora noite alterada.";
//             c_ok = true;
//             break;
//           default:
//             msg += "Erro comando.";
//             break;
//         }
//       } else {
//         msg += "Dado inválido."; DEBUG_PRINTLNF("Erro - Dado");
//       }
//     } else {
//         msg += "Formato dos dados incorretos!"; DEBUG_PRINTLN("Erro dado");
//     }
//   } else {
//         msg += "Dados enviados incorretos."; DEBUG_PRINTLN("Erro json");
//   }

//   if ( c_ok ) {
//     msg += "\",\"Res\":\"Ok\"";
//     g_Stat.JSON_OK++;
//     DEBUG_PRINTLN("OK");
//   }
//   else {
//     msg += "\",\"Res\":\"Erro\"";
//     DEBUG_PRINTLN("Erro");
//     g_Stat.JSON_Erro++;
//   }
//   msg += "}";

//   DEBUG_PRINTLN(msg);

//   server.send(200, "application/json", msg);

//   if ( c_ok ) {
//     DMD_EEPROM_Save((char *)&g_Dados, sizeof(g_Dados));
//   }
// }


// void f_json_Config_Topic_Temp () {
//   DEBUG_PRINT("WJ - Topic Temp - ");

//   bool c_ok = false;
//   uint16_t l_dev, l_valor;
  
//   String msg = "{\"Status\":\"";

//   //for (uint8_t i=0; i<server.args(); i++){
//   if( server.args() == 1 ) {
//     int i = 0;
//     DEBUG_PRINTLN(" Arg -> "+ server.argName(i) + " : "+ server.arg(i));
//     if(server.argName(i) == "plain") {
//       DynamicJsonDocument root(150);
//       DeserializationError error = deserializeJson( root, server.arg(i));
//       if (error) {
//         msg += "\"Formato dos dados incorretos.\""; DEBUG_PRINTLN("Erro Parsing");
//         return;
//       }
//       DEBUG_PRINTLN("JSON parsing OK!");
//       if ( root.containsKey("c_D") && root.containsKey("c_V")){
//         l_dev = root["c_D"];  DEBUG_PRINT("c_D ");  DEBUG_PRINT(l_dev);
//         l_valor = root["c_V"];DEBUG_PRINT("c_V ");  DEBUG_PRINT(l_valor);
//         if ( l_dev < g_OW_DS.getDeviceCount() ) {
//           // if ( l_valor < DADOS_TOPIC_MAX_INDEX) {
//             g_Dados.Temp_Topico_Index[l_dev] = l_valor;
//             DEBUG_PRINTLN("Tópico alterado");
//             msg += "Tópico do slave " + String(l_dev) + " alterado para tópico " + String(l_valor) ;
//             c_ok = true;
//           // } else {
//           //   msg += "\"Tópico inválido!\""; DEBUG_PRINTLN("Erro tópico");
//           // }
//         } else {
//           msg += "\"Device inválido.\""; DEBUG_PRINTLN("Erro device");
//         }
//       } else {
//         msg += "\"Formato dos dados incorretos!\""; DEBUG_PRINTLN("Erro dado");
//       }
//     } else {
//         msg += "\"Dados enviados incorretos.\""; DEBUG_PRINTLN("Erro json");
//     }
//   }

//   if ( c_ok ) {
//     msg += "\",\"Res\":\"Ok\"";
//     g_Stat.JSON_OK++;
//     DEBUG_PRINTLN("OK");
//   }
//   else {
//     msg += "\",\"Res\":\"Erro\"";
//     DEBUG_PRINTLN("Erro");
//     g_Stat.JSON_Erro++;
//   }

//   msg += "}";

//   DEBUG_PRINTLN(msg);

//   server.send(200, "application/json", msg);

//   if ( c_ok ) {
//     DMD_EEPROM_Save((char *)&g_Dados, sizeof(g_Dados));
//     //DMD_WifiConectWait15(g_Dados.AP_ssid , g_Dados.AP_pass);
//   }
// }

// void f_json_Config_AqueSolar () {
//   DEBUG_PRINT("WJ - Config AqueSolar - ");

//   bool c_ok = false;
//   //int temp;
  
//   String msg = "{\"Status\":\"";
//   msg.reserve(300);

//   //for (uint8_t i=0; i<server.args(); i++){
//   if( server.args() == 1 ) {
//     int i = 0;
//     DEBUG_PRINTLN(" Arg -> "+ server.argName(i) + " : "+ server.arg(i));
//     if(server.argName(i) == "plain") {
//       DynamicJsonDocument root(150);
//       DeserializationError error = deserializeJson( root, server.arg(i));
//       if (error) {
//         msg += "Formato dos dados incorretos."; DEBUG_PRINTLN("Erro Parsing");
//         return;
//       }

//       if ( root.containsKey("c_D") ){
//         int l_cmd = root["c_D"];
//         uint8_t l_dado;
//         float l_float;
//         if ( root.containsKey("c_No") ) {
//           l_dado = root["c_No"];
//         } else {
//           l_dado = 0;
//         }
//         if ( root.containsKey("c_Fo") ) {
//           l_float = root["c_Fo"];
//         } else {
//           l_float = 0;
//         }
//         DEBUG_PRINT("Dado: "); DEBUG_PRINT(l_dado);
//         DEBUG_PRINT("Float: "); DEBUG_PRINT(l_float);
//         switch (l_cmd) {
//           case 0:   // Habilitar Aquecedor
//             if ( g_Aquec_Solar.On() ) {
//               msg += "Aquecedor Ativado";
//             } else {
//               msg += "Aquecedor NÂO Ativado (Bloqueado)";
//             }
//             c_ok = true;
//             break;
//           case 1:   // Desabilitar Aquecedor
//             if ( g_Aquec_Solar.Off() ) {
//               msg += "Aquecedor Desativado";
//             } else {
//               msg += "Aquecedor Bloqueado";
//             }
//             c_ok = true;
//             break;
//           case 2:   // Alterar Slave device
//             if ( l_dado < g_ATTiny_Atuador.getDeviceCount() ) {
//               // g_Dados.Aque_Solar_cfg.Atuador_Dev = l_dado;
//               g_Aquec_Solar.Set_Atuador_Dev(l_dado);
//               msg += "Slave Atuador alterado.";
//               c_ok = true;
//             } else {
//               msg += "Erro ao alterar Slave Atuador.";
//             }
//             break;
//           case 3:   // Alterar Saida do Slave
//             if ( l_dado < ATTiny_MAX_ATUADOR ) {
//               // g_Dados.Aque_Solar_cfg.Atuador_Saida = l_dado;
//               g_Aquec_Solar.Set_Atuador_Saida(l_dado);
//               msg += "Saida do Slave Atuador.";
//               c_ok = true;
//             } else {
//               msg += "Erro ao alterar Saída do Atuador.";
//             }
//             break;
//           case 4:   // Device Sensor Temp Caixa
//             if ( l_dado < g_OW_DS.getDeviceCount() ) {
//               // g_Dados.Aque_Solar_cfg.T_Caixa_Dev = l_dado;
//               g_Aquec_Solar.Set_Sensor_Caixa(l_dado);
//               msg += "Sensor Temp Caixa alterado.";
//               c_ok = true;
//             } else {
//               msg += "Erro ao alterar Sensor Temp Caixa.";
//             }
//             break;
//           case 5:   // Device Sensor Temp Retorno
//             if ( l_dado < g_OW_DS.getDeviceCount() ) {
//               // g_Dados.Aque_Solar_cfg.T_Retorno_Dev = l_dado;
//               g_Aquec_Solar.Set_Sensor_Retorno(l_dado);
//               msg += "Sensor Temp Retorno alterado.";
//               c_ok = true;
//             } else {
//               msg += "Erro ao alterar Sensor Temp Retorno.";
//             }
//             break;
//           case 6:   // Device Sensor Temp Entrada Aquecedor
//             if ( l_dado < g_OW_DS.getDeviceCount() ) {
//               // g_Dados.Aque_Solar_cfg.T_Aq_Entrada_Dev = l_dado;
//               g_Aquec_Solar.Set_Sensor_Aq_Entrada(l_dado);
//               msg += "Sensor Temp Entrada Aquecedor alterado.";
//               c_ok = true;
//             } else {
//               msg += "Erro ao alterar Sensor Temp Entrada Aquecedor.";
//             }
//             break;
//           case 7:   // Device Sensor Temp Saida Aquecedor
//             if ( l_dado < g_OW_DS.getDeviceCount() ) {
//               // g_Dados.Aque_Solar_cfg.T_Aq_Saida_Dev = l_dado;
//               g_Aquec_Solar.Set_Sensor_Aq_Saida(l_dado);
//               msg += "Sensor Temp Saida alterado.";
//               c_ok = true;
//             } else {
//               msg += "Erro ao alterar Sensor Temp Saida Aquecedor.";
//             }
//             break;
//           case 8:   // Delta Ativação do Aquecedor
//             if ( l_dado < 30 ) {
//               // g_Dados.Aque_Solar_cfg.Temp_Delta_Ativacao = l_dado;
//               g_Aquec_Solar.Set_Temp_Delta_Ativação(l_dado);
//               msg += "Temp Delta Ativação alterado.";
//               c_ok = true;
//             } else {
//               msg += "Erro ao alterar Temp Delta Ativação.";
//             }
//             break;
//           case 9:   // Temp Congelamento
//             if ( l_dado < 10) {
//               // g_Dados.Aque_Solar_cfg.Temp_Congelamento = l_dado;
//               g_Aquec_Solar.Set_Temp_Congelamento(l_dado);
//               msg += "Temp Congelamento alterado.";
//               c_ok = true;
//             } else {
//               msg += "Erro ao alterar Sensor Temp Congelamento.";
//             }
//             break;
//           case 10:   // PWM Bomba
//             if ( l_dado < 256) {
//               // g_Dados.Aque_Solar_cfg.Atuador_PWM = l_dado;
//               g_Aquec_Solar.Bomba_SetPWM(l_dado);
//               msg += "PWM da Bomba alterado.";
//               c_ok = true;
//             } else {
//               msg += "Erro ao alterar PWM da Bomba.";
//             }
//             break;    

//           // Beer    
//           case 20:   // Habilitar Beer
//             if ( g_Beer.On() ) {
//               msg += "Beer Ativado";
//             } else {
//               msg += "Beer NÂO Ativado (Bloqueado)";
//             }
//             c_ok = true;
//             break;
//           case 21:   // Desabilitar Beer
//             if ( g_Beer.Off() ) {
//               msg += "Beer Desativado";
//             } else {
//               msg += "Beer Bloqueado";
//             }
//             c_ok = true;
//             break;  
//           case 28:   // Delta Ativação do Beer
//             if ( l_dado < 5 ) {
//               // g_Dados.Aque_Solar_cfg.Temp_Delta_Ativacao = l_dado;
//               g_Beer.Set_Temp_Delta_Ativacao(l_float);
//               msg += "Temp Delta Ativação alterado para " + (String)g_Beer.Get_Temp_Delta_Ativacao();
//               c_ok = true;
//             } else {
//               msg += "Erro ao alterar Temp Delta Ativação.";
//             }
//             break;
//           case 29:   // Temp Preset
//             if ( l_dado < DMD_BEER_TEMP_MAX ) {
//               // g_Dados.Aque_Solar_cfg.Temp_Congelamento = l_dado;
//               g_Beer.Set_Temp_Preset(l_dado);
//               msg += "Temp Preset alterado.";
//               c_ok = true;
//             } else {
//               msg += "Erro ao alterar Sensor Temp Preset.";
//             }
//             break;
//           default:
//             msg += "Erro comando.";
//             break;
//         }
//       } else {
//         msg += "Dado inválido."; DEBUG_PRINTLNF("Erro - Dado");
//       }
//     } else {
//         msg += "Formato dos dados incorretos!"; DEBUG_PRINTLN("Erro dado");
//     }
//   } else {
//         msg += "Dados enviados incorretos."; DEBUG_PRINTLN("Erro json");
//   }

//   if ( c_ok ) {
//     msg += "\",\"Res\":\"Ok\"";
//     g_Stat.JSON_OK++;
//     DEBUG_PRINTLN("OK");
//   }
//   else {
//     msg += "\",\"Res\":\"Erro\"";
//     DEBUG_PRINTLN("Erro");
//     g_Stat.JSON_Erro++;
//   }
//   msg += "}";

//   DEBUG_PRINTLN(msg);

//   server.send(200, "application/json", msg);

//   if ( c_ok ) {
//     DMD_EEPROM_Save((char *)&g_Dados, sizeof(g_Dados));
//   }
// }



// void f_json_Config_Beer_Ferm () {
//   DEBUG_PRINT("WJ - Config Beer Ferm - ");

//   bool c_ok = false;
//   //int temp;
  
//   String msg = "{\"Status\":\"";
//   msg.reserve(300);

//   //for (uint8_t i=0; i<server.args(); i++){
//   if( server.args() == 1 ) {
//     int i = 0;
//     DEBUG_PRINTLN(" Arg -> "+ server.argName(i) + " : "+ server.arg(i));
//     if(server.argName(i) == "plain") {
//       DynamicJsonDocument root(150);
//       DeserializationError error = deserializeJson( root, server.arg(i));
//       if (error) {
//         msg += "Formato dos dados incorretos."; DEBUG_PRINTLN("Erro Parsing");
//         return;
//       }

//       if ( root.containsKey("c_D") ){
//         int l_cmd = root["c_D"];
//         uint8_t l_dado;
//         float l_float;
//         if ( root.containsKey("c_No") ) {
//           l_dado = root["c_No"];
//         } else {
//           l_dado = 0;
//         }
//         if ( root.containsKey("c_Fo") ) {
//           l_float = root["c_Fo"];
//         } else {
//           l_float = 0;
//         }
//         // DEBUG_PRINT("Dado: "); DEBUG_PRINT(l_dado);
//         // DEBUG_PRINT("Float: "); DEBUG_PRINT(l_float);
//         switch (l_cmd) {
//           case 20:   // Habilitar Beer
//             if ( g_Beer.On() ) {
//               msg += "Beer Ativado";
//             } else {
//               msg += "Beer NÂO Ativado (Bloqueado)";
//             }
//             c_ok = true;
//             break;
//           case 21:   // Desabilitar Beer
//             if ( g_Beer.Off() ) {
//               msg += "Beer Desativado";
//             } else {
//               msg += "Beer Bloqueado";
//             }
//             c_ok = true;
//             break;  

//           case 2:   // Alterar Slave device
//             if ( l_dado < g_ATTiny_Atuador.getDeviceCount() ) {
//               g_Beer.Set_Atuador_Dev(l_dado);
//               msg += "Slave Atuador alterado.";
//               c_ok = true;
//             } else {
//               msg += "Erro ao alterar Slave Atuador.";
//             }
//             break;
//           case 3:   // Alterar Saida do Slave
//             if ( l_dado < ATTiny_MAX_ATUADOR ) {
//               g_Beer.Set_Atuador_Saida(l_dado);
//               msg += "Saida do Slave Atuador alterado.";
//               c_ok = true;
//             } else {
//               msg += "Erro ao alterar Saída do Atuador.";
//             }
//             break;
//           case 4:   // Device Sensor Interno
//             if ( l_dado < g_OW_DS.getDeviceCount() ) {
//               g_Beer.Set_Sensor_Interna(l_dado);
//               msg += "Sensor Temp Interno alterado.";
//               c_ok = true;
//             } else {
//               msg += "Erro ao alterar Sensor Temp Interno.";
//             }
//             break;
//           case 5:   // Device Sensor Temp Retorno
//             if ( l_dado < g_OW_DS.getDeviceCount() ) {
//               g_Beer.Set_Sensor_Beer1(l_dado);
//               msg += "Sensor Temp Beer1 alterado.";
//               c_ok = true;
//             } else {
//               msg += "Erro ao alterar Sensor Temp Beer1.";
//             }
//             break;
//           case 6:   // Device Sensor Temp Entrada Aquecedor
//             if ( l_dado < g_OW_DS.getDeviceCount() ) {
//               g_Beer.Set_Sensor_Beer2(l_dado);
//               msg += "Sensor Temp Beer2 alterado.";
//               c_ok = true;
//             } else {
//               msg += "Erro ao alterar Sensor Temp Beer2.";
//             }
//             break;
//           case 7:   // Device Sensor Temp Saida Aquecedor
//             if ( l_dado < g_OW_DS.getDeviceCount() ) {
//               g_Beer.Set_Sensor_Beer3(l_dado);
//               msg += "Sensor Beer3 alterado.";
//               c_ok = true;
//             } else {
//               msg += "Erro ao alterar Sensor Temp Beer3.";
//             }
//             break;

//           case 28:   // Delta Ativação do Beer
//             if ( l_dado < 5 ) {
//               g_Beer.Set_Temp_Delta_Ativacao(l_float);
//               msg += "Temp Delta Ativação alterado para " + (String)g_Beer.Get_Temp_Delta_Ativacao();
//               c_ok = true;
//             } else {
//               msg += "Erro ao alterar Temp Delta Ativação.";
//             }
//             break;
//           case 29:   // Temp Preset
//             if ( l_dado < DMD_BEER_TEMP_MAX ) {
//               g_Beer.Set_Temp_Preset(l_dado);
//               msg += "Temp Preset alterado.";
//               c_ok = true;
//             } else {
//               msg += "Erro ao alterar Sensor Temp Preset.";
//             }
//             break;
//           case 30:   // Usar Aquecedor
//             switch ( l_dado ) {
//               case 0:
//                 g_Beer.AquecedorUsar_Off();
//                 msg += "Uso do aquecedor desativado.";
//                 c_ok = true;
//                 break;
//               case 1:
//                 g_Beer.AquecedorUsar_On();
//                 msg += "Uso do aquecedor ativado.";
//                 c_ok = true;
//                 break;
//               default:
//                 msg += "Erro ao alterar Usar Aquecedor.";
//                 break;
//             }
//             break;

//           case 31:   // Fast Cool
//             switch ( l_dado ) {
//               case 0:
//                 g_Beer.FastCool_Off();
//                 msg += "Desativado o Fast Cool.";
//                 c_ok = true;
//                 break;
//               case 1:
//                 g_Beer.FastCool_On();
//                 msg += "Ativado o Fast Cool.";
//                 c_ok = true;
//                 break;
//               default:
//                 msg += "Erro ao alterar Fast Cool.";
//                 break;
//             }
//             break;

//           case 35:   // Modo Monitor
//             switch ( l_dado ) {
//               case 0:
//                 g_Beer.ModoMonitor_Off();
//                 msg += "Desativado o Modo Monitor.";
//                 c_ok = true;
//                 break;
//               case 1:
//                 g_Beer.ModoMonitor_On();
//                 msg += "Ativado o Modo Monitor.";
//                 c_ok = true;
//                 break;
//               default:
//                 msg += "Erro ao alterar Modo Monitor.";
//                 break;
//             }
//             break;
//           default:
//             msg += "Erro comando.";
//             break;
//         }
//       } else {
//         msg += "Dado inválido."; DEBUG_PRINTLNF("Erro - Dado");
//       }
//     } else {
//         msg += "Formato dos dados incorretos!"; DEBUG_PRINTLN("Erro dado");
//     }
//   } else {
//         msg += "Dados enviados incorretos."; DEBUG_PRINTLN("Erro json");
//   }

//   if ( c_ok ) {
//     msg += "\",\"Res\":\"Ok\"";
//     g_Stat.JSON_OK++;
//     DEBUG_PRINTLN("OK");
//   }
//   else {
//     msg += "\",\"Res\":\"Erro\"";
//     DEBUG_PRINTLN("Erro");
//     g_Stat.JSON_Erro++;
//   }
//   msg += "}";

//   DEBUG_PRINTLN(msg);

//   server.send(200, "application/json", msg);

//   if ( c_ok ) {
//     DMD_EEPROM_Save((char *)&g_Dados, sizeof(g_Dados));
//   }
// }


// void f_json_RF_Add_SMW210 () {
//   DEBUG_PRINT("WJ - Add SMW210 - ");

//   bool c_ok = false;
//   //int temp;
  
//   String msg = "{\"Status\":\"";
//   msg.reserve(300);


//   if ( RF_Add_Device (DMD_RF_SMW210) ) {
//     msg += "Dispositivo adicionado";
//     c_ok = true;
//   } else {
//     msg += "Erro";
//   }

//   if ( c_ok ) {
//     msg += "\",\"Res\":\"Ok\"";
//     g_Stat.JSON_OK++;
//     DEBUG_PRINTLN("OK");
//   }
//   else {
//     msg += "\",\"Res\":\"Erro\"";
//     DEBUG_PRINTLN("Erro");
//     g_Stat.JSON_Erro++;
//   }
//   msg += "}";

//   DEBUG_PRINTLN(msg);

//   server.send(200, "application/json", msg);

//   if ( c_ok ) {
//     DMD_EEPROM_Save((char *)&g_Dados, sizeof(g_Dados));
//   }
// }

// void f_json_RF_Topic () {
//   DEBUG_PRINT("WJ - Topic RF - ");

//   bool c_ok = false;
//   uint16_t l_dev, l_valor;
  
//   String msg = "{\"Status\":\"";

//   //for (uint8_t i=0; i<server.args(); i++){
//   if( server.args() == 1 ) {
//     int i = 0;
//     DEBUG_PRINTLN(" Arg -> "+ server.argName(i) + " : "+ server.arg(i));
//     if(server.argName(i) == "plain") {
//       DynamicJsonDocument root(150);
//       DeserializationError error = deserializeJson( root, server.arg(i));
//       if (error) {
//         msg += "\"Formato dos dados incorretos.\""; DEBUG_PRINTLN("Erro Parsing");
//         return;
//       }
//       DEBUG_PRINTLN("JSON parsing OK!");
//       if ( root.containsKey("c_D") && root.containsKey("c_V")){
//         l_dev = root["c_D"];  DEBUG_PRINT("c_D ");  DEBUG_PRINT(l_dev);
//         l_valor = root["c_V"];DEBUG_PRINT("c_V ");  DEBUG_PRINT(l_valor);
//         if ( l_dev < DMD_RF_MAX_DISP ) {
//             g_Dados.RF[l_dev].Topic_Index = l_valor;
//             DEBUG_PRINTLN("Tópico alterado");
//             msg += "Tópico do slave " + String(l_dev) + " alterado para tópico " + String(l_valor) ;
//             c_ok = true;
//         } else {
//           msg += "\"Device inválido.\""; DEBUG_PRINTLN("Erro device");
//         }
//       } else {
//         msg += "\"Formato dos dados incorretos!\""; DEBUG_PRINTLN("Erro dado");
//       }
//     } else {
//         msg += "\"Dados enviados incorretos.\""; DEBUG_PRINTLN("Erro json");
//     }
//   }

//   if ( c_ok ) {
//     msg += "\",\"Res\":\"Ok\"";
//     g_Stat.JSON_OK++;
//     DEBUG_PRINTLN("OK");
//   }
//   else {
//     msg += "\",\"Res\":\"Erro\"";
//     DEBUG_PRINTLN("Erro");
//     g_Stat.JSON_Erro++;
//   }

//   msg += "}";

//   DEBUG_PRINTLN(msg);

//   server.send(200, "application/json", msg);

//   if ( c_ok ) {
//     DMD_EEPROM_Save((char *)&g_Dados, sizeof(g_Dados));
//   }
// }

// void f_json_RF_Del () {
//   DEBUG_PRINT("WJ - RF Del - ");

//   bool c_ok = false;
//   uint16_t l_dev, l_valor;
  
//   String msg = "{\"Status\":\"";

//   //for (uint8_t i=0; i<server.args(); i++){
//   if( server.args() == 1 ) {
//     int i = 0;
//     DEBUG_PRINTLN(" Arg -> "+ server.argName(i) + " : "+ server.arg(i));
//     if(server.argName(i) == "plain") {
//       DynamicJsonDocument root(150);
//       DeserializationError error = deserializeJson( root, server.arg(i));
//       if (error) {
//         msg += "\"Formato dos dados incorretos.\""; DEBUG_PRINTLN("Erro Parsing");
//         return;
//       }
//       DEBUG_PRINTLN("JSON parsing OK!");
//       if ( root.containsKey("c_D") && root.containsKey("c_V")){
//         l_dev = root["c_D"];  DEBUG_PRINT("c_D ");  DEBUG_PRINT(l_dev);
//         l_valor = root["c_V"];DEBUG_PRINT("c_V ");  DEBUG_PRINT(l_valor);
//         if ( l_dev < DMD_RF_MAX_DISP ) {
//             RF_Delete_Sensor(l_dev);
//             DEBUG_PRINTLN("SW210 deletado");
//             msg += "SW210 " + String(l_dev) + " deletado.";
//             c_ok = true;
//         } else {
//           msg += "\"Device inválido.\""; DEBUG_PRINTLN("Erro device");
//         }
//       } else {
//         msg += "\"Formato dos dados incorretos!\""; DEBUG_PRINTLN("Erro dado");
//       }
//     } else {
//         msg += "\"Dados enviados incorretos.\""; DEBUG_PRINTLN("Erro json");
//     }
//   }

//   if ( c_ok ) {
//     msg += "\",\"Res\":\"Ok\"";
//     g_Stat.JSON_OK++;
//     DEBUG_PRINTLN("OK");
//   }
//   else {
//     msg += "\",\"Res\":\"Erro\"";
//     DEBUG_PRINTLN("Erro");
//     g_Stat.JSON_Erro++;
//   }

//   msg += "}";

//   DEBUG_PRINTLN(msg);

//   server.send(200, "application/json", msg);

//   if ( c_ok ) {
//     DMD_EEPROM_Save((char *)&g_Dados, sizeof(g_Dados));
//   }
// }

