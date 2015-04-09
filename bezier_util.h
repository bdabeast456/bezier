#pragma once
#ifndef _vector
#define _vector
#include <vector>
#endif

using namespace std;

class Surface{
    /*
    * Surface is a class that holds 4 4x3 patches. The coordinates of these patches
    * are specified by the input .bez file
    */
    public:
        Surface();
        Surface(double fi[], double se[], double th[], double fo[]);
        vector<double> first;
        vector<double> second;
        vector<double> third;
        vector<double> fourth;
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
	vector<double> v1;
	vector<double> v2;
	vector<double> v3;
	vector<double> v4;
	int id; 
};
