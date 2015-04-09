#pragma once
#ifndef _vector
#define _vector
#include <vector>
#endif

using namespace std;

class Bezier{
    /*
    * Bezier is a class that holds the bezier data for a given surface. 
    */
    public:
    Bezier(vector<double*> patch);
    Bezier(){};
    vector<double> getPoint(double u);
    vector<double> cx;
    vector<double> cy;
    vector<double> cz;
    
};

class Surface{
    /*
    * Surface is a class that holds 4 4x3 patches. The coordinates of these patches
    * are specified by the input .bez file
    */
    public:
        Surface();
        Surface(double fi[][3], double se[][3], double th[][3], double fo[][3]);
        vector<double*> first;
        Bezier bez1;
        vector<double*> second;
        Bezier bez2;
        vector<double*> third;
        Bezier bez3;
        vector<double*> fourth;
        Bezier bez4;
        vector<double> getSurfacePoint(double u, double v);
};


class Polygon {
	/*
	*  Polygon is a container class that holds the vertices of polygon (for the sake
	*  of this project, only 4). Each polygon also maintains an int ID to keep track 
	*  of which file it came from.
	*/
public:
	Polygon(){};
	Polygon(double vx1[], double vx2[], double vx3[], double vx4[], int ident);
	Vector4 v1;
	Vector4 v2;
	Vector4 v3;
	Vector4 v4;
	int id; 
};

class Vector4 {
public:
    Vector4();
    Vector4(double px, double py, double pz, double pw);
    double xc();
    double yc();
    double zc();
    double wc();
    double dot4(Vector4 v);
private:
    double x;
    double y;
    double z;
    double w;
};

class matrix {
//0: translation, 1: scale, 2: rotation
//Warning: don't use the invmult on a matrix created by transposeInverse
public:
    matrix();
	matrix(double a, double b, double c, int mtype);
    matrix(double x1, double x2, double x3, double x4,
           double y1, double y2, double y3, double y4,
           double z1, double z2, double z3, double z4,
           double w1, double w2, double w3, double w4);
	Vector4 multiplyv(Vector4 v);
    Vector4 invmult(Vector4 v);
	void multiplym(matrix m);
    matrix transposeInverse();
    void printMatrix();
private:
    vector<Vector4> mtrx;
    vector<Vector4> inv;
};
