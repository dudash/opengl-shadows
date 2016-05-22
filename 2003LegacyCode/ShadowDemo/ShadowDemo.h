#ifndef MAIN_H
#define MAIN_H

#include <gl/glaux.h>
#include <vector>
#include "Model3D.h"
#include <GL/glut.h>

namespace SML_APP
{
// define global data
//-----------------------------------------------------------------------------
static const int WINDOW_WIDTH = 600;
static const int WINDOW_HEIGHT = 600;
bool fullscreen = false;

/** textures that get loaded into memory */
enum DisplayListTextures
{
   GRASS_TEXTURE = 0,
   FIREBALL_TEXTURE,
   TANK_TEXTURE,
   EVIL_TANK_TEXTURE,
   TEXTURE_LIST_SIZE  //this must be the last element
};

/** Commands that can be taken in the GLUT menus */
enum MenuCommands
{
   MENU_EXIT = 1,
   MENU_TOGGLE_ATTACH_TO_TANK,
   MENU_TOGGLE_FULLSCREEN,
   MENU_TOGGLE_SHOWLIGHTS,
   MENU_SHADOWS_ON,
   MENU_SHADOWS_OFF,
   MENU_SIDE_CAM,
   MENU_HIGHUP_CAM,
   MENU_NONE
};

/** Each model must have it's own display list right now define them here */
enum DisplayLists
{
   CAR = 1,
   TEAPOT,
   GROUND,
   EVIL_TANK,
   TANK,
   FIREBALL
};

// data used to draw the map
UINT textureList[TEXTURE_LIST_SIZE];
float mapSizeX = 25.5;
float mapSizeZ = 25.5;

// kludge fireballs and tanks
std::vector<SML_CORE::Model3D*> fireballs;
int numFired = 0;
bool cheaterB=false;
bool cheaterE=false;
bool cheaterE2=false;
int currentTank = 1;

// declare our functions
//-----------------------------------------------------------------------------
// initalize application
void initialize();

// exit the app
void finalize();

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

// create a texture
void createTexture(UINT textureArray[], LPSTR fileName, int id);

// setup our display lists
void initDisplayLists();

// kludge to shoot a fireball from the given model (just for fun)
void handleFireballs();

// create a fireball at the tanks current position and add it to the fireball list
void createFireball(SML_CORE::Model3D* theModel);

}
#endif