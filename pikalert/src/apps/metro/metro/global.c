/* *=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=* */
/* ** Copyright UCAR (c) 1992 - 2014 */
/* ** University Corporation for Atmospheric Research(UCAR) */
/* ** National Center for Atmospheric Research(NCAR) */
/* ** Research Applications Laboratory(RAL) */
/* ** P.O.Box 3000, Boulder, Colorado, 80307-3000, USA */
/* ** See LICENCE.TXT if applicable for licence details */
/* ** 2014/10/20 13:00:00 */
/* *=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=* */
#include "global.h"
#include <string.h>


/***********************************************
** Set method
**********************************************/
void SetVocal(BOOL bVocal)
{
  g_bVocal = bVocal;
}

void SetNoEol(BOOL bNoEol)
{
  g_bNoEol = bNoEol;
}

void SetForMan(BOOL bForMan)
{
  g_bForMan = bForMan;
}

void SetManuel(BOOL bManuel)
{
  g_bManuel = bManuel;
}

void SetDual(BOOL bDual)
{
  g_bDual = bDual;
}

void SetProtect(BOOL bProtect)
{
  g_bProtect = bProtect;
}

void SetVerbose(BOOL bVerbose)
{
  g_bVerbose = bVerbose;
}

void SetVerPlus(BOOL bVerPlus)
{
  g_bVerPlus = bVerPlus;
}

void SetHLocal(BOOL bHLocal)
{
  g_bHLocal = bHLocal;
}

void SetLang(char* cpLang)
{
  strcpy(g_cpLang,cpLang);
}

void SetMess(char** cppMess, long nDim)
{
  long i;

  for(i=0; i<nDim; i++)
    strcpy(g_cppMess[i], cppMess[i]);
}

/***********************************************
** Get method
**********************************************/
BOOL GetVocal()
{
  return g_bVocal;
}

BOOL GetNoEol()
{
  return g_bNoEol;
}

BOOL GetForMan()
{
  return g_bForMan;
}

BOOL GetManuel()
{
  return g_bManuel;
}

BOOL GetDual()
{
  return g_bDual;
}

BOOL GetProtect()
{
  return g_bProtect;
}

BOOL GetVerbose()
{
  return g_bVerbose;
}

BOOL GetVerPlus()
{
  return g_bVerPlus;
}

BOOL GetHLocal()
{
  return g_bHLocal;
}

char* GetLang()
{
  return g_cpLang;
}

char** GetMess()
{
  return g_cppMess;
}


