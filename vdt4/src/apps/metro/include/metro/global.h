/* *=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=* */
/* ** Copyright UCAR (c) 1992 - 2014 */
/* ** University Corporation for Atmospheric Research(UCAR) */
/* ** National Center for Atmospheric Research(NCAR) */
/* ** Research Applications Laboratory(RAL) */
/* ** P.O.Box 3000, Boulder, Colorado, 80307-3000, USA */
/* ** See LICENCE.TXT if applicable for licence details */
/* ** 2014/10/20 13:00:00 */
/* *=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=* */
#define BOOL long
#define TRUE 1
#define FALSE 0


static BOOL g_bVocal=FALSE;
static BOOL g_bNoEol=FALSE;
static BOOL g_bForMan;
static BOOL g_bManuel;
static BOOL g_bDual;
static BOOL g_bProtect;
static BOOL g_bVerbose;
static BOOL g_bVerPlus;
static BOOL g_bHLocal;
static char* g_cpLang;
static char** g_cppMess;

/* Set */
void SetVocal(BOOL bVocal);
void SetNoEol(BOOL bNoEol);
void SetForMan(BOOL bForMan);
void SetManuel(BOOL bManuel);
void SetDual(BOOL bDual);
void SetProtect(BOOL bProtect);
void SetVerbose(BOOL bVerbose);
void SetVerPlus(BOOL bVerPlus);
void SetHLocal(BOOL bHLocal);
void SetLang(char* cpLang);
void SetMess(char** cppMess, long nDim);

/* Get */
BOOL GetVocal();
BOOL GetNoEol();
BOOL GetForMan();
BOOL GetManuel();
BOOL GetDual();
BOOL GetProtect();
BOOL GetVerbose();
BOOL GetVerPlus();
BOOL GetHLocal();
char* GetLang();
char** GetMess();


