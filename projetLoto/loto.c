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
 *  loto	: pointeur sur un élément loto crée en variable globale
 *			  de façon à modifier son contenu
 *
 *  return	: void
 */
void createLoto(ST_Loto* loto) {

	// ---------------- Variables ---------------- 

	FILE* file = fopen("DESCAR.txt", "r");
	int tabLigneFichier[16];
	ST_Carton* pCartons;

	if (file != NULL)
	{
		// Initialisations des variables
		loto->NbCartonsGardes = 0;
		loto->NbCartonsGagnants = 0;
		loto->DureeMarquage = 0;

		for (int i = 0; i < LOTO_NB_MAX; i++)
		{
			loto->TbNumeros[i].bCrie = false;
			loto->TbNumeros[i].NbOccurrenceAttend = 0;
			loto->TbNumeros[i].NbOccurrenceMarque = 0;
		}

		// Lecture du fichier 
		while (lectureLigneFichier(file, tabLigneFichier) != EOF)
		{
			loto->NbCartonsGardes++;
			pCartons = realloc(loto->TbCartons, sizeof(ST_Carton) * loto->NbCartonsGardes);

			// Sécurité en cas de soucis allocation dynamique
			if (pCartons != NULL)
			{
				loto->TbCartons = pCartons;
			}
			else
			{
				supprimerVariableDynamique(loto);

				printf("Erreur lors de la re-allocation mémoire !!!!\n");
				exit(EXIT_FAILURE);
			}
			
			loto->TbCartons[loto->NbCartonsGardes - 1].NumeroCarton = tabLigneFichier[0];

			for (int i = 0; i < NB_LIGNE; i++)
			{
				loto->TbCartons[loto->NbCartonsGardes - 1].TbLigne[i].Marqueur = 0;
				for (int k = 0; k < NB_CASE; k++)
				{
					loto->TbCartons[loto->NbCartonsGardes - 1].TbLigne[i].TbCase[k] = 0;
				}
				for (int j = (NB_ELEM_LIGNE * i) + 1; j < (NB_ELEM_LIGNE * (i + 1)) + 1 ; j++)
				{
					loto->TbCartons[loto->NbCartonsGardes - 1].TbLigne[i].TbCase[tabLigneFichier[j] / 10] = tabLigneFichier[j];
				}
			}
		}
		fclose(file);
	}
	else
	{
		printf("Une erreur est survenue lors de la lecture du fichier.\n");
		exit(EXIT_FAILURE);
	}
}

/*
 *	Function : int lectureLigneFichier(FILE* file, int tab[])
 *	----------------------------
 *	Permet de récupérer les données en ligne dans le fichier sous le
 *	format "nCarton;x;x;x;x;x;x;x;x;x;x;x;x;x;x;x"
 *
 *  file	: pointeur sur le fichier dans lequel on va lire les valeurs
 *	tab		: tableau pour stocker les données lues dans le fichier
 *
 *  return	: valeur de retour de la méthode fscanf(arg1, arg2)
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
 *	Fonction qui permet à l'utilisateur de renseigner les numéros qui ont été cries
 *
 *  loto	: pointeur sur le jeu loto créé précédement afin de pouvoir
 *			  le transmettre au moteur de jeu pour en modifier son contenu
 *
 *  return	: void
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
		if (bAgain) cartonGagnantNLignes(loto, iNum - 1, &iStep);

	// On met iStep != 4 car incrémentation lors de l'étape 3 également
	} while (bAgain && (iStep != 4));

	bAgain ? 
		printf("\n\t==> Le jeu est termine, il y a au moins un carton plein.\n") :
		printf("\n\t==> Vous avez decide d'arreter le loto avant sa fin.\n");

	// Libération de l'espace mémoire allouée dynamiquement
	supprimerVariableDynamique(loto);

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
 *  loto	: pointeur sur le jeu loto créé précédement
 *	iNum	: numéro qui vient d'être crié
 *	iStep	: pointeur sur l'étape de jeu du loto afin de modifier l'étape
 *
 *  return	: true si au moins un carton est gagnant sinon false
 */
void cartonGagnantNLignes(ST_Loto* loto, int iNum, int* iStep)
{

	// ---------------- Variables ----------------
	int iSommeMarqueur = 0;
	int iCase = 0;
	int iTmp = 0;
	int* pGagnants;
	LARGE_INTEGER start, end;

	// Mesure de la performance d'un marquage
	QueryPerformanceCounter(&start);

	// ---------------- Boucles de jeu ----------------
	loto->TbNumeros[iNum].bCrie = true;
	loto->NbCartonsGagnants = 0;

	for (int i = 0; i < loto->NbCartonsGardes; i++)
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
					loto->TbNumeros[iCase - 1].NbOccurrenceMarque++;
				}
			}
			if (loto->TbCartons[i].TbLigne[j].Marqueur == 5) iSommeMarqueur += 5;
		}
		// On met -1 car le premier carton est 1 et que le premier index d'un tableau est 0
		if (iSommeMarqueur == (5 * (*iStep)))
		{
			loto->NbCartonsGagnants++;

			pGagnants = realloc(loto->TbCartonsGagnants, sizeof(int) * loto->NbCartonsGagnants);

			// Sécurité en cas de soucis allocation dynamique
			if (pGagnants != NULL)
			{
				loto->TbCartonsGagnants = pGagnants;
			}
			else
			{
				supprimerVariableDynamique(loto);

				printf("Erreur lors de la re-allocation mémoire !!!!\n");
				exit(EXIT_FAILURE);
			}

			loto->TbCartonsGagnants[loto->NbCartonsGagnants - 1] = loto->TbCartons[i].NumeroCarton - 1;
		}
	}

	// Mesure de la performance d'un marquage
	QueryPerformanceCounter(&end);
	loto->DureeMarquage = (1000.0 * (end.QuadPart - start.QuadPart)) / 10000000.0; // Conversion en millsecondes

#ifdef PERFORMANCE
	printf("> Marquage effectue en %f s.\n", loto->DureeMarquage);
#endif // PERFORMANCE


	if (loto->NbCartonsGagnants)
	{
		printf("\tLe(s) carton(s) gagnant(s) est/sont :\n");
		for (int i = 0; i < loto->NbCartonsGagnants; i++)
		{
			iTmp = loto->TbCartonsGagnants[i];
			affichageCarton(loto->TbCartons[iTmp]);
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
 *  carton : carton qu'on souhaite afficher
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

/*
 *	Function : void supprimerVariableDynamique(ST_Loto* loto)
 *	----------------------------
 *	Permet de supprimer les blocs mémoires alloués dynamiquement
 *  afin d'éviter les fuites mémoires
 *
 *  carton : jeu loto dans lequel on veut supprimer les bloc mémoires alloués dynamiquement
 *
 *  return : void
 */
void supprimerVariableDynamique(ST_Loto* loto) {
	free(loto->TbCartons);
	free(loto->TbCartonsGagnants);
}
