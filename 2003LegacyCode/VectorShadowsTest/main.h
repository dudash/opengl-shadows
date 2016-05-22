#ifndef MAIN_H
#define MAIN_H
 
// define global data
//-----------------------------------------------------------------------------
static const int WINDOW_WIDTH = 500;
static const int WINDOW_HEIGHT = 500;

enum MenuCommands
{
   TOGGLE_LIGHTING = 1,
   TOGGLE_LIGHT0,
   MENU_SHADOWS_TOGGLE,
   MENU_EXIT,
   TOGGLE_OBJECT_ROTATION,
   TOGGLE_LIGHT_LINE,
   DO_NOTHING
};

bool shadowsOn = true;
bool lightingOn = false;
bool light0On = false;
bool lightLineOn = false;
GLfloat lightPosition0[4] = { 10.0, 20.0, 6.0, 1.0 };
GLfloat objectPosition[4] = { 20.0, 5.0, 3.0, 0.0 };

// free mode variables to hold the current camera data
GLfloat cameraRotationX;
GLfloat cameraRotationY;
GLfloat cameraRotationZ;
GLfloat cameraX;
GLfloat cameraY;
GLfloat cameraZ;
GLfloat lookX;
GLfloat lookY;
GLfloat lookZ;
GLfloat upX;
GLfloat upY;
GLfloat upZ;

float tetraTurn = 0;
float rotationSpeed = 0.5;
bool objectRotating = true;

GLfloat floorVerticies[4][3] =
{
   {-5.0,0.0,150.0},
   {25.0,0.0,150.0},
   {25.0,0.0,-150.0},
   {-5.0,0.0,-150.0}
};

GLfloat wallVerticies[4][3] =
{
   {25.0,0.0,-150.0},
   {25.0,0.0,150.0},
   {25.0,30.0,150.0},
   {25.0,30.0,-150.0}
};

GLfloat* floorPlane;
GLfloat* wallPlane;
static GLfloat shadowMatrix[4][4];

// declare our functions
//-----------------------------------------------------------------------------
// initalize drawing related variables
void initialize();

// load the menus and setup their callbacks
void initMenuSystem();

// callback for when openGL decides to redraw the window
void handleDisplay();

// callback for when the window is resized
void handleReshape(int width, int height);

// callback for when a main menu item has been selected
void handleMainMenuInput(int input);

// callback for when a mouse event occurs
void handleMouse(int button, int state, int x, int y);

// callback for when a key has been pressed
void handleKeyboardInput(unsigned char key, int x, int y);

// callback to handle idle actions
void handleIdle();

// set the camera properties
void setCamera();

// draws a couple of walls to use as a backdrop
void drawBackdropGeometry();

// draws the objects in the scene
void drawScene();

// show where the light source is
void drawLightSource();

// claculate the shadows
void calculateShadows(GLfloat* projectionPlane);

// draw the shadows
void drawShadows();

// draw the shadows
GLfloat* calculatePlaneFromPoints(GLfloat* p0, GLfloat* p1, GLfloat* p2);

#endif