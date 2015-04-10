
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
inline double distance(vector<double> a, vector<double> b) { return pow(pow(a[0]-b[0], 2)+pow(a[1]-b[1], 2)+pow(a[2]-b[2], 2), .5);}

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
Viewport	viewport;
int numSurfaces;
vector<Surface> surfaces;
vector<Polygon> polygons;
int tessellationStrat = 0;
int currID = 0;
double step;
bool flatShading = false; // if false, do smooth shading. if true, do flat shading
bool wireFrame = false; // if false, do filled. if true, do wireframe
//bool shiftDown = false; // if shiftKey down
double increment = 0.1;



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

  glClear(GL_COLOR_BUFFER_BIT);				// clear the color buffer

  glMatrixMode(GL_MODELVIEW);			        // indicate we are specifying camera transformations
  glLoadIdentity();				        // make sure transformation is "zero'd"

  /*
  * CALL THE FUNCTIONS THAT DO BEZIER STUFF HERE
  */


  glFlush();
  glutSwapBuffers();					// swap buffers (we earlier set double buffer)
}

void transformPolygons(matrix m){
    for(std::vector<Polygon>::iterator poly = polygons.begin(); poly != polygons.end(); ++poly) {
        Polygon polygon = *poly;
        vector<Vector4> newVertices;
        for(std::vector<Vector4>::iterator vert = polygon.vertices.begin(); vert != polygon.vertices.end(); ++vert){
            Vector4 vertex = *vert;
            Vector4 newVertex = m.multiplyv(vertex);
            newVertices.push_back(newVertex);
        }

    }

}

void myKey(unsigned char key, int x, int y) {
  if(key==32) {
    exit(0);
  }
  
  if(key == 115){ // 's' toggle between flat and smooth
      //cout << flatShading << endl;
      if(flatShading == true){
        flatShading = false;
      }
      else{
        flatShading = true;
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
        //cout << "shift and up" << endl;
        m = matrix(0,increment, 0 , 0);
    }
    else if(key == GLUT_KEY_UP){
        //cout << "up" << endl;
        m = matrix(0,increment,0,2);
    }
    if(glutGetModifiers() == GLUT_ACTIVE_SHIFT && key == GLUT_KEY_RIGHT){ 
        //cout << "shift and right" << endl;
        m = matrix(increment,0, 0 , 0);
    }
    else if(key == GLUT_KEY_RIGHT){
        //cout << "right" << endl;
        m = matrix(increment,0,0,2);
    }

    if(glutGetModifiers() == GLUT_ACTIVE_SHIFT && key == GLUT_KEY_DOWN){
        m = matrix(0,-increment, 0 , 0);
    }
    else if(key == GLUT_KEY_DOWN){
        m = matrix(0,-increment,0,2);
    }

    if(glutGetModifiers() == GLUT_ACTIVE_SHIFT && key == GLUT_KEY_LEFT){
        m = matrix(-increment,0, 0 , 0);
    }
    else if(key == GLUT_KEY_LEFT){
        m = matrix(-increment,0,0,2);
    }
    transformPolygons(m);
    
}


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
    polygons.push_back(Polygon(poly, currID));
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
      polygons.push_back(Polygon(poly, currID));
    } else {
      double halfStep = step/2;
      tessellate(s, halfStep, u, v);
      tessellate(s, halfStep, u+halfStep, v);
      tessellate(s, halfStep, u+halfStep, v+halfStep);
      tessellate(s, halfStep, u, v+halfStep);
    }
  } 
  return;
}

//****************************************************
// the usual stuff, nothing exciting here
//****************************************************
int main(int argc, char *argv[]) {
  
    /*
    * INSERT PARSER HERE
    */
  /*
  if (argc == 1) {
    cout << "No input file specified.";
    exit(0);
  }
  for (int i=1; i<argc; i++) {  
    for (int i=0; i<surfaces.size(); i++) {
      for (double v=0; v<1; v+=step) {
        for (double u=0; u<1; u+=step) {
          tessellate(surfaces[i], step, 0, 0);
        }
      }
    }*/


  










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

  initScene();							// quick function to set up scene

  glutDisplayFunc(myDisplay);				// function to run when its time to draw something
  glutReshapeFunc(myReshape);				// function to run when the window gets resized
  glutKeyboardFunc(myKey);
  glutSpecialFunc(specialKey);
  glutMainLoop();							// infinite loop that will keep drawing and resizing
  // and whatever else

  return 0;
  }











