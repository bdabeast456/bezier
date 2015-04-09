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
	id = ident;
}

Vector4::Vector4() {
    x = 0;
    y = 0;
    z = 0;
    w = 0;
}

Vector4::Vector4(double px, double py, double pz, double pw) {
    x = px;
    y = py;
    z = pz;
    w = pw;
}

double Vector4::xc() {
    return x;
}

double Vector4::yc() {
    return y;
}

double Vector4::zc() {
    return z;
}

double Vector4::wc() {
    return w;
}

double Vector4::dot4(Vector4 v) {
    return x*v.xc() + y*v.yc() + z*v.zc() + w*v.wc();
}

matrix::matrix() {
	mtrx.clear();
	inv.clear();
    mtrx.push_back(Vector4(1.0, 0.0, 0.0, 0.0));
    mtrx.push_back(Vector4(0.0, 1.0, 0.0, 0.0));
    mtrx.push_back(Vector4(0.0, 0.0, 1.0, 0.0));
    mtrx.push_back(Vector4(0.0, 0.0, 0.0, 1.0));
    inv.push_back(Vector4(1.0, 0.0, 0.0, 0.0));
    inv.push_back(Vector4(0.0, 1.0, 0.0, 0.0));
    inv.push_back(Vector4(0.0, 0.0, 1.0, 0.0));
    inv.push_back(Vector4(0.0, 0.0, 0.0, 1.0));

}

matrix::matrix(double a, double b, double c, int mtype) {
	mtrx.clear();
	inv.clear();
    if (mtype == 0) { // translation 
        mtrx.push_back(Vector4(1.0, 0.0, 0.0, a));
        mtrx.push_back(Vector4(0.0, 1.0, 0.0, b));
        mtrx.push_back(Vector4(0.0, 0.0, 1.0, c));
        inv.push_back(Vector4(1.0, 0.0, 0.0, -a));
        inv.push_back(Vector4(0.0, 1.0, 0.0, -b));
        inv.push_back(Vector4(0.0, 0.0, 1.0, -c));
    } else if (mtype == 1) { // scale
        mtrx.push_back(Vector4(a, 0.0, 0.0, 0.0));
        mtrx.push_back(Vector4(0.0, b, 0.0, 0.0));
        mtrx.push_back(Vector4(0.0, 0.0, c, 0.0));
        inv.push_back(Vector4(1.0/a, 0.0, 0.0, 0.0));
        inv.push_back(Vector4(0.0, 1.0/b, 0.0, 0.0));
        inv.push_back(Vector4(0.0, 0.0, 1.0/c, 0.0));
    } else if (mtype == 2) { //rotation
        double theta = pow(pow(a, 2) + pow(b, 2) + pow(c, 2), .5)*PI_rad;
        Vector rotation = Vector(a, b, c);
        rotation.unit();
        double x = rotation.xc();
        double y = rotation.yc();
        double z = rotation.zc();
        mtrx.push_back(Vector4(pow(x, 2)+(pow(z, 2)+pow(y, 2))*cos(theta),
                         x*y-z*sin(theta)-x*y*cos(theta),
                         x*z+y*sin(theta)-x*z*cos(theta), 0.0));
        mtrx.push_back(Vector4(x*y+z*sin(theta)-x*y*cos(theta),
                        pow(y, 2)+(pow(x, 2)+pow(z, 2))*cos(theta),
                        y*z-x*sin(theta)-y*z*cos(theta), 0.0));
        mtrx.push_back(Vector4(x*z-y*sin(theta)-x*z*cos(theta),
                         y*z+x*sin(theta)-z*y*cos(theta),
                         pow(z, 2)+(pow(x, 2)+pow(y, 2))*cos(theta), 0.0));
        inv.push_back(Vector4(pow(x, 2)+(pow(z, 2)+pow(y, 2))*cos(-theta),
                         x*y-z*sin(-theta)-x*y*cos(-theta),
                         x*z+y*sin(-theta)-x*z*cos(-theta), 0.0));
        inv.push_back(Vector4(x*y+z*sin(-theta)-x*y*cos(-theta),
                        pow(y, 2)+(pow(x, 2)+pow(z, 2))*cos(-theta),
                        y*z-x*sin(-theta)-y*z*cos(-theta), 0.0));
        inv.push_back(Vector4(x*z-y*sin(-theta)-x*z*cos(-theta),
                         y*z+x*sin(-theta)-z*y*cos(-theta),
                         pow(z, 2)+(pow(x, 2)+pow(y, 2))*cos(-theta), 0.0));
        }
    mtrx.push_back(Vector4(0.0, 0.0, 0.0, 1.0));
    inv.push_back(Vector4(0.0, 0.0, 0.0, 1.0));
}

matrix::matrix(double x1, double x2, double x3, double x4,
               double y1, double y2, double y3, double y4,
               double z1, double z2, double z3, double z4,
               double w1, double w2, double w3, double w4) {
	mtrx.clear();
	inv.clear();
    mtrx.push_back(Vector4(x1, y1, z1, w1));
    mtrx.push_back(Vector4(x2, y2, z2, w2));
    mtrx.push_back(Vector4(x3, y3, z3, w3));
    mtrx.push_back(Vector4(x4, y4, z4, w4));

    for (int i=0; i<4; i++) {
        inv.push_back(Vector4());
    }
}

Vector4 matrix::multiplyv(Vector4 v) {
    return Vector4(mtrx[0].dot4(v), mtrx[1].dot4(v), mtrx[2].dot4(v), mtrx[3].dot4(v));
}

Vector4 matrix::invmult(Vector4 v) {
    return Vector4(inv[0].dot4(v), inv[1].dot4(v), inv[2].dot4(v), inv[3].dot4(v));
}

void matrix::multiplym(matrix m) {
    Vector4 a = Vector4(m.mtrx[0].xc(), m.mtrx[1].xc(), m.mtrx[2].xc(), m.mtrx[3].xc());
    Vector4 b = Vector4(m.mtrx[0].yc(), m.mtrx[1].yc(), m.mtrx[2].yc(), m.mtrx[3].yc());
    Vector4 c = Vector4(m.mtrx[0].zc(), m.mtrx[1].zc(), m.mtrx[2].zc(), m.mtrx[3].zc());
    Vector4 d = Vector4(m.mtrx[0].wc(), m.mtrx[1].wc(), m.mtrx[2].wc(), m.mtrx[3].wc());
    for (int i=0; i<4; i++) {
        mtrx[i] = Vector4(mtrx[i].dot4(a), mtrx[i].dot4(b), mtrx[i].dot4(c), mtrx[i].dot4(d));
    }
    a = Vector4(inv[0].xc(), inv[1].xc(), inv[2].xc(), inv[3].xc());
    b = Vector4(inv[0].yc(), inv[1].yc(), inv[2].yc(), inv[3].yc());
    c = Vector4(inv[0].zc(), inv[1].zc(), inv[2].zc(), inv[3].zc());
    d = Vector4(inv[0].wc(), inv[1].wc(), inv[2].wc(), inv[3].wc());
    for (int i=0; i<4; i++) {
        inv[i] = Vector4(m.inv[i].dot4(a), m.inv[i].dot4(b), m.inv[i].dot4(c), m.inv[i].dot4(d));
    }
}

matrix matrix::transposeInverse() {
    return matrix(inv[0].xc(), inv[0].yc(), inv[0].zc(), inv[0].wc(),
                  inv[1].xc(), inv[1].yc(), inv[1].zc(), inv[1].wc(),
                  inv[2].xc(), inv[2].yc(), inv[2].zc(), inv[2].wc(),
                  inv[3].xc(), inv[3].yc(), inv[3].zc(), inv[3].wc());
}

void matrix::printMatrix() {
	cout << "Printing matrix" << endl;
	for (int i=0; i<4; i++) {
		cout << mtrx[i].xc() << " " << mtrx[i].yc() << " " << mtrx[i].zc() << " " << mtrx[i].wc() << endl;
	}
	cout << endl;
	for (int i=0; i<4; i++) {
		cout << inv[i].xc() << " " << inv[i].yc() << " " << inv[i].zc() << " " << inv[i].wc() << endl;
	}
	cout << endl;
}
