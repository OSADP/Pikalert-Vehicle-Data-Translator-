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


