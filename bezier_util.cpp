#include <math.h>
#include <cmath>
#ifndef _vector
#define _vector
#include <vector>
#endif

#include <iostream>
#include "bezier_util.h"

using namespace std;
const double PI_rad = 3.141592653589793/180;

Bezier::Bezier(vector<double*> patch){
   /*
   * argument in the form of one = {{x,y,z},{x,y,z},{x,y,z},{x,y,z}};
   */
   // X COMPONENT
   cx.push_back(patch[0][0]);
   cx.push_back(-3*patch[0][0] + 3*patch[1][0]);
   cx.push_back(3*patch[0][0] -6*patch[1][0] + 3*patch[2][0]);
   cx.push_back(-patch[0][0] + 3*patch[1][0] -3*patch[2][0] + patch[3][0]);

   // Y COMPONENT
   cy.push_back(patch[0][1]);
   cy.push_back(-3*patch[0][1] + 3*patch[1][1]);
   cy.push_back(3*patch[0][1] -6*patch[1][1] + 3*patch[2][1]);
   cy.push_back(-patch[0][1] + 3*patch[1][1] -3*patch[2][1] + patch[3][1]);

   // Z COMPONENT
   cz.push_back(patch[0][2]);
   cz.push_back(-3*patch[0][2] + 3*patch[1][2]);
   cz.push_back(3*patch[0][2] -6*patch[1][2] + 3*patch[2][2]);
   cz.push_back(-patch[0][2] + 3*patch[1][2] -3*patch[2][2] + patch[3][2]);


}

vector<double> Bezier::getPoint(double u){
    /*
    * returns {x,y,z} from the bezier formula with degree up to u^3
    */
    double x = cx[0] + cx[1]*u + cx[2]*u*u + cx[3]*u*u*u;
    double y = cy[0] + cy[1]*u + cy[2]*u*u + cy[3]*u*u*u;
    double z = cz[0] + cz[1]*u + cz[2]*u*u + cz[3]*u*u*u;
    vector<double> rv;
    rv.push_back(x);
    rv.push_back(y);
    rv.push_back(z);
    return rv;
}


Surface::Surface(){
}

Surface::Surface(double fi[][3], double se[][3], double th[][3], double fo[][3]){
    /*
    * arguments in the form of fi = {{x,y,z},{x,y,z},{x,y,z},{x,y,z}}
    */
    first.push_back(fi[0]);
    first.push_back(fi[1]);
    first.push_back(fi[2]);
    first.push_back(fi[3]);
    bez1 = Bezier(first);
    
    second.push_back(se[0]);
    second.push_back(se[1]);
    second.push_back(se[2]);
    second.push_back(se[3]);
    bez2 = Bezier(second);

    third.push_back(th[0]);
    third.push_back(th[1]);
    third.push_back(th[2]);
    third.push_back(th[3]);
    bez3 = Bezier(third);

    fourth.push_back(fo[0]);
    fourth.push_back(fo[1]);
    fourth.push_back(fo[2]);
    fourth.push_back(fo[3]);
    bez4 = Bezier(fourth);

}

vector<double> Surface::getSurfacePoint(double u, double v){
    /*
    * getSurfacePoint uses the Tensor-Product algorithm to get the point 
    * at u and v
    */
    vector<double> point1 = bez1.getPoint(u);
    vector<double> point2 = bez2.getPoint(u);
    vector<double> point3 = bez3.getPoint(u);
    vector<double> point4 = bez4.getPoint(u);

    double point1_array[3] = {point1[0],point1[1],point1[2]};
    double point2_array[3] = {point2[0],point2[1],point2[2]};
    double point3_array[3] = {point3[0],point3[1],point3[2]};
    double point4_array[3] = {point4[0],point4[1],point4[2]};

    vector<double*> v_patch;
    v_patch.push_back(point1_array);
    v_patch.push_back(point2_array);
    v_patch.push_back(point3_array);
    v_patch.push_back(point4_array);

    Bezier v_bez = Bezier(v_patch);
    return v_bez.getPoint(v);
}






Polygon::Polygon(double vx1[], double vx2[], double vx3[], double vx4[], int ident) {
	v1 = Vector4(vx1[0], vx1[1], vx1[2], 1);
	v2 = Vector4(vx2[0], vx2[1], vx2[2], 1);
	v3 = Vector4(vx3[0], vx3[1], vx3[2], 1);
	v4 = Vector4(vx4[0], vx4[1], vx4[2], 1);
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
        double length = pow(pow(a, 2) + pow(b, 2) + pow(c, 2), .5);
        double theta = pow(pow(a, 2) + pow(b, 2) + pow(c, 2), .5)*PI_rad;
        double x = a/length;
        double y = b/length;
        double z = c/length;
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
