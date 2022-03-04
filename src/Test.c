#include <stdio.h>
#include <stdlib.h>
#include <math.h>

typedef struct {
	unsigned char r, g, b;
} pixel;

typedef struct {
     int x, y;
     pixel *boja;
} img;

int main(int argc, char* argv[]) {

	float k1[8][8] = {
		{16, 11, 10, 16, 24, 40, 51, 61},
		{12, 12, 14, 19, 26, 58, 60, 55},
		{14, 13, 16, 24, 40, 57, 69, 56},
		{14, 17, 22, 29, 51, 87, 80, 62},
        {18, 22, 37, 56, 68, 109, 103, 77},
		{24, 35, 55, 64, 81, 104, 113, 92},
        {49, 64, 78, 87, 103, 121, 120, 101},
        {72, 92, 95, 98, 112, 100, 103, 99}};

	float k2[8][8] = {
		{17, 18, 24, 47, 99, 99, 99, 99},
        {18, 21, 26, 66, 99, 99, 99, 99},
		{24, 26, 56, 99, 99, 99, 99, 99},
		{47, 66, 99, 99, 99, 99, 99, 99},
		{99, 99, 99, 99, 99, 99, 99, 99},
		{99, 99, 99, 99, 99, 99, 99, 99},
        {99, 99, 99, 99, 99, 99, 99, 99},
		{99, 99, 99, 99, 99, 99, 99, 99}};

	char skip[10];
	FILE *input, *output;
	img *slika;

	input = fopen(argv[1],  "rb");
	fscanf(input, "%s", skip);
	slika = (img*) malloc(sizeof(img));
	fscanf(input, "%d %d", &slika->x, &slika->y);
	fscanf(input, "%s", skip);
	while (fgetc(input) != '\n');

	slika->boja = (pixel*)malloc(slika->x*slika->y*sizeof(pixel));
	fread(slika->boja, 3*slika->x, slika->y, input);
	fclose(input);

	int y = 512 * 8 * (atoi(argv[2])/64);
	int x = 0, k = 0, i, j;
	float Y[8][8] = {0};
	float U[8][8] = {0};
	float V[8][8] = {0};
	for (i = 0; i < 8; i++, x += 512, k = 0) {
		for (j = (atoi(argv[2]) % 64) * 8 + y + x; j < (atoi(argv[2]) % 64) * 8 + y + x + 8; ++j, ++k) {
			Y[i][k] = 0.299*(int)slika->boja[j].r + 0.587*(int)slika->boja[j].g + 0.114*(int)slika->boja[j].b - 128;
			U[i][k] = -0.1687*(int)slika->boja[j].r - 0.3313*(int)slika->boja[j].g + 0.5*(int)slika->boja[j].b;
			V[i][k] = 0.5*(int)slika->boja[j].r - 0.4187*(int)slika->boja[j].g - 0.0813*(int)slika->boja[j].b;
		}
	}

	float transY[8][8] = {0};
	float transU[8][8] = {0};
	float transV[8][8] = {0};
	double sumY, sumU, sumV, raz;
	int l;

	for (i = 0; i < 8; ++i) {
        for(j = 0; j < 8; ++j) {

            sumY = 0;
            sumU = sumY;
            sumV = sumU;
            for(k = 0; k < 8; k++) {
                for(l = 0; l < 8; l++) {
                	sumY += Y[k][l] * cos((2 * k + 1) * i * M_PI / 16.0) * cos((2 * l + 1) * j * M_PI / 16.0);
                	sumU += U[k][l] * cos((2 * k + 1) * i * M_PI / 16.0) * cos((2 * l + 1) * j * M_PI / 16.0);
                	sumV += V[k][l] * cos((2 * k + 1) * i * M_PI / 16.0) * cos((2 * l + 1) * j * M_PI / 16.0);
                }
            }

            if (i == 0 && j == 0) raz = 0.5;
            else if (i != 0 && j != 0) raz = 1;
            else raz = 1.0/sqrt(2.0);

            transY[i][j] = 0.25 * raz * sumY;
            transU[i][j] = 0.25 * raz * sumU;
            transV[i][j] = 0.25 * raz * sumV;
        }
    }

	for (i = 0; i < 8; i++) {
		for (j = 0; j < 8; j++) {
			transY[i][j] = roundf(transY[i][j]/k1[i][j]);
			transU[i][j] = roundf(transU[i][j]/k2[i][j]);
			transV[i][j] = roundf(transV[i][j]/k2[i][j]);
		}
	}

	output = fopen(argv[3], "w+");

    for(i = 0; i < 8; i++) {
            for(j = 0; j < 8; j++) fprintf(output, "%d ", (int)transY[i][j]);
            fprintf(output, "\n");
    }
    fprintf(output, "\n");

    for(i= 0; i < 8; i++) {
            for(j = 0; j < 8; j++) fprintf(output, "%d ", (int)transU[i][j]);
            fprintf(output, "\n");
    }
    fprintf(output, "\n");

    for(i = 0; i < 8; i++) {
            for(j = 0; j < 8; j++) fprintf(output, "%d ", (int)transV[i][j]);
            fprintf(output, "\n");
    }
    fprintf(output, "\n");

	fclose(output);
	return 0;
}
