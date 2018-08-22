// Julia.cpp : définit le point d'entrée pour l'application console.
//

#include "stdafx.h"
#include "EasyBMP.h"
#include "omp.h"
#include "QPCT.h"
#include <time.h>
#include <windows.h>
#include <strsafe.h>
#include <stdio.h>
#include <time.h>
#include <stdlib.h>

#define SIZE 3000
#define VAL1 -1.5
#define VAL2 1.5
#define NB_ITERATIONS 255

BMP bmpJuliaSeq;
BMP bmpJuliaPara;
BMP bmpGraphH;
BMP bmpGraphV;

RGBApixel pix;

int tabH[SIZE];
int tabV[SIZE];

double A = -0.0849;
double B = 0.8721;

/* Fonction permettant de retourner un niveau de couleur */
int DoMagic(double x, double y)
{
	int num = 255;
	for (num = 0; num < NB_ITERATIONS; ++num)
	{
		if (x * x + y * y > 4) break;
		long double new_x = x * x - y * y + A;
		long double new_y = 2 * x * y + B;
		x = new_x;
		y = new_y;
	}
	return num;
}

/* Fonction permettant de convertir les pixels en coordonnées */
double convertPix(int p)
{
	double newVal;
	// Convertion
	newVal =  VAL1 + (p * VAL2 / (SIZE / 2));
	return newVal;
}

/* Fonction qui permet de dessiner Julia en Parralèlle et en Sequentiel*/
void drawBMPPara()
{
	double x, y;
	int magicNumber;
	RGBApixel pix;
	for (int i = 0; i < SIZE; i++)
	{
#pragma omp parallel for private (x, y, magicNumber, pix)
		// Parcourt du bmp
		for (int j = 0; j < SIZE; j++)
		{
			// Conversion des pixel
			x = convertPix(i);
			y = convertPix(j);
			magicNumber = DoMagic(x, y);
			// Attribution du nombre magique à deux couleurs (pour avoir autre chose que du gris)
			pix.Blue = pix.Green = magicNumber;
			// Dessin du pixel sur le bmp
			bmpJuliaPara.SetPixel(i, j, pix);

			// Stockage des valeur max horrizontal et vertiacle.
			if (magicNumber > tabH[j])
				tabH[j] = magicNumber;

			if (magicNumber > tabV[i])
				tabV[i] = magicNumber;
		}
	}
}

void drawBMPSeq()
{
	double x, y;
	int magicNumber;
	RGBApixel pix;
	// Parcourt du bmp
	for (int i = 0; i < SIZE; i++)
	{
		for (int j = 0; j < SIZE; j++)
		{
			// Conversion des pixel
			x = convertPix(i);
			y = convertPix(j);
			magicNumber = DoMagic(x, y);
			// Attribution du nombre magique à deux couleurs (pour avoir autre chose que du gris)
			pix.Red = pix.Blue = magicNumber;
			bmpJuliaSeq.SetPixel(i, j, pix);
		}
	}
}

void startSeq()
{
	printf("Julia Sequentiel : \n");
	START_QPCT
		drawBMPSeq();
	STOP_QPCT
}

void startPara()
{
	printf("Julia parallele : \n");
	START_QPCT
		drawBMPPara();
	STOP_QPCT
}

/* Fonction qui calcul l'espace vertiacle et horrizontal de Julien en parralèle et sequentiel */
void calculSpaceSeq()
{
	pix.Red = 55;
	pix.Green = 24;
	pix.Blue = 200;

	START_QPCT
	for (int i = 0; i < SIZE; i++)
	{
		// Dessin du graphique Horrizontal
		for (int j = 0; j < tabH[i]; j++)
		{
			bmpGraphH.SetPixel(i, 255 - j, pix);
		}

		// Dessin du graphique Vertical
		for (int j = 0; j < tabV[i]; j++)
		{
			bmpGraphV.SetPixel(i, 255 - j, pix);
		}
	}
	STOP_QPCT
}

void calculSpacePara()
{
	pix.Red = 55;
	pix.Green = 24;
	pix.Blue = 200;

	START_QPCT
	for (int i = 0; i < SIZE; i++)
	{
#pragma omp parallel for
		for (int j = 0; j < tabH[i]; j++)
		{
			bmpGraphH.SetPixel(i, 255 - j, pix);
		}
#pragma omp parallel for
		for (int j = 0; j < tabV[i]; j++)
		{
			bmpGraphV.SetPixel(i, 255 - j, pix);
		}
	}
	STOP_QPCT
}

int main(int argc, char* argv[])
{
	bmpJuliaPara.SetBitDepth(24);
	bmpJuliaSeq.SetBitDepth(24);
	bmpGraphH.SetBitDepth(24);
	bmpGraphV.SetBitDepth(24);

	bmpJuliaPara.SetSize(SIZE, SIZE);
	bmpJuliaSeq.SetSize(SIZE, SIZE);
	bmpGraphH.SetSize(SIZE, 300);
	bmpGraphV.SetSize(SIZE, 300);

	// Initialisation des tableau de calcul d'espace
	for (int i = 0; i < SIZE; i++)
	{
		tabH[i] = 0;
		tabV[i] = 0;
	}

	// Démarage de la création de Julien
	startSeq();
	startPara();

	// Démarage des calculs d'espace de Julia
	printf("Calcul de l'espace sequentiel : \n");
	calculSpaceSeq();
	printf("Calcul de l'espace parallele : \n");
	calculSpacePara();

	// Création des images
	bmpJuliaSeq.WriteToFile("juliaSeq.bmp");
	bmpJuliaPara.WriteToFile("juliaPara.bmp");
	bmpGraphH.WriteToFile("GraphH.bmp");
	bmpGraphV.WriteToFile("GraphV.bmp");

	return 0;
}

