/*******************************************************************************
 * Autor:   Diego Dallastella
 * Projeto: Base
 * Modulo:  Controle do receptor de RF
 * Versão:  0.1
 * Histórico > 
 * 
 * Elementos:
 *
 * Libraries:
 *           HT60P20              > lib_deps = incluida no projeto
 *
 * Pinos Utilizados
 *          RX do receptor de RF
 * 
  ******************************************************************************/
/*
 * Definições do Projeto
 */
#include "DMD_Globais.h"

/*
* Inclusao de libraries
*/
//#include <.h> Incluida no .h para que sejam replicadas definições - incluir somente o DMD_*.H
#include "DMD_RF_RC_Switch.h"


/*
* Inclusao funções e dados do projeto
*/

/*
* Variável Global
*/
RCSwitch DMD_Switch = RCSwitch();

DMD_RC_RF g_RC_RF;

void DMD_RC_RF::Init (int f_pin, DMD_RF_Dado *f_RF_Data, void (*f_CallBack)(uint8_t)){
  DMD_Switch.enableReceive(f_pin);

  c_RF_Data = f_RF_Data;

  c_CallBack = f_CallBack;

  for (size_t i = 0; i < DMD_RC_RF_MAX_BT; i++) {
    if ( c_RF_Data[i].State == 0xFF ) {
      // Dado não valido
      c_RF_Data[i].State = DMD_RF_Data_OFF;
    }
  }

}

void DMD_RC_RF::Loop ( ){
  unsigned long f_add;

  if (DMD_Switch.available()) {
    f_add = DMD_Switch.getReceivedValue();

    if ( c_Wait_Add ) {
      D_P_RF("RF Add");

      if ( ( millis() - c_last_mills ) < 5000 ) { // 5 segundos para adicionar um disp
        D_P_RF(" OK ");
        for (size_t i = 0; i < DMD_RC_RF_MAX_BT; i++) {
          if ( c_RF_Data[i].State == DMD_RF_Data_Wait_ADD ) {
            D_P_RF(" Encontrado:"); D_P_RF(i);
            
            c_RF_Data[i].Address = f_add;
            c_RF_Data[i].State = DMD_RF_Data_ACTIV;
            c_Wait_Add = false;
            c_last_mills = millis();
            D_P_RF(" Addr:"); D_P_RF_LN(c_RF_Data[i].Address);
          }
        }
        DMD_Switch.resetAvailable();
        return;
      } else {
        D_P_RF("Add Timeout");
        c_Wait_Add = false;
        for (size_t i = 0; i < DMD_RC_RF_MAX_BT; i++) {
          if ( c_RF_Data[i].State == DMD_RF_Data_Wait_ADD ) {
            c_RF_Data[i].State == DMD_RF_Data_OFF;
          }
        }
      }
    }

    if ( ( millis() - c_last_mills ) < g_Dados.RF_Repeat_Time ) {
      D_P_RF("Received time repeat");
      c_last_mills = millis();
      DMD_Switch.resetAvailable();
      return;
    }

    D_P_RF("Received "); D_P_RF( f_add );  D_P_RF(" / ");
    D_P_RF( DMD_Switch.getReceivedBitlength() );
    D_P_RF("bit - Protocol: "); D_P_RF_LN( DMD_Switch.getReceivedProtocol() );

    for (size_t i = 0; i < DMD_RC_RF_MAX_BT; i++) {
      if ( c_RF_Data[i].State == DMD_RF_Data_ACTIV ) {
        if ( c_RF_Data[i].Address == f_add ) {
          if (c_CallBack )
            c_CallBack(c_RF_Data[i].BT);
          c_last_mills = millis();
        }
      }
    }

    DMD_Switch.resetAvailable();
  }
}

bool DMD_RC_RF::Add_Device ( uint8_t f_bt ) {

  if ( f_bt >= DMD_RC_RF_MAX_BT )
    return false;

  c_Wait_Add = true;
  c_RF_Data[f_bt].State = DMD_RF_Data_Wait_ADD;
  c_last_mills = millis();
  return true;
}

bool DMD_RC_RF::Rem_Device ( uint8_t f_bt ) {

  if ( f_bt >= DMD_RC_RF_MAX_BT )
    return false;

  c_RF_Data[f_bt].State = DMD_RF_Data_OFF;
  c_RF_Data[f_bt].BT = DMD_RF_NO_BT;
  c_RF_Data[f_bt].Address = 0;

  return true;
}
