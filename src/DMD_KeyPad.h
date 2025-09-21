/*
  Autor:    Diego M Dallastella
  Revisão:  1.0

   Pinos do dispositivo:


  Keypad https://www.tme.eu/en/details/kb304-pnb/plastic-keypads/accord/ak-304-n-bbw/
*/

#include <arduino.h>

#include "DMD_Config.h"
#include "DMD_BT.h"

#include <Keypad.h>


#define ROWS 4
#define COLS 3

#define PIN_KEY_LINHA_1 8
#define PIN_KEY_LINHA_2 3
#define PIN_KEY_LINHA_3 4
#define PIN_KEY_LINHA_4 6

#define PIN_KEY_COL_1 7
#define PIN_KEY_COL_2 9
#define PIN_KEY_COL_3 5

// byte g_rowPins[ROWS] = {PIN_KEY_LINHA_1, PIN_KEY_LINHA_2, PIN_KEY_LINHA_3, PIN_KEY_LINHA_4};
// byte g_colPins[COLS] = {PIN_KEY_COL_1, PIN_KEY_COL_2, PIN_KEY_COL_3};
// Keypad g_keypad = Keypad(makeKeymap(keys), g_rowPins, g_colPins, ROWS, COLS);

/*
 * Variaveis Globais
 */
#define DETEC_STATUS_IDLE 1
#define DETEC_STATUS_DEBOUNCE 2
#define DETEC_STATUS_WAIT_END 3
#define DETEC_STATUS_OFF 6

class DMD_KeyPad {
  private:
    byte c_rowPins[ROWS];
    byte c_colPins[COLS];

    DMD_BT c_Bt[ROWS][COLS];

  public:

    void Set_Pin_Rows( uint8_t pin_1, uint8_t pin_2, uint8_t pin_3, uint8_t pin_4) {
      c_rowPins [0] = pin_1;
      c_rowPins [1] = pin_2;
      c_rowPins [2] = pin_3;
      c_rowPins [3] = pin_4;
    }

    void Set_Pin_Cols( uint8_t pin_1, uint8_t pin_2, uint8_t pin_3) {
      c_colPins [0] = pin_1;
      c_colPins [1] = pin_2;
      c_colPins [2] = pin_3;
    }

    void Init (char f_Map[ROWS][COLS] )  {
      DEBUG_PRINTLN(" -- Init KeyPad --");
    
      for (size_t i = 0; i < COLS; i++) {
        DEBUG_PRINT(" col="); DEBUG_PRINT(i);
        pinMode(c_colPins [i], OUTPUT);
      }
      
      for (size_t i = 0; i < ROWS; i++) {
        DEBUG_PRINT(" row="); DEBUG_PRINT(i);
        pinMode(c_rowPins [i], INPUT);
      }

      for (size_t r = 0; r < ROWS; r++) {
        for (uint8_t c = 0; c < COLS; c++) {
          c_Bt[r][c].Init(f_Map[r][c]);
        }
      }
    }

    bool loop( ) {
      bool f_press = false;

      for (byte c=0; c < COLS; c++) {
        // pinMode(c_colPins[c],OUTPUT);
        digitalWrite(c_colPins[c], HIGH);	// Begin column pulse output.
        for (byte r=0; r < ROWS; r++) {
          // if (digitalRead(c_rowPins[r]) )
            // DEBUG_PRINTLN("?");
          if ( c_Bt[r][c].Loop(digitalRead(c_rowPins[r])) ){
            f_press = true;
          }
        }

        digitalWrite(c_colPins[c], LOW);	// Begin column pulse output.
        // pinMode(c_colPins[c],INPUT);
        delay(1);
      }
      return f_press;
    }

    char GetPressedAndClear(){
      char f_map;

      for (size_t r = 0; r < ROWS; r++) {
        for (size_t c = 0; c < COLS; c++) {
          f_map = c_Bt[r][c].GetPressedAndClear();
          if ( f_map != DMD_BT_NO_BUTTON)
            return f_map;
        }
      }
      return DMD_BT_NO_BUTTON;
    }

};
