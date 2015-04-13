
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

#include "bezier_util.h"

#define PI 3.14159265  // Should be used from mathlib
inline float sqr(float x) { return x*x; }
inline float min(float x, float y) {if (x < y) { return x;} else {return y;}}
inline bool distance(double x1, double y1, double z1, vector<double> coords) {if (errorBound > pow(pow(x1-coords[0], 2)+pow(y1-coords[1], 2)+pow(z1-coords[2]), 2)) 
                                                                                          { return true;} else { return false}}
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
int numSurfaces;
vector<Surface> surfaces;
vector<Polygon*> polygons;
int tessellationStrat = 0;
int currID = 0;
double step;
bool flatShading = false; // if false, do smooth shading. if true, do flat shading
bool wireFrame = false; // if false, do filled. if true, do wireframe
//bool shiftDown = false; // if shiftKey down
double increment = 0.1;
vector<double> centerPoint;


//****************************************************
// Simple init function
//****************************************************
void initScene(){


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
    gluOrtho2D(0, viewport.w, 0, viewport.h);

}


//****************************************************
// A routine to set a pixel by drawing a GL point.  This is not a
// general purpose routine as it assumes a lot of stuff specific to
// this example.
//****************************************************

void setPixel(int x, int y, GLfloat r, GLfloat g, GLfloat b) {
    glColor3f(r, g, b);
    glVertex2f(x + 0.5, y + 0.5);   // The 0.5 is to target pixel
}




//****************************************************
// function that does the actual drawing of stuff
//***************************************************
void myDisplay() {

    glClear(GL_COLOR_BUFFER_BIT);               // clear the color buffer

    glMatrixMode(GL_MODELVIEW);                 // indicate we are specifying camera transformations
    glLoadIdentity();                       // make sure transformation is "zero'd"

    /*
     * CALL THE FUNCTIONS THAT DO BEZIER STUFF HERE
     */


    glFlush();
    glutSwapBuffers();                  // swap buffers (we earlier set double buffer)
}

void transformPolygons(matrix m){
    for(std::vector<Polygon*>::iterator poly = polygons.begin(); poly != polygons.end(); ++poly) {
        Polygon polygon = **poly;
        vector<Vector4> newVertices;
        for(std::vector<Vector4>::iterator vert = polygon.vertices.begin(); vert != polygon.vertices.end(); ++vert){
            Vector4 vertex = *vert;
            Vector4 newVertex = m.multiplyv(vertex);
            newVertices.push_back(newVertex);
        }

    }

}

void findCenterPoint(int idCheck){
    vector<double> center;
    int iterationCount = 1;
    for(std::vector<Polygon*>::iterator poly = polygons.begin(); poly != polygons.end(); ++poly) {
        Polygon polygon = **poly;
        if(polygon.id == idCheck){
            for(std::vector<Vector4>::iterator vert = polygon.vertices.begin(); vert != polygon.vertices.end(); ++vert){
                Vector4 vertex = *vert; 
                if(iterationCount == 1){ // first iteration
                    center.push_back(vertex.xc());
                    center.push_back(vertex.yc());
                    center.push_back(vertex.zc());
                }
                else{
                    center[0] = center[0] + vertex.xc();
                    center[1] = center[1] + vertex.yc();
                    center[2] = center[2] + vertex.zc();
                }
                iterationCount = iterationCount+1;
            }
        }

    }
    center[0] = center[0] / (polygons.size()*4); // divide by # vertices
    center[1] = center[1] / (polygons.size()*4);
    center[2] = center[2] / (polygons.size()*4);

    centerPoint = center;
}

void myKey(unsigned char key, int x, int y) {
    if(key==32) {
        exit(0);
    }

    if(key == 115){ // 's' toggle between flat and smooth
        //cout << flatShading << endl;
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
    if(key == 43){ // '+' zoom in
        m = matrix(0,0,-increment,0);
    }
    if(key == 45){ // '-' zoom out
        m = matrix(0,0,increment,0);
    }
    transformPolygons(m);
}

void specialKey(int key, int x, int y){
    matrix m;
    if(glutGetModifiers() == GLUT_ACTIVE_SHIFT && key == GLUT_KEY_UP){
        m = matrix(0,increment, 0 , 0);
    }
    else if(key == GLUT_KEY_UP){

        m = matrix(-centerPoint[0],-centerPoint[1],-centerPoint[2],0);
        m.multiplym(matrix(0,increment,0,2));
        m.multiplym(matrix(centerPoint[0],centerPoint[1],centerPoint[2],0));
    }
    if(glutGetModifiers() == GLUT_ACTIVE_SHIFT && key == GLUT_KEY_RIGHT){ 
        m = matrix(increment,0, 0 , 0);
    }
    else if(key == GLUT_KEY_RIGHT){
        m = matrix(-centerPoint[0],-centerPoint[1],-centerPoint[2],0);
        m.multiplym(matrix(increment,0,0,2));
        m.multiplym(matrix(centerPoint[0],centerPoint[1],centerPoint[2],0));

    }

    if(glutGetModifiers() == GLUT_ACTIVE_SHIFT && key == GLUT_KEY_DOWN){
        m = matrix(0,-increment, 0 , 0);
    }
    else if(key == GLUT_KEY_DOWN){
        m = matrix(-centerPoint[0],-centerPoint[1],-centerPoint[2],0);
        m.multiplym(matrix(0,-increment,0,2));
        m.multiplym(matrix(centerPoint[0],centerPoint[1],centerPoint[2],0));

    }

    if(glutGetModifiers() == GLUT_ACTIVE_SHIFT && key == GLUT_KEY_LEFT){
        m = matrix(-increment,0, 0 , 0);
    }
    else if(key == GLUT_KEY_LEFT){
        m = matrix(-centerPoint[0],-centerPoint[1],-centerPoint[2],0);
        m.multiplym(matrix(-increment,0,0,2));
        m.multiplym(matrix(centerPoint[0],centerPoint[1],centerPoint[2],0));

    }
    transformPolygons(m);    
}

void adaptRecurse(Surface s, vector<vector<double> > realcoords, vector<vector<double> > uvcoords) {
  bool e1 = distance((realcoords[0][0]+realcoords[1][0])/2, (realcoords[0][1]+realcoords[1][1])/2, 
                     (realcoords[0][2]+realcoords[1][2])/2, s.getSurfacePoint((uvcoords[0][0]+uvcoords[1][0])/2, (uvcoords[0][1]+uvcoords[1][1])/2));
  bool e2 = distance((realcoords[1][0]+realcoords[2][0])/2, (realcoords[1][1]+realcoords[2][1])/2, 
                     (realcoords[1][2]+realcoords[2][2])/2, s.getSurfacePoint((uvcoords[1][0]+uvcoords[2][0])/2, (uvcoords[1][1]+uvcoords[2][1])/2));
  bool e3 = distance((realcoords[2][0]+realcoords[0][0])/2, (realcoords[2][1]+realcoords[0][1])/2, 
                     (realcoords[2][2]+realcoords[0][2])/2, s.getSurfacePoint((uvcoords[2][0]+uvcoords[0][0])/2, (uvcoords[2][1]+uvcoords[0][1])/2));
  if (e1 && e2 && e3) {
    Polygon * newPoly = Polygon(realcoords, currID);
    polygons.push_back(newPoly);
    return;
  } else if (!e1 && e2 && e3) {
    vector<double> newpoint1 = s.getSurfacePoint((uvcoords[0][0]+uvcoords[1][0])/2, (uvcoords[0][1]+uvcoords[1][1])/2);
    double insert1[] = {(uvcoords[0][0]+uvcoords[1][0])/2, (uvcoords[0][1]+uvcoords[1][1])/2};
    vector<double> newpt1 (insert1, insert1 + sizeof(insert1) / sizeof(double));
    vector<vector<double> > trgl1;
    vector<vector<double> > uv1;    
    vector<vector<double> > trgl2;
    vector<vector<double> > uv2;
    trgl1.push_back(newpoint1);
    trgl1.push_back(realcoords[2]);
    trgl1.push_back(realcoords[0]);
    uv1.push_back(newpt1);
    uv1.push_back(uvcoords[2]);
    uv1.push_back(uvcoords[0]);
    trgl2.push_back(newpoint1);
    trgl2.push_back(realcoords[1]);
    trgl2.push_back(realcoords[2]);
    uv2.push_back(newpt1);
    uv2.push_back()

  } else if (e1 && !e2 && e3) {

  } else if (e1 && e2 && !e3) {

  } else if (!e1 && !e2 && e3) {

  } else if (e1 && !e2 && !e3) {

  } else if (!e1 && e2 && !e3) {

  } else {

  }
}

void adaptTessellate(Surface s, double u, double v) {
  vector<double> point1 = s.getSurfacePoint(u, v);
  vector<double> pt1uv;
  pt1uv.push_back(u);
  pt1uv.push_back(v);
  vector<double> point2 = s.getSurfacePoint(u+step, v);
  vector<double> pt2uv;
  pt2uv.push_back(u+step);
  pt2uv.push_back(v);
  vector<double> point3 = s.getSurfacePoint(u+step, v+step);
  vector<double> pt3uv;
  pt1uv.push_back(u+step);
  pt1uv.push_back(v+step);
  vector<double> point4 = s.getSurfacePoint(u, v+step);
  vector<double> pt4uv;
  pt4uv.push_back(u);
  pt4uv.push_back(v+step);
  vector<vector<double> > trgl1;
  vector<vector<double> > uv1;
  vector<vector<double> > trgl2;
  vector<vector<double> > uv2;
  trgl.push_back(point1);
  trgl.push_back(point2);
  trgl.push_back(point3);
  uv1.push_back(pt1uv);
  uv1.push_back(pt2uv);
  uv1.push_back(pt3uv);
  trgl2.push_back(point3);
  trgl2.push_back(point4);
  trgl2.push_back(point1);
  uv2.push_back(pt3uv);
  uv2.push_back(pt4uv);
  uv2.push_back(pt1uv);
  adaptRecurse(s, trgl1, uv1);
  adaptRecurse(s, trgl2, uv2);
}

<<<<<<< HEAD
void tessellate(Surface s, double step, double u, double v) {
    vector<double> point1 = s.getSurfacePoint(u, v);
    vector<double> point2 = s.getSurfacePoint(u+step, v);
    vector<double> point3 = s.getSurfacePoint(u+step, v+step);
    vector<double> point4 = s.getSurfacePoint(u, v+step);
    if (!tessellationStrat) {
        vector<vector<double> > poly;
        poly.push_back(point1);
        poly.push_back(point2);
        poly.push_back(point3);
        poly.push_back(point4);
        Polygon toPush = Polygon(poly,currID);
        polygons.push_back(&toPush);
    } else {
        vector<double> actual = s.getSurfacePoint((4*u+2*step)/4, (4*v+2*step)/4);
        vector<double> current;
        for (int j=0; j<3; j++) {
            current.push_back((point1[j]+point2[j]+point3[j]+point4[j])/4);
        }
        if (distance(current, actual) < step) {
            vector<vector<double> > poly;
            poly.push_back(point1);
            poly.push_back(point2);
            poly.push_back(point3);
            poly.push_back(point4);
            Polygon toPush = Polygon(poly,currID);
            polygons.push_back(&toPush);
        } else {
            double halfStep = step/2;
            tessellate(s, halfStep, u, v);
            tessellate(s, halfStep, u+halfStep, v);
            tessellate(s, halfStep, u+halfStep, v+halfStep);
            tessellate(s, halfStep, u, v+halfStep);
        }
    } 
    return;
=======
void tessellate(Surface s) {
  /*
  * Perform uniform tessellation on Surface s. Step is specified as a global variable.
  */
  int steps = (int)(1/step);
  for (int vb=0; vb<steps; vb++) {
    double v = (double)(vb*step);
    for (int ub=0; ub<steps; ub++) {
      double u = (double)(ub*step);
      vector<double> point1 = s.getSurfacePoint(u, v);
      vector<double> point2 = s.getSurfacePoint(u+step, v);
      vector<double> point3 = s.getSurfacePoint(u+step, v+step);
      vector<double> point4 = s.getSurfacePoint(u, v+step);
      vector<vector<double> > poly;
      poly.push_back(point1);
      poly.push_back(point2);
      poly.push_back(point3);
      poly.push_back(point4);
      Polygon * newPoly = Polygon(poly, currID);
      polygons.push_back(newPoly);
    }
  }
>>>>>>> 7650d65af33dc35eb86e10a49970c4fb1b2fd88b
}

//****************************************************
// the usual stuff, nothing exciting here
//****************************************************
int main(int argc, char *argv[]) {

    /*
     * INSERT PARSER HERE
     */
    if (argc == 1) {
       cout << "No input file specified.";
       exit(0);
    }
    if (!tessellationStrat) {
        for (int i=0; i<surfaces.size(); i++) {
               tessellate(surfaces[i]);
        }
    } else {
        int steps = (int)(1/step);
        for (int s=0; s<surfaces.size(); i++) {
            for (int vb=0; vb<steps; vb++) {
                double v = (double)(ub*step);
                for (int ub=0; ub<steps; ub++) {
                    double u = (double)(ub*step);
                    adaptTessellate(s, u, v);
                }
            }
        }
    }













    //This initializes glut
    glutInit(&argc, argv);
    //This tells glut to use a double-buffered window with red, green, and blue channels 
    std::cout << "Have " << argc << " arguments:" << std::endl;

    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);

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
    glutMainLoop();                         // infinite loop that will keep drawing and resizing
    // and whatever else

    return 0;
}











