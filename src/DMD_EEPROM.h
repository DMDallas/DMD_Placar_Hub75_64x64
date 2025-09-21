/*******************************************************************************
 * Autor:   Diego Dallastella
 * Projeto: Base
 * Modulo:  Gravação e leitura EEPROM
 * Versão:  1.2
 *
 * Elementos:
 *           Placa ESP8266
 *
 * Libraries:
 *           EEPROM
 *
 * Utilizaa a memória interna do ESP8266
  ******************************************************************************/
#ifndef DMD_EEPROM_H
#define DMD_EEPROM_H

/*
 * Inclusao de libraries
 */
#include <EEPROM.h>

/*
 * Declaração da Funções
 */

void DMD_EEPROM_Init(unsigned int );

bool DMD_EEPROM_ForceLoad(char * , unsigned int );

bool DMD_EEPROM_Load(char * , unsigned int );
void DMD_EEPROM_Save(char * , unsigned int );

bool DMD_EEPROM_Check_Version(const char * ) ;

#endif /* DMD_EEPROM_H */
