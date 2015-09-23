//////////////////////////////////////////////////////////////////////////
//
// RBFInterpolator : interpolation by radial basis functions
//
// 2009 Karsten Noe
//
// Read the blog at cg.alexandra.dk for more information
//
//////////////////////////////////////////////////////////////////////////

#define CP_NUM 29 // control point
#define VX_NUM 2087 // vertices

#define EYES 16134

#include "RBFInterpolator.h"
#include <stdio.h>
#include <math.h>


RBFInterpolator::RBFInterpolator()
{
	successfullyInitialized = false;
}

RBFInterpolator::RBFInterpolator(vector<real> x, vector<real> y, vector<real> z, vector<real> f)
{
	successfullyInitialized = false; // default value for if we end init prematurely.
	isGeodesic = false;

	cp_numbers = new int[CP_NUM];
	cp_numbers[0]=17784; cp_numbers[1]=17918; cp_numbers[2]=17643; cp_numbers[3]=17946;	cp_numbers[4]=16999; cp_numbers[5]=16921; cp_numbers[6]=17668; cp_numbers[7]=17766;
	cp_numbers[8]=17930; cp_numbers[9]=17915; cp_numbers[10]=17944; cp_numbers[11]=16589; cp_numbers[12]=16728; cp_numbers[13]=17184; cp_numbers[14]=16900; cp_numbers[15]=16791;
	cp_numbers[16]=17433; cp_numbers[17]=16908; cp_numbers[18]=17691; cp_numbers[19]=18152; cp_numbers[20]=17309; cp_numbers[21]=17063; cp_numbers[22]=17142; cp_numbers[23]=17375;
	cp_numbers[24]=17771; cp_numbers[25]=17982; cp_numbers[26]=17972; cp_numbers[27]=18180; cp_numbers[28]=17695;

	M = f.size();

	// all four input vectors must have the same length.
	if ( x.size() != M || y.size() != M || z.size() != M )
		return;

	F = ColumnVector(M + 4);
	P = Matrix(M, 3);

	Matrix G(M + 4,M + 4);

	// copy function values
	for (int i = 1; i <= M; i++)
		F(i) = f[i-1];

	F(M+1) = 0;  F(M+2) = 0;  F(M+3) = 0;  F(M+4) = 0;

	// fill xyz coordinates into P
	for (int i = 1; i <= M; i++)
	{
		P(i,1) = x[i-1];
		P(i,2) = y[i-1];
		P(i,3) = z[i-1];
	}

	// READING DISTANCES FROM FILE
	string dist_file = "cp_distances.txt";
	FILE *fp = fopen(dist_file.c_str(), "r");
	geodesic = new double*[CP_NUM];
	for(int i = 0; i < CP_NUM; i++) geodesic[i] = new double[VX_NUM];
	for (int i = 0; i < CP_NUM; i++) {
		for (int j = 0; j < VX_NUM; j++) {
			double dist;
			fscanf(fp, "%lf", &dist);
			geodesic[i][j]=dist;
		}
	}
	fclose(fp);

	// the matrix below is symmetric, so I could save some calculations Hmmm. must be a todo
	for (int i = 1; i <= M; i++)
	for (int j = 1; j <= M; j++)
	{
		// INIT PART
		if (isGeodesic) {
			G(i,j) = g( geodesic[i-1][cp_numbers[j-1]-EYES]*geodesic[i-1][cp_numbers[j-1]-EYES] );
			printf(" %f ", G(i,j));
		}
		else {
			real dx = x[i-1] - x[j-1];
			real dy = y[i-1] - y[j-1];
			real dz = z[i-1] - z[j-1];

			real distance_squared = dx*dx + dy*dy + dz*dz;

			G(i,j) = g(distance_squared);
		}
	}

	//Set last 4 columns of G
	for (int i = 1; i <= M; i++)
	{
		G( i, M+1 ) = 1;
		G( i, M+2 ) = x[i-1];
		G( i, M+3 ) = y[i-1];
		G( i, M+4 ) = z[i-1];
	}

	for (int i = M+1; i <= M+4; i++)
	for (int j = M+1; j <= M+4; j++)
		G( i, j ) = 0;

	//Set last 4 rows of G
	for (int j = 1; j <= M; j++)
	{
		G( M+1, j ) = 1;
		G( M+2, j ) = x[j-1];
		G( M+3, j ) = y[j-1];
		G( M+4, j ) = z[j-1];
	}

	Try
	{
		Ginv = G.i();

		A = Ginv*F;
		successfullyInitialized = true;
	}
    CatchAll { cout << BaseException::what() << endl; }

}

RBFInterpolator::~RBFInterpolator()
{

}

real RBFInterpolator::interpolate(real x, real y, real z, int pt_num)
{
	if (!successfullyInitialized)
		return 0.0f;

	real sum = 0.0f;

	// RBF part
	for (int i = 1; i <= M; i++)
	{
		// INTERPOLATION PART
		if (isGeodesic) sum += A(i) * g( geodesic[i-1][pt_num-EYES]*geodesic[i-1][pt_num-EYES] ); // + F
		else {
			real dx = x - P(i,1);
			real dy = y - P(i,2);
			real dz = z - P(i,3);

			real distance_squared = dx*dx + dy*dy + dz*dz;

			sum += A(i) * g(distance_squared);
		}
	}

	//affine part
	sum += A(M+1) + A(M+2)*x + A(M+3)*y + A(M+4)*z;

	return sum;
}

//note: assuming the input is t squared
real RBFInterpolator::g(real t_squared)
{
	return sqrt(log10(t_squared + 1.0f));
}

void RBFInterpolator::UpdateFunctionValues(vector<real> f)
{
	successfullyInitialized = false;

	//ColumnVector F(M+4);

	// copy function values
	for (int i = 1; i <= M; i++)
		F(i) = f[i-1];

	F(M+1) = 0;  F(M+2) = 0;  F(M+3) = 0;  F(M+4) = 0;

	Try
	{
		A = Ginv*F;
		successfullyInitialized = true;
	}
    CatchAll { cout << BaseException::what() << endl; }

}

void RBFInterpolator::toggleGeodesic() {
	if (isGeodesic) isGeodesic = false;
	else isGeodesic = true;
};
