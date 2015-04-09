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
