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
 *           ESP8266HTTPUpdateServer
 * 
 * Utiliza a memória interna do ESP8266
  ******************************************************************************/
/*
 * Definições do Projeto
 */
#include "DMD_Config.h"

/*
 * Inclusao de libraries
 */
#include <TimeLib.h>

// #include <ESP8266HTTPUpdateServer.h>
#include <HTTPUpdate.h>

/*
 * Inclusao funções e dados do projeto
 */
#include "DMD_EEPROM.h"
#include "DMD_WiFi.h"
#include "DMD_Stat.h"
#include "DMD_Globais.h"
#include "DMD_WebServer.h"
// #include "DMD_HTTP_Update.h"

#include "DMD_WebServer_JSON.h"

#include "DMD_WebServer_Prog.h"

// #include <ESP8266httpUpdate.h>

// #include "DMD_OW_ATtiny_Int.h"
// #include "DMD_OW_ATtiny_Atuador.h"
// #include "DMD_OW_ATtiny_PWM.h"
// #include "DMD_OW_ATtiny_Medidor.h"
// #include "DMD_OW_ProMini_Atuador.h"
// #include "DMD_OW_DS18B20.h"
// #include "DMD_TM1637.h"

#include "DMD_MQTT.h"

#include <HTTPUpdateServer.h>
HTTPUpdateServer httpUpdater;

 /*
  * Variaveis globais
  */
WebServer server(80);

// ESP8266HTTPUpdateServer httpUpdater;

/*
 * Inicializa o Servidor Web registrando os callback da rotas
 *    Rotas registradas
 *      /           > Tela principal
 *    Rotas protegidas por senha
 *      /wifi       > Configuração do Wifi
 *      /config_relay
 *      /config_pw
 *      /info
 *      /factoryreset
 *      /reset
 *      /firmware
 *      /config_ns
 */
void DMD_WebServer_Init ( ){
  DEBUG_PRINTLN("-- WebServer Init --");

  // server.on("/", f_handleRoot);
  
  DMD_WebServer_Init_Prog_JSON();

  server.on("/wifi", f_handleWifi);

  server.on("/config", f_handleConfig);
  // server.on("/config_link", f_handleConfigLink);
  server.on("/config_nome", f_handleConfigNome);
  server.on("/config_ip", f_handleConfigIp);
  // server.on("/config_int", f_handleConfigInt);
  // server.on("/config_int_cor", f_handleConfigIntColorSch);

  // server.on("/config_int_debounce", f_handleConfigDebounce);
  // server.on("/config_medidor", f_handleConfigMedidor);
  // server.on("/config_pwm", f_handleConfigPWM);

  // server.on("/config_topicos", f_handleConfigTopicos);
  // server.on("/config_temp", f_handleConfigTopicoTemp);

  // server.on("/config_rf", f_handleConfigRF);
  
  // server.on("/config_aq_solar", f_handleConfigAquecSolar);

  // server.on("/config_beer", f_handleConfigBeer);

  // server.on("/config_relogio", f_handleConfigRelogio);
  server.on("/config_pw", f_handleConfigLocalPW);

  // server.on("/get_update", f_GetWebUpdate);

  // if ( Check_NS() ) {
  //   server.on("/config_ns", f_handleConfigNS);
  // }

  server.on("/info", f_handleInfo);
  
  server.on("/factoryreset", f_reset_factory);
  server.on("/eraseconfig", f_eraseconfig);
  server.on("/reset", f_reset);

  // Se existir o arquivo envia para o cliente
  server.onNotFound([]() { server.send(404, "text/plain", "404: Not found"); });

  DMD_WebServer_Init_JSON();

  server.begin();

  httpUpdater.setup(&server);
  server.on("/firmware", HTTP_GET,f_update);

  DEBUG_PRINTLN("-- WebServer Init --");
}

/*
 *  Função do Loop principal do servidor Web
 */
void DMD_WebServer_Handler () {
  server.handleClient();
}

/*
 *  Função para inserir o cabeçalho na string da página
 *    msg   -> Ponteiro para a String da mensagem
 *    maxW  -> largura da página a ser indicada para o navegador
 */
void f_html_header(String *msg, int maxW){
  // Cabeçalho do HTML
  *msg = "<html><head><title>"+String(SYSTEM_NAME)+"</title>";
  *msg += "<meta http-equiv=\"X-UA-Compatible\" content=\"IE-edge\">";
  *msg += "<meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">";

  // Style
  *msg += "<style>body{background-color:#f5f5f5;color:#555;margin:auto;text-align:center;";
  *msg += "font-family:\"Helvetica Neue\",Helvetica,Arial,sans-serif;";
  *msg += "max-width: " + String(maxW) + "px;}";
  *msg += ".b{margin: 5px;border:1px solid #ABC3D1;border-radius:15px;}";
  *msg += ".m{padding:5px;margin:6px 5px;font-size:2.0em;";
  *msg += "background-color:#B3CFDE;border-radius:15px;cursor:pointer;}";
  *msg += ".m_t{font-size:0.3em;}";
  *msg += ".g{margin:5px;display:grid;grid-gap:5px;align-content: center;";
  *msg += "grid-template-columns: repeat(auto-fit, minmax(200px, 2fr));}";
  *msg += ".g_d{padding:5px 0px;border:1px solid #ABC3D1;border-radius:15px;}";
  *msg += ".g_d_s{font-size:0.6em;}";
  *msg += "p, .p_h{margin:5px;padding:4px 5px;background-color:#D0E3ED;";
  *msg += "border-radius:15px;font-size:0.8em;}";
  *msg += ".p_h:hover{background-color:#BADAEC;cursor:pointer;}";
  *msg += "input {display: block;width:100%;padding: 1px;border-radius: 15px;border: 0px;";
  *msg += "background-color:#f5f5f5;text-align:center;}";
  *msg += "input:hover, input:focus {opacity: .7;color:#08c;";
  *msg += "box-shadow: 0px 1px 0px rgba(255,255,255,.25),inset 0px 3px 6px rgba(0,0,0,.25);}";
  *msg += "button, .btn{background:rgba(255,255,255,0);width: 100%;border: 0px;color:#555;}";
  *msg += "input[type=file] {display: none;}";
  *msg += "#txt{display: none;}";
  *msg += ".i_g{display:grid;grid-template-columns: repeat(auto-fit, minmax(50px, 2fr));}";
  *msg += ".i_t{font-size:0.8em;}";
  *msg += ".At{font-size:2.0em;background-color:#f5f5f5;}";
  *msg += ".Mx {background-color:#EDD0D4;}";
  *msg += ".Md {background-color:#D4EDD0;}";
  *msg += ".Mi {background-color:#D6E6EF;}";
  *msg += "</style></head>\n";

  // Titulo
  *msg += "<body><div class=\"b\"><div class=\"m\" onclick=\"location.href='/'\">";
  *msg += "<div class=\"m_t\">Sistema Embarcado</div><div>";
  *msg += String(SYSTEM_NAME)+" - "+String(g_Dados.Name)+"</div>";
  time_t now = time(nullptr);
  *msg += "<div class=\"m_t\">" +  String(ctime(&now))  + "</div>";
  *msg += "</div>";
}

/*
 *  Insere na msg recebida o código para a imagem de carregando
 */
void f_insert_load(String *msg){

  *msg += "<svg width=\"57\" height=\"57\" viewBox=\"0 0 57 57\" stroke=\"#BADAEC\">";
  *msg += "<g fill=\"none\" fill-rule=\"evenodd\">";
  *msg += "<g transform=\"translate(1 1)\" stroke-width=\"2\">";
  *msg += "<circle cx=\"5\" cy=\"50\" r=\"5\">";
  *msg += "<animate attributeName=\"cy\"begin=\"0s\" dur=\"2.2s\"values=\"50;5;50;50\"";
  *msg += "calcMode=\"linear\"repeatCount=\"indefinite\" />";
  *msg += "<animate attributeName=\"cx\"begin=\"0s\" dur=\"2.2s\"";
  *msg += "values=\"5;27;49;5\"calcMode=\"linear\"repeatCount=\"indefinite\" /></circle>";
  *msg += "<circle cx=\"27\" cy=\"5\" r=\"5\"><animate attributeName=\"cy\"";
  *msg += "begin=\"0s\" dur=\"2.2s\"from=\"5\" to=\"5\"values=\"5;50;50;5\"calcMode=\"linear\"";
  *msg += "repeatCount=\"indefinite\" />";
  *msg += "<animate attributeName=\"cx\"begin=\"0s\" dur=\"2.2s\"from=\"27\" to=\"27\"";
  *msg += "values=\"27;49;5;27\"calcMode=\"linear\"repeatCount=\"indefinite\" /></circle>";
  *msg += "<circle cx=\"49\" cy=\"50\" r=\"5\"><animate attributeName=\"cy\"";
  *msg += "begin=\"0s\" dur=\"2.2s\"values=\"50;50;5;50\"calcMode=\"linear\"";
  *msg += "repeatCount=\"indefinite\" />";
  *msg += "<animate attributeName=\"cx\"from=\"49\" to=\"49\"begin=\"0s\" dur=\"2.2s\"";
  *msg += "values=\"49;5;27;49\"calcMode=\"linear\"repeatCount=\"indefinite\"/>";
  *msg += "</circle></g></g></svg>";
}


void f_handleRoot() {
  g_Stat.Web_GET++;
  DEBUG_PRINTF(" Web - Root");

  String msg;
  msg.reserve(4000);
  f_html_header(&msg,630);
  
  msg += "<div class=\"g\">";

  // for ( int i = 0 ; i < g_ATTiny_Int.getDeviceCount(); i++ ) {
  //   msg += "<div class=\"g_d\"> Interruptor: " + String(i);
  //   msg += "<p onclick=\"f_int("+ String(i)+",0)\" ";
  //   (g_ATTiny_Int.Read_Int_Status(i,0)? msg += "class=\"Md\" > Interruptor 0: ON</p>" : msg += "class=\"Mx\"> Interruptor 0: OFF</p>" );
  //   msg += "<p onclick=\"f_int("+ String(i)+",1)\" ";
  //   (g_ATTiny_Int.Read_Int_Status(i,1)? msg += "class=\"Md\"> Interruptor 1: ON</p>" : msg += "class=\"Mx\"> Interruptor 1: OFF</p>" );
  //   msg += "<p onclick=\"f_int("+ String(i)+",2)\" ";
  //   (g_ATTiny_Int.Read_Int_Status(i,2)? msg += "class=\"Md\"> Interruptor 2: ON</p>" : msg += "class=\"Mx\"> Interruptor 2: OFF</p>" );
  //   msg += "</div>";
  // }

  // for ( int i = 0 ; i < g_ATTiny_Atuador.getDeviceCount(); i++ ) {
  //   msg += "<div class=\"g_d\"> Atuador: " + String(i);
  //   msg += "<p onclick=\"f_int(1" + String(i) + ",0)\" class=\"";
  //   (g_ATTiny_Atuador.GetStatus(i,0)? msg += "Md\"> Saída 0: ON</p>" : msg += "Mx\"> Saída 0: OFF</p>");
  //   msg += "<p onclick=\"f_int(1" + String(i) + ",1)\" class=\"";
  //   (g_ATTiny_Atuador.GetStatus(i,1)? msg += "Md\"> Saída 1: ON</p>" : msg += "Mx\"> Saída 1: OFF</p>");
  //   msg += "<p onclick=\"f_int(1" + String(i) + ",2)\" class=\"";
  //   (g_ATTiny_Atuador.GetStatus(i,2)? msg += "Md\"> Saída 2: ON</p>" : msg += "Mx\"> Saída 2: OFF</p>");
  //   msg += "</div>";
  // }

  // for ( int i = 0 ; i < g_ATTiny_PWM.getDeviceCount(); i++ ) {
  //   msg += "<div class=\"g_d\"> PWM: " + String(i);
  //   msg += "<p> PWM 0:" + String(g_ATTiny_PWM.GetPWM(i,0)) + "</p>";
  //   msg += "<p> PWM 1:" + String(g_ATTiny_PWM.GetPWM(i,1)) + "</p>";
  //   msg += "</div>";
  // }

  // if ( g_ProMini_Atuador.GetDevStatus() ) {
  //   msg += "<div class=\"g_d\"> ProMini: ";
  //   (g_ProMini_Atuador.IsOnline() ? msg += "Online" : msg += "Offline");
  //   for (uint8_t i = 0; i < ProMini_MAX_ATUADOR ; i++) {
  //     msg += "<p onclick=\"f_int(20,"+ String(i)+")\" class=\"";
  //     ( g_ProMini_Atuador.GetStatus(i) ? msg +="Md": msg += "Mx");
  //     msg += "\"> Saída " + String(i) + ": ";
  //     ( g_ProMini_Atuador.GetStatus(i) ? msg += "ON</p>" : msg += "OFF</p>");
  //   }
  //   for (uint8_t i = 0; i < ProMini_MAX_PWM ; i++) {
  //     msg += "<p> PWM "+ String(i) + ": " + String(g_ProMini_Atuador.GetPWM(i)) + "</p>";
  //   }

  //   msg += "</div>";
  // }

  // if ( g_OW_DS.IsPresent() ) {
  //   msg += "<div class=\"g_d\"> DS18B20 "; //+ String(g_OW_DS.getDeviceCount());
  //   for (uint8_t i = 0; i < g_OW_DS.getDeviceCount() ; i++) {
  //     msg += "<p class=\"Md\"> Temp " + String(i) + ": ";
  //     if (g_OW_DS.HasData(i) ) {
  //       msg += String(g_OW_DS.GetTemp_MedNoRead(i)) + " ºC - ";
  //     } else {
  //       msg += "ND - ";
  //     }
  //     // if ( g_Dados.Temp_Topico_Index[i] < DADOS_TOPIC_MAX_INDEX) {
  //     //   msg += g_Dados.Topicos[g_Dados.Temp_Topico_Index[i]]; 
  //     // } else {
  //     //   msg += "-"; 
  //     // }

  //     msg += "</p>";
  //   }
  //   msg += "</div>";
  // }

  // if ( g_ATTiny_Med.getDeviceCount() > 0 ) {
  //   msg += "<div class=\"g_d\">Medidores"; // + String(g_ATTiny_Med.getDeviceCount());
  //   for (uint8_t i = 0; i < g_ATTiny_Med.getDeviceCount() ; i++) {
  //     msg += "<p class=\"Md\">Med(" + String(i) + ") ";
  //     if (g_ATTiny_Med.HasData(i) ) {
  //       msg += String(g_ATTiny_Med.GetMedidaNoRead_cm(i)) + " cm (" + String(g_ATTiny_Med.GetMedidaNoRead(i)) + ") " + String(g_ATTiny_Med.GetPercentNoRead(i)) +  " %</p>";
  //     } else {
  //       msg += "ND</p>";
  //     }
  //   }
  //   msg += "</div>";
  // }

  msg += "<div class=\"g_d\">Info";
  msg += "<p>Nome: "+String(g_Dados.Name)+"</p>";
 
  if ( g_MQTT.IsConnected() ) 
    msg += "<p>Servidor: Online</p>";
  else 
    msg += "<p class=\"Mx\">Servidor: Offline </p>";
  msg += "<p>FW: "+String(VERSION_FW)+" ("+String(g_Dados.version_data)+")</p>";

  msg += "<p>Ligado a: " + String (g_contador_seg) + " segundos</p>";
  msg += "<p>Loops: "+String(g_contador_loop)+"</p>";
  msg += "</div>";

  msg += "<div class=\"g_d\">Configurações";
  msg += "<p class=\"p_h\" onclick=\"location.href='/config_link'\">Geral</p>";
  msg += "<p class=\"p_h\" onclick=\"location.href='/config'\">Slaves</p>";
  msg += "<p class=\"p_h\" onclick=\"location.href='/config_topicos'\">Tópicos</p>";
  msg += "<p class=\"p_h\" onclick=\"location.href='/config_temp'\">Tópicos Temp</p>";
  // if ( g_ATTiny_Med.getDeviceCount() > 0 ) {
  //   msg += "<p class=\"p_h\" onclick=\"location.href='/config_medidor'\">Medidores</p>";
  // }
  msg += "<p class=\"p_h\" onclick=\"location.href='/config_relogio'\">Relogio</p>";
  msg += "<p class=\"p_h\" onclick=\"location.href='/config_aq_solar'\">Aquecedor Solar</p>";
  msg += "<p class=\"p_h\" onclick=\"location.href='/config_beer'\">Beer</p>";
  msg += "</div>";
  msg += "</div></div>";

  msg += "\n<script>";
  msg += "function f_int(x,y){\n";
  msg +=   "const params={c_D:x,c_I:y};\n";
  msg +=   "const http=new XMLHttpRequest();\n";
  msg +=   "http.open('POST','/json_cmd',true);";
  msg +=   "http.setRequestHeader('Content-type','application/json');";
  msg +=   "http.send(JSON.stringify(params));";
  msg +=   "http.onload=function(){";
  msg +=   "data_json=JSON.parse(http.responseText);\n";
  msg +=   "console.log(data_json);\n";
  msg +=   "if(data_json.Res==\"Ok\"){\n";
  msg +=     "alert(data_json.Status);\n";
  // msg +=     "c_txt=document.getElementById('txt');";
  // msg +=     "c_txt.style.display=\"block\";";
  // msg +=     "c_txt.innerHTML=data_json.Status";
  msg += "  }else{alert(data_json.Status);}\n";
//  msg +=     "c_b_1.disabled = false; c_b_1.innerHTML = \"Remover\";";
  msg += "  };";
  msg += "}\n";
  msg += "</script>";

  msg += "</body></html>";

  server.send(200, "text/html; charset=utf-8", msg);
}


// void f_handleConfigLink() {
//   g_Stat.Web_GET++;
//   DEBUG_PRINTF(" Web - Links");

//   String msg;
//   msg.reserve(2000);
//   f_html_header(&msg,630);
//   msg += "<div class=\"g\">";

//   msg += "<div class=\"g_d\">Detectados";
//   msg += "<p>DS18b20: " +String(g_OW_DS.getDeviceCount());//  ( g_OW_DS.IsPresent() ? msg += "Detectado" : msg += "Não detectado"); msg +="</p>";
//   msg += "<p>Interruptor: " + String (g_ATTiny_Int.getDeviceCount()) + "</p>";
//   msg += "<p>Atuador: " + String (g_ATTiny_Atuador.getDeviceCount()) + "</p>";
//   msg += "<p>PWM: " + String (g_ATTiny_PWM.getDeviceCount()) + "</p>";
//   msg += "<p>Medidor: " + String (g_ATTiny_Med.getDeviceCount()) + "</p>";
//   msg += "<p>ProMini: "; ( g_ProMini_Atuador.GetDevStatus() ? msg += "On" : msg += "Off"); msg += "</p>";
//   if ( g_Relogio.IsOn() ) {
//     #ifdef MODULO_RELOGIO_TM1637_QUARTO
//       msg += "<p>Relogio On (Quarto)</p>";
//     #else
//       msg += "<p>Relogio On</p>";
//     #endif
//   } else {
//     msg += "<p>Relogio Off</p>";
//   }
//   msg += "</div>";

//   msg += "<div class=\"g_d\">Conexão";
//   msg += "<p>Wifi: ";
//   switch (WiFi.status()) {
//     case 0: msg += "Alterando </p>"; break;
//     case 1: msg += "Rede não encontrada </p>"; break;
//     case 3: msg += "Conectado </p>"; break;
//     case 4: msg += "Senha incorreta </p>"; break;
//     case 6: msg += "Modo Desligado </p>"; break;
//     default: msg += "Indefinido </p>"; break;
//   }
//   msg += "<p>Rede: "+WiFi.SSID()+" ("+String(WiFi.RSSI())+" dBm)</p>";
//   msg += "<p>IP: "+WiFi.localIP().toString()+"</p>";
//   //msg += "<p>Mac: "+WiFi.macAddress()+"</p>";
//   msg += "</div>";

//   msg += "<div class=\"g_d\">Configurações";
//   msg += "<p class=\"p_h\" onclick=\"location.href='/config_nome'\">Nome da Placa</p>";
//   msg += "<p class=\"p_h\" onclick=\"location.href='/config_ip'\">IP do Servidor</p>";
//   msg += "<p class=\"p_h\" onclick=\"location.href='/config'\">Slaves</p>";
//   msg += "<p class=\"p_h\" onclick=\"location.href='/config_pwm'\">PWM Máximo</p>";
//   msg += "<p class=\"p_h\" onclick=\"location.href='/config_int'\">Interruptor</p>";
//   msg += "<p class=\"p_h\" onclick=\"location.href='/config_topicos'\">Tópicos</p>";
//   msg += "<p class=\"p_h\" onclick=\"location.href='/config_temp'\">Tópicos Temp</p>";
//   msg += "<p class=\"p_h\" onclick=\"location.href='/config_int_cor'\">Esquema de Cor</p>";
//   msg += "<p class=\"p_h\" onclick=\"location.href='/config_int_debounce'\">Debounce</p>";
//   if ( g_ATTiny_Med.getDeviceCount() > 0 ) {
//     msg += "<p class=\"p_h\" onclick=\"location.href='/config_medidor'\">Medidores</p>";
//   }
//   msg += "<p class=\"p_h\" onclick=\"location.href='/config_relogio'\">Relogio</p>";
//   msg += "<p class=\"p_h\" onclick=\"location.href='/config_aq_solar'\">Aquecedor Solar</p>";
//   msg += "<p class=\"p_h\" onclick=\"location.href='/wifi'\">Wi-Fi</p>";
//   msg += "<p class=\"p_h\" onclick=\"location.href='/config_pw'\">Senha Local</p>";
//   msg += "</div>";
//   msg += "</div></div>";

//   msg += "</body></html>";

//   server.send(200, "text/html; charset=utf-8", msg);
// }

void f_handleInfo() {
  g_Stat.Web_GET++;

  if (!server.authenticate(DEFAULT_LOCAL_USER_NAME, g_Dados.Local_PW)) {
    return server.requestAuthentication();
  }

  String msg;
  msg.reserve(1500);
  f_html_header(&msg,630);

  msg += "<div class=\"g\">";

  msg += "<div class=\"g_d\">Dados do Processador";
  // msg += "<p>ChipID: "+String(ESP.getChipId())+"</p>";
  msg += "<p>CpuFreqMHz: "+String(ESP.getCpuFreqMHz())+"</p>";
  msg += "<p>FreeHeap: "+String(ESP.getFreeHeap())+"</p>";
  msg += "<p>SdkVersion: "+String(ESP.getSdkVersion())+"</p>";
  // msg += "<p>CoreVersion: "+ESP.getCoreVersion()+"</p>";
//  msg += "<p>FullVersion: "+ESP.getFullVersion()+"</p>";
  // msg += "<p>BootVersion: "+String(ESP.getBootVersion())+"</p>";
  // msg += "<p>BootMode: "+String(ESP.getBootMode())+"</p>";
  // msg += "<p>getResetReason: "+String(ESP.getResetReason())+"</p>";
  // msg += "<p>getResetInfo: "+ESP.getResetInfo()+"</p>";
  msg += "</div>";

  msg += "<div class=\"g_d\">Dados da Flash";
  // msg += "<p>FlashChipId: "+String(ESP.getFlashChipId())+"</p>";
  // msg += "<p>FlashChipRealSize: "+String(ESP.getFlashChipRealSize())+"</p>";
  msg += "<p>FlashChipSize: "+String(ESP.getFlashChipSize())+"</p>";
  msg += "<p>FlashChipSpeed: "+String(ESP.getFlashChipSpeed())+"</p>";
  msg += "<p>FlashChipMode: "+String(ESP.getFlashChipMode())+"</p>";
  // msg += "<p>FlashChipSizeByChipId: "+String(ESP.getFlashChipSizeByChipId())+"</p>";
  msg += "</div>";

  msg += "<div class=\"g_d\">Dados da Programa";
  msg += "<p>Nome do Sistema: "+String(SYSTEM_NAME)+"</p>";
  msg += "<p>Versão do Firmware: "+String(VERSION_FW)+"</p>";
  msg += "<p>Versão dos Dados: "+String(g_Dados.version_data)+"</p>";
  msg += "<p>SketchSize: "+String(ESP.getSketchSize())+"</p>";
  msg += "<p>SketchMD5: "+ESP.getSketchMD5()+"</p>";
  msg += "<p>FreeSketchSpace: "+String(ESP.getFreeSketchSpace())+"</p>";
  msg += "</div>";

  f_wifi_box (&msg);

  msg += "</div></div></body></html>";

  server.send(200, "text/html; charset=utf-8", msg);
}

void f_wifi_box (String *msg, bool duplo){

  if ( duplo )
    *msg += "<div class=\"g_d\" style=\"grid-row-start: 1;grid-row-end: 3;\">Dados da Conexão";
  else
    *msg += "<div class=\"g_d\">Dados da Conexão";

  switch (WiFi.status()) {
    case 0: *msg += "<p>Status: Alterando </p>"; break;
    case 1: *msg += "<p>Status: Rede não encontrada </p>"; break;
    case 3: *msg += "<p>Status: Conectado </p>"; break;
    case 4: *msg += "<p>Status: Senha incorreta </p>"; break;
    case 6: *msg += "<p>Status: Modo Desligado </p>"; break;
    default: *msg += "<p>Status: Indefinido </p>"; break;
  }
  *msg += "<p>Rede WiFi: "+WiFi.SSID()+" ("+String(WiFi.RSSI())+" dBm)</p>";
  // *msg += "<p>Hostname: "+WiFi.hostname()+"</p>";
  *msg += "<p>Endereço IP: "+WiFi.localIP().toString()+"</p>";
  *msg += "<p>Gateway: "+WiFi.gatewayIP().toString()+"</p>";
  
  *msg += "</div>";
}

void f_handleWifi() {
  g_Stat.Web_GET++;

  if (!server.authenticate(DEFAULT_LOCAL_USER_NAME, g_Dados.Local_PW)) {
    return server.requestAuthentication();
  }

  String msg;
  msg.reserve(1500);

  f_html_header(&msg,630);

  msg += "<div class=\"g\">";

  f_wifi_box (&msg,true);

  msg += "<div class=\"g_d\" id=\"b_w\"> Procurando redes<br>";

  f_insert_load(&msg);

  msg += "<div class=\"g_d_s\"><br>O sistema está procurando as rede disponívels,";
  msg += "por favor aguarde.</div> </div>";

  msg += "<div class=\"g_d\"> Nova Rede";
  msg += "<p><input type=text id=\"w_R\" placeholder=\"Nome da Rede\"></p>";
  msg += "<p><input type=password id=\"w_R_PW\" placeholder=\"Senha da Rede\"></p>";
  msg += "<p class=\"p_h\" id=\"b_1\" onclick=\"f_salve()\">Salvar</p>";
  msg += "<p id=\"txt\"></p>";
  //msg += "<p class=\"p_h\" id=\"b_2\" onclick=\"f_reset()\">Reset</p>";
  msg += "<br> <p class=\"p_h\" id=\"b_2\" onclick=\"location.href='/reset'\">Reset</p>";
  msg += "</div>";

  msg += "</div></div>";

  msg += "<script>\n";
  msg += "function f_s(e){document.getElementById(\"w_R\").value=e.innerHTML;}\n";
  msg += "function f_salve(){";
  msg += "c_b_1 = document.getElementById('b_1');";
  msg += "c_b_1.disabled = true; c_b_1.innerHTML = \"Aguardando resposta\";";
  msg += "const params = { c_R: document.querySelector('#w_R').value,";
  msg += "c_R_PW: document.querySelector('#w_R_PW').value };";
  msg += "const http = new XMLHttpRequest();";
  msg += "http.open('POST', '/json_wifi',true);";
  msg += "http.setRequestHeader('Content-type', 'application/json');";
  msg += "http.send(JSON.stringify(params));";
  msg += "http.onload = function() {";
  msg += "data_json = JSON.parse(http.responseText);";
  msg += "console.log (data_json);";
  msg += "if(data_json.Res == \"Ok\") {";
  msg += "c_txt = document.getElementById('txt');";
  msg += "c_txt.style.display = \"block\";";
  msg += "c_txt.innerHTML = \"Rede gravada com sucesso, no próximo reset o sistema se conectará a rede '\" + document.querySelector('#w_R').value + \"'\";";
  msg += "c_b_2 = document.getElementById('b_2');";
  msg += "c_b_2.style.display = \"block\";";
  msg += "} else { alert(data_json.Status); }";
  msg += "c_b_1.disabled = false; c_b_1.innerHTML = \"Salvar\";";
  msg += "};";
  msg += "}\n";
  // msg += "function f_reset(){";
  // msg += "c_b_2 = document.getElementById('b_2');";
  // msg += "c_b_2.disabled = true; c_b_2.innerHTML = \"Aguardando resposta\";";
  // msg += "const params = { c_S: 157 };";
  // msg += "const http = new XMLHttpRequest();";
  // msg += "http.open('POST', '/json_reset',true);";
  // msg += "http.setRequestHeader('Content-type', 'application/json');";
  // msg += "http.send(JSON.stringify(params));";
  // msg += "http.onload = function() {";
  // msg += "data_json = JSON.parse(http.responseText);";
  // msg += "console.log (data_json);";
  // msg += "if(data_json.Res == \"Ok\") { alert(data_json.Status); }";
  // msg += "c_b_2.innerHTML = \"Reiniciando Sistema\";";
  // msg += "};";
  // msg += "}\n";
  // msg += "window.onload=function(){ document.getElementById('b_2').disable = true; ";
  msg += "window.onload=function(){ ";
  msg += "c_b_2 = document.getElementById('b_2');";
  msg += "c_b_2.style.display = \"none\";";

  msg += "f_wifi_detect();};\n";
  msg += "function f_wifi_detect(){const http = new XMLHttpRequest();";
  msg += "http.open('POST', '/json_wifi_detect',true);";
  msg += "http.setRequestHeader('Content-type', 'application/json'); http.send();";
  msg += "http.onload = function() { data_json = JSON.parse(http.responseText);";
  msg += "console.log (data_json); c_b_w = document.getElementById('b_w');";
  msg += "if(data_json.Total == \"0\") { c_b_w.innerHTML = \"Nenhuma rede encontrada\";";
  msg += "} else {c_msg=\"Redes disponíveis: \"+ data_json.Total;";
  msg += "for (i = 0; i < data_json.Total; i++){\n";
  msg += "c_msg += \"<p class=\\\"p_h\\\" onclick=\\\"f_s(this)\\\">\"+ data_json.Redes[i].Nome+\"</p>\";";
  msg += "} c_msg += \"<div class=\\\"g_d_s\\\">Escolha a rede desejada</div>\";";
  msg += "c_b_w.innerHTML = c_msg;}}}";
  msg += "</script>";

  msg += "</body></html>";

  server.send(200, "text/html; charset=utf-8", msg);
}

void f_update() {
  g_Stat.Web_GET++;

  if (!server.authenticate(DEFAULT_LOCAL_USER_NAME, g_Dados.Local_PW)) {
    return server.requestAuthentication();
  }

  String msg;
  msg.reserve(1000);
  f_html_header(&msg,630);

  msg += "<div class=\"g\">";

  msg += "<div class=\"g_d\">ATENÇÂO <br> Esta página é para Atualização do Sistema <br>";
  msg += "<div class=\"g_d_s\">Todos os dados poderão ser apagados  <br>";
  msg += "Caso não tenha certeza do procedimento saia desta página </div>";
  msg += "<form method=\"POST\" action=\"/update\" enctype=\"multipart/form-data\">";
  msg += "<p class=\"p_h\"><label id=\"lb_f\" for=\"sel_f\">Selecione novo Firmware</label>";
  msg += "<input id=\"sel_f\" type=\"file\" accept=\".bin\" name=\"name\"></p>";
  msg += "<p class=\"p_h\"><input class=\"btn\" type=\"submit\" value=\"Upload\"></p>";
  msg += "</form></div></div>";
  msg += "<script> window.onload = function() {";
  msg += "var $input = document.getElementById('sel_f'),";
  msg += "$fileName = document.getElementById('lb_f');";
  msg += "$input.addEventListener('change', function(){";
  msg += "$fileName.textContent = this.value.replace(/\\\\/g, '/').replace(/.*\\//, '');";
  msg += "});}</script>";
  msg += "</div></body></html>";

  server.send(200, "text/html; charset=utf-8", msg);
}

void f_reset(){
  g_Stat.Web_GET++;
  DEBUG_PRINTF(" Web - Reset");

  if (!server.authenticate(DEFAULT_LOCAL_USER_NAME, g_Dados.Local_PW)) {
    return server.requestAuthentication();
  }

  String msg;
  msg.reserve(500);
  f_html_header(&msg,630);

  msg += "<div class=\"g\">";

  msg += "<div class=\"g_d\"> Sistema será reiniciado em alguns instantes<br> <br>";
  msg += "<p class=\"p_h\" onclick=\"location.href='/'\">Página principal</p>";
  msg += "</div>";

  msg += "</div></div></body></html>";

  server.send(200, "text/html; charset=utf-8", msg);

  delay(200);  yield();  delay(200);  yield();  delay(200);  yield();

  ESP.restart();
}

void f_eraseconfig(){
  g_Stat.Web_GET++;
  DEBUG_PRINTF(" Web - Reset");

  if (!server.authenticate(DEFAULT_LOCAL_USER_NAME, g_Dados.Local_PW)) {
    return server.requestAuthentication();
  }

  String msg;
  msg.reserve(500);
  f_html_header(&msg,630);

  msg += "<div class=\"g\">";

  msg += "<div class=\"g_d\"> PArâmetros do processador zerados <br> Sistema será reiniciado em alguns instantes<br> <br>";
  msg += "<p class=\"p_h\" onclick=\"location.href='/'\">Página principal</p>";
  msg += "</div>";

  msg += "</div></div></body></html>";

  server.send(200, "text/html; charset=utf-8", msg);

  // ESP.eraseConfig();

  delay(200);  yield();  delay(200);  yield();  delay(200);  yield();

  ESP.restart();
}

void f_reset_factory(){
  g_Stat.Web_GET++;

  DEBUG_PRINTF(" Web - Factory Reset");

  if (!server.authenticate(DEFAULT_LOCAL_USER_NAME, g_Dados.Local_PW)) {
    return server.requestAuthentication();
  }

  g_Dados.reset_type = RESET_TYPE_FACTORY;
  DMD_EEPROM_Save((char *)&g_Dados, sizeof(g_Dados));

  String msg;
  msg.reserve(500);
  f_html_header(&msg,630);

  msg += "<div class=\"g\">";

  msg += "<div class=\"g_d\"> Sistema irá zerar todas as configurações.<br> Reiniciado... <br>";
  msg += "<p class=\"p_h\" onclick=\"location.href='/'\">Página principal</p>";
  msg += "</div>";

  msg += "</div></div></body></html>";

  server.send(200, "text/html; charset=utf-8", msg);

  delay(200);  yield();  delay(200);

  // ESP.eraseConfig();

  delay(200);  yield();  delay(200);  yield();  delay(200);  yield();

  ESP.restart();
}

void f_handleConfig() {
  g_Stat.Web_GET++;

  if (!server.authenticate(DEFAULT_LOCAL_USER_NAME, g_Dados.Local_PW)) {
    return server.requestAuthentication();
  }

  String msg;
  msg.reserve(2000);
  f_html_header(&msg,630);

  msg += "<div class=\"g\">";

  // msg += "<div class=\"g_d\"> Realizar busca de novos Slaves";
  // msg += "<p class=\"p_h\" id=\"b_1\" onclick=\"f_rem(9,0)\">Iniciar Busca</p>";
  // msg += "<p class=\"p_h\" id=\"b_1\" onclick=\"f_rem(7,0)\">Criar Int Virtual</p>";
  // if ( g_OW_DS.getDeviceCount() > 0 ) {
  //   msg += "<p class=\"p_h\" id=\"b_1\" onclick=\"f_rem(6,0)\">Apagar DS18b20</p>";
  // }
  // msg += "</div>";

  // msg += "<div class=\"g_d\"> Slaves gravados <br>";
  // for ( int i = 0 ; i <  ATTiny_MAX_SLAVES ; i++ ) {
  //   if ( g_Dados.ATT_Slave[i].Addr[0] == 0 ) 
  //     continue;
  //   msg += "<p class=\"p_h\"> OW " + String(i) + " : ";
  //   for (int j = 0; j < ( DMD_OW_ADDR_SIZE - 1 ) ; j++){
  //     msg += String(g_Dados.ATT_Slave[i].Addr[j]) + ".";
  //   }
  //   msg += String(g_Dados.ATT_Slave[i].Addr[( DMD_OW_ADDR_SIZE - 1 )]) + "</p>";
  // }
  // msg += "</div>";

  // for ( int i = 0 ; i < g_ATTiny_Int.getDeviceCount(); i++ ) {
  //   msg += "<div class=\"g_d\"> ATTiny Interruptor: " + String(i);
  //   if ( g_ATTiny_Int.IsVirtual(i) ) {
  //      msg += " - Virtual";
  //   } else {
  //     (g_ATTiny_Int.IsOnline(i) ? msg += " - Online" : msg += " - Offline" );
  //   }
  //   msg += "<p class=\"p_h\" id=\"b_1\" onclick=\"f_rem(0," + String(i) + ")\">Remover</p>";
  //   msg += "</div>";
  // }

  // for ( int i = 0 ; i < g_ATTiny_Atuador.getDeviceCount(); i++ ) {
  //   msg += "<div class=\"g_d\"> ATTiny Atuador: " + String(i);
  //   (g_ATTiny_Atuador.IsOnline(i) ? msg += " - Online" : msg += " - Offline" );
  //   msg += "<p class=\"p_h\" id=\"b_1\" onclick=\"f_rem(1," + String(i) + ")\">Remover</p>";
  //   msg += "</div>";
  // }

  // for ( int i = 0 ; i < g_ATTiny_PWM.getDeviceCount(); i++ ) {
  //   msg += "<div class=\"g_d\"> ATTiny PWM: " + String(i);
  //   (g_ATTiny_PWM.IsOnline(i) ? msg += " - Online" : msg += " - Offline" );
  //   msg += "<p class=\"p_h\" id=\"b_1\" onclick=\"f_rem(2," + String(i) + ")\">Remover</p>";
  //   msg += "</div>";
  // }

  // for ( int i = 0 ; i < g_ATTiny_Med.getDeviceCount(); i++ ) {
  //   msg += "<div class=\"g_d\"> ATTiny Medidor: " + String(i);
  //   (g_ATTiny_Med.IsOnline(i) ? msg += " - Online" : msg += " - Offline" );
  //   msg += "<p class=\"p_h\" id=\"b_1\" onclick=\"f_rem(3," + String(i) + ")\">Remover</p>";
  //   msg += "</div>";
  // }

  // if ( g_ProMini_Atuador.GetDevStatus() ) {
  //   msg += "<div class=\"g_d\"> ATTiny Promini ";
  //   (g_ProMini_Atuador.IsOnline() ? msg += "Online" : msg += "Offline" );
  //   msg += "<p class=\"p_h\" id=\"b_1\" onclick=\"f_rem(8,0)\">Remover</p>";
  //   msg += "</div>";
  // }

  msg += "<div class=\"g_d\"> Resposta do sistema";
  msg += "<p id=\"txt\"></p>";
  msg += "</div>";

  msg += "<script>\n";
  msg += "function f_rem(x,y){";
  msg += "const params = { c_Tipo: x, c_REM: y };";
  msg += "const http = new XMLHttpRequest();";
  msg += "http.open('POST', '/json_config_rem',true);";
  msg += "http.setRequestHeader('Content-type', 'application/json');";
  msg += "http.send(JSON.stringify(params));";
  msg += "http.onload = function() {";
  msg += "data_json = JSON.parse(http.responseText);";
  msg += "console.log (data_json);";
  msg += "if(data_json.Res == \"Ok\") {";
  msg += "c_txt = document.getElementById('txt');";
  msg += "c_txt.style.display = \"block\";";
  msg += "c_txt.innerHTML = data_json.Status";
  msg += "} else { alert(data_json.Status); }";
  msg += "};";
  msg += "}\n";
  msg += "</script>";

  msg += "</body></html>";

  server.send(200, "text/html; charset=utf-8", msg);
}

// void f_handleConfigInt() {
//   g_Stat.Web_GET++;
//   DEBUG_PRINTF(" Web - Config_Int");

//   if (!server.authenticate(DEFAULT_LOCAL_USER_NAME, g_Dados.Local_PW)) {
//     return server.requestAuthentication();
//   }

//   String msg;
//   msg.reserve(8000);
//   f_html_header(&msg,630);

//   msg += "<div class=\"g\">";

//   for ( int i = 0 ; i < g_ATTiny_Int.getDeviceCount(); i++ ) {
//     msg += "<div class=\"g_d\"> Slave ATTiny Int " + String(i);

//     for ( int l_int = 0 ; l_int < ATTiny_MAX_INT; l_int++ ) {
//       msg += "<div class=\"g_d\">Configuração do Interruptor - " + String(l_int) + "<p>Tópico do Interruptor</p>";
//       msg += "<p><input type=text id=\"w_No"+String(i)+"_"+String(l_int) +"\" placeholder=\""+ g_Dados.ATT_SlaveInt_Cfg[i].Topico_Int[l_int] +"\" maxlength=\""+ String(DADOS_INT_TOPIC-1)+"\"></p>";
//       msg += "<p class=\"p_h\" onclick=\"f_s("+String(i)+","+String(l_int)+",'w_No"+String(i)+"_"+String(l_int)+"')\">Salvar</p>";
//       msg += "<p>Saídas Ativadas por este interruptor:</p>";
//       msg += "<div class=\"i_g\">\n";
//       for (size_t l_saida = 0; l_saida < ATTiny_MAX_ATUADOR_SISTEMA; l_saida++) {
//         msg += "<p class=";
//         (g_Dados.ATT_SlaveInt_Cfg[i].AtivarSaida[l_int][l_saida]? msg += "\"Md\"" : msg += "\"Mi\"" );
//         msg += " onclick=\"f_int("+ String(i)+ "," +String(l_int)+ "," +String(l_saida)+ ",1)\">Saida " +String(l_saida)+ "</p>";
//         if ( ((l_saida+1)%3 == 0) & (l_saida != 0) )
//           msg += "</div> <div class=\"i_g\">\n";

//       }
//       msg += "</div>\n";

//       msg += "<p>Aumenta o valor para o PWM</p>";
//       msg += "<div class=\"i_g\">\n";
//       for (size_t l_saida = 0; l_saida < ATTiny_MAX_PWM_SISTEMA; l_saida++) {
//         msg += "<p class=";
//         (g_Dados.ATT_SlaveInt_Cfg[i].AtivarPWM_UP[l_int][l_saida]? msg += "\"Md\"" : msg += "\"Mi\"" );
//         msg += " onclick=\"f_int("+ String(i)+ "," +String(l_int)+ "," +String(l_saida)+ ",3)\">PWM " +String(l_saida)+ "</p>";
//       }
//       msg += "</div> </div> <br>";
//     }
//     msg += "<div>Clique nas saídas para selecionar.</div>";
//     msg += "</div>";
//   }

//   msg += "<script>\n";
//   msg += "function f_s(x,y,z){";
//   msg += "c_b_1 = document.getElementById(z);";
//   msg += "const params={c_D:x,c_I:y,c_No:c_b_1.value};";
//   msg += "const http=new XMLHttpRequest();";
//   msg += "http.open('POST','/json_config_int_topic',true);";
//   msg += "http.setRequestHeader('Content-type','application/json');";
//   msg += "http.send(JSON.stringify(params));";
//   msg += "http.onload=function(){";
//   msg += "data_json=JSON.parse(http.responseText);";
//   msg += "console.log(data_json);";
//   msg += "if(data_json.Res==\"Ok\") {";
//   msg += "}else{alert(data_json.Status);}";
//   msg += "};";
//   msg += "}\n";
//   msg += "function f_int(x,y,z,t){";
//   msg += "if(t==1) {params={c_D:x,c_I:y,c_S:z};}";
//   msg += "if(t==2) {params={c_D:x,c_I:y,c_PD:z};}";
//   msg += "if(t==3) {params={c_D:x,c_I:y,c_PU:z};}";
//   msg += "const http=new XMLHttpRequest();";
//   msg += "http.open('POST','/json_config_int',true);";
//   msg += "http.setRequestHeader('Content-type','application/json');";
//   msg += "http.send(JSON.stringify(params));";
//   msg += "http.onload=function(){";
//   msg += "data_json=JSON.parse(http.responseText);";
//   msg += "console.log(data_json);";
//   msg += "if(data_json.Res==\"Ok\") {";
//   msg += "}else{alert(data_json.Status);}";
//   msg += "};";
//   msg += "}\n";
//   msg += "</script>";

//   msg += "</body></html>";

//   server.send(200, "text/html; charset=utf-8", msg);
// }

// void f_handleConfigIntTopic() {
//   g_Stat.Web_GET++;
//   DEBUG_PRINTF(" Web - Config_Int_Topic");

//   if (!server.authenticate(DEFAULT_LOCAL_USER_NAME, g_Dados.Local_PW)) {
//     return server.requestAuthentication();
//   }

//   String msg;
//   msg.reserve(2000);
//   f_html_header(&msg,630);

//   msg += "<div class=\"g\">";

//   for ( int i = 0 ; i < g_ATTiny_Int.getDeviceCount(); i++ ) {
//     msg += "<div class=\"g_d\"> Slave ATTiny Int " + String(i);
//     for ( int l_int = 0 ; l_int < ATTiny_MAX_INT; l_int++ ) {
//       msg += "<p>Saídas p/ Interruptor - " + String(l_int) + "</p>";
//       msg += "<div class=\"i_g\">";
//       for (size_t l_saida = 0; l_saida < ATTiny_MAX_ATUADOR_SISTEMA; l_saida++) {
//         msg += "<p class=";
//         (g_Dados.ATT_SlaveInt_Cfg[i].AtivarSaida[l_int][l_saida]? msg += "\"Md\"" : msg += "\"Mi\"" );
//         msg += " onclick=\"f_int("+ String(i)+ "," +String(l_int)+ "," +String(l_saida)+ ")\">Saida " +String(l_saida)+ "</p>";
//         if ( ((l_saida+1)%3 == 0) & (l_saida != 0) )
//           msg += "</div> <div class=\"i_g\">";

//       }
//       msg += "</div>";
//     }
//     msg += "<div class=\"m_t\">Clique nas saídas para selecionar.</div>";
//     msg += "</div>";
//   }

//   msg += "<script>\n";
//   msg += "function f_int(x,y,z){";
//   msg += "const params={c_D:x,c_I:y,c_S:z};";
//   msg += "const http=new XMLHttpRequest();";
//   msg += "http.open('POST','/json_config_int',true);";
//   msg += "http.setRequestHeader('Content-type','application/json');";
//   msg += "http.send(JSON.stringify(params));";
//   msg += "http.onload=function(){";
//   msg += "data_json=JSON.parse(http.responseText);";
//   msg += "console.log(data_json);";
//   msg += "if(data_json.Res==\"Ok\") {";
//   msg += "}else{alert(data_json.Status);}";
//   msg += "};";
//   msg += "}\n";
//   msg += "</script>";

//   msg += "</body></html>";

//   server.send(200, "text/html; charset=utf-8", msg);
// }


void f_handleConfigNome() {
  g_Stat.Web_GET++;

  if (!server.authenticate(DEFAULT_LOCAL_USER_NAME, g_Dados.Local_PW)) {
    return server.requestAuthentication();
  }

  String msg;
  msg.reserve(1000);
  f_html_header(&msg,630);

  msg += "<div class=\"g\">";

  msg += "<div class=\"g_d\"> Nome da Placa";
  msg += "<p><input type=text id=\"w_No\" placeholder=\""+ String(g_Dados.Name) +"\" maxlength=\""+ String(DADOS_NAME_SIZE-1)+"\"></p>";
  msg += "<p class=\"p_h\" id=\"b_1\" onclick=\"f_salve()\">Salvar</p>";
  msg += "<p id=\"txt\"></p>";
  msg += "</div>";

  msg += "<script>\nfunction f_salve(){";
  msg +=   "c_b_1=document.getElementById('b_1');";
  msg +=   "c_b_1.disabled=true; c_b_1.innerHTML=\"Aguardando resposta\";";
  msg +=   "const params={c_No:document.querySelector('#w_No').value};";
  msg +=   "const http=new XMLHttpRequest();";
  msg +=   "http.open('POST','/json_config_nome',true);";
  msg +=   "http.setRequestHeader('Content-type','application/json');";
  msg +=   "http.send(JSON.stringify(params));";
  msg +=   "http.onload=function(){data_json=JSON.parse(http.responseText);";
  msg +=     "console.log(data_json);";
  msg +=     "if(data_json.Res==\"Ok\"){";
  msg +=       "c_txt=document.getElementById('txt');c_txt.style.display=\"block\";c_txt.innerHTML=data_json.Status;";
  msg +=     "}else{alert(data_json.Status);}";
  msg +=     "c_b_1.disabled = false; c_b_1.innerHTML = \"Salvar\";";
  msg +=   "};";
  msg += "}\n</script>";

  msg += "</body></html>";

  server.send(200, "text/html; charset=utf-8", msg);
}

void f_handleConfigIp() {
  g_Stat.Web_GET++;

  if (!server.authenticate(DEFAULT_LOCAL_USER_NAME, g_Dados.Local_PW)) {
    return server.requestAuthentication();
  }

  String msg;
  msg.reserve(1000);
  f_html_header(&msg,630);

  msg += "<div class=\"g\">";

  msg += "<div class=\"g_d\"> Endereço Ip do Servidor";
  msg += "<p><input type=text id=\"w_Ip\" placeholder=\""+ String(g_Dados.MQTT_Srv_IP) +"\" maxlength=\""+ String(DADOS_IP_SIZE-1)+"\"></p>";
  msg += "<p class=\"p_h\" id=\"b_1\" onclick=\"f_salve()\">Salvar</p>";
  msg += "<p id=\"txt\"></p>";
  msg += "</div>";

  msg += "<script>\nfunction f_salve(){";
  msg +=   "c_b_1=document.getElementById('b_1');";
  msg +=   "c_b_1.disabled=true; c_b_1.innerHTML=\"Aguardando resposta\";";
  msg +=   "const params={c_IP:document.querySelector('#w_Ip').value};";
  msg +=   "const http=new XMLHttpRequest();";
  msg +=   "http.open('POST','/json_config_ip',true);";
  msg +=   "http.setRequestHeader('Content-type','application/json');";
  msg +=   "http.send(JSON.stringify(params));";
  msg +=   "http.onload=function(){data_json=JSON.parse(http.responseText);";
  msg +=     "console.log(data_json);";
  msg +=     "if(data_json.Res==\"Ok\"){";
  msg +=       "c_txt=document.getElementById('txt');c_txt.style.display=\"block\";c_txt.innerHTML=data_json.Status;";
  msg +=     "}else{alert(data_json.Status);}";
  msg +=     "c_b_1.disabled = false; c_b_1.innerHTML = \"Salvar\";";
  msg +=   "};";
  msg += "}\n</script>";

  msg += "</body></html>";

  server.send(200, "text/html; charset=utf-8", msg);
}


void f_handleConfigNS() {
  g_Stat.Web_GET++;

  String msg;
  msg.reserve(1000);
  f_html_header(&msg,630);

  msg += "<div class=\"g\">";

  msg += "<div class=\"g_d\"> Numero de Série";
  msg += "<p><input type=text id=\"w_NS\" placeholder=\"Indique NS da placa (10 caracteres)\"></p>";
  msg += "<p class=\"p_h\" id=\"b_1\" onclick=\"f_salve()\">Salvar</p>";
  msg += "<p id=\"txt\"></p>";
  msg += "</div>";

  msg += "<div class=\"g_d\"> Comandos de teste";
  msg += "<p class=\"p_h\" onclick=\"f_R(1)\">Teste Relé 1</p>";
  msg += "<p class=\"p_h\" onclick=\"f_R(2)\">Teste Relé 2</p>";
  msg += "<p class=\"p_h\" onclick=\"f_R(3)\">Teste Buzzer</p>";
  msg += "</div>";

  msg += "<script>\n";
  msg += "function f_salve(){";
  msg += "c_b_1 = document.getElementById('b_1');";
  msg += "c_b_1.disabled = true; c_b_1.innerHTML = \"Aguardando resposta\";";
  msg += "const params = { c_NS: document.querySelector('#w_NS').value };";
  msg += "const http = new XMLHttpRequest();";
  msg += "http.open('POST', '/json_config_ns',true);";
  msg += "http.setRequestHeader('Content-type', 'application/json');";
  msg += "http.send(JSON.stringify(params));";
  msg += "http.onload = function() {";
  msg += "data_json = JSON.parse(http.responseText);";
  msg += "console.log (data_json);";
  msg += "if(data_json.Res == \"Ok\") {";
  msg += "c_txt = document.getElementById('txt');";
  msg += "c_txt.style.display = \"block\";";
  msg += "c_txt.innerHTML = data_json.Status";
  msg += "} else { alert(data_json.Status); }";
  msg += "c_b_1.disabled = false; c_b_1.innerHTML = \"Salvar\";";
  msg += "};";
  msg += "}\n";
  msg += "function f_R(x){";
  msg += "const params = { c_cmd: x };";
  msg += "const http = new XMLHttpRequest();";
  msg += "http.open('POST', '/json_cmd',true);";
  msg += "http.setRequestHeader('Content-type', 'application/json');";
  msg += "http.send(JSON.stringify(params));";
  msg += "http.onload = function() {";
  msg += "data_json = JSON.parse(http.responseText);";
  msg += "console.log (data_json);";
  msg += "};";
  msg += "}\n";
  msg += "</script>";

  msg += "</body></html>";

  server.send(200, "text/html; charset=utf-8", msg);
}

void f_handleConfigLocalPW() {
  g_Stat.Web_GET++;

  if (!server.authenticate(DEFAULT_LOCAL_USER_NAME, g_Dados.Local_PW)) {
    return server.requestAuthentication();
  }

  String msg;
  msg.reserve(1000);
  f_html_header(&msg,630);

  msg += "<div class=\"g\">";

  msg += "<div class=\"g_d\"> Senha Local do sistema";
  msg += "<p><input type=text id=\"w_PW\" placeholder=\" Senha do sistema ( ao menos 8 caracteres)\"></p>";
  msg += "<p class=\"p_h\" id=\"b_1\" onclick=\"f_salve()\">Salvar</p>";
  msg += "<p id=\"txt\"></p>";
  msg += "</div>";

  msg += "<script>\n";
  msg += "function f_salve(){";
  msg += "c_b_1 = document.getElementById('b_1');";
  msg += "c_b_1.disabled = true; c_b_1.innerHTML = \"Aguardando resposta\";";
  msg += "const params = { c_PW: document.querySelector('#w_PW').value };";
  msg += "const http = new XMLHttpRequest();";
  msg += "http.open('POST', '/json_config_local_pw',true);";
  msg += "http.setRequestHeader('Content-type', 'application/json');";
  msg += "http.send(JSON.stringify(params));";
  msg += "http.onload = function() {";
  msg += "data_json = JSON.parse(http.responseText);";
  msg += "console.log (data_json);";
  msg += "if(data_json.Res == \"Ok\") {";
  msg += "c_txt = document.getElementById('txt');";
  msg += "c_txt.style.display = \"block\";";
  msg += "c_txt.innerHTML = data_json.Status";
  msg += "} else { alert(data_json.Status); }";
  msg += "c_b_1.disabled = false; c_b_1.innerHTML = \"Salvar\";";
  msg += "};";
  msg += "}\n";
  msg += "</script>";

  msg += "</body></html>";

  server.send(200, "text/html; charset=utf-8", msg);
}


void f_handleConfigIntColorSch() {
  g_Stat.Web_GET++;
  DEBUG_PRINTF(" Web - Config_IntColorSch");

  if (!server.authenticate(DEFAULT_LOCAL_USER_NAME, g_Dados.Local_PW)) {
    return server.requestAuthentication();
  }

  String msg;
  msg.reserve(5000);
  f_html_header(&msg,630);

  msg += "<div class=\"g\">";

  msg += "<div class=\"g_d\"> Configuração dos Interruptores<br>";
  msg += "<div class=\"g_d\">";
  msg +=   "<p> Opções de Esquemas de Cores </p>";
  msg +=   "<div class=\"i_g\">\n";
  msg +=      "<p style=\"background-color:#0000FF;\" onclick=\"f_cor(0)\">Azul </p>";
  msg +=      "<p style=\"background-color:#FF0000;\" onclick=\"f_cor(1)\">Vermelho </p>";
  msg +=      "<p style=\"background-color:#00FF00;\" onclick=\"f_cor(2)\">Verde </p>";
  msg +=      "<p style=\"background-color:#FFD700;\" onclick=\"f_cor(3)\">Gold </p>";
  msg +=   "</div> <div class=\"i_g\">\n";
  msg +=      "<p style=\"background-color:#7FFFD4;\" onclick=\"f_cor(4)\">Aqua Marine </p>";
  msg +=      "<p style=\"background-color:#FF1493;\" onclick=\"f_cor(5)\">Deep Pink</p>";
  msg +=      "<p style=\"background-color:#00BFFF;\" onclick=\"f_cor(6)\">Deep Sky Blue </p>";
  msg +=      "<p style=\"background-color:#FFFFFF;\" onclick=\"f_cor(7)\">Branco </p>";
  msg +=   "</div> <div class=\"i_g\">\n";
  msg +=      "<p onclick=\"f_cor(10)\">Noite </p>";
  msg +=      "<p onclick=\"f_cor(11)\">Dia</p>";
  msg +=   "</div>\n";
  msg += "</div> <br>";
  msg += "</div>";

  msg += "<script>\n";
  msg +=  "function f_cor(x){";
  msg +=    "const params={c_C:x};";
  msg +=    "const http=new XMLHttpRequest();";
  msg +=    "http.open('POST','/json_config_int_color_sch',true);";
  msg +=    "http.setRequestHeader('Content-type','application/json');";
  msg +=    "http.send(JSON.stringify(params));";
  msg +=    "http.onload=function(){";
  msg +=      "data_json=JSON.parse(http.responseText);";
  msg +=      "console.log(data_json);";
  msg +=      "alert(data_json.Status);";
  msg +=    "};";
  msg +=  "}\n";
  msg += "</script>";

  msg += "</body></html>";

  server.send(200, "text/html; charset=utf-8", msg);
}


// bool WebUpdate(const char * f_fw) {
//   WiFiClient client;

//   ESPhttpUpdate.rebootOnUpdate(false);

//   HTTPUpdateResult ret = ESPhttpUpdate.update(client, f_fw);
//   switch(ret) {
//     case HTTP_UPDATE_FAILED:
//     case HTTP_UPDATE_NO_UPDATES:
//         return false;
//         break;
//     case HTTP_UPDATE_OK:
//         return true;
//         break;
//   }
//   return false;
// }

// String WebUpdate_GetErro() {
//   return ESPhttpUpdate.getLastErrorString() ;
// }



// void f_handleConfigDebounce() {
//   g_Stat.Web_GET++;

//   if (!server.authenticate(DEFAULT_LOCAL_USER_NAME, g_Dados.Local_PW)) {
//     return server.requestAuthentication();
//   }

//   String msg;
//   msg.reserve(1000);
//   f_html_header(&msg,630);

//   msg += "<div class=\"g\">";

//   msg += "<div class=\"g_d\"> Tempo Debounce (em ms - valor máximo 5000)";
//   msg += "<p><input type=number id=\"w_No\" min=\"1\" max=\"5000\" placeholder=\""+ String(g_Dados.Debounce) +"\" ></p>";
//   msg += "<p class=\"p_h\" id=\"b_1\" onclick=\"f_salve()\">Salvar</p>";
//   msg += "<p id=\"txt\"></p>";
//   msg += "</div>";

//   msg += "<script>\nfunction f_salve(){";
//   msg +=   "c_b_1=document.getElementById('b_1');";
//   msg +=   "c_b_1.disabled=true; c_b_1.innerHTML=\"Aguardando resposta\";";
//   msg +=   "const params={c_No:document.querySelector('#w_No').value};";
//   msg +=   "const http=new XMLHttpRequest();";
//   msg +=   "http.open('POST','/json_config_debounce',true);";
//   msg +=   "http.setRequestHeader('Content-type','application/json');";
//   msg +=   "http.send(JSON.stringify(params));";
//   msg +=   "http.onload=function(){data_json=JSON.parse(http.responseText);";
//   msg +=     "console.log(data_json);";
//   msg +=     "if(data_json.Res==\"Ok\"){";
//   msg +=       "c_txt=document.getElementById('txt');c_txt.style.display=\"block\";c_txt.innerHTML=data_json.Status;";
//   msg +=     "}else{alert(data_json.Status);}";
//   msg +=     "c_b_1.disabled = false; c_b_1.innerHTML = \"Salvar\";";
//   msg +=   "};";
//   msg += "}\n</script>";

//   msg += "</body></html>";

//   server.send(200, "text/html; charset=utf-8", msg);
// }


// void f_handleConfigMedidor() {
//   g_Stat.Web_GET++;

//   if (!server.authenticate(DEFAULT_LOCAL_USER_NAME, g_Dados.Local_PW)) {
//     return server.requestAuthentication();
//   }

//   String msg;
//   msg.reserve(1000);
//   f_html_header(&msg,630);

//   msg += "<div class=\"g\">";

//   for ( int i = 0 ; i < g_ATTiny_Med.getDeviceCount(); i++ ) {
//     msg += "<div class=\"g_d\"> ATTiny Medidor: " + String(i);
//     (g_ATTiny_Med.IsOnline(i) ? msg += " - Online" : msg += " - Offline" );
//     msg += "<div class=\"i_g\">\n";
//     msg +=   "<p class=\"p_h\"> Minimo:</p>";
//     msg +=   "<p><input type=number id=\"w_Mi"+String(i)+"\" placeholder=\""+ String(g_Dados.ATT_SlaveMed[i].Minimo) +"\"></p>";
//     msg +=   "<p class=\"p_h\" onclick=\"f_s(1,"+String(i)+",'w_Mi"+String(i)+"')\">Salvar</p>";
//     msg += "</div>";

//     msg += "<div class=\"i_g\">\n";
//     msg +=   "<p class=\"p_h\"> Máximo:</p>";
//     msg +=   "<p><input type=number id=\"w_Ma"+String(i)+"\" placeholder=\""+ String(g_Dados.ATT_SlaveMed[i].Maximo) +"\"></p>";
//     msg +=   "<p class=\"p_h\" onclick=\"f_s(2,"+String(i)+",'w_Ma"+String(i)+"')\">Salvar</p>";

//     msg += "</div>";

//     msg += "<div class=\"i_g\">\n";
//     msg +=   "<p class=\"p_h\"> Tópico:</p>";
//     msg +=   "<p><input type=number id=\"w_To"+String(i)+"\" placeholder=\""+ String(g_Dados.ATT_SlaveMed[i].Topico_Index) +"\"></p>";
//     msg +=   "<p class=\"p_h\" onclick=\"f_s(3,"+String(i)+",'w_To"+String(i)+"')\">Salvar</p>";

//     msg += "</div>";
//     msg += "</div>";
//   }
//   msg += "</div>";

//   msg += "<div class=\"g_d\"> Tópicos <br>";
//   msg += "<div class=\"i_g\">\n";
//   for ( int i = 0 ; i <  DADOS_TOPIC_MAX_INDEX ; i++ ) {
//     msg += "<p class=\"p_h\">" + String(i) + "> ";
//     msg += String(g_Dados.Topicos[i]) + "</p>";
//     if ( ((i+1)%3 == 0) & (i != 0) )
//       msg += "</div> <div class=\"i_g\">\n";
//   }
//   msg += "</div></div>";

//   msg += "<script>\nfunction f_s(x,y,z){";
//   msg +=   "c_b_1 = document.getElementById(z);";
//   msg +=   "const params={c_T:x,c_D:y,c_V:c_b_1.value};";
//   msg +=   "c_b_1.disabled=true; c_b_1.innerHTML=\"Aguardando resposta\";";
//   msg +=   "const http=new XMLHttpRequest();";
//   msg +=   "http.open('POST','/json_config_medidor',true);";
//   msg +=   "http.setRequestHeader('Content-type','application/json');";
//   msg +=   "http.send(JSON.stringify(params));";
//   msg +=   "http.onload=function(){data_json=JSON.parse(http.responseText);";
//   msg +=     "console.log(data_json);";
//   msg +=     "if(data_json.Res==\"Ok\"){";
//   msg +=       "c_b_1.disabled=false; c_b_1.innerHTML=\"Salvo\"; alert(data_json.Status);";
//   msg +=     "}else{alert(data_json.Status);}";
//   msg +=   "};";
//   msg += "}\n</script>";

//   msg += "</body></html>";

//   server.send(200, "text/html; charset=utf-8", msg);
// }



// void f_handleConfigPWM() {
//   g_Stat.Web_GET++;

//   if (!server.authenticate(DEFAULT_LOCAL_USER_NAME, g_Dados.Local_PW)) {
//     return server.requestAuthentication();
//   }

//   String msg;
//   msg.reserve(1000);
//   f_html_header(&msg,630);

//   msg += "<div class=\"g\">";

//   msg += "<div class=\"g_d\"> Valores Máximos PWM";
//   for ( int i = 0 ; i < ATTiny_MAX_PWM_SISTEMA ; i++ ) {
//     msg += "<div class=\"i_g\">\n";
//     msg +=   "<p class=\"p_h\"> PWM" + String(i) + "</p>";
//     msg +=   "<p><input type=number id=\"w_Mi"+String(i)+"\" placeholder=\""+ String(g_Dados.PWM_Max[i]) +"\"></p>";
//     msg +=   "<p class=\"p_h\" onclick=\"f_s(1,"+String(i)+",'w_Mi"+String(i)+"')\">Salvar</p>";
//     msg += "</div>";
//   }
//   msg += "</div>";

//   msg += "<script>\nfunction f_s(x,y,z){";
//   msg +=   "c_b_1 = document.getElementById(z);";
//   msg +=   "const params={c_T:x,c_D:y,c_V:c_b_1.value};";
//   msg +=   "c_b_1.disabled=true; c_b_1.innerHTML=\"Aguardando resposta\";";
//   msg +=   "const http=new XMLHttpRequest();";
//   msg +=   "http.open('POST','/json_config_pwm',true);";
//   msg +=   "http.setRequestHeader('Content-type','application/json');";
//   msg +=   "http.send(JSON.stringify(params));";
//   msg +=   "http.onload=function(){data_json=JSON.parse(http.responseText);";
//   msg +=     "console.log(data_json);";
//   msg +=     "if(data_json.Res==\"Ok\"){";
//   msg +=       "c_b_1.disabled=false; c_b_1.innerHTML=\"Salvo\"; alert(data_json.Status);";
//   msg +=     "}else{alert(data_json.Status);}";
//   msg +=   "};";
//   msg += "}\n</script>";

//   msg += "</body></html>";

//   server.send(200, "text/html; charset=utf-8", msg);
// }


// void f_handleConfigTopicos() {
//   g_Stat.Web_GET++;
//   DEBUG_PRINTF(" Web - Config_Temp");

//   if (!server.authenticate(DEFAULT_LOCAL_USER_NAME, g_Dados.Local_PW)) {
//     return server.requestAuthentication();
//   }

//   String msg;
//   msg.reserve(2000);
//   f_html_header(&msg,630);

//   msg += "<div class=\"g_d\"> Configuração dos Tópicos do Sistema";

//   // for ( int i = 0 ; i < g_OW_DS.getDeviceCount(); i++ ) {
//   //   msg += "<div class=\"g_d\"> Tópico p/ DS18b20 " + String(i);
//   for ( int i = 0 ; i < DADOS_TOPIC_MAX_INDEX; i++ ) {
//     msg += "<div class=\"i_g\">\n";
//     msg += "<p class=\"p_h\"> Tópico " + String(i);
//     msg += "<p><input type=text id=\"w_No"+String(i)+ "\" placeholder=\""+ g_Dados.Topicos[i] +"\" maxlength=\""+ String(DADOS_INT_TOPIC-1)+"\"></p>";
//     msg += "<p class=\"p_h\" onclick=\"f_s("+String(i)+",'w_No"+String(i)+"')\">Salvar</p>";
//     msg += "</div>";    
//   }
// //  msg += "<p><input type=text id=\"w_No_57\" placeholder=\""+ g_Dados.Topicos[i] +"\" maxlength=\""+ String(DADOS_INT_TOPIC-1)+"\"></p>";
//   msg += "<p class=\"p_h\" onclick=\"f_s(57,'w_No1')\">Apagar todos</p>";

//   msg += "</div>";

//   msg += "<script>\n";
//   msg += "function f_s(x,z){";
//   msg +=   "c_b_1 = document.getElementById(z);";
//   msg +=   "const params={c_D:x,c_No:c_b_1.value};";
//   msg +=   "const http=new XMLHttpRequest();";
//   msg +=   "http.open('POST','/json_config_topic',true);";
//   msg +=   "http.setRequestHeader('Content-type','application/json');";
//   msg +=   "http.send(JSON.stringify(params));";
//   msg +=   "http.onload=function(){";
//   msg +=     "data_json=JSON.parse(http.responseText);";
//   msg +=     "console.log(data_json);";
//   msg +=     "if(data_json.Res==\"Ok\") {";
//   msg +=     "}else{alert(data_json.Status);}";
//   msg +=   "};";
//   msg += "}\n";
//   msg += "</script>";

//   msg += "</body></html>";

//   server.send(200, "text/html; charset=utf-8", msg);
// }

// void f_handleConfigRelogio() {
//   g_Stat.Web_GET++;
//   DEBUG_PRINTF(" Web - Config_Relogio");

//   if (!server.authenticate(DEFAULT_LOCAL_USER_NAME, g_Dados.Local_PW)) {
//     return server.requestAuthentication();
//   }

//   String msg;
//   msg.reserve(2000);
//   f_html_header(&msg,630);

//   msg += "<div class=\"g\">";
//     msg += "<div class=\"g_d\"> Configuração Relógio ";
//       if ( g_Relogio.IsOn() ) {
//         msg += "<p> Relogio Ativado </p>";
//         msg += "<p class=\"p_h\" onclick=\"f_s(5,'w_No1')\">Desativar</p>";
//         msg += "<div class=\"i_g\">";
//           msg += "<p> Fade Low</p>";
//           msg += "<p><input type=text id=\"w_No1\" placeholder=\""+ String(g_Relogio.Get_Fade_Low()) +"\" maxlength=\"7\"></p>";
//           msg += "<p class=\"p_h\" onclick=\"f_s(1,'w_No1')\">Salvar</p>";
//         msg += "</div>\n";
//         msg += "<div class=\"i_g\">";
//           msg += "<p> Fade High</p>";
//           msg += "<p><input type=text id=\"w_No2\" placeholder=\""+ String(g_Relogio.Get_Fade_High()) +"\" maxlength=\"7\"></p>";
//           msg += "<p class=\"p_h\" onclick=\"f_s(2,'w_No2')\">Salvar</p>";
//         msg += "</div>\n";
//         msg += "<div class=\"i_g\">";
//           msg += "<p> Hora Dia</p>";
//           msg += "<p><input type=text id=\"w_No3\" placeholder=\""+ String(g_Relogio.Get_Time_Day()) +"\" maxlength=\"7\"></p>";
//           msg += "<p class=\"p_h\" onclick=\"f_s(3,'w_No3')\">Salvar</p>";
//         msg += "</div>\n";
//         msg += "<div class=\"i_g\">";
//           msg += "<p> Hora Noite</p>";
//           msg += "<p><input type=text id=\"w_No4\" placeholder=\""+ String(g_Relogio.Get_Time_Night()) +"\" maxlength=\"7\"></p>";
//           msg += "<p class=\"p_h\" onclick=\"f_s(4,'w_No4')\">Salvar</p>";
//         msg += "</div>\n";
//       } else {
//         msg += "<p id=\"w_No1\"> Relogio desativado </p>";
//         msg += "<p class=\"p_h\" onclick=\"f_s(6,'w_No1')\">Ativar</p>";
//       }
//     msg += "</div>\n";
//   msg += "</div>";

//   msg += "<script>\n";
//   msg += "function f_s(x,z){";
//   msg +=   "c_b_1 = document.getElementById(z);";
//   msg +=   "const params={c_D:x,c_No:c_b_1.value};";
//   msg +=   "const http=new XMLHttpRequest();";
//   msg +=   "http.open('POST','/json_config_relogio',true);";
//   msg +=   "http.setRequestHeader('Content-type','application/json');";
//   msg +=   "http.send(JSON.stringify(params));";
//   msg +=   "http.onload=function(){";
//   msg +=     "data_json=JSON.parse(http.responseText);";
//   msg +=     "console.log(data_json);";
//   // msg +=     "if(data_json.Res==\"Ok\") {";
//   // msg +=     "}else{alert(data_json.Status);}";
//   msg +=     "alert(data_json.Status);";
//   msg +=   "};";
//   msg += "}\n";
//   msg += "</script>";

//   msg += "</body></html>";

//   server.send(200, "text/html; charset=utf-8", msg);
// }

// void f_handleConfigTopicoTemp() {
//   g_Stat.Web_GET++;

//   if (!server.authenticate(DEFAULT_LOCAL_USER_NAME, g_Dados.Local_PW)) {
//     return server.requestAuthentication();
//   }

//   String msg;
//   msg.reserve(1000);
//   f_html_header(&msg,630);

//   msg += "<div class=\"g\">";

//   for ( int i = 0 ; i < g_OW_DS.getDeviceCount(); i++ ) {
//     msg += "<div class=\"g_d\"> DS18b20: " + String(i);
//     msg +=  "<div class=\"i_g\">";
//     msg +=    "<p><input type=number id=\"w_To"+String(i)+"\" placeholder=\""+ String(g_Dados.Temp_Topico_Index[i]) +"\"></p>";
//     msg +=    "<p class=\"p_h\" onclick=\"f_s("+String(i)+",'w_To"+String(i)+"')\">Salvar</p>";
//     msg +=  "</div>";
//     msg += "</div>";
//   }
//   msg += "</div>";

//   msg += "<div class=\"g_d\"> Tópicos <br>";
//   msg += "<div class=\"i_g\">\n";
//   for ( int i = 0 ; i <  DADOS_TOPIC_MAX_INDEX ; i++ ) {
//     msg += "<p class=\"p_h\">" + String(i) + "> ";
//     msg += String(g_Dados.Topicos[i]) + "</p>";
//     if ( ((i+1)%3 == 0) & (i != 0) )
//       msg += "</div> <div class=\"i_g\">\n";
//   }
//   msg += "</div></div>";

//   msg += "<script>\nfunction f_s(y,z){";
//   msg +=   "c_b_1 = document.getElementById(z);";
//   msg +=   "const params={c_D:y,c_V:c_b_1.value};";
//   msg +=   "c_b_1.disabled=true; c_b_1.innerHTML=\"Aguardando resposta\";";
//   msg +=   "const http=new XMLHttpRequest();";
//   msg +=   "http.open('POST','/json_config_topico_temp',true);";
//   msg +=   "http.setRequestHeader('Content-type','application/json');";
//   msg +=   "http.send(JSON.stringify(params));";
//   msg +=   "http.onload=function(){data_json=JSON.parse(http.responseText);";
//   msg +=     "console.log(data_json);";
//   msg +=     "if(data_json.Res==\"Ok\"){";
//   msg +=       "c_b_1.disabled=false; c_b_1.innerHTML=\"Salvo\"; alert(data_json.Status);";
//   msg +=     "}else{alert(data_json.Status);}";
//   msg +=   "};";
//   msg += "}\n</script>";

//   msg += "</body></html>";

//   server.send(200, "text/html; charset=utf-8", msg);
// }

// void f_handleConfigRF() {
//   g_Stat.Web_GET++;

//   if (!server.authenticate(DEFAULT_LOCAL_USER_NAME, g_Dados.Local_PW)) {
//     return server.requestAuthentication();
//   }

//   String msg;
//   msg.reserve(1000);
//   f_html_header(&msg,630);

//   msg += "<div class=\"g\">";

//   for ( int i = 0 ; i < DMD_RF_MAX_DISP ; i++ ) {
//     msg += "<div class=\"g_d\">";
//     msg +=  "<div class=\"i_g\">";
//     if ( g_Dados.RF[i].DeviceType == DMD_RF_SMW210 ) {
//       msg +=    "<p class=\"Md\" onclick=\"f_d("+String(i)+")\" >SW210: " + String(i) + "</p>";
//     } else {
//       msg +=    "<p class=\"Mx\">No Init (" + String(i) + ")</p>";
//     }
//     msg +=    "<p><input type=number id=\"w_To"+String(i)+"\" placeholder=\""+ String(g_Dados.RF[i].Topic_Index) +"\"></p>";
//     msg +=    "<p class=\"p_h\" onclick=\"f_s("+String(i)+",'w_To"+String(i)+"')\">Salvar</p>";
//     msg +=  "</div>";
//     msg += "</div>";
//   }
//   msg += "</div>";

//   msg += "<div class=\"g\">";
//   msg +=  "<div class=\"g_d\"> Buscar sensores RF";
//   msg +=    "<div class=\"i_g\">";
//   msg +=      "<p class=\"p_h\" onclick=\"f_b()\">Adicionar SW210</p>";
//   msg +=    "</div>";
//   msg +=   "</div>";
//   msg += "</div>";


//   msg += "<div class=\"g_d\"> Tópicos <br>";
//   msg += "<div class=\"i_g\">\n";
//   for ( int i = 0 ; i <  DADOS_TOPIC_MAX_INDEX ; i++ ) {
//     msg += "<p class=\"p_h\">" + String(i) + "> ";
//     msg += String(g_Dados.Topicos[i]) + "</p>";
//     if ( ((i+1)%3 == 0) & (i != 0) )
//       msg += "</div> <div class=\"i_g\">\n";
//   }
//   msg += "</div></div>";

//   msg += "<script>\nfunction f_s(y,z){";
//   msg +=   "c_b_1 = document.getElementById(z);";
//   msg +=   "const params={c_D:y,c_V:c_b_1.value};";
//   msg +=   "c_b_1.disabled=true; c_b_1.innerHTML=\"Aguardando resposta\";";
//   msg +=   "const http=new XMLHttpRequest();";
//   msg +=   "http.open('POST','/json_rf_topico',true);";
//   msg +=   "http.setRequestHeader('Content-type','application/json');";
//   msg +=   "http.send(JSON.stringify(params));";
//   msg +=   "http.onload=function(){data_json=JSON.parse(http.responseText);";
//   msg +=     "console.log(data_json);";
//   msg +=     "if(data_json.Res==\"Ok\"){";
//   msg +=       "c_b_1.disabled=false; c_b_1.innerHTML=\"Salvo\"; alert(data_json.Status);";
//   msg +=     "}else{alert(data_json.Status);}";
//   msg +=   "};};";
//   msg += "\nfunction f_d(y){";
//   msg +=   "const params={c_D:y,c_V:0};";
//   msg +=   "const http=new XMLHttpRequest();";
//   msg +=   "http.open('POST','/json_rf_del',true);";
//   msg +=   "http.setRequestHeader('Content-type','application/json');";
//   msg +=   "http.send(JSON.stringify(params));";
//   msg +=   "http.onload=function(){data_json=JSON.parse(http.responseText);";
//   msg +=     "console.log(data_json);";
//   msg +=     "if(data_json.Res==\"Ok\"){";
//   msg +=       "c_b_1.disabled=false; c_b_1.innerHTML=\"Salvo\"; alert(data_json.Status);";
//   msg +=     "}else{alert(data_json.Status);}";
//   msg +=   "};};";

//   msg += "\nfunction f_b(){";
//   msg +=   "const http=new XMLHttpRequest();";
//   msg +=   "http.open('POST','/json_rf_add_smw210',true);";
//   msg +=   "http.setRequestHeader('Content-type','application/json');";
//   msg +=   "http.send();";
//   msg +=   "http.onload=function(){data_json=JSON.parse(http.responseText);";
//   msg +=   "alert(data_json.Status);";
//   msg += "};};\n</script>";

//   msg += "</body></html>";

//   server.send(200, "text/html; charset=utf-8", msg);
// }

// void f_handleConfigAquecSolar() {
//   g_Stat.Web_GET++;
//   DEBUG_PRINTF(" Web - Config_Aq_Solar");

//   if (!server.authenticate(DEFAULT_LOCAL_USER_NAME, g_Dados.Local_PW)) {
//     return server.requestAuthentication();
//   }

//   String msg;
//   msg.reserve(2000);
//   f_html_header(&msg,630);

//   msg += "<div class=\"g\">";
//     msg += "<div class=\"g_d\"> Configuração Aquecedor Solar ";
//       msg += "<p id=\"w_No1\"> Status:";
//       switch ( g_Aquec_Solar.GetStatus() ) {
//         case DMD_AQ_SOLAR_OFF:         msg += " Off </p>";  break;
//         case DMD_AQ_SOLAR_INIT:        msg += " Init </p>";  break;
//         case DMD_AQ_SOLAR_IDLE:        msg += " Idle </p>";  break;
//         case DMD_AQ_SOLAR_AQUECENDO_BOMBA_MAX: msg += " Aquecendo (Max) </p>";  break;
//         case DMD_AQ_SOLAR_AQUECENDO:   msg += " Aquecendo </p>";  break;
//         case DMD_AQ_SOLAR_ANTICONGELAMENTO: msg += " Anticongelamento </p>";  break;
//         case DMD_AQ_SOLAR_ERRO:        msg += " Erro </p>";  break;
//         default:
//         break;
//       }
//       if ( g_Aquec_Solar.IsOn() ) {
//         msg += "<p class=\"p_h\" onclick=\"f_s(1,'w_No1')\">Desativar</p>";
//         msg += "<div class=\"i_g\">";
//           msg += "<p> Slave Atuador</p>";
//           msg += "<p><input type=text id=\"w_No2\" placeholder=\""+ String(g_Dados.Aque_Solar_cfg.Atuador_Dev) +"\" maxlength=\"1\"></p>";
//           msg += "<p class=\"p_h\" onclick=\"f_s(2,'w_No2')\">Salvar</p>";
//           if ( g_Aquec_Solar.Bomba_Dev_Status() ) {
//             msg += "<p class=\"p_h\"\"> Online </p>";
//           } else {
//             msg += "<p class=\"p_h\"\"> Offline </p>";
//           }
//         msg += "</div>\n";
//         msg += "<div class=\"i_g\">";
//           msg += "<p> Saida do Atuador</p>";
//           msg += "<p><input type=text id=\"w_No3\" placeholder=\""+ String(g_Dados.Aque_Solar_cfg.Atuador_Saida) +"\" maxlength=\"1\"></p>";
//           msg += "<p class=\"p_h\" onclick=\"f_s(3,'w_No3')\">Salvar</p>";
//           // if ( g_Aquec_Solar.Bomba_Status() ) {
//           //   msg += "<p class=\"p_h\"\"> Ligada </p>";
//           // } else {
//           //   msg += "<p class=\"p_h\"\"> Desligada </p>";
//           // }
//           if ( g_Aquec_Solar.Bomba_Status() ) {
//             msg += "<p class=\"p_h\"\">" + String(g_Aquec_Solar.Bomba_GetPWM()) + "</p>";
//           } else {
//             msg += "<p class=\"p_h\"\"> Desligada </p>";
//           }
//         msg += "</div>\n";
//         msg += "<div class=\"i_g\">";
//           msg += "<p> PWM Bomba</p>";
//           msg += "<p><input type=text id=\"w_No10\" placeholder=\""+ String(g_Dados.Aque_Solar_cfg.Atuador_PWM) +"\" maxlength=\"3\"></p>";
//           msg += "<p class=\"p_h\" onclick=\"f_s(10,'w_No10')\">Salvar</p>";
//           msg += "<p class=\"p_h\"\"></p>";
//         msg += "</div>\n";
//         msg += "<div class=\"i_g\">";
//           msg += "<p> Sensor Caixa </p>";
//           msg += "<p><input type=text id=\"w_No4\" placeholder=\""+ String(g_Dados.Aque_Solar_cfg.T_Caixa_Dev) +"\" maxlength=\"1\"></p>";
//           msg += "<p class=\"p_h\" onclick=\"f_s(4,'w_No4')\">Salvar</p>";
//           msg += "<p class=\"p_h\"\">" + String(g_Aquec_Solar.Get_Temp_Caixa()) + "</p>";
//         msg += "</div>\n";
//         msg += "<div class=\"i_g\">";
//           msg += "<p> Sensor Retorno </p>";
//           msg += "<p><input type=text id=\"w_No5\" placeholder=\""+ String(g_Dados.Aque_Solar_cfg.T_Retorno_Dev) +"\" maxlength=\"1\"></p>";
//           msg += "<p class=\"p_h\" onclick=\"f_s(5,'w_No5')\">Salvar</p>";
//           msg += "<p class=\"p_h\"\">" + String(g_Aquec_Solar.Get_Temp_Retorno()) + "</p>";
//         msg += "</div>\n";
//         msg += "<div class=\"i_g\">";
//           msg += "<p> Sensor Aq Entrada </p>";
//           msg += "<p><input type=text id=\"w_No6\" placeholder=\""+ String(g_Dados.Aque_Solar_cfg.T_Aq_Entrada_Dev) +"\" maxlength=\"1\"></p>";
//           msg += "<p class=\"p_h\" onclick=\"f_s(6,'w_No6')\">Salvar</p>";
//           msg += "<p class=\"p_h\"\">" + String(g_Aquec_Solar.Get_Temp_Aq_Entrada()) + "</p>";
//         msg += "</div>\n";
//         msg += "<div class=\"i_g\">";
//           msg += "<p> Sensor Aq Saida </p>";
//           msg += "<p><input type=text id=\"w_No7\" placeholder=\""+ String(g_Dados.Aque_Solar_cfg.T_Aq_Saida_Dev) +"\" maxlength=\"1\"></p>";
//           msg += "<p class=\"p_h\" onclick=\"f_s(7,'w_No7')\">Salvar</p>";
//           msg += "<p class=\"p_h\"\">" + String(g_Aquec_Solar.Get_Temp_Aq_Saida()) + "</p>";
//         msg += "</div>\n";
//         msg += "<div class=\"i_g\">";
//           msg += "<p> Delta Ativação Aquecedor </p>";
//           msg += "<p><input type=text id=\"w_No8\" placeholder=\""+ String(g_Dados.Aque_Solar_cfg.Temp_Delta_Ativacao) +"\" maxlength=\"2\"></p>";
//           msg += "<p class=\"p_h\" onclick=\"f_s(8,'w_No8')\">Salvar</p>";
//         msg += "</div>\n";
//         msg += "<div class=\"i_g\">";
//           msg += "<p> Temp Congelamento </p>";
//           msg += "<p><input type=text id=\"w_No9\" placeholder=\""+ String(g_Dados.Aque_Solar_cfg.Temp_Congelamento) +"\" maxlength=\"2\"></p>";
//           msg += "<p class=\"p_h\" onclick=\"f_s(9,'w_No9')\">Salvar</p>";
//         msg += "</div>\n";
//       } else {
//         msg += "<p id=\"w_No0\"> Aquecedor desativado </p>";
//         msg += "<p class=\"p_h\" onclick=\"f_s(0,'w_No0')\">Ativar</p>";
//       }
//     msg += "</div>\n";
//   msg += "</div>";

//   msg += "<script>\n";
//   msg += "function f_s(x,z){";
//   msg +=   "c_b_1 = document.getElementById(z);";
//   msg +=   "const params={c_D:x,c_No:c_b_1.value};";
//   msg +=   "const http=new XMLHttpRequest();";
//   msg +=   "http.open('POST','/json_config_aquec_solar',true);";
//   msg +=   "http.setRequestHeader('Content-type','application/json');";
//   msg +=   "http.send(JSON.stringify(params));";
//   msg +=   "http.onload=function(){";
//   msg +=     "data_json=JSON.parse(http.responseText);";
//   msg +=     "console.log(data_json);";
//   // msg +=     "if(data_json.Res==\"Ok\") {";
//   // msg +=     "}else{alert(data_json.Status);}";
//   msg +=     "alert(data_json.Status);";
//   msg +=   "};";
//   msg += "}\n";
//   msg += "</script>";

//   msg += "</body></html>";

//   server.send(200, "text/html; charset=utf-8", msg);
// }

// void f_handleConfigBeer() {
//   g_Stat.Web_GET++;
//   DEBUG_PRINTF(" Web - Config_Beer");

// #ifdef ENABLE_WEB_PASSWORD
//   if (!server.authenticate(DEFAULT_LOCAL_USER_NAME, g_Dados.Local_PW)) {
//     return server.requestAuthentication();
//   }
// #endif

//   String msg;
//   msg.reserve(2000);
//   f_html_header(&msg,630);

//   msg += "<div class=\"g\">";
//     msg += "<div class=\"g_d\"> Configuração Beer ";
//       msg += "<p id=\"w_No1\"> Status:";
//       switch ( g_Beer.GetStatus() ) {
//         case DMD_BEER_OFF:              msg += " Off </p>";  break;
//         case DMD_BEER_INIT:             msg += " Init </p>"; break;
//         case DMD_BEER_IDLE:             msg += " Idle </p>"; break;
//         case DMD_BEER_RESFRIANDO:       msg += " Resfriando (" + String(g_Beer.Get_Seg()) + ") </p>"; break;
//         case DMD_BEER_RESFRIANDO_WAIT:  msg += " Resfriando (Wait - " + String(g_Beer.Get_Seg()) + ") </p>"; break;
//         case DMD_BEER_AQUECENDO:        msg += " Aquecendo </p>";  break;
//         case DMD_BEER_ANTICONGELAMENTO: msg += " Anticongelamento </p>";  break;
//         case DMD_BEER_ERRO:             msg += " Erro </p>";  break;
//         default:
//         break;
//       }
//       if ( g_Beer.IsOn() ) {
//         msg += "<p class=\"p_h\" onclick=\"f_s(21,'w_No1')\">Desativar</p>";

//         msg += "<div class=\"i_g\">";
//           msg += "<p> Modo Monitor</p>";
//           if ( g_Beer.Get_ModoMonitor() ) {
//             msg += "<p class=\"Md\"\"> ON </p>";
//           } else {
//             msg += "<p class=\"Mx\"\"> OFF </p>";
//           }
//           msg += "<p class=\"p_h\" onclick=\"f_s(35,1)\">Ativar</p>";
//           msg += "<p class=\"p_h\" onclick=\"f_s(35,0)\">Desativar</p>";
//         msg += "</div>\n";

//         msg += "<div class=\"i_g\">";
//           msg += "<p> Fast Cool</p>";
//           if ( g_Beer.Get_FastCool() ) {
//             msg += "<p class=\"Md\"\"> ON </p>";
//           } else {
//             msg += "<p class=\"Mx\"\"> OFF </p>";
//           }
//           msg += "<p class=\"p_h\" onclick=\"f_s(31,1)\">Ativar</p>";
//           msg += "<p class=\"p_h\" onclick=\"f_s(31,0)\">Desativar</p>";
//         msg += "</div>\n";

//         msg += "<div class=\"i_g\">";
//           msg += "<p> Usar Aquecedor</p>";
//           if ( g_Beer.Get_AquecedorUsar() ) {
//             msg += "<p class=\"Md\"\"> ON </p>";
//           } else {
//             msg += "<p class=\"Mx\"\"> OFF </p>";
//           }
//           msg += "<p class=\"p_h\" onclick=\"f_s(30,1)\">Ativar</p>";
//           msg += "<p class=\"p_h\" onclick=\"f_s(30,0)\">Desativar</p>";
//         msg += "</div>\n";

//         msg += "<p class=\"p_h\"></p>";

//         msg += "<div class=\"i_g\">";
//           msg += "<p> Slave Atuador</p>";
//           msg += "<p><input type=text id=\"w_No2\" placeholder=\""+ String(g_Beer.Get_Atuador_Dev()) +"\" maxlength=\"1\"></p>";
//           msg += "<p class=\"p_h\" onclick=\"f_s(2,'w_No2')\">Salvar</p>";
//           if ( g_Beer.Motor_Dev_Status() ) {
//             msg += "<p class=\"p_h\"\"> Online </p>";
//           } else {
//             msg += "<p class=\"p_h\"\"> Offline </p>";
//           }
//         msg += "</div>\n";
//         msg += "<div class=\"i_g\">";
//           msg += "<p> Saida Motor</p>";
//           msg += "<p><input type=text id=\"w_No3\" placeholder=\""+ String(g_Beer.Get_Atuador_Saida()) +"\" maxlength=\"1\"></p>";
//           msg += "<p class=\"p_h\" onclick=\"f_s(3,'w_No3')\">Salvar</p>";
//           // if ( g_Aquec_Solar.Bomba_Status() ) {
//           //   msg += "<p class=\"p_h\"\"> Ligada </p>";
//           // } else {
//           //   msg += "<p class=\"p_h\"\"> Desligada </p>";
//           // }
//           if ( g_Beer.Motor_Status() ) {
//             msg += "<p class=\"p_h\"\"> Ligado </p>";
//           } else {
//             msg += "<p class=\"p_h\"\"> Desligado </p>";
//           }
//         msg += "</div>\n";

//         msg += "<div class=\"i_g\">";
//           msg += "<p> Saida Aquecedor</p>";
//           msg += "<p><input type=text id=\"w_No3\" placeholder=\""+ String( g_Beer.Get_Atuador_Saida() + 1) +"\" maxlength=\"1\"></p>";
//           msg += "<p class=\"p_h\"> </p>";
//           if ( g_Beer.Res_Status() ) {
//             msg += "<p class=\"p_h\"\"> Ligado </p>";
//           } else {
//             msg += "<p class=\"p_h\"\"> Desligado </p>";
//           }
//         msg += "</div>\n";

//         // msg += "<div class=\"i_g\">";
//         //   msg += "<p> PWM Bomba</p>";
//         //   msg += "<p><input type=text id=\"w_No10\" placeholder=\""+ String(g_Dados.Aque_Solar_cfg.Atuador_PWM) +"\" maxlength=\"3\"></p>";
//         //   msg += "<p class=\"p_h\" onclick=\"f_s(10,'w_No10')\">Salvar</p>";
//         //   msg += "<p class=\"p_h\"\"></p>";
//         // msg += "</div>\n";
//         msg += "<div class=\"i_g\">";
//           msg += "<p> Sensor Interno </p>";
//           msg += "<p><input type=text id=\"w_No4\" placeholder=\""+ String(g_Beer.Get_Sensor_Interna()) +"\" maxlength=\"1\"></p>";
//           msg += "<p class=\"p_h\" onclick=\"f_s(4,'w_No4')\">Salvar</p>";
//           msg += "<p class=\"p_h\"\">" + String(g_Beer.Get_Temp_Interna()) + "</p>";
//         msg += "</div>\n";
//         msg += "<div class=\"i_g\">";
//           msg += "<p> Sensor Beer 1 </p>";
//           msg += "<p><input type=text id=\"w_No5\" placeholder=\""+ String(g_Beer.Get_Sensor_Beer1()) +"\" maxlength=\"1\"></p>";
//           msg += "<p class=\"p_h\" onclick=\"f_s(5,'w_No5')\">Salvar</p>";
//           msg += "<p class=\"p_h\"\">" + String(g_Beer.Get_Temp_Beer1()) + "</p>";
//         msg += "</div>\n";
//         msg += "<div class=\"i_g\">";
//           msg += "<p> Sensor Beer 2 </p>";
//           msg += "<p><input type=text id=\"w_No6\" placeholder=\""+ String(g_Beer.Get_Sensor_Beer2()) +"\" maxlength=\"1\"></p>";
//           msg += "<p class=\"p_h\" onclick=\"f_s(6,'w_No6')\">Salvar</p>";
//           msg += "<p class=\"p_h\"\">" + String(g_Beer.Get_Temp_Beer2()) + "</p>";
//         msg += "</div>\n";
//         msg += "<div class=\"i_g\">";
//           msg += "<p> Sensor Beer 2 </p>";
//           msg += "<p><input type=text id=\"w_No7\" placeholder=\""+ String(g_Beer.Get_Sensor_Beer3()) +"\" maxlength=\"1\"></p>";
//           msg += "<p class=\"p_h\" onclick=\"f_s(7,'w_No7')\">Salvar</p>";
//           msg += "<p class=\"p_h\"\">" + String(g_Beer.Get_Temp_Beer3()) + "</p>";
//         msg += "</div>\n";

//         msg += "<p class=\"p_h\"></p>";

//         msg += "<div class=\"i_g\">";
//           msg += "<p> Delta ativação</p>";
//           msg += "<p><input type=text id=\"w_No8\" placeholder=\""+ String(g_Beer.Get_Temp_Delta_Ativacao()) +"\" maxlength=\"3\"></p>";
//           msg += "<p class=\"p_h\" onclick=\"f_s(28,'w_No8')\">Salvar</p>";
//         msg += "</div>\n";
//         msg += "<div class=\"i_g\">";
//           msg += "<p> Temp Preset </p>";
//           msg += "<p><input type=text id=\"w_No9\" placeholder=\""+ String(g_Beer.Get_Temp_Preset()) +"\" maxlength=\"2\"></p>";
//           msg += "<p class=\"p_h\" onclick=\"f_s(29,'w_No9')\">Salvar</p>";
//         msg += "</div>\n";
//       } else {
//         msg += "<p id=\"w_No0\"> Beer desativado </p>";
//         msg += "<p class=\"p_h\" onclick=\"f_s(20,'w_No0')\">Ativar</p>";
//       }
//     msg += "</div>\n";
//   msg += "</div>";

//   msg += "<script>\n";
//   msg += "function f_s(x,z){";
//   msg +=   "l_val = 0;";
//   msg +=   "if (z == 0 || z == 1 ) { l_val = z } else {";
//   msg +=     "c_b_1 = document.getElementById(z); l_val = c_b_1.value;";
//   msg +=   "} const params={c_D:x,c_No:l_val,c_Fo:l_val};";
//   msg +=   "const http=new XMLHttpRequest();";
//   msg +=   "http.open('POST','/json_config_beer_ferm',true);";
//   msg +=   "http.setRequestHeader('Content-type','application/json');";
//   msg +=   "http.send(JSON.stringify(params));";
//   msg +=   "http.onload=function(){";
//   msg +=     "data_json=JSON.parse(http.responseText);";
//   msg +=     "console.log(data_json);";
//   // msg +=     "if(data_json.Res==\"Ok\") {";
//   // msg +=     "}else{alert(data_json.Status);}";
//   msg +=     "alert(data_json.Status);";
//   msg +=   "};";
//   msg += "}\n";
//   msg += "</script>";

//   msg += "</body></html>";

//   server.send(200, "text/html; charset=utf-8", msg);
// }

