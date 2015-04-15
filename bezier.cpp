
#include <string.h>
#include <fstream>
#include <cmath>

#ifdef _WIN32
#include <windows.h>
#else
#include <sys/time.h>
#endif

#ifdef OSX
#include <GLUT/glut.h>
#include <OpenGL/glu.h>
#else
#include <GL/glut.h>
#include <GL/glu.h>
#endif

#include <time.h>
#include <math.h>
#include <iostream>
#include <algorithm>
#include <iostream>
#include <iterator>
#include <ostream>
#include <numeric>
#include <cstdlib>
#include <vector>
#include <cmath>
#include <string.h>

#include "bezier_util.h"

#define PI 3.14159265  // Should be used from mathlib
inline float sqr(float x) { return x*x; }
inline float min(float x, float y) {if (x < y) { return x;} else {return y;}}



using namespace std;

//****************************************************
// Some Classes
//****************************************************
class Viewport;

class Viewport {
    public:
        int w, h; // width and height
};


//****************************************************
// Global Variables
//****************************************************
Viewport    viewport;
int rCalls = 0;
int numSurfaces;
vector<Surface> surfaces;
vector<Polygon*> polygons;
int tessellationStrat = 0;
int currID = 0;
double step = .2;
double errorBound;
bool flatShading = true; // if false, do smooth shading. if true, do flat shading
bool wireFrame = false; // if false, do filled. if true, do wireframe
//bool shiftDown = false; // if shiftKey down
double rotIncrement =1.5;
double translateIncrement = 0.1;
vector<double> centerPoint;
double zoom = 1;


//****************************************************
// Simple init function
//****************************************************
void initScene(){
    GLfloat mat_specular[] = { 0.1, 0.1, 0.1, 1.0 };
    GLfloat mat_shininess[] = { 350 };
    GLfloat mat_amb_diff[] = { 0.1, 0.5, 0.8, 1.0 };
    //GLfloat light_position[] = { 1.0, 1.0, 1.0, 0.0 };
    glClearColor (0.0, 0.0, 0.0, 0.0);
    glShadeModel (GL_FLAT);
    //glLightModelf(GL_LIGHT_MODEL_TWO_SIDE, GL_TRUE);

    glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular);
    glMaterialfv(GL_FRONT, GL_SHININESS, mat_shininess);
    glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, mat_amb_diff);


    GLfloat light_ambient[] = { 0.5, 0.5, 0.5, 1.0 };
    GLfloat light_diffuse[] = { 1.0, 1.0, 1.0, 1.0 };
    GLfloat light_specular[] = { 1.0, 1.0, 1.0, 1.0 };
    GLfloat light_position[] = { 0.0, 0.0, 1.0, 0.0 };

    //glLightfv(GL_LIGHT0, GL_AMBIENT, light_ambient);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, light_diffuse);
    //glLightfv(GL_LIGHT0, GL_SPECULAR, light_specular);
    glLightfv(GL_LIGHT0, GL_POSITION, light_position);

    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    glEnable(GL_DEPTH_TEST);


}


//****************************************************
// reshape viewport if the window is resized
//****************************************************
void myReshape(int w, int h) {
    viewport.w = w;
    viewport.h = h;

    glViewport (0,0,viewport.w,viewport.h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(-3, 3, -3, 3, 20, -1500);
    //gluPerspective(60,200,-15000,-15000);
}

//****************************************************
// function that does the actual drawing of stuff
//***************************************************
void myDisplay() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);               // clear the color buffer

    glMatrixMode(GL_MODELVIEW);                 // indicate we are specifying camera transformations
    glLoadIdentity();                       // make sure transformation is "zero'd"
    glScalef(zoom,zoom,zoom);
    //glEnable(GL_LIGHTING);
    for (int i=0; i<polygons.size(); i++) {
        Polygon* temp = polygons[i];
        if(tessellationStrat){
            glBegin(GL_TRIANGLES);
        }
        else if(!tessellationStrat){
            glBegin(GL_QUADS);
        }
        vector<Vector4> verTemp = temp->vertices;
        int triangleCount = 0;
        for (int j= 0; j < verTemp.size(); j++) {
            //cout << triangleCount << " " << j << endl;
            /*if(!tessellationStrat && !flatShading && triangleCount != 0){
              if(triangleCount == 3){
              cout << "here?" << endl;
              j-=1;
              }
              if(triangleCount == 5){
              cout << "here...????" << endl;
              j-=5;
              }
              if(triangleCount == 6){
              j+=5;
              triangleCount = 0;
              cout << j << endl;
              }
              }*/
            if(flatShading || tessellationStrat){
                Vector4 v2 = verTemp[(j-1) % verTemp.size()].sub(verTemp[j]);
                Vector4 v1 = verTemp[(j+1) % verTemp.size()].sub(verTemp[j]);
                Vector4 crossP = v2.cross(v1);
                crossP.unit();
                glNormal3f(crossP.xc(), crossP.yc(), crossP.zc());
                //glVertex3f(verTemp[j].xc(), verTemp[j].yc(), verTemp[j].zc());
            }
            else{
                glNormal3f(temp->normals[j][0],temp->normals[j][1],temp->normals[j][2]);
                //cout << temp->normals[j][0] << endl;
            }
            glVertex3f(verTemp[j].xc(), verTemp[j].yc(), verTemp[j].zc());
            triangleCount +=1;

        }
        glEnd();
    }

    glFlush();
    glutSwapBuffers();                  // swap buffers (we earlier set double buffer)
}

bool distance(double x1, double y1, double z1, vector<double> coords) {
    /*
     * Tells if errorBound is larger than the distance between points.
     */
    if (errorBound > sqrt(sqr(x1-coords[0])+sqr(y1-coords[1])+sqr(z1-coords[2]))) { 
        return true;
    } else { 
        return false;
    }
}

void transformPolygons(matrix m){
    /*
     * Applies the transformation m to points defining tessellated polygons.
     */
    for(std::vector<Polygon*>::iterator poly = polygons.begin(); poly != polygons.end(); ++poly) {
        Polygon polygon = **poly;
        vector<Vector4> newVertices;
        vector<vector<double> > newNormals;
        for(std::vector<Vector4>::iterator vert = polygon.vertices.begin(); vert != polygon.vertices.end(); ++vert){
            Vector4 vertex = *vert;
            Vector4 newVertex = m.multiplyv(vertex);
            newVertices.push_back(newVertex);
        }
        for(std::vector<vector<double> >::iterator normPoint = polygon.normals.begin(); normPoint != polygon.normals.end(); ++normPoint){
            vector<double> norm = *normPoint;
            Vector4 normalPoint = Vector4(norm[0],norm[1],norm[2],0);
            matrix ma = m.transposeInverse();
            Vector4 newNormal = ma.multiplyv(normalPoint);
            vector<double> newNormalVector;
            newNormalVector.push_back(newNormal.xc());
            newNormalVector.push_back(newNormal.yc());
            newNormalVector.push_back(newNormal.zc());

            newNormals.push_back(newNormalVector);

        }
        (**poly).normals = newNormals;
        (**poly).vertices = newVertices;


    }
    myDisplay();

}

void findCenterPoint(int idCheck){
    /*
     * Calculates "center" of the shape represented by an input file.
     */
    for (int i=0; i<3; i++) {
        centerPoint.push_back(0);
    }
    int iterationCount = 0;
    for(int i=0; i<polygons.size(); i++) {
        Polygon polygon = *(polygons[i]);
        if(polygon.id[0] == idCheck){
            for(int j=0; j<polygon.vertices.size(); j++){
                Vector4 vertex = polygon.vertices[j]; 
                centerPoint[0] = centerPoint[0] + vertex.xc();
                centerPoint[1] = centerPoint[1] + vertex.yc();
                centerPoint[2] = centerPoint[2] + vertex.zc();
                iterationCount++;
            }
        }

    }
    centerPoint[0] = centerPoint[0] / iterationCount; // divide by # vertices
    centerPoint[1] = centerPoint[1] / iterationCount;
    centerPoint[2] = centerPoint[2] / iterationCount;
}

void myKey(unsigned char key, int x, int y) {
    /*
     * General input key handling.
     */
    if(key==32) {
        cout << rCalls << endl;
        cout << polygons.size() << endl;
        exit(0);
    }

    if(key == 115){ // 's' toggle between flat and smooth
        if(flatShading == true){
            flatShading = false;
            glShadeModel(GL_SMOOTH);
        }
        else{
            flatShading = true;
            glShadeModel(GL_FLAT);
        }
    }
    if(key == 119){ // 'w' toggle between filled and wireframe
        if(wireFrame == true){
            wireFrame = false;
            glPolygonMode( GL_FRONT_AND_BACK, GL_FILL );
        }
        else{
            wireFrame = true;
            glPolygonMode( GL_FRONT_AND_BACK, GL_LINE );
        }

    }
    matrix m;
    //zoom = 1;
    if(key == 43){ // '+' zoom in
        m = matrix(0,0,-translateIncrement,0);
        Vector4 curCenter = Vector4(centerPoint[0],centerPoint[1],centerPoint[2],1);
        curCenter = m.multiplyv(curCenter);
        centerPoint[0] = curCenter.xc();
        centerPoint[1] = curCenter.yc();
        centerPoint[2] = curCenter.zc();
        zoom += translateIncrement;

    }
    if(key == 45){ // '-' zoom out
        m = matrix(0,0,translateIncrement,0);
        Vector4 curCenter = Vector4(centerPoint[0],centerPoint[1],centerPoint[2],1);
        curCenter = m.multiplyv(curCenter);
        centerPoint[0] = curCenter.xc();
        centerPoint[1] = curCenter.yc();
        centerPoint[2] = curCenter.zc();
        zoom -= translateIncrement;

    }
    transformPolygons(m);
}

void specialKey(int key, int x, int y){
    /*
     * Shift and direction key handling.
     */
    matrix m;
    if(glutGetModifiers() == GLUT_ACTIVE_SHIFT && key == GLUT_KEY_UP){
        m = matrix(0,translateIncrement, 0 , 0);
        centerPoint[1] = centerPoint[1] + translateIncrement;
    }
    else if(key == GLUT_KEY_UP){
        m = matrix(centerPoint[0],centerPoint[1],centerPoint[2],0);
        m.multiplym(matrix(rotIncrement,0,0,2));
        m.multiplym(matrix(-centerPoint[0],-centerPoint[1],-centerPoint[2],0));
    }
    if(glutGetModifiers() == GLUT_ACTIVE_SHIFT && key == GLUT_KEY_RIGHT){ 
        m = matrix(translateIncrement,0, 0 , 0);
        centerPoint[0] = centerPoint[0] + translateIncrement;
    }
    else if(key == GLUT_KEY_RIGHT){
        m = matrix(centerPoint[0],centerPoint[1],centerPoint[2],0);
        m.multiplym(matrix(0,rotIncrement,0,2));
        m.multiplym(matrix(-centerPoint[0],-centerPoint[1],-centerPoint[2],0));

    }

    if(glutGetModifiers() == GLUT_ACTIVE_SHIFT && key == GLUT_KEY_DOWN){
        m = matrix(0,-translateIncrement, 0 , 0);
        centerPoint[1] = centerPoint[1] - translateIncrement;
    }
    else if(key == GLUT_KEY_DOWN){
        m = matrix(centerPoint[0],centerPoint[1],centerPoint[2],0);
        m.multiplym(matrix(-rotIncrement,0,0,2));
        m.multiplym(matrix(-centerPoint[0],-centerPoint[1],-centerPoint[2],0));

    }

    if(glutGetModifiers() == GLUT_ACTIVE_SHIFT && key == GLUT_KEY_LEFT){
        m = matrix(-translateIncrement,0, 0 , 0);
        centerPoint[0] = centerPoint[0] - translateIncrement;
    }
    else if(key == GLUT_KEY_LEFT){
        m = matrix(centerPoint[0],centerPoint[1],centerPoint[2],0);
        m.multiplym(matrix(0,-rotIncrement,0,2));
        m.multiplym(matrix(-centerPoint[0],-centerPoint[1],-centerPoint[2],0));
    }
    //findCenterPoint(currID);
    transformPolygons(m);    
}

void adaptRecurse(Surface * s, vector<vector<double> > * realcoords, vector<vector<double> > * uvcoords) {
    /*
     * Recursive routine for adaptive tessellation.
     */
    rCalls++;
    bool e1 = distance(((*realcoords)[0][0]+(*realcoords)[1][0])/2, ((*realcoords)[0][1]+(*realcoords)[1][1])/2, 
            ((*realcoords)[0][2]+(*realcoords)[1][2])/2, s->getSurfacePoint(((*uvcoords)[0][0]+(*uvcoords)[1][0])/2, ((*uvcoords)[0][1]+(*uvcoords)[1][1])/2));
    bool e2 = distance(((*realcoords)[1][0]+(*realcoords)[2][0])/2, ((*realcoords)[1][1]+(*realcoords)[2][1])/2, 
            ((*realcoords)[1][2]+(*realcoords)[2][2])/2, s->getSurfacePoint(((*uvcoords)[1][0]+(*uvcoords)[2][0])/2, ((*uvcoords)[1][1]+(*uvcoords)[2][1])/2));
    bool e3 = distance(((*realcoords)[2][0]+(*realcoords)[0][0])/2, ((*realcoords)[2][1]+(*realcoords)[0][1])/2, 
            ((*realcoords)[2][2]+(*realcoords)[0][2])/2, s->getSurfacePoint(((*uvcoords)[2][0]+(*uvcoords)[0][0])/2, ((*uvcoords)[2][1]+(*uvcoords)[0][1])/2));
    if (rCalls == 481) {
        for (int i=0; i<3; i++) {
            for (int j=0; j<3; j++) {
                cout << (*realcoords)[i][j] << endl;
            }
        }
    }
    if (e1 && e2 && e3) {
        Polygon* newPoly = new Polygon(*realcoords, currID);
        polygons.push_back(newPoly);
        return;
    } else if (!e1 && e2 && e3) {
        vector<double> * newpt1 = new vector<double>();
        newpt1->push_back(((*uvcoords)[0][0]+(*uvcoords)[1][0])/2);
        newpt1->push_back(((*uvcoords)[0][1]+(*uvcoords)[1][1])/2);
        vector<double> newpoint1 = s->getSurfacePoint((*newpt1)[0], (*newpt1)[1]);
        vector<vector<double> > * trgl1 = new vector<vector<double> >();
        vector<vector<double> > * uv1 = new vector<vector<double> >();    
        vector<vector<double> > * trgl2 = new vector<vector<double> >();
        vector<vector<double> > * uv2 = new vector<vector<double> >();
        trgl1->push_back(newpoint1);
        trgl1->push_back((*realcoords)[2]);
        trgl1->push_back((*realcoords)[0]);
        uv1->push_back(*newpt1);
        uv1->push_back((*uvcoords)[2]);
        uv1->push_back((*uvcoords)[0]);
        trgl2->push_back(newpoint1);
        trgl2->push_back((*realcoords)[1]);
        trgl2->push_back((*realcoords)[2]);
        uv2->push_back(*newpt1);
        uv2->push_back((*uvcoords)[1]);
        uv2->push_back((*uvcoords)[2]);
        adaptRecurse(s, trgl1, uv1);
        delete trgl1;
        delete uv1;
        adaptRecurse(s, trgl2, uv2);
        delete trgl2;
        delete uv2;
        delete newpt1;
        return;
    } else if (e1 && !e2 && e3) {
        /*if (rCalls == 481) {
          for (int i=0; i<3; i++) {
          for (int j=0; j<3; j++) {
          cout << (*realcoords)[i][j] << endl;
          }
          }
          }*/
        vector<double> * newpt1 = new vector<double>();
        newpt1->push_back(((*uvcoords)[0][0]+(*uvcoords)[2][0])/2);
        newpt1->push_back(((*uvcoords)[0][1]+(*uvcoords)[2][1])/2);
        vector<double> newpoint1 = s->getSurfacePoint((*newpt1)[0], (*newpt1)[1]);
        vector<vector<double> > * trgl1 = new vector<vector<double> >();
        vector<vector<double> > * uv1 = new vector<vector<double> >();    
        vector<vector<double> > * trgl2 = new vector<vector<double> >();
        vector<vector<double> > * uv2 = new vector<vector<double> >();
        trgl1->push_back(newpoint1);
        trgl1->push_back((*realcoords)[0]);
        trgl1->push_back((*realcoords)[1]);
        uv1->push_back(*newpt1);
        uv1->push_back((*uvcoords)[0]);
        uv1->push_back((*uvcoords)[1]);
        trgl2->push_back(newpoint1);
        trgl2->push_back((*realcoords)[1]);
        trgl2->push_back((*realcoords)[2]);
        uv2->push_back(*newpt1);
        uv2->push_back((*uvcoords)[1]);
        uv2->push_back((*uvcoords)[2]);
        /*if (rCalls == 481) {
          exit(0);
          }*/
        adaptRecurse(s, trgl1, uv1);
        delete trgl1;
        delete uv1;
        adaptRecurse(s, trgl2, uv2);
        delete trgl2;
        delete uv2;
        delete newpt1;
        return;        
    } else if (e1 && e2 && !e3) {
        vector<double> * newpt1 = new vector<double>();
        newpt1->push_back(((*uvcoords)[1][0]+(*uvcoords)[2][0])/2);
        newpt1->push_back(((*uvcoords)[1][1]+(*uvcoords)[2][1])/2);
        vector<double> newpoint1 = s->getSurfacePoint((*newpt1)[0], (*newpt1)[1]);
        vector<vector<double> > * trgl1 = new vector<vector<double> >();
        vector<vector<double> > * uv1 = new vector<vector<double> >();    
        vector<vector<double> > * trgl2 = new vector<vector<double> >();
        vector<vector<double> > * uv2 = new vector<vector<double> >();
        trgl1->push_back(newpoint1);
        trgl1->push_back((*realcoords)[0]);
        trgl1->push_back((*realcoords)[1]);
        uv1->push_back(*newpt1);
        uv1->push_back((*uvcoords)[0]);
        uv1->push_back((*uvcoords)[1]);
        trgl2->push_back(newpoint1);
        trgl2->push_back((*realcoords)[2]);
        trgl2->push_back((*realcoords)[0]);
        uv2->push_back(*newpt1);
        uv2->push_back((*uvcoords)[2]);
        uv2->push_back((*uvcoords)[0]);
        adaptRecurse(s, trgl1, uv1);
        delete trgl1;
        delete uv1;
        adaptRecurse(s, trgl2, uv2);
        delete trgl2;
        delete uv2;
        delete newpt1;
        return;
    } else if (!e1 && !e2 && e3) {
        vector<double> * newpt1 = new vector<double>();
        vector<double> * newpt2 = new vector<double>();
        newpt1->push_back(((*uvcoords)[0][0]+(*uvcoords)[1][0])/2);
        newpt1->push_back(((*uvcoords)[0][1]+(*uvcoords)[1][1])/2);
        newpt2->push_back(((*uvcoords)[2][0]+(*uvcoords)[0][0])/2);
        newpt2->push_back(((*uvcoords)[2][1]+(*uvcoords)[0][1])/2);
        vector<double> newpoint1 = s->getSurfacePoint((*newpt1)[0], (*newpt1)[1]);
        vector<double> newpoint2 = s->getSurfacePoint((*newpt2)[0], (*newpt2)[1]);
        vector<vector<double> > * trgl1 = new vector<vector<double> >();
        vector<vector<double> > * uv1 = new vector<vector<double> >();    
        vector<vector<double> > * trgl2 = new vector<vector<double> >();
        vector<vector<double> > * uv2 = new vector<vector<double> >();
        vector<vector<double> > * trgl3 = new vector<vector<double> >();
        vector<vector<double> > * uv3 = new vector<vector<double> >(); 
        trgl1->push_back(newpoint1);
        trgl1->push_back(newpoint2);
        trgl1->push_back((*realcoords)[0]);
        uv1->push_back(*newpt1);
        uv1->push_back(*newpt2);
        uv1->push_back((*uvcoords)[0]);
        trgl2->push_back(newpoint1);
        trgl2->push_back((*realcoords)[1]);
        trgl2->push_back(newpoint2);
        uv2->push_back(*newpt1);
        uv2->push_back((*uvcoords)[1]);
        uv2->push_back(*newpt2);
        trgl3->push_back(newpoint2);
        trgl3->push_back((*realcoords)[1]);
        trgl3->push_back((*realcoords)[2]);
        uv3->push_back(*newpt2);
        uv3->push_back((*uvcoords)[1]);
        uv3->push_back((*uvcoords)[2]);
        adaptRecurse(s, trgl1, uv1);
        delete trgl1;
        delete uv1;
        adaptRecurse(s, trgl2, uv2);
        delete trgl2;
        delete uv2;
        adaptRecurse(s, trgl3, uv3);
        delete trgl3;
        delete uv3;
        delete newpt1;
        delete newpt2;
        return;
    } else if (e1 && !e2 && !e3) {
        vector<double> * newpt1 = new vector<double>();
        vector<double> * newpt2 = new vector<double>();
        newpt1->push_back(((*uvcoords)[1][0]+(*uvcoords)[2][0])/2);
        newpt1->push_back(((*uvcoords)[1][1]+(*uvcoords)[2][1])/2);
        newpt2->push_back(((*uvcoords)[2][0]+(*uvcoords)[0][0])/2);
        newpt2->push_back(((*uvcoords)[2][1]+(*uvcoords)[0][1])/2);
        vector<double> newpoint1 = s->getSurfacePoint((*newpt1)[0], (*newpt1)[1]);
        vector<double> newpoint2 = s->getSurfacePoint((*newpt2)[0], (*newpt2)[1]);
        vector<vector<double> > * trgl1 = new vector<vector<double> >();
        vector<vector<double> > * uv1 = new vector<vector<double> >();    
        vector<vector<double> > * trgl2 = new vector<vector<double> >();
        vector<vector<double> > * uv2 = new vector<vector<double> >();
        vector<vector<double> > * trgl3 = new vector<vector<double> >();
        vector<vector<double> > * uv3 = new vector<vector<double> >(); 
        trgl1->push_back(newpoint1);
        trgl1->push_back((*realcoords)[0]);
        trgl1->push_back((*realcoords)[1]);
        uv1->push_back(*newpt1);
        uv1->push_back((*uvcoords)[0]);
        uv1->push_back((*uvcoords)[1]);
        trgl2->push_back(newpoint1);
        trgl2->push_back(newpoint2);
        trgl2->push_back((*realcoords)[0]);
        uv2->push_back(*newpt1);
        uv2->push_back(*newpt2);
        uv2->push_back((*uvcoords)[0]);
        trgl3->push_back(newpoint1);
        trgl3->push_back((*realcoords)[2]);
        trgl3->push_back(newpoint2);
        uv3->push_back(*newpt1);
        uv3->push_back((*uvcoords)[2]);
        uv3->push_back(*newpt2);
        adaptRecurse(s, trgl1, uv1);
        delete trgl1;
        delete uv1;
        adaptRecurse(s, trgl2, uv2);
        delete trgl2;
        delete uv2;
        adaptRecurse(s, trgl3, uv3);
        delete trgl3;
        delete uv3;
        delete newpt1;
        delete newpt2;
        return;
    } else if (!e1 && e2 && !e3) {
        vector<double> * newpt1 = new vector<double>();
        vector<double> * newpt2 = new vector<double>();
        newpt1->push_back(((*uvcoords)[0][0]+(*uvcoords)[1][0])/2);
        newpt1->push_back(((*uvcoords)[0][1]+(*uvcoords)[1][1])/2);
        newpt2->push_back(((*uvcoords)[1][0]+(*uvcoords)[2][0])/2);
        newpt2->push_back(((*uvcoords)[1][1]+(*uvcoords)[2][1])/2);
        vector<double> newpoint1 = s->getSurfacePoint((*newpt1)[0], (*newpt1)[1]);
        vector<double> newpoint2 = s->getSurfacePoint((*newpt2)[0], (*newpt2)[1]);
        vector<vector<double> > * trgl1 = new vector<vector<double> >();
        vector<vector<double> > * uv1 = new vector<vector<double> >();    
        vector<vector<double> > * trgl2 = new vector<vector<double> >();
        vector<vector<double> > * uv2 = new vector<vector<double> >();
        vector<vector<double> > * trgl3 = new vector<vector<double> >();
        vector<vector<double> > * uv3 = new vector<vector<double> >();
        trgl1->push_back(newpoint1);
        trgl1->push_back((*realcoords)[2]);
        trgl1->push_back((*realcoords)[0]);
        uv1->push_back(*newpt1);
        uv1->push_back((*uvcoords)[2]);
        uv1->push_back((*uvcoords)[0]);
        trgl2->push_back(newpoint1);
        trgl2->push_back((*realcoords)[1]);
        trgl2->push_back(newpoint2);
        uv2->push_back(*newpt1);
        uv2->push_back((*uvcoords)[1]);
        uv2->push_back(*newpt2);
        trgl3->push_back(newpoint2);
        trgl3->push_back((*realcoords)[2]);
        trgl3->push_back(newpoint1);
        uv3->push_back(*newpt2);
        uv3->push_back((*uvcoords)[2]);
        uv3->push_back(*newpt1);
        adaptRecurse(s, trgl1, uv1);
        delete trgl1;
        delete uv1;
        adaptRecurse(s, trgl2, uv2);
        delete trgl2;
        delete uv2;
        adaptRecurse(s, trgl3, uv3);
        delete trgl3;
        delete uv3;
        delete newpt1;
        delete newpt2;
        return;
    } else {
        vector<double> * newpt1 = new vector<double>();
        vector<double> * newpt2 = new vector<double>();
        vector<double> * newpt3 = new vector<double>();
        newpt1->push_back(((*uvcoords)[0][0]+(*uvcoords)[1][0])/2);
        newpt1->push_back(((*uvcoords)[0][1]+(*uvcoords)[1][1])/2);
        newpt2->push_back(((*uvcoords)[1][0]+(*uvcoords)[2][0])/2);
        newpt2->push_back(((*uvcoords)[1][1]+(*uvcoords)[2][1])/2);
        newpt3->push_back(((*uvcoords)[2][0]+(*uvcoords)[0][0])/2);
        newpt3->push_back(((*uvcoords)[2][1]+(*uvcoords)[0][1])/2);
        vector<double> newpoint1 = s->getSurfacePoint((*newpt1)[0], (*newpt1)[1]);
        vector<double> newpoint2 = s->getSurfacePoint((*newpt2)[0], (*newpt2)[1]);
        vector<double> newpoint3 = s->getSurfacePoint((*newpt3)[0], (*newpt3)[1]);
        vector<vector<double> > * trgl1 = new vector<vector<double> >();
        vector<vector<double> > * uv1 = new vector<vector<double> >();    
        vector<vector<double> > * trgl2 = new vector<vector<double> >();
        vector<vector<double> > * uv2 = new vector<vector<double> >();
        vector<vector<double> > * trgl3 = new vector<vector<double> >();
        vector<vector<double> > * uv3 = new vector<vector<double> >();
        vector<vector<double> > * trgl4 = new vector<vector<double> >();
        vector<vector<double> > * uv4 = new vector<vector<double> >();        
        trgl1->push_back(newpoint1);
        trgl1->push_back(newpoint3);
        trgl1->push_back((*realcoords)[0]);
        uv1->push_back(*newpt1);
        uv1->push_back(*newpt3);
        uv1->push_back((*uvcoords)[0]);
        trgl2->push_back(newpoint1);
        trgl2->push_back(newpoint2);
        trgl2->push_back(newpoint3);
        uv2->push_back(*newpt1);
        uv2->push_back(*newpt2);
        uv2->push_back(*newpt3);
        trgl3->push_back(newpoint1);
        trgl3->push_back((*realcoords)[1]);
        trgl3->push_back(newpoint2);
        uv3->push_back(*newpt1);
        uv3->push_back((*uvcoords)[1]);
        uv3->push_back(*newpt2);
        trgl4->push_back(newpoint2);
        trgl4->push_back((*realcoords)[2]);
        trgl4->push_back(newpoint3);
        uv4->push_back(*newpt2);
        uv4->push_back((*uvcoords)[2]);
        uv4->push_back(*newpt3);
        adaptRecurse(s, trgl1, uv1);
        delete trgl1;
        delete uv1;
        adaptRecurse(s, trgl2, uv2);
        delete trgl2;
        delete uv2;
        adaptRecurse(s, trgl3, uv3);
        delete trgl3;
        delete uv3;
        adaptRecurse(s, trgl4, uv4);
        delete trgl4;
        delete uv4;
        delete newpt1;
        delete newpt2; 
        delete newpt3;
        return;
    }
}

void adaptTessellate(Surface s, double u, double v, double uend, double vend) {
    /*
     * Starting routine for adaptive tessellation.
     */
    vector<double> point1 = s.getSurfacePoint(u, v);
    vector<double> pt1uv;
    pt1uv.push_back(u);
    pt1uv.push_back(v);
    vector<double> point2 = s.getSurfacePoint(uend, v);
    vector<double> pt2uv;
    pt2uv.push_back(uend);
    pt2uv.push_back(v);
    vector<double> point3 = s.getSurfacePoint(uend, vend);
    vector<double> pt3uv;
    pt3uv.push_back(uend);
    pt3uv.push_back(vend);
    vector<double> point4 = s.getSurfacePoint(u, vend);
    vector<double> pt4uv;
    pt4uv.push_back(u);
    pt4uv.push_back(vend);
    vector<vector<double> > * trgl1 = new vector<vector<double> >();
    vector<vector<double> > * uv1 = new vector<vector<double> >();
    vector<vector<double> > * trgl2 = new vector<vector<double> >();
    vector<vector<double> > * uv2 = new vector<vector<double> >();
    trgl1->push_back(point1);
    trgl1->push_back(point2);
    trgl1->push_back(point3);
    uv1->push_back(pt1uv);
    uv1->push_back(pt2uv);
    uv1->push_back(pt3uv);
    trgl2->push_back(point3);
    trgl2->push_back(point4);
    trgl2->push_back(point1);
    uv2->push_back(pt3uv);
    uv2->push_back(pt4uv);
    uv2->push_back(pt1uv);
    adaptRecurse(&s, trgl1, uv1);
    adaptRecurse(&s, trgl2, uv2);
    delete trgl1;
    delete uv1;
    delete trgl2;
    delete uv2;
}

void tessellate(Surface s) {
    /*
     * Perform uniform tessellation on Surface s. Step is specified as a global variable.
     */
    //cout << "tessellate" << endl;
    int steps = (int)(1/step);
    for (int vb=0; vb<steps; vb++) {
        double v = (double)(vb*step);
        for (int ub=0; ub<steps; ub++) {
            double u = (double)(ub*step);
            //cout << "start getpoints" << endl;
            vector<double> point1 = s.getSurfacePoint(u, v);
            vector<double> normal1 = s.getSurfaceNormal(u,v);

            vector<double> point2 = s.getSurfacePoint(u+step, v);
            vector<double> normal2 = s.getSurfaceNormal(u+step,v);

            vector<double> point3 = s.getSurfacePoint(u+step, v+step);
            vector<double> normal3 = s.getSurfaceNormal(u+step,v+step);

            vector<double> point4 = s.getSurfacePoint(u, v+step);
            vector<double> normal4 = s.getSurfaceNormal(u,v+step);

            vector<vector<double> > poly;
            poly.push_back(point1);
            poly.push_back(point2);
            poly.push_back(point3);
            poly.push_back(point4);
            Polygon* newPoly = new Polygon(poly, currID);

            newPoly->normals.push_back(normal1);
            newPoly->normals.push_back(normal2);
            newPoly->normals.push_back(normal3);
            newPoly->normals.push_back(normal4);
            polygons.push_back(newPoly);
        }
    }
    //Cleanup if necessary.
    if (1-(steps*step) > .0000001) {
        double v = (double)(steps*step);
        for (int ub=0; ub<steps; ub++) {
            double u = (double)(ub*step);
            vector<double> point1 = s.getSurfacePoint(u, v);
            vector<double> normal1 = s.getSurfaceNormal(u,v);

            vector<double> point2 = s.getSurfacePoint(u+step, v);
            vector<double> normal2 = s.getSurfaceNormal(u+step,v);

            vector<double> point3 = s.getSurfacePoint(u+step, 1);
            vector<double> normal3 = s.getSurfaceNormal(u+step,1);

            vector<double> point4 = s.getSurfacePoint(u, 1);
            vector<double> normal4 = s.getSurfaceNormal(u,1);

            vector<vector<double> > poly;
            poly.push_back(point1);
            poly.push_back(point2);
            poly.push_back(point3);
            poly.push_back(point4);
            Polygon* newPoly = new Polygon(poly, currID);
            newPoly->normals.push_back(normal1);
            newPoly->normals.push_back(normal2);
            newPoly->normals.push_back(normal3);
            newPoly->normals.push_back(normal4);
            polygons.push_back(newPoly);
        }
        double u = (double)(steps*step);
        for (int vb=0; vb<steps; vb++) {
            v = (double)(vb*step);
            vector<double> point1 = s.getSurfacePoint(u, v);
            vector<double> normal1 = s.getSurfaceNormal(u,v);

            vector<double> point2 = s.getSurfacePoint(1, v);
            vector<double> normal2 = s.getSurfaceNormal(1,v);

            vector<double> point3 = s.getSurfacePoint(1, v+step);
            vector<double> normal3 = s.getSurfaceNormal(1,v+step);

            vector<double> point4 = s.getSurfacePoint(u, v+step);
            vector<double> normal4 = s.getSurfaceNormal(u,v+step);

            vector<vector<double> > poly;
            poly.push_back(point1);
            poly.push_back(point2);
            poly.push_back(point3);
            poly.push_back(point4);
            Polygon* newPoly = new Polygon(poly, currID);
            newPoly->normals.push_back(normal1);
            newPoly->normals.push_back(normal2);
            newPoly->normals.push_back(normal3);
            newPoly->normals.push_back(normal4);
            polygons.push_back(newPoly);
        }
        v = steps*step;
        vector<double> point1 = s.getSurfacePoint(u, v);
        vector<double> normal1 = s.getSurfaceNormal(u,v);

        vector<double> point2 = s.getSurfacePoint(1, v);
        vector<double> normal2 = s.getSurfaceNormal(1,v);

        vector<double> point3 = s.getSurfacePoint(1, 1);
        vector<double> normal3 = s.getSurfaceNormal(1,1);

        vector<double> point4 = s.getSurfacePoint(u, 1);
        vector<double> normal4 = s.getSurfaceNormal(u,1);

        vector<vector<double> > poly;
        poly.push_back(point1);
        poly.push_back(point2);
        poly.push_back(point3);
        poly.push_back(point4);
        Polygon* newPoly = new Polygon(poly, currID);
        newPoly->normals.push_back(normal1);
        newPoly->normals.push_back(normal2);
        newPoly->normals.push_back(normal3);
        newPoly->normals.push_back(normal4);
        polygons.push_back(newPoly);        
    }
}

//****************************************************
// the usual stuff, nothing exciting here
//****************************************************
int main(int argc, char *argv[]) {

    /*
     * INSERT PARSER HERE
     */
    const int MAX_CHARS_PER_LINE = 512;
    const int MAX_TOKENS_PER_LINE = 17;
    const char* const DELIMITER = " ";

    string readFile;
    if (argc < 2) {
        cout << "No input file or step size specified." << endl;
        exit(0);
    }
    //#include <string.h>
    //for (int i=1; i<argc; i++) {
    string arg1 = string(argv[1]);
    string arg2 = string(argv[2]);
    if(strlen(arg1.c_str()) >= 4){
        string last4 = arg1.substr(strlen(arg1.c_str())-4,string::npos);
        if(last4 == ".bez"){
            readFile = arg1;
        }
        else{
            cout << "input file not in .bez format" << endl;
            exit(0);
        }
    }

    double possibleStep = atof(string(arg2).c_str());
    if(argc == 3){ // uniform
        step = possibleStep;

    }
    else if(argc > 3 && string(argv[3]) == "-a"){
        if(possibleStep <= 0.02){
            possibleStep = 0.021;
        }
        errorBound = possibleStep;
        tessellationStrat = 1;
    }
    else {
        cout << "Unrecognized argument. Please review usage." << endl;
        exit(0);
    }
    //start parsing readFile
    ifstream myFile;
    myFile.open(readFile);
    if(readFile == ""){
        cout << "No input provided. Please review usage." << endl;
        exit(0);
    }
    else{
        int lineNumber= 1;
        vector<int> patchNum; // when == 4, parse current set of patches into surfaces
        patchNum.push_back(0);
        double patchOne[4][3];
        double patchTwo[4][3];
        double patchThree[4][3];
        double patchFour[4][3];
        cout << "Parsing BEZ file" << endl;
        std::string str;
        string to_add = " ";
        std::vector<std::string> vec, result_vec;
        while (std::getline(myFile, str)) {
            vec.push_back(str);
        }
        for(std::vector<string>::iterator it = vec.begin(); it != vec.end(); ++it) {
            string s = *it;
            s.insert(s.length(),1, to_add[0]);
            s.erase(std::remove(s.begin(), s.end(), '\r'), s.end());
            result_vec.push_back(s);
        }
        std::ofstream out_file(readFile);
        std::copy(result_vec.begin(), result_vec.end(), std::ostream_iterator<std::string>(out_file, "\n"));
        out_file.clear();
        out_file.close();

        myFile.clear();
        myFile.seekg(0, ios::beg);

        while (!myFile.eof()){
            char buf[MAX_CHARS_PER_LINE];
            myFile.getline(buf, MAX_CHARS_PER_LINE);
            const char* token[MAX_TOKENS_PER_LINE] = {}; 
            token[0] = strtok(buf, DELIMITER); // first token

            if (token[0]){
                int length = 0;
                for (int n = 1; n < MAX_TOKENS_PER_LINE; n++) {
                    token[n] = strtok(0,DELIMITER);
                    length +=1;
                    if (!token[n]){
                        break;
                    }
                }

                string first = string(token[0]).c_str();
                //cout << "line, patch, second #: " << lineNumber << "," << patchNum[0] << "," << first << "END"<< endl;
                if(lineNumber == 1){
                    numSurfaces = atof(string(token[0]).c_str());
                }
                else{
                    double totalPatch[4][3];
                    totalPatch[0][0] = atof(string(token[0]).c_str());
                    totalPatch[0][1] = atof(string(token[1]).c_str());
                    totalPatch[0][2] = atof(string(token[2]).c_str());

                    totalPatch[1][0] = atof(string(token[3]).c_str());
                    totalPatch[1][1] = atof(string(token[4]).c_str());
                    totalPatch[1][2] = atof(string(token[5]).c_str());

                    totalPatch[2][0] = atof(string(token[6]).c_str());
                    totalPatch[2][1] = atof(string(token[7]).c_str());
                    totalPatch[2][2] = atof(string(token[8]).c_str());

                    totalPatch[3][0] = atof(string(token[9]).c_str());
                    totalPatch[3][1] = atof(string(token[10]).c_str());
                    totalPatch[3][2] = atof(string(token[11]).c_str());
                    if(patchNum[0] == 0){
                        for(int i = 0; i < 4; i ++){
                            for (int j = 0; j < 3; j++){
                                patchOne[i][j] =  totalPatch[i][j];
                            }
                        }
                    }
                    else if(patchNum[0] == 1){
                        for(int i = 0; i < 4; i ++){
                            for (int j = 0; j < 3; j++){
                                patchTwo[i][j] =  totalPatch[i][j];
                            }
                        }                    
                    }
                    else if(patchNum[0] == 2){
                        for(int i = 0; i < 4; i ++){
                            for (int j = 0; j < 3; j++){
                                patchThree[i][j] =  totalPatch[i][j];
                            }
                        }
                    }
                    else if(patchNum[0] == 3){
                        for(int i = 0; i < 4; i ++){
                            for (int j = 0; j < 3; j++){

                                patchFour[i][j] =  totalPatch[i][j];
                            }
                        }
                    }
                    patchNum[0]+=1;
                    if(patchNum[0] >=4){ // CALCULATE SURFACE
                        Surface sur = Surface(patchOne,patchTwo,patchThree,patchFour);
                        surfaces.push_back(sur);
                        //curSurface.clear();
                        patchNum[0] = 0;
                    }

                }
                lineNumber+=1;
            } // end of if(token[0])
        } // end of while(!myFile.eof())
        if (!tessellationStrat) {
            cout << "UNIFORM TESSELLATION" << endl;
            for (int i=0; i<surfaces.size(); i++) {
                tessellate(surfaces[i]);
            }
        } else {
            cout << "ADAPTIVE TESSELLATION" << endl;
            int steps = (int)(1/step);
            for (int s=0; s<surfaces.size(); s++) {
                for (int vb=0; vb<steps; vb++) {
                    double v = (double)(vb*step);
                    double vend = v+step;
                    for (int ub=0; ub<steps; ub++) {
                        double u = (double)(ub*step);
                        adaptTessellate(surfaces[s], u, v, u+step, vend);
                    }
                }
                //Cleanup if necessary.
                if (1-(steps*step) > .0000001) {
                    double v = (double)(steps*step);
                    for (int ub=0; ub<steps; ub++) {
                        double u = (double)(ub*step);
                        adaptTessellate(surfaces[s], u, v, u+step, 1);
                    }
                    double u = (double)(steps*step);
                    for (int vb=0; vb<steps; vb++) {
                        v = (double)(vb*step);
                        adaptTessellate(surfaces[s], u, v, 1, v+step);
                    }
                    v = steps*step;
                    adaptTessellate(surfaces[s], u, v, 1, 1);   
                }
            }
        }
        findCenterPoint(currID);
        surfaces.clear();

    } // end of parsing 
    cout << "End of parsing" << endl;
    //This initializes glut
    glutInit(&argc, argv);
    //This tells glut to use a double-buffered window with red, green, and blue channels 

    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);

    // Initalize theviewport size
    viewport.w = 400;
    viewport.h = 400;

    //The size and position of the window
    glutInitWindowSize(viewport.w, viewport.h);
    glutInitWindowPosition(0,0);
    glutCreateWindow(argv[0]);

    initScene();                            // quick function to set up scene

    glutDisplayFunc(myDisplay);             // function to run when its time to draw something
    glutReshapeFunc(myReshape);             // function to run when the window gets resized
    glutKeyboardFunc(myKey);
    glutSpecialFunc(specialKey);
    glEnable(GL_DEPTH_TEST | GL_LIGHTING);
    glDepthFunc(GL_LEQUAL);
    glutMainLoop();                         // infinite loop that will keep drawing and resizing
    // and whatever else


    return 0;
}











