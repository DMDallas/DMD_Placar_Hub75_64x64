/*******************************************************************************
 * Autor:   Diego Dallastella
 * Projeto: Base
 * Modulo:  Armazenamento de dados para Sensores
 * Versão:  2.0
 *
 * Elementos:
 *
 * Libraries:
 *
 * Pinos Utilizados
 *
  ******************************************************************************/

/*
 * Definições do Projeto
 */
#include "DMD_Config.h"

/*
 * Inclusao funções e dados do projeto
 */
#include "DMD_Dado.h"

/*
 * Variável para definir quantidade de leitura a considerar para a média
 */
uint8_t g_Med_Num = DMD_DADO_MED_NUM;

void SetGlobalMedNum (uint8_t l_Med_Num) {
  g_Med_Num = l_Med_Num; 
}

/*
 *  Inicializa a classe limpando os dados
 */
DMD_DADO_FLOAT::DMD_DADO_FLOAT() {
  Clear();
}

/*
 *  Zera os valores de todos as variáveis e inicializa demais dados
 */
void DMD_DADO_FLOAT::Clear () {
  Dado = 0;
  Data_Count = 0;
  Dado_Min = FLT_MAX;
  Dado_Max = 0;
  Dado_Med = 0;

  c_Data_OK = false;
  c_Data_Novo = false;
}

/*
 *  Retorna o último dado armazenado e seta como lido
 */
float DMD_DADO_FLOAT::Get(){
  D_P_DADO("F Get ");
  if (c_Data_OK) {
    D_P_DADO(Dado);
    SetRead ();
    return (Dado);
  } else {
    D_P_DADO("Erro");
    return (FLT_MIN);
  }
}

/*
 *  Salva o dado recebido, calculando média, min e max
 */
void DMD_DADO_FLOAT::Save(float l_data){
  Dado = l_data;

  if ( l_data > Dado_Max )
    Dado_Max = l_data;

  if ( l_data < Dado_Min )
    Dado_Min = l_data;

  if ( Data_Count < g_Med_Num ) {
    Data_Count++;
  } else {
    Dado_Med -= Dado_Med/Data_Count;
  }

  Dado_Med = Dado_Med + l_data;

  D_P_DADO(" F Dado("); D_P_DADO(Data_Count);
  D_P_DADO(")= ");      D_P_DADO(Dado);
  D_P_DADO(" > Max=");  D_P_DADO(String(Get_Max()));
  D_P_DADO(" > Min=");  D_P_DADO(String(Get_Min()));
  D_P_DADO(" > Med=");  D_P_DADO_LN(String(Get_Med()));
  
  c_Data_OK = true;
  c_Data_Novo = true;
}


/*
 *  Inicializa a classe limpando os dados
 */
DMD_DADO_INT::DMD_DADO_INT() {
  Clear();
}

/*
 *  Zera os valores de todos as variáveis e inicializa demais dados
 */
void DMD_DADO_INT::Clear () {
  Dado = 0;
  Data_Count = 0;
  Dado_Min = INT_MAX;
  Dado_Max = 0;
  Dado_Med = 0;

  c_Data_OK = false;
  c_Data_Novo = false;
}

/*
 *  Retorna o último dado armazenado e seta como lido
 */
int DMD_DADO_INT::Get(){
  D_P_DADO("I Get");
  if (c_Data_OK) {
    D_P_DADO(Dado);
    SetRead ();
    return (Dado);
  } else {
    D_P_DADO("Erro");    
    return (INT_MIN);
  }
}

/*
 *  Salva o dado recebido sobrepondo o dado mais antigo
 */
void DMD_DADO_INT::Save(int l_data){
  Dado = l_data;

  if ( l_data > Dado_Max )
    Dado_Max = l_data;

  if ( l_data < Dado_Min )
    Dado_Min = l_data;

  if ( Data_Count < g_Med_Num ) {
    Data_Count++;
  } else {
    Dado_Med -= Dado_Med/Data_Count;
  }

  Dado_Med = Dado_Med + l_data;

  D_P_DADO(" I Dado("); D_P_DADO(Data_Count);
  D_P_DADO(")= ");      D_P_DADO(Dado);
  D_P_DADO(" > Max=");  D_P_DADO(String(Get_Max()));
  D_P_DADO(" > Min=");  D_P_DADO(String(Get_Min()));
  D_P_DADO(" > Med=");  D_P_DADO_LN(String(Get_Med()));
  
  c_Data_OK = true;
  c_Data_Novo = true;
}

uint16_t dmd_map(uint16_t x, uint16_t in_min, uint16_t in_max, uint16_t out_min, uint16_t out_max) {
  long l_outmax_X10 = out_max * 10000;

  if ( (in_max == 0) && (in_min == 0) ) {
    return 0;
  }

  if ( in_min < in_max ) {
    // Variação positiva
    if ( x <= in_min )
      return 0;
  } else {
    // Variação negativa
    if ( x >= in_min )
      return 0;
  }

  long l_res = (l_outmax_X10 - out_min) / (in_max - in_min);

  l_res = (x - in_min) * l_res + out_min;

  return l_res/10000;
}