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
    /*
     * Constructs the 0 Vector4.
     */
    x = 0;
    y = 0;
    z = 0;
    w = 0;
}

Vector4::Vector4(double px, double py, double pz, double pw) {
    /*
     * Constructs new Vector4 with specified components.
     */
    x = px;
    y = py;
    z = pz;
    w = pw;
}

double Vector4::xc() {
    /*
     * Returns x component of this Vector4.
     */
    return x;
}

double Vector4::yc() {
    /*
     * Returns y component of this Vector4.
     */
    return y;
}

double Vector4::zc() {
    /*
     * Returns z component of this Vector4.
     */
    return z;
}

double Vector4::wc() {
    /*
     * Returns w component of this Vector4.
     */
    return w;
}

double Vector4::dot4(Vector4 v) {
    /*
     * Returns the dot product of this Vector4 and v.
     */
    return x*v.xc() + y*v.yc() + z*v.zc() + w*v.wc();
}

void Vector4::unit() {
    if (w == 1) {
        x = x/w;
        y = y/w;
        z = z/w;
        w = 1.0;
        return;
    } else {
        double length = pow(x*x + y*y + z*z, .5);
        x = x/length;
        y = y/length;
        z = z/length;
        return;
    }
}

Vector4 Vector4::sub(Vector4 v) {
    Vector4 newV = Vector4(x-v.xc(), y-v.yc(), z-v.zc(), 0);
    return newV;
}

Vector4 Vector4::cross(Vector4 v) {
    return Vector4(y*v.zc()-z*v.yc(), -(x*v.zc()-z*v.xc()), x*v.yc()-y*v.xc(), 0);
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
    vector<double> x_comp;
    x_comp.push_back(patch[0][0]);
    x_comp.push_back(patch[0][1]);
    x_comp.push_back(patch[0][2]);

    // Y COMPONENT
    cy.push_back(patch[0][1]);
    cy.push_back(-3*patch[0][1] + 3*patch[1][1]);
    cy.push_back(3*patch[0][1] -6*patch[1][1] + 3*patch[2][1]);
    cy.push_back(-patch[0][1] + 3*patch[1][1] -3*patch[2][1] + patch[3][1]);
    vector<double> y_comp;
    y_comp.push_back(patch[1][0]);
    y_comp.push_back(patch[1][1]);
    y_comp.push_back(patch[1][2]);


    // Z COMPONENT
    cz.push_back(patch[0][2]);
    cz.push_back(-3*patch[0][2] + 3*patch[1][2]);
    cz.push_back(3*patch[0][2] -6*patch[1][2] + 3*patch[2][2]);
    cz.push_back(-patch[0][2] + 3*patch[1][2] -3*patch[2][2] + patch[3][2]);
    vector<double> z_comp;
    z_comp.push_back(patch[2][0]);
    z_comp.push_back(patch[2][1]);
    z_comp.push_back(patch[2][2]);

    vector<double> fourth_comp;
    fourth_comp.push_back(patch[3][0]);
    fourth_comp.push_back(patch[3][1]);
    fourth_comp.push_back(patch[3][2]);



    patch_store.push_back(x_comp);
    patch_store.push_back(y_comp);
    patch_store.push_back(z_comp);



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

vector<double> Surface::getSurfaceNormal(double u, double v){
    /*
     * get surface normal at point u and v
     */
    vector<double> point1 = bez1.getPoint(u);
    vector<double> point2 = bez2.getPoint(u);
    vector<double> point3 = bez3.getPoint(u);
    vector<double> point4 = bez4.getPoint(u);

    double v_patch[4][3] = {{point1[0], point1[1], point1[2]},
        {point2[0], point2[1], point2[2]},
        {point3[0], point3[1], point3[2]},
        {point4[0], point4[1], point4[2]}};
    Bezier v_bez = Bezier(v_patch);
    vector<double> v_point = v_bez.getPoint(v);

    Bezier bez1Inv;
    double input1[4][3] = {{bez1.patch_store[0][0],bez1.patch_store[0][1],bez1.patch_store[0][2]},
        {bez2.patch_store[0][0],bez2.patch_store[0][1],bez2.patch_store[0][2]},
        {bez3.patch_store[0][0],bez3.patch_store[0][1],bez3.patch_store[0][2]},
        {bez4.patch_store[0][0],bez4.patch_store[0][1],bez4.patch_store[0][2]}};
    bez1Inv = Bezier(input1);

    Bezier bez2Inv;
    double input2[4][3] = {{bez1.patch_store[1][0],bez1.patch_store[1][1],bez1.patch_store[1][2]},
        {bez2.patch_store[1][0],bez2.patch_store[1][1],bez2.patch_store[1][2]},
        {bez3.patch_store[1][0],bez3.patch_store[1][1],bez3.patch_store[1][2]},
        {bez4.patch_store[1][0],bez4.patch_store[1][1],bez4.patch_store[1][2]}};
    bez2Inv = Bezier(input2);

    Bezier bez3Inv;
    double input3[4][3] = {{bez1.patch_store[2][0],bez1.patch_store[2][1],bez1.patch_store[2][2]},
        {bez2.patch_store[2][0],bez2.patch_store[2][1],bez2.patch_store[2][2]},
        {bez3.patch_store[2][0],bez3.patch_store[2][1],bez3.patch_store[2][2]},
        {bez4.patch_store[2][0],bez4.patch_store[2][1],bez4.patch_store[2][2]}};
    bez3Inv = Bezier(input3);

    Bezier bez4Inv;
    double input4[4][3] = {{bez1.patch_store[3][0],bez1.patch_store[3][1],bez1.patch_store[3][2]},
        {bez2.patch_store[3][0],bez2.patch_store[3][1],bez2.patch_store[3][2]},
        {bez3.patch_store[3][0],bez3.patch_store[3][1],bez3.patch_store[3][2]},
        {bez4.patch_store[3][0],bez4.patch_store[3][1],bez4.patch_store[3][2]}};
    bez4Inv = Bezier(input4);

    vector<double> point1u = bez1Inv.getPoint(v);
    vector<double> point2u = bez2Inv.getPoint(v);
    vector<double> point3u = bez3Inv.getPoint(v);
    vector<double> point4u = bez4Inv.getPoint(v);

    double u_patch[4][3] = {{point1u[0], point1u[1], point1u[2]},
        {point2u[0], point2u[1], point2u[2]},
        {point3u[0], point3u[1], point3u[2]},
        {point4u[0], point4u[1], point4u[2]}};

    Bezier u_bez = Bezier(u_patch);
    vector<double> u_point = u_bez.getPoint(v);

    // V DERIVATIVE CALCULATION
    double A[3] = {(v_bez).patch_store[0][0]*(1-v) + v_bez.patch_store[1][0]*v, 
        v_bez.patch_store[0][1]*(1-v)+ v_bez.patch_store[1][1]*v,
        v_bez.patch_store[0][2]*(1-v)+ v_bez.patch_store[1][2]*v};
    double B[3] = {(v_bez).patch_store[1][0]*(1-v) + v_bez.patch_store[2][0]*v, 
        v_bez.patch_store[1][1]*(1-v)+ v_bez.patch_store[2][1]*v,
        v_bez.patch_store[1][2]*(1-v)+ v_bez.patch_store[2][2]*v};
    double C[3] = {(v_bez).patch_store[2][0]*(1-v) + v_bez.patch_store[3][0]*v, 
        v_bez.patch_store[2][1]*(1-v)+ v_bez.patch_store[3][1]*v,
        v_bez.patch_store[2][2]*(1-v)+ v_bez.patch_store[3][2]*v};
    double D[3] = {A[0]*(1-v) + B[0]*v, A[1]*(1-v)+B[1]*v, A[2]*(1-v)*B[2]*v};//A*(1-u) + B*u
    double E[3] = {B[0]*(1-v) + C[0]*v, B[1]*(1-v)+C[1]*v, B[2]*(1-v)*C[2]*v}; //B*(1-u) + C*u:w
    double dv[3]= {E[0]-D[0],E[1]-D[1],E[2]-D[2]};

    //U DERIVATE CALCULATION
    double A1[3] = {(u_bez).patch_store[0][0]*(1-u) + u_bez.patch_store[1][0]*u, 
        u_bez.patch_store[0][1]*(1-u)+ u_bez.patch_store[1][1]*u,
        u_bez.patch_store[0][2]*(1-u)+ u_bez.patch_store[1][2]*u};
    double B1[3] = {(u_bez).patch_store[1][0]*(1-u) + u_bez.patch_store[2][0]*u, 
        u_bez.patch_store[1][1]*(1-u)+ u_bez.patch_store[2][1]*u,
        u_bez.patch_store[1][2]*(1-u)+ u_bez.patch_store[2][2]*u};
    double C1[3] = {(u_bez).patch_store[2][0]*(1-u) + u_bez.patch_store[3][0]*u, 
        u_bez.patch_store[2][1]*(1-u)+ u_bez.patch_store[3][1]*u,
        u_bez.patch_store[2][2]*(1-u)+ u_bez.patch_store[3][2]*u};
    double D1[3] = {A1[0]*(1-u) + B1[0]*u, A1[1]*(1-u)+B1[1]*u, A1[2]*(1-u)*B1[2]*u};//A*(1-u) + B*u
    double E1[3] = {B1[0]*(1-u) + C1[0]*u, B1[1]*(1-u)+C1[1]*u, B1[2]*(1-u)*C1[2]*u}; //B*(1-u) + C*u:w
    double du[3]= {E1[0]-D1[0],E1[1]-D1[1],E1[2]-D1[2]};

    Vector4 dpdu = Vector4(du[0],du[1],du[2],0);
    Vector4 dpdv = Vector4(dv[0],dv[1],dv[2],0);
    Vector4 uv = dpdu.cross(dpdv);

    vector<double> return_value;
    return_value.push_back(uv.xc());
    return_value.push_back(uv.yc());
    return_value.push_back(uv.zc());
    return return_value;

}


Polygon::Polygon(vector<vector<double> > vx, int ident) {
    /*
     * Constructs a Polygon with an arbitrary number of vertices.
     */
    for (int i=0; i<vx.size(); i++) {
        vertices.push_back(Vector4(vx[i][0], vx[i][1], vx[i][2], 1));
    }
    id.push_back(ident);
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
        Vector4 rotation = Vector4(a, b, c,0);
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


