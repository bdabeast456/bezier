#pragma once
#ifndef _vector
#define _vector
#include <vector>
#endif

using namespace std;

class Surface{
    public:
        Surface();
        Surface(double fi[], double se[], double th[], double fo[]);
        vector<double> first;
        vector<double> second;
        vector<double> third;
        vector<double> fourth;
};
