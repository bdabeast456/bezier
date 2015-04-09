#include <math.h>
#include <cmath>
#ifndef _vector
#define _vector
#include <vector>
#endif

#include <iostream>
#include "bezier_util.h"

using namespace std;

Surface::Surface(){
}

Surface::Surface(double fi[], double se[], double th[], double fo[]){
    first.push_back(fi[0]);
    first.push_back(fi[1]);
    first.push_back(fi[2]);
    
    second.push_back(se[0]);
    second.push_back(se[1]);
    second.push_back(se[2]);

    third.push_back(th[0]);
    third.push_back(th[1]);
    third.push_back(th[2]);

    fourth.push_back(fo[0]);
    fourth.push_back(fo[1]);
    fourth.push_back(fo[2]);

}

Polygon::Polygon(double vx1[], double vx2[], double vx3[], double vx4[], int ident) {
	for (int i=0; i<3; i++) {
		v1.push_back(vx1[i]);
		v2.push_back(vx1[i]);
		v3.push_back(vx1[i]);
		v4.push_back(vx1[i]);
	}
	id = ident
}
