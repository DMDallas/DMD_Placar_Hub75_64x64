/**
 * @brief Controle do status de um Botão.
 * 
 * Autor:    Diego M Dallastella
 * Revisão:  1.0
 * 
 */
#ifndef DMD_BT_h
#define DMD_BT_h

/*
 * Inclusao de libraries
 */

/*
 * Inclusao funções e dados do projeto
 */
#include "DMD_Config.h" 

/*
 * Definições gerais do Modulo
 */
#ifdef DMD_DEBUG_BT
  #define D_BT_LN(x)    Serial.println(x)
  #define D_BT(x)       Serial.print(x)
  #define D_BT_LN_F(x)  Serial.println(F(x))
  #define D_BT_F(x)     Serial.print(F(x))
#else
  #define D_BT_LN(x)
  #define D_BT(x)
  #define D_BT_LN_F(x)
  #define D_BT_F(x)
#endif


/*
 * Dados do projeto devem ser definidos o DMD_Config.h do projeto
 */
#ifndef DMD_BT_TEMP_DEBOUNCE
  #define DMD_BT_TEMP_DEBOUNCE 25
#endif

#ifndef DMD_BT_TEMP_LONG
  #define DMD_BT_TEMP_LONG   500
#endif

#define DMD_BT_NO_BUTTON           0xFF

#define DETEC_BT_STATUS_IDLE       1
#define DETEC_BT_STATUS_DEBOUNCE   2
#define DETEC_BT_STATUS_WAIT_LONG  3
#define DETEC_BT_STATUS_WAIT_END   4
#define DETEC_BT_STATUS_OFF        6

/*
 * Classe de controle de Botões ADC
 */
class DMD_BT {
    uint8_t  c_Detec_Status;
    uint16_t c_Tempo;

    bool     c_BT_Press;
    bool     c_BT_PressLong;
    bool     c_BT_Status;
    
    uint16_t c_Temp_Debounce;
    uint16_t c_Temp_Longo;

    char     c_Map;

  public:

    DMD_BT() {
      c_Temp_Debounce = DMD_BT_TEMP_DEBOUNCE;
      c_Temp_Longo = DMD_BT_TEMP_LONG;
      c_Map = '0';
    }

    void Clear(){
      c_Detec_Status = DETEC_BT_STATUS_OFF;
      c_Tempo = 0;
      c_BT_Press = false;
      c_BT_PressLong = false;
      c_BT_Status = false;
    }

    void Init (char f_Map) {
      D_BT_LN (" Init BT");
      Clear();

      c_Detec_Status = DETEC_BT_STATUS_IDLE;
      c_Map = f_Map;
    }

    bool Loop ( bool f_status ) {
      c_BT_Status = f_status;
      switch (c_Detec_Status) {
        case DETEC_BT_STATUS_IDLE:
        default:
          if ( c_BT_Status ){ // Botao Pressionado
            c_Detec_Status = DETEC_BT_STATUS_DEBOUNCE;
            c_Tempo = 0;
          }
          break;
        case DETEC_BT_STATUS_DEBOUNCE:
          if ( c_BT_Status ){ // Continua Pressionado
            c_Tempo++;
            if ( c_Tempo >= c_Temp_Debounce){
              // Chegou a tempo para considerar Pressionado
              c_Detec_Status = DETEC_BT_STATUS_WAIT_LONG;
              c_BT_Press = true;
              D_BT_F (" Press "); D_BT (c_Map);
              return true;
            }
          } else {
            // Somente um pulso muito curto, desconsidera (provável erro)
            c_Detec_Status = DETEC_BT_STATUS_IDLE;
            c_Tempo = 0;
          }
          break;
        case DETEC_BT_STATUS_WAIT_LONG:
          if ( c_BT_Status ){ // Continua Pressionado
            c_Tempo++;
            if ( c_Tempo >= c_Temp_Longo){
              // Chegou a tempo para considerar Pressionado Longo
              c_Detec_Status = DETEC_BT_STATUS_WAIT_END;
              c_BT_PressLong = true;
              D_BT_F (" Press Long - "); D_BT (c_Map);
              return true;
            }
          } else {
            // Somente um pulso muito curto, desconsidera (provável erro)
            c_Detec_Status = DETEC_BT_STATUS_IDLE;
            c_Tempo = 0;
          }
          break;
        case DETEC_BT_STATUS_WAIT_END:
          if ( !c_BT_Status ){ // Aguarda liberar o Botão
            c_Detec_Status = DETEC_BT_STATUS_IDLE;
            c_Tempo = 0;
          }
          break;
        case DETEC_BT_STATUS_OFF:
          break;
      }
      return false;
    }

    char GetPressedAndClear(){
      if ( c_BT_Press ){
        c_BT_Press = false;
        return c_Map;
      }
      return DMD_BT_NO_BUTTON;
    }

    void SetTempDebouce (uint16_t f_temp_debouce) {
      c_Temp_Debounce = f_temp_debouce;
    }
};

#endif // DMD_BT_h
