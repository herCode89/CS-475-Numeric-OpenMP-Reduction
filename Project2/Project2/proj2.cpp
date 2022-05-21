#include <stdlib.h>
#include <stdio.h>
#include <omp.h>
#include <math.h>

#ifndef NUMT
#define NUMT  4
#endif

#ifndef NUMNODES
#define NUMNODES 1000
#endif

#ifndef NUMTRIALS
#define NUMTRIALS  1
#endif
// The main Program

#define XMIN     -1.
#define XMAX      1.
#define YMIN     -1.
#define YMAX      1.


// The code to evaluate the height at a given <i>iu</i> and <i>iv</i> is:

const float N = 2.5f;
const float R = 1.2f;

float Height(int, int);	// function prototype
float Height(int iu, int iv)	// iu,iv = 0 .. NUMNODES-1
{
	float x = -1. + 2. * (float)iu / (float)(NUMNODES - 1);	// -1. to +1.
	float y = -1. + 2. * (float)iv / (float)(NUMNODES - 1);	// -1. to +1.

	float xn = pow(fabs(x), (double)N);
	float yn = pow(fabs(y), (double)N);
	float rn = pow(fabs(R), (double)N);
	float r = rn - xn - yn;
	if (r <= 0.)
		return 0.;

	float height = pow(r, 1. / (double)N);
	return height;
}

int main(int argc, char* argv[])
{
#ifndef   _OPENMP

	fprintf(stderr, "OpenMP is not supported\n ");
	return 1;
#endif
	// the area of a single full-sized tile:
	// (not all tiles are full-sized, though)

	float fullTileArea = (((XMAX - XMIN) / (float)(NUMNODES - 1)) *
						((YMAX - YMIN) / (float)(NUMNODES - 1)));

	// tiles along the edges will have only 1/2 the area of tiles in the middle.
	//float edgeArea = (fullTileArea / 4);

	//  tiles in the corners will have only 1/4 the area of tiles in the middle.
	//float cornerArea = (fullTileArea / 2);

	// The volume contribution of each extruded height tile needs to be weighted accordingly
	float FullVolume = 0;

	//FullVolume += z * 1.0 * fullTileArea;

// sum up the weighted heights into the variable "volume"
// using an OpenMP for-loop and a reduction:

// Or, you could also use the <i>collapse</i> OpenMP clause:
// The (2) means you are collapsing 2 nested for-loops into one
// The end effect is exactly like what is shown above, but without you needing to do the mod and divide
		//double start_time;
		//double end_time;
	double start_time = omp_get_wtime();
	//int FullVolume = 0;
#pragma omp parallel for collapse(2) default(none) reduction(+:FullVolume) shared(fullTileArea) 

	for (int iv = 0; iv < NUMNODES; iv++)
	{
		for (int iu = 0; iu < NUMNODES; iu++)
		{
			float z = Height(iu, iv);
			
			if ((iu == 0 && iv == 0) || (iu == 0 && iv == NUMNODES - 1) || (iu == NUMNODES - 1 && iv == 0)
				|| (iu == NUMNODES - 1 && iv == NUMNODES - 1)) //{
				FullVolume += z * 0.25 * fullTileArea;
				//continue;
			//}

			else if (iu == 0 || iv == 0 || iu == NUMNODES - 1 || iv == NUMNODES - 1)// {
				FullVolume += z * 0.5 * fullTileArea;
				//continue;
			//}
			else FullVolume += z * fullTileArea;
		}
	}
	FullVolume = FullVolume * 2;

	double end_time = omp_get_wtime();
	double MegaHeightsPerSecond = ((double)NUMNODES * NUMNODES / (end_time - start_time) / 1000000);
	//fprintf(stderr," %2d   %8d  %6.2lf \n", NUMNODES, MegaHeightsPerSecond, (end_time - start_time), FullVolume));
	printf("NODES:  %d |  MegaHeightsPerSecond:   %f |  Time:   %f   | FullVolume:   %f \n", NUMNODES, MegaHeightsPerSecond, (end_time - start_time), FullVolume);
	//return 0;
}


