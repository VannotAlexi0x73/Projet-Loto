//////////////////////////////////////////////////////////////////////
// Fichier		: loto.c											//
// Sommaire		: Contient toutes les fonctionnalit�s du jeu loto	//
// Date			: 25/10/2020										//
// Version		: 1.0												//
// Auteur		: VANNOT Alexis										//
//////////////////////////////////////////////////////////////////////

 #include "loto.h"


/*
 *	Function : void createLoto(ST_Loto* loto)
 *	----------------------------
 *	Elle permet de cr�er toutes les donn�es n�cessaires
 *	au fonctionnement du jeu gr�ce au fichier DESCAR.txt
 *
 *  loto	: pointeur sur un �l�ment loto cr�� dans le main()
 *			  de fa�on � modifier son contenu
 *
 *  return	: void
 */
void createLoto(ST_Loto* loto) {

	// ---------------- Variables ---------------- 

	FILE* file = fopen("DESCAR.txt", "r");
	int tabLigneFichier[16];
	ST_Carton* pCartons;
	iNbCartons = 0;


	// ---------------- Lecture du fichier ---------------- 
	if (file != NULL) {
		while (lectureLigneFichier(file, tabLigneFichier) != EOF)
		{
			iNbCartons++;
			pCartons = realloc(loto->TbCartons, sizeof(ST_Carton) * iNbCartons);

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
			
			loto->TbCartons[iNbCartons - 1].NumeroCarton = tabLigneFichier[0];

			for (int i = 0; i < NB_LIGNE; i++)
			{
				for (int k = 0; k < NB_CASE; k++)
				{
					loto->TbCartons[iNbCartons - 1].TbLigne[i].TbCase[k] = 0;
				}
				for (int j = (NB_ELEM_LIGNE * i) + 1; j < (NB_ELEM_LIGNE * (i + 1)) + 1 ; j++)
				{

					loto->TbCartons[iNbCartons - 1].TbLigne[i].TbCase[tabLigneFichier[j] / 10] = tabLigneFichier[j];
				}
				loto->TbCartons[iNbCartons - 1].TbLigne[i].Marqueur = 0;
			}
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
 *	Permet de r�cup�rer les donn�es en ligne dans le fichier sous le
 *	format "nCarton;x;x;x;x;x;x;x;x;x;x;x;x;x;x;x"
 *
 *  file	: pointeur sur le fichier dans lequel on va lire les valeurs
 *	tab		: tableau pour stocker les donn�es lues dans le fichier
 *
 *  return	: valeur de retour de la m�thode fscanf(arg1, arg2)
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
 *	Fonction qui permet � l'utilisateur de renseigner les num�ros qui ont �t� cri�s
 *
 *  loto	: pointeur sur le jeu loto cr�� pr�c�dement afin de pouvoir
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

		// On met -1 car les nombres du loto commencent � 1 et que le premier index d'un tableau est 0
		cartonGagnantNLignes(loto, iNum - 1, &iStep);

	// On met iStep != 4 car incr�mentation lors de l'�tape 3 �galement
	} while (bAgain && (iStep != 4));

	if (bAgain)
	{
		printf("\n\t==> Le jeu est termine, il y a au moins un carton plein.\n");
	}
	else
	{
		printf("\n\t==> Vous avez decide d'arreter le loto avant sa fin.\n");
	}

	// Libération de l'espace mémoire allouée dynamiquement
	supprimerVariableDynamique(loto);

}

/*
 *	Function : bool cartonGagnantNLignes(ST_Loto* loto, int iNum, int* iStep)
 *	----------------------------
 *	Moteur de jeu du loto en 3 �tapes :
 *	- 1re : 1 ligne d'un carton
 *	- 2e : 2 lignes d'un carton
 *	- 3e : 3 lignes d'un carton
 *	Lorsque une �tape est valid�e, la prochaine it�ration dans cette
 *	fonction serra l'�tape suivante gr�ce au pointeur iStep
 *
 *  loto	: pointeur sur le jeu loto cr�� pr�c�dement
 *	iNum	: num�ro qui vient d'�tre cri�
 *	iStep	: pointeur sur l'�tape de jeu du loto afin de modifier l'�tape
 *
 *  return	: true si au moins un carton est gagnant sinon false
 */
bool cartonGagnantNLignes(ST_Loto* loto, int iNum, int* iStep)
{

	// ---------------- Variables ----------------
	int nbCountLigneGagnante = 0;
	int iSommeMarqueur = 0;
	int iCase = 0;
	int iTmp = 0;
	int* pGagnants;
	clock_t cDebut = clock();

	// ---------------- Boucles de jeu ----------------
	loto->TbNumeros[iNum].bCrie = true;

	for (int i = 0; i < iNbCartons; i++)
	{
		iSommeMarqueur = 0;
		for (int j = 0; j < NB_LIGNE; j++)
		{
			loto->TbCartons[i].TbLigne[j].Marqueur = 0;

			for (int k = 0; k < NB_CASE; k++)
			{
				iCase = loto->TbCartons[i].TbLigne[j].TbCase[k];
				// On met -1 car les nombres du loto commencent � 1 et que le premier index d'un tableau est 0
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

			pGagnants = realloc(loto->TbCartonsGagnants, sizeof(int) * nbCountLigneGagnante);

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

			loto->TbCartonsGagnants[nbCountLigneGagnante - 1] = loto->TbCartons[i].NumeroCarton - 1;
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
			iTmp = loto->TbCartonsGagnants[i];
			affichageCarton(loto->TbCartons[iTmp]);
		}

		// On incr�mente l'�tape du loto pour passer � plusieurs lignes gagnantes
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
