/*******************************************************************************
 * Autor:   Diego Dallastella
 * Projeto: WebServer
 * Modulo:  Server
 * Versão:  0.5
 * Histórico: 
 * 
 * Elementos:
 *           Placa ESP
 *
  ******************************************************************************/
#ifndef DMD_WebServer_PROG_H
#define DMD_WebServer_PROG_H

#include "Arduino.h"

void DMD_WebServer_Init_Prog_JSON();

void f_handleRoot_Prog();

void f_json_dinamico  ();


const char html_base[] PROGMEM = R"rawliteral(
<html>
<head>
<title>DControl</title>
<meta http-equiv="X-UA-Compatible" content="IE-edge">
<meta name="viewport" content="width=device-width, initial-scale=1">
<style>:root{--primary-100:#;--primary-200:#abc3d1;--primary-300:#;--accent-100:#;--accent-200:#;--text-100:#555;--text-200:#;--bg-100:#f5f5f5;--bg-200:#b3cfde;--bg-300:#d0e3ed;--bg-btn:rgba(255,255,255,0);--bg-Mx:#edd0d4;--bg-Md:#d4edd0;--bg-Mi:#d6e6ef}body{background-color:var(--bg-100);color:var(--text-100);margin:auto;text-align:center;font-family:"Helvetica Neue",Helvetica,Arial,sans-serif;max-width:630px}.b{margin:5px;border:1px solid var(--primary-200);border-radius:15px}.m{padding:5px;margin:6px 5px;font-size:2.0em;background-color:var(--bg-200);border-radius:15px;cursor:pointer}.m_t{font-size:.3em}.g{margin:5px;display:grid;grid-gap:5px;align-content:center;grid-template-columns:repeat(auto-fit,minmax(200px,2fr))}.g_d{padding:5px 0;border:1px solid var(--primary-200);border-radius:15px}.g_d_s{font-size:.6em}p,.p_h{margin:5px;padding:4px 5px;background-color:var(--bg-300);border-radius:15px;font-size:.8em}.p_h:hover{background-color:var(--bg-200);cursor:pointer}input{display:block;width:100%;padding:1px;border-radius:15px;border:0;background-color:var(--bg-100);text-align:center}input:hover,input:focus{opacity:.7;color:#08c;box-shadow:0 1px 0 rgba(255,255,255,.25),inset 0 3px 6px rgba(0,0,0,.25)}button,.btn{background:var(--bg-btn);width:100%;border:0;color:var(--text-100)}input[type=file]{display:none}#txt{display:none}.i_g{display:grid;grid-template-columns:repeat(auto-fit,minmax(50px,2fr))}.i_t{font-size:.8em}.At{font-size:2.0em;background-color:var(--bg-100)}.Mx{background-color:var(--bg-Mx)}.Md{background-color:var(--bg-Md)}.Mi{background-color:var(--bg-Mi)}</style>
</head>
<body>
<div class="b">
<div class="m" onclick="location.href='/'">
<div class="m_t">Sistema Embarcado</div>
<div id="dmd_nome">DMDallas</div>
<div class="m_t" id="dmd_hora">carregando hora do sistema</div>
</div>
<div class="g">
<div class="g" id="main">
</div>
</div>
<div class="g" id="links">
</div>
<script>/*<![CDATA[*/p_Dados={caixa:[{tit:"Carregando dados",item:[]}]};
p_Dados_Links={caixa:[{tit:"Links",item:[
{tipo:2,texto:"Home",cmd:1,sub:1},
{tipo:2,texto:"Config RF",cmd:195,sub:1},
{tipo:2,texto:"Config Cores",cmd:196,sub:1},
{tipo:2,texto:"Atualizar",cmd:1,sub:1}
]}]};function Init(){Criar_Caixa("main");p_Dados=p_Dados_Links;Criar_Caixa("links");f_N(0,0,1,1,0)}Init();function Criar_Caixa(a){c_data="";for(let index=0;index<p_Dados.caixa.length;index++){if(!p_Dados.caixa[index].hasOwnProperty("tit")){continue}c_data+='<div class="g_d">';c_data+=p_Dados.caixa[index].tit;for(let i=0;i<p_Dados.caixa[index].item.length;i++){switch(p_Dados.caixa[index].item[i].tipo){case 0:c_data+="<p>"+p_Dados.caixa[index].item[i].texto+"</p>";break;case 1:c_data+='<p class="p_h" onclick="location.href=\''+p_Dados.caixa[index].item[i].link+"'\">"+p_Dados.caixa[index].item[i].texto+"</p>";break;case 2:c_data+='<p class="p_h" onclick="f_N(2,0,'+p_Dados.caixa[index].item[i].cmd+","+p_Dados.caixa[index].item[i].sub+',0)">'+p_Dados.caixa[index].item[i].texto+"</p>";break;case 5:c_data+='<div class="i_g"><p>'+p_Dados.caixa[index].item[i].texto+"</p>";if(p_Dados.caixa[index].item[i].dado==0){c_data+='<p class="Mx"> OFF </p>';c_data+='<p class="p_h" onclick="f_N(2,\''+p_Dados.caixa[index].item[i].id+"',"+p_Dados.caixa[index].item[i].cmd+","+p_Dados.caixa[index].item[i].sub+',1)"> Ativar</p></div>'}else{c_data+='<p class="Md"> ON </p>';c_data+='<p class="p_h" onclick="f_N(2,\''+p_Dados.caixa[index].item[i].id+"',"+p_Dados.caixa[index].item[i].cmd+","+p_Dados.caixa[index].item[i].sub+',0)"> Desativar</p></div>'}break;case 10:c_data+='<div class="i_g"><p>'+p_Dados.caixa[index].item[i].texto+"</p>";c_data+='<p><input type=text id="'+p_Dados.caixa[index].item[i].id+'" placeholder="'+p_Dados.caixa[index].item[i].dado+'"> </p>';c_data+='<p class="p_h" onclick="f_N(3,\''+p_Dados.caixa[index].item[i].id+"',"+p_Dados.caixa[index].item[i].cmd+","+p_Dados.caixa[index].item[i].sub+","+p_Dados.caixa[index].item[i].dado+')">Salvar</p>';if(p_Dados.caixa[index].item[i].hasOwnProperty('status')){c_data+="<p>"+p_Dados.caixa[index].item[i].status+"</p>"}c_data+="</div>";break;case 15:c_data+='<p class="p_h" id="'+p_Dados.caixa[index].item[i].id+'" onclick="f_N(1,\''+p_Dados.caixa[index].item[i].id+"',"+p_Dados.caixa[index].item[i].cmd+","+p_Dados.caixa[index].item[i].sub+","+p_Dados.caixa[index].item[i].dado+')">'+p_Dados.caixa[index].item[i].texto+"</p>";break;case 20:c_data+='<div class="i_g"><p>'+p_Dados.caixa[index].item[i].texto+"</p>";c_data+='<p><input type=range id="'+p_Dados.caixa[index].item[i].id+'" value="'+p_Dados.caixa[index].item[i].dado+'"min="'+p_Dados.caixa[index].item[i].min+'" max="'+p_Dados.caixa[index].item[i].max+'"> </p>';c_data+='<p class="p_h" onclick="f_N(3,\''+p_Dados.caixa[index].item[i].id+"',"+p_Dados.caixa[index].item[i].cmd+","+p_Dados.caixa[index].item[i].sub+","+p_Dados.caixa[index].item[i].dado+')">Salvar</p>';c_data+="</div>";break}}c_data+="</div>"}document.getElementById(a).innerHTML+=c_data}function Recarrega(){document.getElementById("dmd_nome").innerHTML=p_Dados.Sis+" - "+p_Dados.Nome;document.getElementById("dmd_hora").innerHTML=p_Dados.Hora;document.getElementById("main").innerHTML="";Criar_Caixa("main")}function f_N(a,e,f,c,b){c_log={l_log:"log f_N",f_tipo:a,f_id:e,f_cod:f,f_sub:c,f_dado:b};console.log(c_log);c_id_html=document.getElementById(e);if(a==3){integerValue=parseInt(c_id_html.value);if(!isNaN(integerValue)){l_dado=integerValue}else{l_dado=c_id_html.value}}else{l_dado=b}const g={c_Cod:f,c_Sub:c,c_Dad:l_dado};
console.log(g);const 
d=new XMLHttpRequest();
d.open("POST","/json_din",true);
d.setRequestHeader("Content-type","application/json");
d.send(JSON.stringify(g));
d.onload=function(){
console.log(d.responseText);
p_Dados=JSON.parse(d.responseText);
Recarrega()}};/*]]>*/</script>
</body>
</html>
)rawliteral";
#endif