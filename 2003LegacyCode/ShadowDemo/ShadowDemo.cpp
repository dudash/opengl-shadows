/**
\mainpage
\section title Shadows and Mesh Loading

   Jason Dudash                              <br>
   CS5984 - Computer Graphics, Spring 2003   <br>

\section overview Project Overview:
  This project demonstrates real-time shadowing techniques as well as an
example of loading 3D geometry information from a file.  These topics are
explored in more detail in the accompanying paper: "Shadows and Complex Geometry".

\section features Features:
- Texture Mapping, Mipmapping, Skinabble Meshes
- Point Lights
- Planar Projected Shadows
- Smooth Shading
- Mesh Loading (from .x files)
- Double Buffering
- OpenGL Display Lists (for improved rendering speeds)
- Scene Based Rendering
- Fullscreen Mode

\section build How to Build & Dependencies:
  This project was developed in Microsoft Visual Studio 6.0.  It requires that
you have installed GLUT.  If you are attempting to bulid this project in another
development environment make sure you link in: glaux.lib.

\subsection depends Dependencies include:
    -# tank.x
    -# greentank.bmp
    -# browntank.bmp
    -# fireball.bmp
    -# grass.bmp

\section exec How To Run:
Goto "[install dir]/ShadowDemo/Release" and double-click "ShadowDemo.exe".
Or from a command prompt type "[install dir]/ShadowDemo/Release/ShadowDemo.exe"

\section future Future Feature List
- Sync all activity on a timer
- Unkludge the fireball implementation
- Fix loadable x file mesh texture mapping
- Collision detection
- Create a VolumeShadowScene Class
- Create an ObjectManager/Resource Loader class
- Use a Composite design pattern for Model3D in order to nest models
- Add multiple mesh loading capability to XFileLoader
- Skybox
- Add colors to lights
*/
#include <stdlib.h>
#include <math.h>
#include <iostream>
#include <sstream>
#include "ShadowDemo.h"
#include "XFileLoader.h"
#include "PlanarProjectedShadowScene.h"
#include "Vector3D.h"
#include "Camera.h"
#include "FTM.h"

/** \namespace std namespace is defined by the C++ STL */
using namespace std;
/** \namespace SML_CORE The CORE functionality of the Shadows and Mesh Loading Engine */
using namespace SML_CORE;
/** \namespace SML_APP The APPLICATION that demonstrates the Shadows and Mesh Loading Engine */
using namespace SML_APP;

namespace SML_APP
{
// temp place for variables
ShadowableScene *theScene;
Camera *theCamera;
Model3D *groundModel;
Model3D *teapotModel;
Model3D *evilTankModel;
Model3D *tankModel;

//-----------------------------------------------------------------------------
/**
   Initalize the system
*/
void initialize()
{
   //glClearColor(0.6, 0.6, 0.8, 1.0);
   glClearColor(0.0, 0.0, 0.0, 1.0);
   glEnable(GL_DEPTH_TEST);
   glShadeModel (GL_SMOOTH);

   // load the bitmap textures
   createTexture(textureList, "grass.bmp", GRASS_TEXTURE);
   createTexture(textureList, "fireball.bmp", FIREBALL_TEXTURE);
   createTexture(textureList, "greentank.bmp", TANK_TEXTURE);
   createTexture(textureList, "browntank.bmp", EVIL_TANK_TEXTURE);

   // load in the hardcoded geometry
   initDisplayLists();

   /// \todo replace individual models with a list
   groundModel = new Model3D("Ground", GROUND, Vector3D(0,0,0), false);
   teapotModel = new Model3D("Teapot", TEAPOT, Vector3D(0,5,20));
   evilTankModel = new Model3D("EvilTank", EVIL_TANK, Vector3D(75,0,150));
   tankModel = new Model3D("Tank", TANK, Vector3D(100,0,100));

   // set a texture to be used when drawing these models
   tankModel->setTexture(TANK_TEXTURE, textureList);
   evilTankModel->setTexture(EVIL_TANK_TEXTURE, textureList);

   // load in the mesh data geometry
   XFileLoader tankLoader;
   if (tankLoader.loadXFile("tank.x"))
   {
      tankLoader.createModel3D(tankModel);
      tankModel->createOpenGLDisplayList();
      tankLoader.createModel3D(evilTankModel);
      evilTankModel->createOpenGLDisplayList();
   }
   else
   {
      cout << "ERROR! - Could not load tank model" << endl;
      finalize();
   }

   // Create & setup the scene and the Camera
   theScene =  new PlanarProjectedShadowScene(); 
   theCamera = new Camera(Vector3D(10.0,80.0,100.0), Vector3D(150.0,-50.0,100.0));

   // add all our geometry to the scene
   theScene->addModel(groundModel, ShadowableScene.RECEIVES_SHADOWS);
   theScene->addModel(tankModel, ShadowableScene.CASTS_SHADOWS);
   theScene->addModel(evilTankModel, ShadowableScene.CASTS_SHADOWS);

   theScene->addPointLightSource(50.0, 45.0, 100.0);
   theScene->drawLights(true);
}

//-----------------------------------------------------------------------------
/**
   Initalize a texture map into memory
   Note: this routine is derived from DigiBen's texture mapping tutorial
   http://www.gametutorials.com/
*/
void createTexture(UINT textureArray[], LPSTR fileName, int id)
{
   AUX_RGBImageRec *bitmapPtr = NULL;

   if (!fileName) return;
   bitmapPtr = auxDIBImageLoad(fileName);
   if (bitmapPtr==NULL) exit(0);
   glGenTextures(1, &textureArray[id]);
   glBindTexture(GL_TEXTURE_2D, textureArray[id]);
   gluBuild2DMipmaps(GL_TEXTURE_2D, 3, bitmapPtr->sizeX, bitmapPtr->sizeY, GL_RGB,
      GL_UNSIGNED_BYTE, bitmapPtr->data);
   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_NEAREST);
   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR_MIPMAP_LINEAR);
   if (bitmapPtr)
      if (bitmapPtr->data)
         free(bitmapPtr);
}

//-----------------------------------------------------------------------------
/**
   Initalize display lists with 3d models data
*/
void initDisplayLists()
{
	//----------------------------------------------
   // Initialize the ground plane display list,
   // note: We also stick texture coords to the plane
	glNewList(GROUND, GL_COMPILE);
      glPushMatrix();
		glPushAttrib(GL_ALL_ATTRIB_BITS);
      glEnable(GL_TEXTURE_2D);
      glColor3f(1.0,1.0,1.0);
      glBindTexture(GL_TEXTURE_2D, textureList[GRASS_TEXTURE]);
		glBegin(GL_POLYGON);
         glTexCoord2f(0,1);
         glVertex3f(0.0,0.0,0.0);
         
         glTexCoord2f(0.0,0.0);
         glVertex3f(0.0,0.0,200);

         glTexCoord2f(1.0,0.0);
         glVertex3f(200,0.0,200);
         
         glTexCoord2f(1.0,1.0);
         glVertex3f(200,0.0,0.0);
		glEnd();
      glDisable(GL_TEXTURE_2D);
		glPopAttrib();
		glPopMatrix();
	glEndList();

   //----------------------------------------------
   // Initialize the teapot display list
	glNewList(TEAPOT, GL_COMPILE);
      glPushMatrix();
		glPushAttrib(GL_ALL_ATTRIB_BITS);
      glutSolidTeapot(2.0);
		glPopAttrib();
		glPopMatrix();
	glEndList();

	//----------------------------------------------
   // Initialize the fireball display list
	glNewList(FIREBALL, GL_COMPILE);
      glPushMatrix();
		glPushAttrib(GL_ALL_ATTRIB_BITS);
      glEnable(GL_TEXTURE_2D);
      glEnable(GL_TEXTURE_GEN_S);
      glTranslatef(0.0,5.0,0.0);
      glBindTexture(GL_TEXTURE_2D, textureList[FIREBALL_TEXTURE]);
      glTexCoord2f(0.0,1.0);
      glutSolidSphere (0.6, 20, 16);
      glutSolidTetrahedron();
      glDisable(GL_TEXTURE_GEN_S);
      glDisable(GL_TEXTURE_2D);
		glPopAttrib();
		glPopMatrix();
	glEndList();
}

//-----------------------------------------------------------------------------
/**
   create the menu system and attach it to the proper input
*/
void initMenuSystem()
{
   int keyboardCommandsMenu = glutCreateMenu(handleMainMenuInput);
   glutAddMenuEntry("1,2 : Select Tank", MENU_NONE);
   glutAddMenuEntry("WSAD : Move Current Tank", MENU_NONE);
   glutAddMenuEntry("<space> : Fire!", MENU_NONE);
   glutAddMenuEntry("Q : Quit", MENU_NONE);
   int cameraMenu = glutCreateMenu(handleMainMenuInput);
   glutAddMenuEntry("HighUp Cam", MENU_HIGHUP_CAM);
   glutAddMenuEntry("Side Cam", MENU_SIDE_CAM);
   //glutAddMenuEntry("Tank Cam", MENU_TOGGLE_ATTACH_TO_TANK);
   int mainMenu = glutCreateMenu(handleMainMenuInput);
   glutAddMenuEntry("Shadows On", MENU_SHADOWS_ON);
   glutAddMenuEntry("Shadows Off", MENU_SHADOWS_OFF);
   glutAddMenuEntry("--------------------", MENU_NONE);
   glutAddSubMenu("Camera", cameraMenu);
   glutAddMenuEntry("--------------------", MENU_NONE);
   glutAddMenuEntry("Toggle Fullscreen", MENU_TOGGLE_FULLSCREEN);
   glutAddMenuEntry("Toggle Show Lights", MENU_TOGGLE_SHOWLIGHTS);
   glutAddMenuEntry("--------------------", MENU_NONE);
   glutAddSubMenu("Keyboard Commands", keyboardCommandsMenu);
   glutAddMenuEntry("--------------------", MENU_NONE);
   glutAddMenuEntry("Quit", MENU_EXIT);
   glutAttachMenu(GLUT_RIGHT_BUTTON);
}

//-----------------------------------------------------------------------------
/**
   Callback that occurs when glut thinks its time to redraw the window.
*/
void handleDisplay()
{
   glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

   glMatrixMode(GL_MODELVIEW);
   glLoadIdentity();

   theCamera->update();
   theScene->render();

   glFlush();
   glutSwapBuffers();
}

//-----------------------------------------------------------------------------
/**
   Callback that occurs when the window is resized.
*/
void handleReshape(int width, int height)
{
   // set the perspective
   glMatrixMode(GL_PROJECTION);
   glLoadIdentity();
   float ratio = 1.0 * width / height;
   gluPerspective(60.0,ratio,1.0,500.0);
   glMatrixMode(GL_MODELVIEW);
   glViewport(0, 0, width, height);
}

//-----------------------------------------------------------------------------
/**
   Callback that handle menu input
*/
void handleMainMenuInput(int input)
{
   switch(input)
   {
   case MENU_EXIT:
      finalize();
      break;
   case MENU_SIDE_CAM:
      theCamera->locX = 10;
      theCamera->locY = 30;
      theCamera->locZ = 100;
      break;
   case MENU_HIGHUP_CAM:
      theCamera->locX = 10;
      theCamera->locY = 80;
      theCamera->locZ = 100;
      break;
   case MENU_TOGGLE_ATTACH_TO_TANK:
      if (theCamera->isAttached())
         theCamera->unattach();
      else
         theCamera->attachToModel(tankModel);
      break;
   case MENU_TOGGLE_FULLSCREEN:
      if (fullscreen)
      {
         glutReshapeWindow(WINDOW_WIDTH, WINDOW_HEIGHT);
         glutPositionWindow(50,50);
         fullscreen = false;
      }
      else
      {
         glutFullScreen();
         fullscreen = true;
      }
      break;
   case MENU_TOGGLE_SHOWLIGHTS:
      if (theScene->isDrawLightsOn())
         theScene->drawLights(false);
      else
         theScene->drawLights(true);
      break;
   case MENU_SHADOWS_ON:
      theScene->turnOnShadows();
      break;
   case MENU_SHADOWS_OFF:
      theScene->turnOffShadows();
      break;
   }
   glutPostRedisplay();
}

//-----------------------------------------------------------------------------
/**
   Callback to handle mouse events
*/
void handleMouse(int button, int state, int x, int y)
{
}

//-----------------------------------------------------------------------------
/**
   Callback to handle keyboard events
*/
void handleKeyboardInput(unsigned char key, int x, int y)
{
   switch (key)
   {
   case 'q':
   case 'Q':
      finalize();
      break;
   case 'w':
   case 'W':
      // move forward
      if (currentTank==1) tankModel->moveModel(1,0,0);
      else if (currentTank==2) evilTankModel->moveModel(1,0,0);
      break;
   case 's':
   case 'S':
      // move backward
      if (currentTank==1) tankModel->moveModel(-1,0,0);
      else if (currentTank==2) evilTankModel->moveModel(-1,0,0);
      break;
   case 'a':
   case 'A':
      // turn left
      if (currentTank==1) tankModel->rotateModel(5, 0,1,0);
      else if (currentTank==2) evilTankModel->rotateModel(5, 0,1,0);
      break;
   case 'd':
   case 'D':
      // turn right
      if (currentTank==1) tankModel->rotateModel(-5, 0,1,0);
      else if (currentTank==2) evilTankModel->rotateModel(-5, 0,1,0);
      break;
   case 'i':
   case 'I':

      break;
   case 'k':
   case 'K':

      break;
   case 'j':
   case 'J':

      break;
   case 'l':
   case 'L':

      break;
   case ' ':
      //fireBall(tankModel);
      cout << "Fire!" << endl;
      if (currentTank==1) createFireball(tankModel);
      else if (currentTank==2) createFireball(evilTankModel);
      break;
   case '1':
      currentTank = 1;
      break;
   case '2':
      currentTank = 2;
      break;
   case 'B':
   case 'b':
      cheaterB=true;
      break;
   case 'e':
      if (!cheaterB) break;
      if (!cheaterE) {cheaterE=true;break;}
      if (cheaterE2) {
         // turn it off
         cheaterB = false;
         cheaterE = false;
         cheaterE2 = false;
         cout << "normal fire mode" << endl;
         break;
      }

      else cheaterE2 = true;
      cout << "KILLER BEES!" << endl;
   }
}

//-----------------------------------------------------------------------------
/**
   Callback to handle idle mode
*/
void handleIdle()
{
   glutPostRedisplay();
   handleFireballs();
}

//-----------------------------------------------------------------------------
/**
   Cleanup the application before we close
*/
void finalize()
{
   glDeleteTextures(1, &textureList[GRASS_TEXTURE]);

   if (theScene) delete theScene;
   if (theCamera) delete theCamera;
   if (groundModel) delete groundModel;
   if (teapotModel) delete teapotModel;
   if (evilTankModel) delete evilTankModel;
   if (tankModel) delete tankModel;

   //cleanup fireballs (if any)
   Model3D* tempModel;
   for (int fireIndex=0; fireIndex<fireballs.size(); fireIndex++)
   {
      tempModel = fireballs[fireIndex];
      delete tempModel;
   }

   exit(0);
}


//-----------------------------------------------------------------------------
/**
   A really quick kuldge to make the demo more fun.  A real implmentation
   would require alot more functionality in the system (time calcs, world to
   store geometry and update, etc...)
*/
void handleFireballs()
{
   // loop thru the list of fireballs
   Model3D* tempModel;
   for (int fireIndex=0; fireIndex<fireballs.size(); fireIndex++)
   {
      tempModel = fireballs[fireIndex];
      // dont ever remove any... it's just temp code anyway.
      if (false)
      {
         // remove it
         theScene->removeModel(tempModel->getName());
         delete tempModel;
         tempModel = 0;
         fireballs.erase(&fireballs[fireIndex]);
      }
      else
      {
         tempModel->moveModel(2.5,0,0);

         // Remove this comment for "killer bees"
         if (cheaterE2) tempModel->rotateModel(5, 0,1,0);
      }
   }
}

//-----------------------------------------------------------------------------
/**
   A really quick kuldge to make the demo more fun.  A real implmentation
   would require alot more functionality in the system (time calcs, world to
   store geometry and update, etc...)
*/
void createFireball(Model3D *tankModel)
{
   numFired++;
   Vector3D initialPos = tankModel->getPosition();
   FTM initialOrient = tankModel->getFTM();
   stringstream streamName("");
   streamName << "Fireball" << numFired;
   Model3D* tempModel = new Model3D(streamName.str(), FIREBALL, initialPos);
   tempModel->setTransformMatrix(initialOrient);
   // nudge it forward a bit
   tempModel->moveModel(5,0,0);
   tempModel->setTexture(FIREBALL_TEXTURE, textureList);
   fireballs.push_back(tempModel);

   theScene->addModel(tempModel, ShadowableScene.CASTS_SHADOWS);
}
}

/**
  This is the entry point into the application
  */
int main(int argc, char** argv)
{
   // setup glut and create a window
   glutInit(&argc,argv);
   glutInitDisplayMode (GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
   glutInitWindowSize(WINDOW_WIDTH,WINDOW_HEIGHT);
   glutInitWindowPosition(150,150);
   glutCreateWindow("Shadows & Mesh Loading");

   // program initilization
   initialize();
   initMenuSystem();

   // setup our callbacks
   glutDisplayFunc(handleDisplay);
   glutReshapeFunc(handleReshape);
   glutMouseFunc(handleMouse);
   glutKeyboardFunc(handleKeyboardInput);
   glutIdleFunc(handleIdle);

   // start
   glutMainLoop();

   return 0;
}