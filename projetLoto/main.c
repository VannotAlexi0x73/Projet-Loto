//////////////////////////////////////////////////////////////////////
// Fichier		: main.c											//
// Sommaire		: Ce fichier contient la fonction 'main'.           //
//                L'exécution du programme commence et se           //
//                termine à cet endroit.	                        //
// Date			: 25/10/2020										//
// Version		: 1.0												//
// Auteur		: VANNOT Alexis										//
//////////////////////////////////////////////////////////////////////

#include "loto.h"

/*
 *	Function : int main(int argc, char* argv[])
 *	----------------------------
 *	L'exécution du programme commence et se termine à cet endroit.
 *
 *  argc :	le nombre de chaînes de caractères sur lequel pointe argv
 * 
 *	argv : pointeur sur les chaînes de caractères passées en console
 *
 *  return : 1 (EXIT_FAILURE) ou 0 (EXIT_SUCCESS)
 */
int main(int argc, char* argv[])
{
	// ---------------- Variable ----------------

	// ---------------- Moteur de jeu ----------------
	createLoto(&loto);
	jouerLoto(&loto);

	return EXIT_SUCCESS;
}
