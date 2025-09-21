/**
 * @brief Controle das entradas e saidas placa
 * 
 * Autor:    Diego M Dallastella
 * Revisão:  0.5
 * 
 */


/*
 * Inclusao de libraries
 */

/*
 * Inclusao funções e dados do projeto
 */
#include "DMD_Config.h"
#include "DMD_Globais.h"
#include "DMD__Placa.h"
#include "DMD_Matrix_Hub75.h"


/*
 * Definições gerais do Modulo
 */
DMD_Buzzer g_Buzzer;


void Ativar_Rele    ()  {  g_Rele.On();  }
void Desativar_Rele ()  {  g_Rele.Off(); }

void DMD_Start      () {
  g_Buzzer.Ativar(4);

  if ( g_DadoTela.Not_Ready() )
    return;

  g_DadoTela.Start();
  // DMD_Level_Update();
  Hub75_Update (g_contador_seg, false);
}

void DMD_Stop       () {
  g_Buzzer.Ativar(4);

  if ( g_DadoTela.Not_Ready() )
    return;

  g_DadoTela.Stop();
  // DMD_Level_Update();
  Hub75_Update (g_contador_seg, false);
}

void DMD_Start_Stop() {
    g_Buzzer.Ativar(4);

  if ( g_DadoTela.Not_Ready() )
    return;

  if ( g_DadoTela.Is_Running() )
    g_DadoTela.Stop();
  else 
    g_DadoTela.Start();

  // DMD_Level_Update();
  Hub75_Update (g_contador_seg, false);
}

void DMD_Clear      () {
  g_Buzzer.Ativar(4);

  if ( g_DadoTela.Not_Ready() )
    return;

  g_DadoTela.Stop();
  g_DadoTela.Clear();
  Hub75_Update (g_contador_seg, false);
}

/// @brief 



void Init_Placa ( uint16_t f_step_divider ) {
  Ativar_Rele();

  // g_Field.Set_PWM_Step ( 255 / g_DadoTela.Get_Graf_Max() );
 
  
  delay(250);
  g_Field.PWM_Max( );

  g_Buzzer.Ativar(10);
}