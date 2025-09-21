/*
  Autor:    Diego M Dallastella
  Revisão:  0.5
  
  ADC no ESP32 C3

   Pinos do dispositivo:
                               ___ ___
   (RESET) N/U (ADC0 - PB5) 1 |o      | 8  VCC
               (ADC3 - PB3) 2 |       | 7 (PB2 - ADC1)
               (ADC2 - PB4) 3 |       | 6 (PB1 - PWM )
                       GND  4 |_______| 5 (PB0 - PWM ) 
*/

#ifndef DMD_ADC_h
#define DMD_ADC_h

/*
 * Inclusao de libraries
 */
#include <arduino.h>

/*
 * Inclusao funções e dados do projeto
 */
#include "DMD_ESP32.h"

/*
 * Definições gerais do Modulo
 */


/*
 * Classe de controle do ADC para a ATTiny85
 */
class DMD_ADC {
  uint8_t c_Input;
  public:
    void Init( int f_ref) {
    };

    void Set_Ref( int f_ref){

    };

    void Set_ADC_Input (int f_adc) {
      c_Input = f_adc;
    }

    int Read_ADC () {
      analogRead(c_Input);
    }
};

#endif // DMD_ADC_h
