/**
 * @brief Controle das entradas e saidas placa
 * 
 * Autor:    Diego M Dallastella
 * Revisão:  0.5
 * 
 */
#ifndef DMD_PLACA_h
#define DMD_PLACA_h

/*
 * Inclusao de libraries
 */

/*
 * Inclusao funções e dados do projeto
 */
#include "DMD_Config.h"
#include "DMD_Globais.h"

/*
 * Definições gerais do Modulo
 */
#ifdef DMD_DEBUG_PLACA
  #define D_PL_LN(x)    Serial.println(x)
  #define D_PL(x)       Serial.print(x)
  #define D_PL_LN_F(x)  Serial.println(F(x))
  #define D_PL_F(x)     Serial.print(F(x))
#else
  #define D_PL_LN(x)
  #define D_PL(x)
  #define D_PL_LN_F(x)
  #define D_PL_F(x)
#endif


/*
 * Dados do projeto devem ser definidos o DMD_Config.h do projeto
 */
#ifndef DMD_BT_TEMP_DEBOUNCE
  #define DMD_BT_TEMP_DEBOUNCE 5
#endif

#define DETEC_TACH_STATUS_IDLE       1
#define DETEC_TACH_STATUS_DEBOUNCE   2
#define DETEC_TACH_STATUS_WAIT_END   3
#define DETEC_TACH_STATUS_OFF        4

/*
 * Classe de controle de Botões ADC
 */
class DMD_Tach {
    uint8_t  c_Detec_Status;
    uint16_t c_Tempo;

    uint8_t  c_Pin;
    
    bool     c_Status;

    uint16_t c_Temp_Debounce;
    uint32_t c_Tach_count;

  public:
    DMD_Tach( uint f_pin ) {
      D_PL_LN (" Tach");
      c_Pin = f_pin;
      pinMode(c_Pin, INPUT_PULLUP);

      c_Temp_Debounce = DMD_BT_TEMP_DEBOUNCE;
      c_Detec_Status = DETEC_TACH_STATUS_IDLE;

      Clear();
    }

    void Clear(){
      c_Tach_count = 0;
    }

    uint32_t Get_Tach_Count (){
      return c_Tach_count;
    }

    bool Loop ( ) {
      c_Status = digitalRead(c_Pin);
      switch (c_Detec_Status) {
        case DETEC_TACH_STATUS_IDLE:
        default:
          if ( c_Status ){ // Botao Pressionado
            c_Detec_Status = DETEC_TACH_STATUS_DEBOUNCE;
            c_Tempo = 0;
          }
          break;
        case DETEC_TACH_STATUS_DEBOUNCE:
          if ( c_Status ){ // Continua Pressionado
            c_Tempo++;
            if ( c_Tempo >= c_Temp_Debounce){
              // Chegou a tempo para considerar Pressionado
              c_Detec_Status = DETEC_TACH_STATUS_WAIT_END;
              // c_BT_Press = true;
              c_Tach_count ++;
              return true;
            }
          } else {
            // Somente um pulso muito curto, desconsidera (provável erro)
            c_Detec_Status = DETEC_TACH_STATUS_IDLE;
            c_Tempo = 0;
          }
          break;
        case DETEC_TACH_STATUS_WAIT_END:
          if ( !c_Status ){ // Aguarda liberar o Botão
            c_Detec_Status = DETEC_TACH_STATUS_IDLE;
            c_Tempo = 0;
          }
          break;
        case DETEC_TACH_STATUS_OFF:
          break;
      }
      return false;
    }

    void SetTempDebouce (uint16_t f_temp_debouce) {
      c_Temp_Debounce = f_temp_debouce;
    }
};

extern DMD_Tach   g_Tach;

class DMD_Buzzer {
  public:
    uint8_t c_ticks;

    void Ativar ( uint8_t f_ticks) {
      g_Bz.On();
      c_ticks = f_ticks;
    }

    void Loop () {
      if (c_ticks != 0) {
        if (c_ticks > 1 ) {
          c_ticks --;
        } else {
          g_Bz.Off();
          c_ticks = 0;
        }
      }
    }

    void Desativar ( ) {
      g_Bz.Off();
    }
};

extern DMD_Buzzer g_Buzzer;

void Ativar_Rele    ();
void Desativar_Rele ();

void Ativar_Bz      ();
void Desativar_Bz   ();

void DMD_Start      ();
void DMD_Stop       ();
void DMD_Start_Stop ();
void DMD_Clear      ();

void DMD_Level_UP    ();
void DMD_Level_Down  ();
void DMD_Level_Update();

void Init_Placa ( uint16_t );

#endif // DMD_PLACA_h
