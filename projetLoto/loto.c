//////////////////////////////////////////////////////////////////////
// Fichier		: loto.c											//
// Sommaire		: Contient toutes les fonctionnalités du jeu loto	//
// Date			: 25/10/2020										//
// Version		: 1.0												//
// Auteur		: VANNOT Alexis										//
//////////////////////////////////////////////////////////////////////

#include "loto.h"


/*
 *	Function : void createLoto(ST_Loto* loto)
 *	----------------------------
 *	Elle permet de créer toutes les données nécessaires
 *	au fonctionnement du jeu grâce au fichier DESCAR.txt
 *
 *  loto :	pointeur sur un élément loto créé dans le main()
 *			de façon à modifier son contenu
 *
 *  return : void
 */
void createLoto(ST_Loto* loto) {

	// ---------------- Variables ---------------- 

	FILE* file = fopen("DESCAR.txt", "r");
	int index = 0;
	int tabLigneFichier[16];

	// ---------------- Lecture du fichier ---------------- 
	if (file != NULL) {
		while (lectureLigneFichier(file, tabLigneFichier) != EOF)
		{

			loto->TbCartons[index].NumeroCarton = tabLigneFichier[0];

			for (int i = 0; i < NB_LIGNE; i++)
			{
				for (int k = 0; k < NB_CASE; k++)
				{
					loto->TbCartons[index].TbLigne[i].TbCase[k] = 0;
				}
				for (int j = (NB_ELEM_LIGNE * i) + 1; j < (NB_ELEM_LIGNE * (i + 1)) + 1 ; j++)
				{
					loto->TbCartons[index].TbLigne[i].TbCase[tabLigneFichier[j] / 10] = tabLigneFichier[j];
				}
				loto->TbCartons[index].TbLigne[i].Marqueur = 0;
			}
			index++;
		}

		for (int i = 0; i < 90; i++)
		{
			loto->TbNumeros[i].bCrie = false;
			loto->TbNumeros[i].NbOccurrenceAttend = 0;
			loto->TbNumeros[i].NbOccurrenceMarque = 0;
		}

		loto->DureeMarquage = 0;
		loto->TypeJeu = 0;

		fclose(file);
	}
	else
	{
		printf("Une erreur est survenue lors de la lecture du fichier.\n");
	}
}

/*
 *	Function : int lectureLigneFichier(FILE* file, int tab[])
 *	----------------------------
 *	Permet de récupérer les données en ligne dans le fichier sous le
 *	format "nCarton;x;x;x;x;x;x;x;x;x;x;x;x;x;x;x"
 *
 *  file :	pointeur sur le fichier dans lequel on va lire les valeurs
 *	tab : tableau pour stocker les données lues dans le fichier
 *
 *  return : valeur de retour de la méthode fscanf(arg1, arg2)
 */
int lectureLigneFichier(FILE* file, int tab[]) {
	return fscanf(
		file,
		"%d;%d;%d;%d;%d;%d;%d;%d;%d;%d;%d;%d;%d;%d;%d;%d",
		&tab[0], &tab[1], &tab[2], &tab[3],
		&tab[4], &tab[5], &tab[6], &tab[7],
		&tab[8], &tab[9], &tab[10], &tab[11],
		&tab[12], &tab[13], &tab[14], &tab[15]
	);
}

/*
 *	Function : void jouerLoto(ST_Loto* loto)
 *	----------------------------
 *	Fonction qui permet à l'utilisateur de renseigner les numéros qui ont été criés
 *
 *  loto :	pointeur sur le jeu loto créé précédement afin de pouvoir
 *			le transmettre au moteur de jeu pour en modifier son contenu
 *
 *  return : void
 */
void jouerLoto(ST_Loto* loto)
{
	// ---------------- Variables ----------------
	int iNum;
	int iStep = 1;
	bool bAgain = true;

	// ---------------- Boucles de jeu ----------------
	printf("\n\tVous jouez au loto 1 ligne gagnante.\n\n");

	do
	{
		do
		{
			printf("Veuillez saisir un nombre du loto (entre 1 et 90 ou -1 pour arreter) : ");
			scanf_s("%d", &iNum);

			if (iNum == -1) bAgain = false;

		} while (bAgain && ((iNum < LOTO_NB_MIN) || (iNum > LOTO_NB_MAX)));

		// On met -1 car les nombres du loto commencent à 1 et que le premier index d'un tableau est 0
		cartonGagnantNLignes(loto, iNum - 1, &iStep);

	// On met iStep != 4 car incrémentation lors de l'étape 3 également
	} while (bAgain && (iStep != 4));

	if (bAgain)
	{
		printf("\n\t==> Le jeu est termine, il y a au moins un carton plein.\n");
	}
	else
	{
		printf("\n\t==> Vous avez decide d'arreter le loto avant sa fin.\n");
	}

}

/*
 *	Function : bool cartonGagnantNLignes(ST_Loto* loto, int iNum, int* iStep)
 *	----------------------------
 *	Moteur de jeu du loto en 3 étapes :
 *	- 1re : 1 ligne d'un carton
 *	- 2e : 2 lignes d'un carton
 *	- 3e : 3 lignes d'un carton
 *	Lorsque une étape est validée, la prochaine itération dans cette
 *	fonction serra l'étape suivante grâce au pointeur iStep
 *
 *  loto :	pointeur sur le jeu loto créé précédement
 *	iNum : numéro qui vient d'être crié
 *	iStep : pointeur sur l'étape de jeu du loto afin de modifier l'étape
 *
 *  return : true si au moins un carton est gagnant sinon false
 */
bool cartonGagnantNLignes(ST_Loto* loto, int iNum, int* iStep)
{

	// ---------------- Variables ----------------
	int nbCountLigneGagnante = 0;
	int iSommeMarqueur = 0;
	int iCase = 0;
	int iIndexGagnant = 0;
	clock_t cDebut = clock();

	// ---------------- Boucles de jeu ----------------
	loto->TbNumeros[iNum].bCrie = true;

	for (int i = 0; i < 1260; i++)
	{
		iSommeMarqueur = 0;
		for (int j = 0; j < NB_LIGNE; j++)
		{
			loto->TbCartons[i].TbLigne[j].Marqueur = 0;

			for (int k = 0; k < NB_CASE; k++)
			{
				iCase = loto->TbCartons[i].TbLigne[j].TbCase[k];
				// On met -1 car les nombres du loto commencent à 1 et que le premier index d'un tableau est 0
				if (iCase != 0 && loto->TbNumeros[iCase - 1].bCrie == true)
				{
					loto->TbCartons[i].TbLigne[j].Marqueur++;
				}
			}
			if (loto->TbCartons[i].TbLigne[j].Marqueur == 5) iSommeMarqueur += 5;
		}
		// On met -1 car le premier carton est 1 et que le premier index d'un tableau est 0
		if (iSommeMarqueur == (5 * (*iStep)))
		{
			nbCountLigneGagnante++;
			loto->TbCartonsGagnants[iIndexGagnant++] = loto->TbCartons[i].NumeroCarton - 1;
		}
	}

	cDebut = clock() - cDebut;
	loto->DureeMarquage = ((double) cDebut) / ((double) CLOCKS_PER_SEC);

#ifdef PERFORMANCE
	printf("> Marquage effectue en %f s.\n", loto->DureeMarquage);
#endif // PERFORMANCE


	if (nbCountLigneGagnante)
	{
		printf("\tLe(s) carton(s) gagnant(s) est/sont :\n");
		for (int i = 0; i < nbCountLigneGagnante; i++)
		{
			iIndexGagnant = loto->TbCartonsGagnants[i];
			affichageCarton(loto->TbCartons[iIndexGagnant]);
		}

		// On incrémente l'étape du loto pour passer à plusieurs lignes gagnantes
		(*iStep)++;
		
		if ((*iStep) == 2)
		{
			printf("\n\tVous jouez maintenant pour 2 lignes gagnantes.\n\n");
		}
		else if ((*iStep) == 3) {
			printf("\n\tVous jouez maintenant pour carton plein.\n\n");
		}
		return true;
	}
	else
	{
		return false;
	}
}

/*
 *	Function : void affichageCarton(ST_Carton carton)
 *	----------------------------
 *	Affiche en console un carton du loto sous la forme :
 * 
 *		Carton numero: XX
 *
 *		---------------------------------------------
 *		| X | XX |    | XX | XX |    |    |    | XX |
 *		---------------------------------------------
 *		| X |    | XX |    |    | XX | XX | XX |    |
 *      ---------------------------------------------
 *      |   | XX |    |    |    | XX | XX | XX | XX |
 *		---------------------------------------------
 *
 *  carton :	carton qu'on souhaite afficher
 *
 *  return : void
 */
void affichageCarton(ST_Carton carton) {
	printf("\n\tCarton numero: %d\n\n\t", carton.NumeroCarton);
	for (int i = 0; i < NB_CASE; i++)
	{
		printf("-----");
	}
	printf("\n\t");
	for (int i = 0; i < NB_LIGNE; i++)
	{
		printf("|");
		for (int j = 0; j < NB_CASE; j++)
		{
			if (carton.TbLigne[i].TbCase[j])
			{
				printf(" %d |", carton.TbLigne[i].TbCase[j]);
			}
			else
			{
				if (j < 1)
				{
					printf("   |");
				}
				else
				{
					printf("    |");
				}
			}
		}
		printf("\n\t");
		for (int i = 0; i < NB_CASE; i++)
		{
			printf("-----");
		}
		printf("\n\t");
	}
	printf("\n");
}
