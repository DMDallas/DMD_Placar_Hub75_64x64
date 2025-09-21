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

#include "DMD_WebServer_Prog.h"

#include "DMD_Matrix_Hub75.h"
#include "DMD__Placa.h"

/*
 * Ajax JSON - Inicializa as funções
 *  Registra a funções na classe Server
 */
void DMD_WebServer_Init_Prog_JSON(){
  // Respostas JSON
  server.on("/", f_handleRoot_Prog);
  
  server.on("/json_din", f_json_dinamico);
}

void f_handleRoot_Prog() {
  server.send(200, "text/html; charset=utf-8", html_base);
}

void f_insert_dados(String *msg){
  *msg += "\"Sis\":\"" + String(SYSTEM_NAME) + "\",";
  *msg += "\"Nome\":\"" + String(g_Dados.Name) + "\",";

  time_t f_now = time(nullptr);
  tm* f_tm = localtime(&f_now);
  char buffer[80];
  strftime (buffer,80," %d %B %Y %H:%M:%S ",f_tm);
  *msg += "\"Hora\":\"" + String(buffer) + "\",";
}

void f_insert_info(String *msg){
  *msg += ",{\"tit\":\"Info\", \"item\":[";
  *msg += "{\"tipo\":0,\"texto\":\"Sistema: " + String(SYSTEM_NAME) + "\"},";
  *msg += "{\"tipo\":0,\"texto\":\"Nome da placa: " + String(g_Dados.Name) + "\"},";
  // if ( g_MQTT.IsConnected() ) 
  //   *msg += "{\"tipo\":0,\"texto\":\"Servidor: Online\"},";
  // else 
  //   *msg += "{\"tipo\":0,\"texto\":\"Servidor: Offline\"},";
  *msg += "{\"tipo\":0,\"texto\":\"FW: "+String(VERSION_FW)+" ("+String(g_Dados.version_data)+ ")\"},";
  *msg += "{\"tipo\":0,\"texto\":\"Ligado a: " + String (g_contador_seg) + " segundos\"},";
  *msg += "{\"tipo\":0,\"texto\":\"Loops: " + String(g_contador_loop) + "\"}";
  *msg += "]}";
}


void f_json_dinamico (){
  DEBUG_PRINT("WJ - DIN - ");
  int l_cod, l_dad, l_sub;
  bool l_save = false;
  
  String msg = "{";

  f_insert_dados(&msg);

  msg += "\"caixa\":[";

  //for (uint8_t i=0; i<server.args(); i++){
  if( server.args() != 1 ) {
    msg += "{\"tit\":\"Dados incorretos\", \"item\": []} ]}"; DEBUG_PRINTLN("Erro json");
    server.send(200, "application/json", msg);
    g_Stat.JSON_Erro++;
    return;
  }

  DEBUG_PRINTLN(" Arg -> "+ server.argName(0) + " : "+ server.arg(0));

  if(server.argName(0) != "plain"){
    msg += "{\"tit\":\"Formato dos dados incorretos!\", \"item\":[]} ]}"; DEBUG_PRINTLN("Erro dado");
    server.send(200, "application/json", msg);
    g_Stat.JSON_Erro++;
    return;
  }

  JsonDocument root;
  DeserializationError error = deserializeJson( root, server.arg(0));
  if (error) {
    msg += "{\"tit\":\"Erro no Parsing do Json\", \"item\":[]} ]}"; DEBUG_PRINTLN("Erro Parsing");
    server.send(200, "application/json", msg);
    g_Stat.JSON_Erro++;
    return;
  }

  DEBUG_PRINTLN("JSON parsing OK!");

  l_cod = root["c_Cod"]; DEBUG_PRINTF(" c_Cod="); DEBUG_PRINT(l_cod);
  l_sub = root["c_Sub"]; DEBUG_PRINTF(" c_Sub="); DEBUG_PRINT(l_sub);
  
  if ( l_cod == 0 || l_sub == 0 ) {
    msg += "{\"tit\":\"Erro dos parâmetros\", \"item\":[]} ]}"; DEBUG_PRINTLN("Erro param");
    server.send(200, "application/json", msg);
    g_Stat.JSON_Erro++;
    return;
  }

  if ( root["c_Dad"].is<int>() )  {
    l_dad = root["c_Dad"];
  } else {
    l_dad = -1;
  }

  msg += "{}";

  DEBUG_PRINTF(" c_Dad="); DEBUG_PRINTLN(l_dad);

  switch (l_cod) {
    case 1:
      switch (l_sub) {
        case 10: DMD_Start_Stop(); break;
        case 11: DMD_Clear(); break;
        // case 12: DMD_Level_UP(); break;
        // case 13: DMD_Level_Down(); break;
      }

      msg += ",";
      msg += "{\"tit\":\"Dados da Tela\", \"item\":[";
      msg += "{\"tipo\":0,\"texto\":\"Tela: " + String(g_DadoTela.c_tela) + "\"},";
      if ( g_DadoTela.Is_Running() )
        msg += "{\"tipo\":0,\"texto\":\"Status: Rodando\"},";
      else
        msg += "{\"tipo\":0,\"texto\":\"Status: Parado\"},";
      msg += "{\"tipo\":0,\"texto\":\"Tempo: " + String(g_DadoTela.c_run_tempo_str) + "\"},";
      msg += "{\"tipo\":0,\"texto\":\"Placar: " + String(g_DadoTela.c_pontos[0]) + "x" + String(g_DadoTela.c_pontos[1]) + "\"},";
      // msg += "{\"tipo\":0,\"texto\":\"BT:";
      //   for (size_t i = 0; i < DMD_LOG_TECLAS; i++) {
      //     if ( g_DMD_Log_Teclas[i] != 0) {
      //       msg += " " + String(g_DMD_Log_Teclas[i]) + " - ";
      //     } else {
      //       msg += " - ";
      //     }
      //   }
      //   msg +="\"},";
      msg += "{\"tipo\":2,\"texto\":\"Start/Stop\", \"cmd\":1, \"sub\":10 },";
      msg += "{\"tipo\":2,\"texto\":\"Clear\", \"cmd\":1, \"sub\":11 }";
      msg += "]}";

      f_insert_info(&msg);
      break;

    case 195:
      switch (l_sub) {
        case 5:
          if (l_dad > 0 ) { // numero negativo ou zero é erro
            DEBUG_PRINTF(" Tempo alterado para "); DEBUG_PRINTLN(l_dad);
            g_Dados.RF_Repeat_Time = (uint16_t)l_dad;
            l_save = true;
          }
          break;
        case 9:
          l_save = true;
          break;
        case 10: case 11: case 12: case 13:
        case 14: case 15: case 16: case 17:
        case 18: case 19: case 20: case 21: case 22:
          g_Dados.RF[l_sub-10].BT = l_dad;
          g_RC_RF.Add_Device (l_sub-10);
          // l_save = true;       
        default:
          break;
      }

      msg += ",";
      msg += "{\"tit\":\"Config RF\", \"item\":[";
      for (size_t i = 0; i < DMD_RC_RF_MAX_BT; i++) {
        msg += "{\"tipo\":10, \"id\":\"s_bt" + String(i) + "\",\"texto\":\"Botão " + String(i) + " - Addr:" + String(g_Dados.RF[i].Address);
        msg += "\",\"cmd\":195,\"sub\":" + String(10+i) + ",\"dado\":" + String(g_Dados.RF[i].BT) + ",\"status\":\"";
        switch (g_Dados.RF[i].State) {
          default:
          case DMD_RF_Data_OFF:      msg += "OFF\"},";      break;
          case DMD_RF_Data_ACTIV:    msg += "ACTIV\"},";    break;
          case DMD_RF_Data_Wait_ADD: msg += "Wait_Add\"},"; break;
          case DMD_RF_Data_END:      msg += "END\"},";      break;
        }
      }
      msg += "{\"tipo\":10, \"id\":\"s_tid\",\"texto\":\"Tempo repetição\",\"cmd\":195,\"sub\":5,\"dado\":" +
              String(g_Dados.RF_Repeat_Time) + "},";
      msg += "{\"tipo\":2,\"texto\":\"Salvar dados\", \"cmd\":195, \"sub\":9 }";
      msg += "]}";
      break;

    case 196:
      switch (l_sub) {
        case 2:
          if (l_dad > 0 ) { // numero negativo ou zero é erro
            g_Dados.c_Color_Hour = (uint16_t)l_dad;
            l_save = true;
          }
          break;
        case 3:
          if (l_dad > 0 ) { // numero negativo ou zero é erro
            g_Dados.c_Color_Hour_Stop = (uint16_t)l_dad;
            l_save = true;
          }
          break;
        case 4:
          if (l_dad > 0 ) { // numero negativo ou zero é erro
            g_Dados.c_Color_Time1 = (uint16_t)l_dad;
            l_save = true;
          }
          break;
        case 5:
          if (l_dad > 0 ) { // numero negativo ou zero é erro
            g_Dados.c_Color_Time2 = (uint16_t)l_dad;
            l_save = true;
          }
          break;
        case 6:
          if (l_dad > 0 ) { // numero negativo ou zero é erro
            g_Dados.c_Color_Textos = (uint16_t)l_dad;
            l_save = true;
          }
          break;
        case 10:
          if (l_dad > 0 ) { // numero negativo ou zero é erro
            DEBUG_PRINTF(" Tempo Idle alterado para "); DEBUG_PRINTLN(l_dad);
            g_Dados.Tempo_Idle = (uint16_t)l_dad;
            l_save = true;
          }
          break;
        default:  break;
      }

      msg += ",";
      msg += "{\"tit\":\"Config Cores\", \"item\":[";
      msg += "{\"tipo\":10, \"id\":\"c_H\",\"texto\":\"Cor Hora\",\"cmd\":196,\"sub\":2,\"dado\":" + String(g_Dados.c_Color_Hour) + "},";
      msg += "{\"tipo\":10, \"id\":\"c_HS\",\"texto\":\"Cor Hora Stop\",\"cmd\":196,\"sub\":3,\"dado\":" + String(g_Dados.c_Color_Hour_Stop) + "},";
      msg += "{\"tipo\":10, \"id\":\"c_Time1\",\"texto\":\"Cor Time 1\",\"cmd\":196,\"sub\":4,\"dado\":" + String(g_Dados.c_Color_Time1) + "},";
      msg += "{\"tipo\":10, \"id\":\"c_Time2\",\"texto\":\"Cor Time 2\",\"cmd\":196,\"sub\":5,\"dado\":" + String(g_Dados.c_Color_Time2) + "},";
      msg += "{\"tipo\":10, \"id\":\"c_textos\",\"texto\":\"Cor Textos\",\"cmd\":196,\"sub\":6,\"dado\":" + String(g_Dados.c_Color_Textos) + "},";
      msg += "{\"tipo\":0,\"texto\":\"Azul - 31 | Verde - 2016 | Vermelhor - 63488 | Branco - 65535 | Vermelho claro - 64490 | Laranja - 64576 | Amarelo - 65504\", \"cmd\":196, \"sub\":1 },";
      msg += "{\"tipo\":1,\"texto\":\"Link para Site com conversor\", \"link\":\"https://rgbcolorpicker.com/565\"},";
      msg += "{\"tipo\":2,\"texto\":\"Atualizar\", \"cmd\":196, \"sub\":1 },";
      msg += "{\"tipo\":10, \"id\":\"s_tid\",\"texto\":\"Tempo Idle\",\"cmd\":196,\"sub\":10,\"dado\":" +
          String(g_Dados.Tempo_Idle) + "}";

      msg += "]}";
      break;


    default:
      break;
  }
  msg += "]}";

  DEBUG_PRINTLN(msg);
  server.send(200, "application/json", msg);

  if ( l_save ) {
    DMD_EEPROM_Save((char *)&g_Dados, sizeof(g_Dados));
  }

  g_Stat.JSON_OK++;
}
