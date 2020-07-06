/*
 * This  file  is  part  of  the  Cadence  LEF/DEF  Open   Source
 * Distribution,  Product Version 5.3, and is subject to the Cadence LEF/DEF
 * Open Source License Agreement.   Your  continued  use  of this file
 * constitutes your acceptance of the terms of the LEF/DEF Open Source
 * License and an agreement to abide by its  terms.   If you  don't  agree
 * with  this, you must remove this and any other files which are part of the
 * distribution and  destroy any  copies made.
 * 
 * For updates, support, or to become part of the LEF/DEF Community, check
 * www.openeda.org for details.
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "defiDebug.h"
char    defDebug[100];

typedef void (*DEFI_LOG_FUNC) (const char *str);

DEFI_LOG_FUNC defiErrorLogFunction = 0;

/*
 * ****************** Debug flags: 0 - 1 - malloc debug 2 - print each
 * history size bump up. 3 - print each call to CatchAll 4 - 5 - 6 - 7 - 8 -
 * 9 - 10 - 11 - lexer debug
 * 
****************************
 */
/* Set flag  */
void 
defiSetDebug(int num, int value)
{
  defDebug[num] = value;
}

/* Read flag  */
int 
defiDebug(int num)
{
  return defDebug[num];
}

void 
defiError(const char *mess)
{
  if (defiErrorLogFunction) {
    char   *str = (char *) malloc(strlen(mess) + 32);

    sprintf(str, "ERROR: %s\n", mess);
    (*defiErrorLogFunction) (str);
    free(str);
  }
  else {
    fprintf(stderr, "ERROR: %s\n", mess);
  }
  exit(2);
}

static char defiShift[] = {
  '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0',
  '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0',
  '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0',
  '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0',
  ' ', '!', '"', '#', '$', '%', '&', '\'',
  '(', ')', '*', '+', ',', '-', '.', '/',
  '0', '1', '2', '3', '4', '5', '6', '7',
  '8', '9', ':', ';', '<', '=', '>', '?',
  '@', 'A', 'B', 'C', 'D', 'E', 'F', 'G',
  'H', 'I', 'J', 'K', 'L', 'M', 'N', 'O',
  'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W',
  'X', 'Y', 'Z', '[', '\\', ']', '^', '_',
  '`', 'A', 'B', 'C', 'D', 'E', 'F', 'G',
  'H', 'I', 'J', 'K', 'l', 'M', 'N', 'O',
  'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W',
  'X', 'Y', 'Z', '{', '|', '}', '~', '\0',
  '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0',
  '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0',
  '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0',
  '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0',
  '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0',
  '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0',
  '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0',
  '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0',
  '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0',
  '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0',
  '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0',
  '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0',
  '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0',
  '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0',
  '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0',
  '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0'
};

const char *
upperCase(const char *str)
{
  static char *shiftBuf = 0;

  static int shiftBufLength = 0;

  char   *place = (char *) str;

  char   *to;

  int     len = strlen(str) + 1;

  if (len > shiftBufLength) {
    if (shiftBuf == 0) {
      len = len < 64 ? 64 : len;
      shiftBuf = (char *) malloc(len);
      shiftBufLength = len;
    }
    else {
      free(shiftBuf);
      shiftBuf = (char *) malloc(len);
      shiftBufLength = len;
    }
  }
  to = shiftBuf;
  while (*place)
    *to++ = defiShift[*place++];
  *to = '\0';
  return shiftBuf;
}
