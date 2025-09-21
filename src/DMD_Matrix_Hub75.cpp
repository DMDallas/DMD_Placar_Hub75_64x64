


#include <Arduino.h>


#include "DMD_Matrix_Hub75.h"
#include "DMD_WiFi.h"
#include "DMD_Globais.h"

#include <FastLED.h>

#include <ESP32-HUB75-MatrixPanel-I2S-DMA.h>

#include "DMD_Matrix_BitMap.h"

// Fonts
#include "DMD_Matrix_Fonts_Pico.h"
#include "DMD_Matrix_Fonts_PicoDMD.h"



// placeholder for the matrix object
MatrixPanel_I2S_DMA *dma_display = nullptr;


CRGB currentColor;
CRGBPalette16 palettes[] = {HeatColors_p, LavaColors_p, RainbowColors_p, RainbowStripeColors_p, CloudColors_p};
CRGBPalette16 currentPalette = palettes[0];

DadosTela g_DadoTela;


void drawXbm565(int x, int y, int width, int height, const char *xbm, uint16_t color = 0xffff) 
{
  if (width % 8 != 0) {
      width =  ((width / 8) + 1) * 8;
  }
    for (int i = 0; i < width * height / 8; i++ ) {
      unsigned char charColumn = pgm_read_byte(xbm + i);
      for (int j = 0; j < 8; j++) {
        int targetX = (i * 8 + j) % width + x;
        int targetY = (8 * i / (width)) + y;
        if (bitRead(charColumn, j)) {
          dma_display->drawPixel(targetX, targetY, color);
        }
      }
    }
}



void DMD_Matrix_HUB75_Init (){
  D_M_HUB_LN(" == Matrix Hub75 INIT ==");
  HUB75_I2S_CFG mxconfig;

  // those are the defaults
  mxconfig.gpio.oe = OE;                    
  mxconfig.gpio.clk = CLK;                    
  mxconfig.gpio.c = CH_C;
  mxconfig.gpio.a = CH_A;
  mxconfig.gpio.b2 = BL2;
  mxconfig.gpio.r2 = R2;
  mxconfig.gpio.b1 = BL1;
  mxconfig.gpio.r1 = R1;
  mxconfig.gpio.g1 = G1;
  mxconfig.gpio.g2 = G2;
  mxconfig.gpio.b = CH_B;
  mxconfig.gpio.lat = LAT;
  mxconfig.gpio.d = CH_D;
  // mxconfig.driver = HUB75_I2S_CFG::SHIFT;   // shift reg driver, default is plain shift register
  mxconfig.i2sspeed = HUB75_I2S_CFG::HZ_16M;// I2S clock speed, better leave as-is unless you want to experiment

  mxconfig.mx_width = PANEL_WIDTH;
  mxconfig.mx_height = PANEL_HEIGHT;      // we have 64 pix heigh panels
  mxconfig.chain_length = PANELS_NUMBER;  // we have 2 panels chained
  // mxconfig.gpio.e = PIN_E;                // we MUST assign pin e to some free pin on a board to drive 64 pix height panels with 1/32 scan
  //mxconfig.driver = HUB75_I2S_CFG::FM6126A;     // in case that we use panels based on FM6126A chip, we can change that


  mxconfig.double_buff = false;             // use double buffer (twice amount of RAM required)
  // mxconfig.double_buff = true; // <------------- Turn on double buffer

  mxconfig.setPixelColorDepthBits(4);

  // OK, now we can create our matrix object
  dma_display = new MatrixPanel_I2S_DMA(mxconfig);

  
  // let's adjust default brightness to about 75%
  dma_display->setBrightness8(255);    // range is 0-255, 0 - 0%, 255 - 100%
  dma_display->setLatBlanking(0);

  // Allocate memory and start DMA display
  if( not dma_display->begin() )
      D_M_HUB_LN("****** !KABOOM! I2S memory allocation failed ***********");
 
  dma_display->setTextWrap(false);

  dma_display->clearScreen();   // Now clear the back-buffer

  // drawXbm565(0,0,BOLA2_WD, BOLA2_HT,bola2_bits,0xfc40);
  dma_display->setTextColor(0xfc40);
  dma_display->setFont(&PicoDMD);
  dma_display->setTextSize(1);
  // dma_display->setCursor(32,14);
  // dma_display->print("BASQUETE");
  // dma_display->setCursor(40,24);
  // dma_display->print("de 5");
  dma_display->setCursor(20,6);
  dma_display->print("PLACAR");
  dma_display->setCursor(28,14);
  dma_display->print("DO");
  dma_display->setTextSize(2);
  dma_display->setTextColor(0xfdac);
  dma_display->setCursor(0,26);
  dma_display->print("QUEIJINHO");

  delay(500);
  
  // dma_display->flipDMABuffer(); // Show the back buffer, set currently output buffer to the back (i.e. no longer being sent to LED panels)

  g_DadoTela.Init();
}

void Hub75_Update (unsigned int f_contador_seg, bool f_conta_seg) {
  static uint8_t f_pos = 0;
  int16_t f_val;
  String f_msg = "";

  if ( g_DadoTela.c_conta_BT_seg > 0 ) {
    g_DadoTela.c_conta_BT_seg--;
    D_M_HUB(" zerar tempo="); D_M_HUB(g_DadoTela.c_conta_BT_seg);
  } else {
    g_DadoTela.c_conta_BT = 0;
    g_DadoTela.c_conta_BT_seg = 0;
  }


  if ( g_DadoTela.c_tela == DMD_TELA_OFF )
    return;

  if ( f_conta_seg ) {
    if (g_DadoTela.c_run) {
      g_DadoTela.c_run_tempo++;
    } else {
      if ( g_DadoTela.c_tela > DMD_TELA_IDLE )
        g_DadoTela.c_tempo_stop++;
    }
  }

  // Retorno automatico para tela de hora após DMD_GERAL_TEMPO_IDLE segundos
  if ( g_DadoTela.c_tempo_stop > g_Dados.Tempo_Idle ) {
    g_DadoTela.c_tela = DMD_TELA_IDLE;
    g_DadoTela.c_tempo_stop = 0;
    g_DadoTela.Clear();
  }

  g_DadoTela.UpdateTime(f_contador_seg);

  g_DadoTela.UpdateRunTime_Str(f_contador_seg);

  switch (g_DadoTela.c_tela) {
    case DMD_TELA_INIT:
      D_M_HUB("i");
      // dma_display->clearScreen();
      // drawXbm565(0,0,LOGO_BODY_CENTER_WD, LOGO_BODY_CENTER_HT,logo_body_center_bits,COLOR_BC_LOGO);
      // dma_display->setTextColor(COLOR_HOUR);
      // dma_display->setFont();
      // dma_display->setTextSize(1);
      // dma_display->setCursor(24,14);
      // dma_display->print("BODY");
      // dma_display->setCursor(28,24);
      // dma_display->print("CENTER");
      // dma_display->setTextColor(COLOR_HOUR);
      // dma_display->drawRect(45,0,64,5,BLACK);
      // dma_display->fillRect(45,0,64,5,BLACK);
      // dma_display->setFont(&PicoDMD);
      // dma_display->setCursor(44,4);
      // dma_display->print(g_DadoTela.c_hora_str);
      g_DadoTela.c_tela = DMD_TELA_IDLE;
      break;

    default:
    case DMD_TELA_HORA:
      D_M_HUB("H");
    case DMD_TELA_IDLE:
      // D_M_HUB("I");
      
      dma_display->clearScreen();

      drawXbm565(0,0,BOLA2_WD, BOLA2_HT,bola2_bits,0xfc40);
      dma_display->setTextColor(0xfc40);
      dma_display->setFont(&PicoDMD);
      dma_display->setTextSize(1);
        // dma_display->setCursor(32,14);
        // dma_display->print("BASQUETE");
        // dma_display->setCursor(40,24);
        // dma_display->print("de 5");
      dma_display->setCursor(36,10);
      dma_display->print("PLACAR");
      dma_display->setCursor(44,18);
      dma_display->print("DO");
      dma_display->setTextColor(0xfdac);
      dma_display->setCursor(32,26);
      dma_display->print("QUEIJINHO");

      // g_DadoTela.c_tela = DMD_TELA_ACTIV;
      break;

    case DMD_TELA_ACTIV:
      // D_M_HUB("A");
      dma_display->clearScreen();
      // Hora
      // dma_display->setTextColor(COLOR_HOUR);
      // dma_display->fillRect(47,0,17,5,BLACK);
      // dma_display->setTextSize(1);
      
      // dma_display->setCursor(26,4);
      // dma_display->print(g_DadoTela.c_hora_str);
      
      // dma_display->drawLine(0,6,63,6,COLOR_HOUR);
      
      dma_display->setFont(&PicoDMD);
      dma_display->setTextSize(3);
      dma_display->setCursor(1,12);
      dma_display->setTextColor(g_Dados.c_Color_Time1);
      dma_display->print(g_DadoTela.c_pontos[0]);

      dma_display->setTextColor(g_Dados.c_Color_Time2);
      if ( g_DadoTela.c_pontos[1] >= 10 )
        dma_display->setCursor(42,12);
      else
        dma_display->setCursor(54,12);
      dma_display->print(g_DadoTela.c_pontos[1]);

      dma_display->setTextSize(2);
      
      if ( g_DadoTela.c_conta_BT == 4) {
        dma_display->setCursor(10,30);
        dma_display->setTextColor(RED);
        dma_display->print("ZERAR ?");
      } else {
        dma_display->setCursor(15,30);
        if ( g_DadoTela.Is_Running() )
          dma_display->setTextColor(g_Dados.c_Color_Hour);
        else
          dma_display->setTextColor(g_Dados.c_Color_Hour_Stop);
        dma_display->print(g_DadoTela.c_run_tempo_str);
      }

      break;
  }
}

void DadosTela::Ponto_UP ( uint8_t f_ponto, uint8_t f_val) {
  if ( f_ponto >= NUM_PLACARES )
    return;
  g_DadoTela.c_pontos[f_ponto] += f_val;
}

void DadosTela::Ponto_DW ( uint8_t f_ponto, uint8_t f_val) {
  if ( f_ponto >= NUM_PLACARES )
    return;
  
  if ( g_DadoTela.c_pontos[f_ponto] > f_val)
    g_DadoTela.c_pontos[f_ponto] -= f_val;
  else 
    g_DadoTela.c_pontos[f_ponto] = 0;
}
