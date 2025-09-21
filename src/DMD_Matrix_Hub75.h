
#ifndef DMD_MATRIX_HUB75_H
#define DMD_MATRIX_HUB75_H

#include <Arduino.h>
#include "DMD_Config.h"
#include "DMD_Globais.h"

#ifdef DMD_DEBUG_MATRIX_HUB
  #define D_M_HUB_LN(x)    Serial.println(x)
  #define D_M_HUB(x)       Serial.print(x)
  #define D_M_HUB_LN_F(x)  Serial.println(F(x))
  #define D_M_HUB_F(x)     Serial.print(F(x))
#else
  #define D_M_HUB_LN(x)
  #define D_M_HUB(x)
  #define D_M_HUB_LN_F(x)
  #define D_M_HUB_F(x)
#endif


#include "DMD_Config.h"
#include "DMD_WiFi.h"
#include "DMD_Globais.h"

// HUB75E pinout
// R1 | G1
// B1 | GND
// R2 | G2
// B2 | E
//  A | B
//  C | D
// CLK| LAT
// OE | GND

#define CH_A  33
#define CH_B  25
#define CH_C  26
#define CH_D  27
#define CLK   14
#define LAT   12
#define OE    13

#define BL2   21
#define G2    19
#define R2    18
#define BL1   5
#define R1    17
#define G1    16


// #define CH_E -1 // assign to any available pin if using two panels or 64x64 panels with 1/32 scan

// Configure for your panel(s) as appropriate!
#define PANEL_WIDTH 64
#define PANEL_HEIGHT 32  	// Panel height of 64 will required PIN_E to be defined.
#define PANELS_NUMBER 1 	// Number of chained panels, if just a single panel, obviously set to 1
// #define PIN_E 32

#define PANE_WIDTH PANEL_WIDTH * PANELS_NUMBER
#define PANE_HEIGHT PANEL_HEIGHT

// Color definitions
#define BLACK   0x0000
#define BLUE    0x001F
#define RED     0xF800
#define GREEN   0x07E0
#define CYAN    0x07FF
#define MAGENTA 0xF81F
#define YELLOW  0xFFE0
#define WHITE   0xFFFF

// #define COLOR_BC_LOGO dma_display->color444(255,102,0)
#define COLOR_BC_LOGO dma_display->color444(15,6,0)
#define COLOR_TEXT    WHITE
#define COLOR_TIME_1  0xffc0 // Amarelo
#define COLOR_TIME_2  RED
#define COLOR_HOUR       GREEN
#define COLOR_HOUR_STOP  RED


enum DMD_Tela_Status {
  DMD_TELA_OFF,
  DMD_TELA_INIT,
  DMD_TELA_HORA,
  DMD_TELA_IDLE,
  DMD_TELA_ACTIV,
  DMD_TELA_ACTIV_1,
  DMD_TELA_ACTIV_2,
  DMD_TELA_ACTIV_3,
  DMD_TELA_ACTIV_4,
  DMD_TELA_MAX
};

class DadosTela {
  public:
    DMD_Tela_Status c_tela;

    char c_hora_str[6];
    
    bool c_run;

    uint16_t c_run_tempo;
    uint16_t c_tempo_stop;

    char c_run_tempo_str[6];

    uint8_t c_pontos[NUM_PLACARES];

    uint8_t c_conta_BT;
    uint8_t c_conta_BT_seg;

  public:
    DadosTela (){
      c_tela = DMD_TELA_OFF;

      c_hora_str[0] = '0';
      c_hora_str[1] = '0';
      c_hora_str[2] = ':';
      c_hora_str[3] = '0';
      c_hora_str[4] = '0';
      c_hora_str[5] = '\0';

      c_run_tempo_str[0] = '0';
      c_run_tempo_str[1] = '0';
      c_run_tempo_str[2] = ':';
      c_run_tempo_str[3] = '0';
      c_run_tempo_str[4] = '0';
      c_run_tempo_str[5] = '\0';

      Clear();
    }

    void Init(){
      c_tela = DMD_TELA_INIT;
    }

    void Clear () {
      c_run_tempo = 0;
      c_tempo_stop = 0;
      c_run = false;
      c_pontos[0] = 0;
      c_pontos[1] = 0;
      c_conta_BT = 0;
    }

    bool Not_Ready() {
      if ( c_tela <= DMD_TELA_INIT )
        return true;
      return false;
    }

    bool Is_Idle() {
        if ( c_tela <= DMD_TELA_IDLE )
          return true;
        return false;
      }

    bool Is_Running () { return c_run; }
    
    void Set_Activ () {
      if ( c_tela <= DMD_TELA_IDLE ) {
        if ( g_Dados.Tela_ativa < (DMD_TELA_MAX - DMD_TELA_ACTIV) )
          c_tela = (DMD_Tela_Status) (DMD_TELA_ACTIV + g_Dados.Tela_ativa);
        else
          c_tela = DMD_TELA_ACTIV;
      }
    }

    void Ponto_UP ( uint8_t f_ponto, uint8_t f_val = 1 );

    void Ponto_DW ( uint8_t f_ponto, uint8_t f_val = 1 );

    void Start () {
      Set_Activ();
      c_run = true;
    }

    void Stop  () {
      c_run = false;
      Set_Activ();
      c_tempo_stop = 0;
    }

    void Start_Stop () {
      if (c_run)
        c_run = false;
      else
        c_run = true;
    }

    void Time_to_Str ( uint8_t f_hora, uint8_t f_min, char *f_char , unsigned int f_contador_seg = 1) {
      if (f_hora <= 9) {
        f_char[0] = '0';
        f_char[1] = '0' + f_hora;
      } else {
        if ( f_hora < 99 ) {
          f_char[0] = '0' + f_hora/10;
          f_char[1] = '0' + f_hora%10;
        } else {
          if ( f_hora > 199) 
            f_char[0] = '0' + (f_hora-200)/10;
          else 
            f_char[0] = '0' + (f_hora-100)/10;
          f_char[1] = '0' + f_hora%10;
        }
      }

      if ( f_contador_seg & 0x1 )
        f_char[2] = ':';
      else
        f_char[2] = ' ';

      if (f_min <= 9) {
        f_char[3] = '0';
        f_char[4] = '0' + f_min;
      } else {
        f_char[3] = '0' + f_min/10;
        f_char[4] = '0' + f_min%10;
      }   
    }

    void UpdateRunTime_Str ( unsigned int f_contador_seg = 1 ){
      if ( c_run )
        Time_to_Str(c_run_tempo/60, c_run_tempo%60 , c_run_tempo_str, f_contador_seg);
      else
        Time_to_Str(c_run_tempo/60, c_run_tempo%60 , c_run_tempo_str);
    };

    void UpdateTime ( unsigned int f_contador_seg = 1 ) {
      struct tm tmstruct;
      getLocalTime(&tmstruct, 5000);
  
      Time_to_Str(tmstruct.tm_hour, tmstruct.tm_min, c_hora_str, f_contador_seg);
    }
};

extern DadosTela g_DadoTela;

void drawXbm565(int x, int y, int width, int height, const char *xbm, uint16_t color);

void DMD_Matrix_HUB75_Init ();

void Hub75_Update (unsigned int f_contador_seg, bool f_conta_seg = true);

#endif // DMD_MATRIX_HUB75_H