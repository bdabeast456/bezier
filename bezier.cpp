
#include <string.h>
#include <vector>
#include <iostream>
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
int a, d, s, toon, invert, greyscale, av;
float ka[3];

float kd[3];

float ks[3];

float sp;

int pl_num = 0;
float plrgb[5][3];
float plxyz[5][3];

int dl_num = 0;
float dlrgb[5][3];
float dlxyz[5][3];
//Viewer location
float v[3] = {0, 0, 1};




//****************************************************
// Simple init function
//****************************************************
void initScene(){

  // Nothing to do here for this simple example.

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
  // centers 
  // Note: Need to check for gap
  // bug on inst machines.
}

//****************************************************
// Draw a filled circle.  
//****************************************************


void diffu(float lv[], float nv[], float diff[], float rgb[]) {
  float l_n = max(0.0f, lv[0]*nv[0] + lv[1]*nv[1] + lv[2]*nv[2]);
  diff[0] = diff[0] + kd[0]*rgb[0]*l_n;
  diff[1] = diff[1] + kd[1]*rgb[1]*l_n;
  diff[2] = diff[2] + kd[2]*rgb[2]*l_n; 
}

void ambien(float rgb[], float ambi[]) {
  ambi[0] = ambi[0] + ka[0] * rgb[0];
  ambi[1] = ambi[1] + ka[1] * rgb[1]; 
  ambi[2] = ambi[2] + ka[2] * rgb[2];
}

void specul(float lv[], float nv[], float spec[], float rgb[]) {
  float l_n = lv[0]*nv[0] + lv[1]*nv[1] + lv[2]*nv[2];              
  float new_normal[] = {2*l_n*nv[0], 2*l_n*nv[1], 2*l_n*nv[2]};
  float r[] = {new_normal[0] - lv[0], new_normal[1]-lv[1], new_normal[2]-lv[2]};
  float r_v = pow(max(0.0f, r[0]*v[0] + r[1]*v[1] + r[2]*v[2]),sp);
  spec[0] = spec[0] + ks[0]*rgb[0]*r_v;
  spec[1] = spec[1] + ks[1]*rgb[1]*r_v;
  spec[2] = spec[2] + ks[2]*rgb[2]*r_v;
}

void circle(float centerX, float centerY, float radius) {
  // Draw inner circle

  glBegin(GL_POINTS);
  // We could eliminate wasted work by only looping over the pixels
  // inside the sphere's radius.  But the example is more clear this
  // way.  In general drawing an object by loopig over the whole
  // screen is wasteful.

  int i,j;  // Pixel indices

  int minI = max(0,(int)floor(centerX-radius));
  int maxI = min(viewport.w-1,(int)ceil(centerX+radius));

  int minJ = max(0,(int)floor(centerY-radius));
  int maxJ = min(viewport.h-1,(int)ceil(centerY+radius));

  for (i=0;i<viewport.w;i++) {
    for (j=0;j<viewport.h;j++) {

      // Location of the center of pixel relative to center of sphere
      float x = (i+0.5-centerX);
      float y = (j+0.5-centerY);
      
      float dist = sqrt(sqr(x) + sqr(y));

      if (dist<=radius) {
        if (pl_num == 0 && dl_num == 0){
            setPixel(i,j, 0.0, 0.0, 0.0);
            continue;
        }
        // This is the front-facing Z coordinate
        float z = sqrt(radius*radius-dist*dist);
        float normal[] = {x/sqrt(sqr(x)+sqr(y)+sqr(z)),y/sqrt(sqr(x)+sqr(y)+sqr(z)),z/sqrt(sqr(x)+sqr(y)+sqr(z))};
        float diffuse[3] = {0,0,0};
        float ambient[3] = {0,0,0};
        float specular[3] = {0,0,0};
        //Diffuse
        for(int c = 0; c < pl_num && d == 1; c++){
            //Point Lights
            float pointCoord1[] = {plxyz[c][0]*radius - x,plxyz[c][1]*radius - y, plxyz[c][2]*radius-z};
            float normalizeFactor = sqrt(sqr(pointCoord1[0])+sqr(pointCoord1[1])+sqr(pointCoord1[2]));
            float plv[3] = {pointCoord1[0]/normalizeFactor,pointCoord1[1]/normalizeFactor, pointCoord1[2]/normalizeFactor};
            diffu(plv, normal, diffuse, plrgb[c]);
        }
        for(int c = 0; c < dl_num && d == 1; c++){
            //Direction Lights
            float normalizeFactor = sqrt(sqr(dlxyz[c][0]) + sqr(dlxyz[c][1]) + sqr(dlxyz[c][2]));
            float dl[3] = {-dlxyz[c][0]/normalizeFactor, -dlxyz[c][1]/normalizeFactor, -dlxyz[c][2]/normalizeFactor};
            diffu(dl, normal, diffuse, dlrgb[c]);
        }
        //Ambient
        for(int c = 0; c < pl_num && a == 1; c++){
            //Point Lights
            ambien(plrgb[c], ambient);
        }
        for(int c = 0; c < dl_num && a == 1; c++){
            //Direction Lights
            ambien(dlrgb[c], ambient);
        }
        //Specular
        for(int c = 0; c < pl_num && s == 1; c++){
            //Point Lights
            float pointCoord1[] = {plxyz[c][0]*radius - x,plxyz[c][1]*radius - y, plxyz[c][2]*radius-z};
            float normalizeFactor = sqrt(sqr(pointCoord1[0])+sqr(pointCoord1[1])+sqr(pointCoord1[2]));
            float plv[] = {pointCoord1[0]/normalizeFactor,pointCoord1[1]/normalizeFactor, pointCoord1[2]/normalizeFactor};
            specul(plv, normal, specular, plrgb[c]);

        }
        for(int c = 0; c < dl_num && s == 1; c++){
            //Direction Lights
            float normalizeFactor = sqrt(sqr(dlxyz[c][0]) + sqr(dlxyz[c][1]) + sqr(dlxyz[c][2]));
            float dl[3] = {-dlxyz[c][0]/normalizeFactor, -dlxyz[c][1]/normalizeFactor, -dlxyz[c][2]/normalizeFactor};
            specul(dl, normal, specular, dlrgb[c]);
        }
        float r = min(diffuse[0] + ambient[0] + specular[0], 1.0f);
        float g = min(diffuse[1] + ambient[1] + specular[1], 1.0f);
        float b = min(diffuse[2] + ambient[2] + specular[2], 1.0f);
        float rgb[] = {r,g,b};
        //Check if -toon flag is present
        if(toon){
           for(int i = 0; i < 3; i++){
                if(rgb[i] == 0.0f) {
                    rgb[i] = 0.0f;
                }
                else if (rgb[i] <= 0.4f) {
                    rgb[i] = 0.4f;
                }
                else if (rgb[i] <= 0.9f) {
                    rgb[i] = 0.8f;
                }
                else{ 
                    rgb[i] = 1.0f;
                }
           }
        }
        //Check if -invert flag is present
        if (invert) {
          rgb[0] = 1.0f - rgb[0];
          rgb[1] = 1.0f - rgb[1];
          rgb[2] = 1.0f - rgb[2];
        }
        if (greyscale) {
          if (av) {
            float average = (rgb[0] + rgb[1] + rgb[2])/3;
            rgb[0] = average;
            rgb[1] = average;
            rgb[2] = average;
          }
          else {
            rgb[0] = max(rgb[0], max(rgb[1], rgb[2]));
            rgb[1] = max(rgb[0], max(rgb[1], rgb[2]));
            rgb[2] = max(rgb[0], max(rgb[1], rgb[2]));
          }
        }
        setPixel(i,j, rgb[0], rgb[1], rgb[2]);

        // This is amusing, but it assumes negative color values are treated reasonably.
        // setPixel(i,j, x/radius, y/radius, z/radius );
      }


    }
  }


  glEnd();
}

void myKey(unsigned char key, int x, int y) {
  if(key==32) {
    exit(0);
  }
}

//****************************************************
// function that does the actual drawing of stuff
//***************************************************
void myDisplay() {

  glClear(GL_COLOR_BUFFER_BIT);				// clear the color buffer

  glMatrixMode(GL_MODELVIEW);			        // indicate we are specifying camera transformations
  glLoadIdentity();				        // make sure transformation is "zero'd"


  // Start drawing
  circle(viewport.w / 2.0 , viewport.h / 2.0 , min(viewport.w, viewport.h) / 3.0);

  glFlush();
  glutSwapBuffers();					// swap buffers (we earlier set double buffer)
}



//****************************************************
// the usual stuff, nothing exciting here
//****************************************************
int main(int argc, char *argv[]) {
  //This initializes glut
  glutInit(&argc, argv);
  //cout << *argv;
  //This tells glut to use a double-buffered window with red, green, and blue channels 
  std::cout << "Have " << argc << " arguments:" << std::endl;
  for (int i = 0; i < argc; i++) {
    std::cout << argv[i] << std::endl;
  }
    for (int i = 0; i < argc; i++) {
        if (i==0) {
          continue;
        }
        string arg = string(argv[i]);
        if (arg=="-sp") {
          sp = atof(string(argv[i+1]).c_str());
          if (sp < 0.0f) {
            std::cout << "Sp value not between 0 and max_float" << std::endl;
            exit(0);
          }
          i += 1;
          continue;
        }
        if (arg=="-ka") {
          ka[0] = atof(string(argv[i+1]).c_str());
          ka[1] = atof(string(argv[i+2]).c_str());
          ka[2] = atof(string(argv[i+3]).c_str());
          for (int j=0; j<3; j++) {
            if (ka[j] > 1 || ka[j] < 0) {
              std::cout << "Ka value not between 0 and 1" << std::endl;
              exit(0);
            }
          }
          a = 1;
          i += 3;
          continue;
        }
        if (arg=="-kd") {
          kd[0] = atof(string(argv[i+1]).c_str());
          kd[1] = atof(string(argv[i+2]).c_str());
          kd[2] = atof(string(argv[i+3]).c_str());
          for (int j=0; j<3; j++) {
            if (kd[j] > 1 || kd[j] < 0) {
              std::cout << "Kd value not between 0 and 1" << std::endl;
              exit(0);
            }
          }
          d = 1;
          i += 3;
          continue;
        }
        if (arg=="-ks") {
          ks[0] = atof(string(argv[i+1]).c_str());
          ks[1] = atof(string(argv[i+2]).c_str());
          ks[2] = atof(string(argv[i+3]).c_str());
          for (int j=0; j<3; j++) {
            if (ks[j] > 1 || ks[j] < 0) {
              std::cout << "Ks value not between 0 and 1" << std::endl;
              exit(0);
            }
          }
          s = 1;
          for (int j = 0; j < argc; j++) {
            if (string(argv[j])=="-sp") {
              break;
            }
            if (j==(argc-1)) {
              std::cout << "Did not specify sp value with -sp v" << std::endl;
              exit(0);
            }
          }
          i += 3;
          continue;
        }
        if (arg=="-pl") {
          plxyz[pl_num][0] = atof(string(argv[i+1]).c_str());
          plxyz[pl_num][1] = atof(string(argv[i+2]).c_str());
          plxyz[pl_num][2] = atof(string(argv[i+3]).c_str());
          plrgb[pl_num][0] = atof(string(argv[i+4]).c_str());
          plrgb[pl_num][1] = atof(string(argv[i+5]).c_str());
          plrgb[pl_num][2] = atof(string(argv[i+6]).c_str());
          for (int j=0; j<3; j++) {
            if (plrgb[pl_num][j] < 0.0f) {
              std::cout << "Pl values not positive" << std::endl;
              exit(0);
            }
          }
          pl_num += 1;
          i += 6;
          continue;
        }
        if (arg=="-dl") {
          dlxyz[dl_num][0] = atof(string(argv[i+1]).c_str());
          dlxyz[dl_num][1] = atof(string(argv[i+2]).c_str());
          dlxyz[dl_num][2] = atof(string(argv[i+3]).c_str());
          dlrgb[dl_num][0] = atof(string(argv[i+4]).c_str());
          dlrgb[dl_num][1] = atof(string(argv[i+5]).c_str());
          dlrgb[dl_num][2] = atof(string(argv[i+6]).c_str());
          for (int j=0; j<3; j++) {
            if (dlrgb[dl_num][j] < 0.0f) {
              std::cout << "Dl values not positive" << std::endl;
              exit(0);
            }
          }
          dl_num += 1;
          i += 6;
          continue;
        }
        if (arg=="-toon") {
          toon = 1;
          continue;
        }
        if (arg=="-invert") {
          invert = 1;
          continue;
        }
        if (arg=="-greyscale") {
          av = atoi(string(argv[i+1]).c_str());
          if (!(av==0 || av==1)) {
            std::cout << "Greyscale mode incorrect: '" << av << "'. Please review usage" << std::endl;
            exit(0);
          }
          greyscale = 1;
          i += 1;
          continue;
        }
        else {
          std::cout << "Invalid syntax (general). '" << argv[i] << "' Please review usage" << std::endl;
          exit(0);
        }
    }
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
  glutMainLoop();							// infinite loop that will keep drawing and resizing
  // and whatever else

  return 0;
}









