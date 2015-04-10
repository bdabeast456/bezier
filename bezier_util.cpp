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

Bezier::Bezier(double patch[][3]){
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
    double x = cx[0] + cx[1]*u + cx[2]*pow(u, 2) + cx[3]*pow(u, 3);
    double y = cy[0] + cy[1]*u + cy[2]*pow(u, 2) + cy[3]*pow(u, 3);
    double z = cz[0] + cz[1]*u + cz[2]*pow(u, 2) + cz[3]*pow(u, 3);
    vector<double> rv;
    rv.push_back(x);
    rv.push_back(y);
    rv.push_back(z);
    return rv;
}


Surface::Surface(double fi[][3], double se[][3], double th[][3], double fo[][3]){
    /*
    * arguments in the form of fi = {{x,y,z},{x,y,z},{x,y,z},{x,y,z}}
    */
    bez1 = Bezier(fi);
    bez2 = Bezier(se);
    bez3 = Bezier(th);
    bez4 = Bezier(fo);
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

    double v_patch[4][3] = {{point1[0], point1[1], point1[2]},
							{point2[0], point2[1], point2[2]},
							{point3[0], point3[1], point3[2]},
							{point4[0], point4[1], point4[2]}};

    Bezier* v_bez = new Bezier(v_patch);
    vector<double> retval = v_bez->getPoint(v);
    delete v_bez;
    return retval;
}


Polygon::Polygon(vector<vector<double> > vx, int ident) {
	/*
	* Constructs a Polygon with an arbitrary number of vertices.
	*/
	for (int i=0; i<vx.size(); i++) {
		vertices.push_back(Vector4(vx[i][0], vx[i][1], vx[i][2], 1));
	}
	id = ident;
}

matrix::matrix() {
	/*
	* Constructs the identity matrix.
	*/
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
	/*
	* Constructs a transformation matrix. 0 = translation,
	* 1 = scaling, and 2 = rotation (exponential map).
	*/
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

Vector4 matrix::multiplyv(Vector4 v) {
    return Vector4(mtrx[0].dot4(v), mtrx[1].dot4(v), mtrx[2].dot4(v), mtrx[3].dot4(v));
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
