
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
vector<Polygon> polygons;
bool flatShading = false; // if false, do smooth shading. if true, do flat shading
bool wireFrame = false; // if false, do filled. if true, do wireframe
bool shiftDown = false; // if shiftKey down


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

void myKey(unsigned char key, int x, int y) {
  if(key==32) {
    exit(0);
  }
  if(key == 115){ // 's' toggle between flat and smooth
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
      }
      else{
        wireFrame = true;
      }
 
  }
  if(key == 16){ // SHIFT is held down
      shiftDown = true;
  }
  else{ // shift ain't held down
      shiftDown = false;
  }

}

void specialKey(int key, int x, int y){
    if(key == GLUT_KEY_UP){
    }
    if(key == GLUT_KEY_RIGHT){
    }
    if(key == GLUT_KEY_DOWN){
    }
    if(key == GLUT_KEY_LEFT){
    }
}

//****************************************************
// the usual stuff, nothing exciting here
//****************************************************
int main(int argc, char *argv[]) {
  
    /*
    * INSERT PARSER HERE
    */

    
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










