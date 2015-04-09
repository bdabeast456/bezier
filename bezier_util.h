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
