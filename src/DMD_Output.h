/*******************************************************************************
 * Autor:   Diego Dallastella
 * Projeto: StepMill
 * Modulo:  
 *
 * Elementos:
 *           Placa ESP32
 *
 * Libraries:
 *
 * Pinos Utilizados
 *
 *
  ******************************************************************************/
#ifndef DMD_OUTPUT_H
#define DMD_OUTPUT_H


/*
 * Inclusao de libraries
 */

/*
 * Inclusao funções e dados do projeto
 */
#include "DMD_Config.h" 

enum DMD_Output_Status {
  DMD_Output_OFF,
  DMD_Output_ON,
  DMD_Output_PWM
};

class DMD_Output {
  private:
    DMD_Output_Status c_Status;
    uint8_t c_pin;
    uint8_t c_PWM_Atual;
    uint8_t c_PWM_Step;
    uint8_t c_PWM_Objetivo;

  public:
    DMD_Output (uint8_t f_pin, bool f_on_off = LOW) {
      c_pin = f_pin;
      pinMode(c_pin, OUTPUT);
      if ( f_on_off )
        On();
      else
        Off();

      c_PWM_Atual = 0;
      c_PWM_Step = 32;
    }

    uint8_t Get_Step () { return c_PWM_Step; }

    DMD_Output_Status Get_Status () { return c_Status; }

    void On  ()               { digitalWrite(c_pin, HIGH);  c_Status = DMD_Output_ON; }
    void Off ()               { digitalWrite(c_pin, LOW);   c_Status = DMD_Output_OFF;}

    void PWM (uint8_t f_PWM)  {
      c_Status = DMD_Output_PWM; 
      c_PWM_Objetivo = c_PWM_Atual = f_PWM;
      analogWrite (c_pin, c_PWM_Atual); 
    }

    void PWM_Fade (uint8_t f_PWM)  {
      c_Status = DMD_Output_PWM; 
      c_PWM_Objetivo = f_PWM;
    }

    void PWM_Fade_Loop ()  {
      if ( c_Status != DMD_Output_PWM )
        return;

      if ( c_PWM_Objetivo == c_PWM_Atual )
        return;

      if ( c_PWM_Atual > c_PWM_Objetivo ) {
        c_PWM_Atual--;
      } else {
        c_PWM_Atual++;
      }

      if (c_PWM_Atual == 0) {
        Off();
        return;
      }

      if (c_PWM_Atual == 255) {
        On();
        return;
      }

      analogWrite (c_pin, c_PWM_Atual); 
    }

    void PWM_Step     (uint8_t f_step ) { 
      c_Status = DMD_Output_PWM;
      c_PWM_Objetivo = f_step*c_PWM_Step;

      if ( c_PWM_Objetivo > 0xFF - c_PWM_Step )
        c_PWM_Objetivo = 255;
        
      analogWrite(c_pin, c_PWM_Atual);
    }

    void PWM_Max  () {
      c_Status = DMD_Output_PWM;
      c_PWM_Objetivo = 255;
      analogWrite(c_pin, c_PWM_Atual);
    }

    uint8_t Get_PWM(){
      if (c_Status == DMD_Output_PWM)
        return c_PWM_Atual;

      return 0;
    }

    void Set_PWM_Step (uint8_t f_step ) { c_PWM_Step = f_step; }
};


#endif // DMD_OUTPUT_H
