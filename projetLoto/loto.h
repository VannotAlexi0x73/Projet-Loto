//////////////////////////////////////////////////////////////////////
// Fichier		: loto.h											//
// Sommaire		: Ensemble des structures / fonctions				//
//                nécessaires à l'exécution du jeu loto	            //
// Date			: 25/10/2020										//
// Version		: 1.0												//
// Auteur		: VANNOT Alexis										//
//////////////////////////////////////////////////////////////////////

#pragma warning(disable:4996)

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>


// ---------------- Define ---------------- 
#define MAX_SIZE		1500
#define LOTO_NB_MIN		1
#define LOTO_NB_MAX		90
#define NB_CASE			9
#define NB_ELEM_LIGNE	5
#define NB_LIGNE		3

//#define PERFORMANCE


// ---------------- Structures ---------------- 
typedef struct {
	int TbCase[NB_CASE];
	int Marqueur;
} ST_Ligne;

typedef struct {
	int NumeroCarton;
	ST_Ligne TbLigne[NB_LIGNE];
} ST_Carton;

typedef struct {
	bool bCrie;
	int NbOccurrenceMarque;			// TODO ??
	int NbOccurrenceAttend;			// TODO ??
} ST_Numero;

typedef struct {
	ST_Carton TbCartons[MAX_SIZE];
	ST_Numero TbNumeros[90];
	int TbCartonsGagnants[MAX_SIZE];
	int TypeJeu;					// TODO ?? => automatique
	double DureeMarquage;
} ST_Loto;



// ---------------- Variable globale ---------------- 
ST_Loto loto;


// ---------------- Prototypes des fonctions ----------------
void createLoto(ST_Loto* loto);
int lectureLigneFichier(FILE* file, int tab[]);
void jouerLoto(ST_Loto* loto);
bool cartonGagnantNLignes(ST_Loto* loto, int iNum, int* iStep);
void affichageCarton(ST_Carton carton);
