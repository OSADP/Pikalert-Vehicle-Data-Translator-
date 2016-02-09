#ifdef __cplusplus
extern "C" {
#endif

/***************************************************************************
**
** Nom:         macadam.h
**
** Auteur:      Miguel Tremblay
**
** Date:        April 16, 2004
**
** Description: .h qui contient les constantes qui étaient autrefois dans 
**  params.cdk.  L'allocation dynamique étant impossible à faire avec un 
**  pointeur créé en C et passé au Fortran, l'utilisation de constante comme
**  DTMAX doit être maintenu.
**
** TODO: Enlever les constantes à mesure que le code fortran les utilisant
**  est remplacé par du code en C.
**
****************************************************************************/

#include "global.h"
#include "number.h"

/* Pas de temps du modèle de bilan énergétique */
#define dDT 30.0

/* Nombre de temps (ligne) permis dans le infile/matrice SCRIBE */
#define nNRECMAX 73

/* Nombre de colonnes permis dans les matrices SCRIBE */
#define nNCOLMAX 73

/* Nombre maximal de niveaux des grilles dans le sol */
#define nNGRILLEMAX 200

/* constantes physiques pr */
#define dPI  3.141592653590e0  
#define dOMEGA  0.7292e-4 

  void Do_Metro(BOOL bFlat, double dMLat, double dMLon, double* dpZones, long nNbrOfZone, long* npMateriau, double* dpTA, double* dpQP, double* dpFF, double* dpPS, double* dpFsPy, double* dpFI, double* dpFT, double* dpTYP, double* dpRc, double* dpTAO,  double* dpRTO, double* dpDTO, double* dpAH, double* dpTimeO, long* npSWO,  BOOL* bpNoObs, double dDeltaT, long nLenObservation, long nNbrTimeSteps, BOOL bSilent, double* outRT, double* outSST, long*   outRC, double* outSN, double* outRA, BOOL *bRet, long tmtTyp);
	      
  void init_structure(long nTimeStepMax, long nGrilleLevelMax, struct longStruct *stRC, struct doubleStruct *stRA, struct doubleStruct *stRT, struct doubleStruct *stIR, 
		      struct doubleStruct *stSF, struct doubleStruct *stSN, struct doubleStruct *stFV, struct doubleStruct *stFC, struct doubleStruct *stFA, 
		      struct doubleStruct *stG, struct doubleStruct *stBB, struct doubleStruct *stFP, struct longStruct *stEc, struct doubleStruct *stSST, 
		      struct doubleStruct *stDepth, struct doubleStruct *stLT);

void free_structure();

void print_input(BOOL bFlat, double dMLat, double dMLon, double* dpZones, long nNbrOfZone,  long* npMateriau, double* dpTA, double* dpQP, double* dpFF,  double* dpPS, double* dpFS, double* dpFI, double* dpFT, double* dpTYP, double* dpRC, double* dpTAO,  double* dpRTO, double* dpDTO, double* dpAH, double* dpTimeO, long* npSwo, BOOL* bpNoObs, double dDeltaT, long nLenObservation, long nNbrTimeSteps);

struct doubleStruct get_ra(void);
struct doubleStruct get_sn(void);
struct longStruct get_rc(void);
struct doubleStruct get_rt(void);
struct doubleStruct get_ir(void);
struct doubleStruct get_sf(void);
struct doubleStruct get_fv(void);
struct doubleStruct get_fc(void);
struct doubleStruct get_fa(void);
struct doubleStruct get_g(void);
struct doubleStruct get_bb(void);
struct doubleStruct get_fp(void);
struct longStruct get_echec(void);
struct doubleStruct get_sst(void);
struct doubleStruct get_depth(void);
long get_nbr_levels(void);
struct doubleStruct get_lt(void);

#ifdef __cplusplus
}
#endif
