#pragma once
#ifndef _vector
#define _vector
#include <vector>
#endif

using namespace std;

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

class Bezier{
    /*
    * Bezier is a class that holds the bezier data for a given surface. 
    */
    public:
    Bezier(double patch[][3]);
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
        Surface(){};
        Surface(double fi[][3], double se[][3], double th[][3], double fo[][3]);
        Bezier bez1;
        Bezier bez2;
        Bezier bez3;
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
	Polygon(vector<vector<double> > vx, int ident);
	vector<Vector4> vertices;
	int id; 
};

class matrix {
//0: translation, 1: scale, 2: rotation
//Warning: don't use the invmult on a matrix created by transposeInverse
public:
    matrix();
	matrix(double a, double b, double c, int mtype);
	Vector4 multiplyv(Vector4 v);
    void printMatrix();
private:
    vector<Vector4> mtrx;
    vector<Vector4> inv;
};
