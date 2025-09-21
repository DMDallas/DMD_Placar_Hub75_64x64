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
/*
 * Definições do Projeto
 */
#include "DMD_Config.h"

/*
 * Inclusao de libraries
 */
// #include <EEPROM.h> Incluida no .h para que sejam replicadas definições - incluir somente o DMD_*.H

/*
 * Inclusao funções e dados do projeto
 */
#include "DMD_EEPROM.h"

// void DMD_EEPROM_Init(unsigned int tamanho) {
//   EEPROM.begin(tamanho);
// }

/*
 * Carrega o tamanho solicitado da memoria Flash interna e retorna TRUE
 * se a versão dos dados gravados é igual a versão atual do sistema.
 */
bool DMD_EEPROM_ForceLoad(char * p_dados, unsigned int tamanho) {
  bool l_ret = false;

  DEBUG_PRINTF("EEPROM ForceLoad tamanho: ");
  DEBUG_PRINTLN(tamanho);

  if ( tamanho > (DMD_EEPROM_SIZE - 1) ) {
    DEBUG_PRINTLNF(" ERRO: A área da EEPROM é menor que dado.");
    return false;
  }

  EEPROM.begin(DMD_EEPROM_SIZE);

  for (unsigned int t = 0; t < tamanho; t++)
    *(p_dados + t) = EEPROM.read(DMD_EEPROM_CONFIG_START + t);

  if (EEPROM.read(DMD_EEPROM_CONFIG_START + 0) == VERSION_DATA[0] &&
      EEPROM.read(DMD_EEPROM_CONFIG_START + 1) == VERSION_DATA[1] &&
      EEPROM.read(DMD_EEPROM_CONFIG_START + 2) == VERSION_DATA[2])
  {
    l_ret = true;
  } else {
    l_ret = false;
  }

  EEPROM.end();

  DEBUG_PRINTLNF(" !");
  return l_ret;
}

/*
 * Carrega a configuração gravada na memoria Flash interna se a versão dos
 * dados gravados é igual a versão atual do sistema, caso tenha sucesso re-
 * torna TRUE caso contrário retorna FALSE
 */
bool DMD_EEPROM_Load(char * p_dados, unsigned int tamanho) {
  
  DEBUG_PRINTF("EEPROM Load tamanho: ");
  DEBUG_PRINTLN(tamanho);

  if ( tamanho > (DMD_EEPROM_SIZE - 1) ) {
    DEBUG_PRINTLNF("ERRO: A área da EEPROM é menor que dado.");
    return false;
  }

  EEPROM.begin(DMD_EEPROM_SIZE);

  // Verifica se os dados gravados são da mesma versão da esperada
  // Caso não seja confirmada a versão os dados não são recuperados
  if (EEPROM.read(DMD_EEPROM_CONFIG_START + 0) == VERSION_DATA[0] &&
      EEPROM.read(DMD_EEPROM_CONFIG_START + 1) == VERSION_DATA[1] &&
      EEPROM.read(DMD_EEPROM_CONFIG_START + 2) == VERSION_DATA[2])
  {
    for (unsigned int t = 0; t < tamanho; t++)
      *(p_dados + t) = EEPROM.read(DMD_EEPROM_CONFIG_START + t);
  } else {
    EEPROM.end();
    return false;
  }
  
  EEPROM.end();
  return true;
}

/*
 * Sava os dados na Flash interna
 */
void DMD_EEPROM_Save(char * p_dados, unsigned int tamanho) {

  DEBUG_PRINTF("EEPROM Save tamanho: ");
  DEBUG_PRINTLN(tamanho);

  EEPROM.begin(DMD_EEPROM_SIZE);

  for (unsigned int t=0; t < tamanho; t++)
    EEPROM.write(DMD_EEPROM_CONFIG_START + t, *(p_dados + t));

  if ( EEPROM.commit() )
    DEBUG_PRINTF("EEPROM Commit OK");
  else
    DEBUG_PRINTF("EEPROM Commit Erro");

  EEPROM.end();
}

/*
 * Compara a versão dos dados gravados com a versão recebida
 */
bool DMD_EEPROM_Check_Version(const char * f_version) {
  bool l_ret = false;

  DEBUG_PRINTF("EEPROM Check Ver ");
  DEBUG_PRINT(f_version);

  EEPROM.begin(DMD_EEPROM_SIZE);

  if (EEPROM.read(DMD_EEPROM_CONFIG_START + 0) == f_version[0] &&
      EEPROM.read(DMD_EEPROM_CONFIG_START + 1) == f_version[1] &&
      EEPROM.read(DMD_EEPROM_CONFIG_START + 2) == f_version[2])
  {
    DEBUG_PRINTLNF(" ==");
    l_ret = true;
  } else {
    DEBUG_PRINTLNF(" !=");
    l_ret = false;
  }

  EEPROM.end();

  return l_ret;
}
