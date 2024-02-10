#pragma once

#include <algorithm>
#include <cmath>
#include <random>
#include <iostream>

class Perlin {
public:
	Perlin(int seed);
	~Perlin();
	double noise(double x);
	double noise(double x, double y);
	double noise(double x, double y, double z);
private:
	const int P = 8;
	const int B = 1 << P;
	const int M = B - 1;
	const int NP = 8;
	const int N = 1 << NP;
	int* p;
	double** g2;
	double* g1;
	double** points;
	double lerp(double t, double a, double b);
	double s_curve(double t);
	double* G(int i);
	void normalize2(double* v);
};