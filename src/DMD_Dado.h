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
#ifndef DMD_DADO_H
#define DMD_DADO_H

/*
 * Inclusao de libraries
 */
#include "float.h"
#include "limits.h"

#include "DMD_Config.h"

/*
 * Definições gerais do Modulo
 */
#ifdef DMD_DEBUG_DADO
  #define D_P_DADO_LN(x)  Serial.println(x)
  #define D_P_DADO(x)     Serial.print(x)
#else
  #define D_P_DADO_LN(x)
  #define D_P_DADO(x)
#endif

/*
 * Variável para definir quantidade de leitura a considerar para a média
 */
extern uint8_t g_Med_Num;

void SetGlobalMedNum (uint8_t l_Med_Num);

/*
 * Classe de armazenamento de dados
 */
class DMD_DADO_FLOAT {
  public:
    bool  c_Data_OK;
    bool  c_Data_Novo;
    uint8_t Data_Count;
    float Dado;
    float Dado_Med;
    float Dado_Max;
    float Dado_Min;

  public:
    DMD_DADO_FLOAT ();

    void  Clear ();

    void  SetRead (){ c_Data_Novo = false; }

    float Get();

    bool  HasData () {
      if ( c_Data_OK )
        return true;
      return false; 
    }

    bool  HasNewData () {
      if ( c_Data_Novo )
        return true;
      return false; 
    }

    void  Save(float);

    float Get_Med(){ SetRead(); return (Dado_Med/Data_Count); }
    float Get_Max(){ return (Dado_Max); }
    float Get_Min(){ return (Dado_Min); }

    float Get_MedNoRead(){ return (Dado_Med/Data_Count); }

};

class DMD_DADO_INT {
  public:
    bool c_Data_OK;
    bool c_Data_Novo;
    uint8_t Data_Count;
    int Dado;
    long Dado_Med;
    int Dado_Max;
    int Dado_Min;

  public:
    DMD_DADO_INT ();

    void  Clear ();

    void  SetRead (){ c_Data_Novo = false; }

    int   Get();

    bool  HasData () {
      if ( c_Data_OK )
        return true;
      return false; 
    }

    bool  HasNewData () {
      if ( c_Data_Novo )
        return true;
      return false; 
    }

    void  Save(int);

    int   Get_Med(){ SetRead(); return (Dado_Med/Data_Count); }
    int   Get_Max(){ return (Dado_Max); }
    int   Get_Min(){ return (Dado_Min); }

    int   Get_MedNoRead(){ return (Dado_Med/Data_Count); }

};

uint16_t dmd_map(uint16_t x, uint16_t in_min, uint16_t in_max, uint16_t out_min, uint16_t out_max);

#endif /* DMD_DADO_H */