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
#ifndef DMD_RF_RECEIVER_H
#define DMD_RF_RECEIVER_H

/*
 * Inclusao de libraries
 */
#include "DMD_Config.h"
#include <RCSwitch.h>

// extern RCSwitch DMD_Switch;

/*
 * Definições gerais do Modulo
 */
#ifdef DMD_DEBUG_RF
  #define D_P_RF_LN(x)  Serial.println(x)
  #define D_P_RF(x)     Serial.print(x)
  #define D_P_RF_LN_F(x)  Serial.println(F(x))
  #define D_P_RF_F(x)     Serial.print(F(x))
#else
  #define D_P_RF_LN(x)
  #define D_P_RF(x)
  #define D_P_RF_LN_F(x)
  #define D_P_RF_F(x)
#endif

#define DMD_RC_RF_MAX_BT 10

#define DMD_RF_NO_BT 0xFF

enum DMD_RF_Data_State {
  DMD_RF_Data_OFF,
  DMD_RF_Data_ACTIV,
  DMD_RF_Data_Wait_ADD,
  DMD_RF_Data_END
};

/*
 * Estruturas de dados
 */
struct DMD_RF_Dado{
  unsigned long Address;
  uint8_t       BT;
  DMD_RF_Data_State State;
};

/*
 * Funções
 */

/*
 * Classe de controle do RF 4 botões
 */
class DMD_RC_RF {
  DMD_RF_Dado *c_RF_Data;
  unsigned long c_last_mills;
  bool c_Wait_Add;

  void (*c_CallBack)(uint8_t);

  public:
    void Init ( int, DMD_RF_Dado *, void (*)(uint8_t) );

    void Loop ( );

    bool Add_Device( uint8_t );

    bool Rem_Device( uint8_t );

};

extern DMD_RC_RF g_RC_RF;

#endif