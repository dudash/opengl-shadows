/**
  Vector Shadows Exmaple
  Jason Dudash
  22 March 2003

  Use the WSAD keys to move the light souce around. E and C move it up and
  down.  If you are having trouble determining the position of the light in the
  3D scene toggle the light lines on.
*/
#include <GL/glut.h>
#include "main.h"

int main(int argc, char** argv)
{
   // setup glut and create a window
   glutInit(&argc,argv);
   glutInitDisplayMode (GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
   glutInitWindowSize(WINDOW_WIDTH,WINDOW_HEIGHT);
   glutInitWindowPosition(150,150);
   glutCreateWindow("Vector Shadow Example");

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


//-----------------------------------------------------------------------------
/**
   Initialize the state of openGL.
*/
void initialize()
{
   glClearColor(0.6, 0.6, 0.8, 1.0);
   glEnable(GL_DEPTH_TEST);
   glShadeModel (GL_SMOOTH);

   // default material properties
   GLfloat matSpecular[] = { 1.0, 1.0, 1.0, 1.0 };
   GLfloat matShininess[] = { 50.0 };
   GLfloat matAmbDiff[] = { 0.6, 0.45, 0.5, 1.0 };
   glMaterialfv(GL_FRONT, GL_SPECULAR, matSpecular);
   glMaterialfv(GL_FRONT, GL_SHININESS, matShininess);
   glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, matAmbDiff);

   GLfloat lmodelAmbient[] = { 0.3, 0.3, 0.3, 1.0 };
   glLightModelfv(GL_LIGHT_MODEL_AMBIENT, lmodelAmbient);

   glEnable(GL_LIGHTING);
   lightingOn = true;

   GLfloat lightAmbient[] = { 0.0, 0.0, 0.5, 1.0};
   glLightfv(GL_LIGHT0, GL_AMBIENT, lightAmbient);
   GLfloat lightDiffuse[] = { 0.5, 0.7, 1.0, 1.0};
   glLightfv(GL_LIGHT0, GL_DIFFUSE, lightDiffuse);

   glEnable(GL_LIGHT0);
   light0On = true;

   // one time calculation of the wall planes and the floor plane
   floorPlane = calculatePlaneFromPoints(floorVerticies[0], floorVerticies[1], floorVerticies[2]);
   wallPlane = calculatePlaneFromPoints(wallVerticies[0], wallVerticies[1], wallVerticies[2]);


   // set the polygon offset values (factor, units)
   glPolygonOffset(-1.0, -2.0);
}

//-----------------------------------------------------------------------------
/**
   create the menu system and attach it to the proper input
*/
void initMenuSystem()
{
   int keyboardCommandsMenu = glutCreateMenu(handleMainMenuInput);
   glutAddMenuEntry("L : Toggle Lighting", DO_NOTHING);
   glutAddMenuEntry("0 : Toggle light0 On/Off", DO_NOTHING);
   glutAddMenuEntry("WSAD,EC : Move Light", DO_NOTHING);
   glutAddMenuEntry("UJHK,OP : Move Object", DO_NOTHING);
   glutAddMenuEntry("I : Toggle Light Line", DO_NOTHING);
   glutAddMenuEntry("Q : Quit", DO_NOTHING);
   int mainMenu = glutCreateMenu(handleMainMenuInput);
   glutAddMenuEntry("Toggle Lighting", TOGGLE_LIGHTING);
   glutAddMenuEntry("Toggle light0 On/Off", TOGGLE_LIGHT0);
   glutAddMenuEntry("Toggle Shadows", MENU_SHADOWS_TOGGLE);
   glutAddMenuEntry("Toggle Object Rotation", TOGGLE_OBJECT_ROTATION);
   glutAddMenuEntry("----------------------", DO_NOTHING);
   glutAddSubMenu("Keyboard Commands", keyboardCommandsMenu);
   glutAddMenuEntry("----------------------", DO_NOTHING);
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

   // update the light position
   glLightfv(GL_LIGHT0, GL_POSITION, lightPosition0);

   glMatrixMode(GL_MODELVIEW);
   glLoadIdentity();
   setCamera();

   // draw the light source
   drawLightSource();

   // draw the wall background
   drawBackdropGeometry();

   // draw scene objects
   glColor3f(0.0,0.0,1.0);
   drawScene();

   // calc and draw shadows
   if (shadowsOn && light0On)
   {
      calculateShadows(floorPlane);
      drawShadows();
      calculateShadows(wallPlane);
      drawShadows();
   }

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
   gluPerspective(60.0,ratio ,1.0,100.0);
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
      exit(0);
      break;
   case TOGGLE_LIGHTING:
      if (lightingOn)
      {
         glDisable(GL_LIGHTING);
         lightingOn = false;
      }
      else
      {
         glEnable(GL_LIGHTING);
         lightingOn = true;
      }
      glutPostRedisplay();
      break;
   case TOGGLE_LIGHT0:
      if (light0On)
      {
         glDisable(GL_LIGHT0);
         light0On = false;
      }
      else
      {
         glEnable(GL_LIGHT0);
         light0On = true;
      }
      glutPostRedisplay();
      break;
   case TOGGLE_OBJECT_ROTATION:
      if (objectRotating)
         objectRotating=false;
      else
         objectRotating=true;
      break;
   case MENU_SHADOWS_TOGGLE:
      if (shadowsOn)
         shadowsOn=false;
      else
         shadowsOn=true;
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
void handleKeyboardInput(unsigned char key, int x, int y )
{
   switch(key)
   {
   case ('i'):
   case ('I'):
      if (lightLineOn)
      {
         lightLineOn = false;
      }
      else
      {
         lightLineOn = true;
      }
      glutPostRedisplay();
      break;
   case ('u'):
   case ('U'):
      objectPosition[0]+=0.5;
      break;
   case ('j'):
   case ('J'):
      objectPosition[0]-=0.5;
      break;
   case ('h'):
   case ('H'):
      objectPosition[2]-=0.5;
      break;
   case('k'):
   case ('K'):
      objectPosition[2]+=0.5;
      break;
   case ('o'):
   case ('O'):
      objectPosition[1]+=0.5;
      break;
   case('p'):
   case ('P'):
      objectPosition[1]-=0.5;
      break;
   case ('w'):
   case ('W'):
      lightPosition0[0]+=0.5;
      break;
   case ('s'):
   case ('S'):
      lightPosition0[0]-=0.5;
      break;
   case ('a'):
   case ('A'):
      lightPosition0[2]-=0.5;
      break;
   case('d'):
   case ('D'):
      lightPosition0[2]+=0.5;
      break;
   case ('e'):
   case ('E'):
      lightPosition0[1]+=0.5;
      break;
   case('c'):
   case ('C'):
      lightPosition0[1]-=0.5;
      break;
   case ('0'):
      if (light0On)
      {
         glDisable(GL_LIGHT0);
         light0On = false;
      }
      else
      {
         glEnable(GL_LIGHT0);
         light0On = true;
      }
      glutPostRedisplay();
      break;
   case ('l'):
   case ('L'):
      if (lightingOn)
      {
         glDisable(GL_LIGHTING);
         lightingOn = false;
      }
      else
      {
         glEnable(GL_LIGHTING);
         lightingOn = true;
      }
      glutPostRedisplay();
      break;
   case ('Q'):
   case ('q'):
      exit(0);
   }
}

//-----------------------------------------------------------------------------
/**
   Callback to handle idle mode
*/
void handleIdle()
{
   if (objectRotating) tetraTurn+=rotationSpeed;
   glutPostRedisplay();
}

//-----------------------------------------------------------------------------
/**
   Setup the camera to view the scene
*/
void setCamera()
{
   cameraRotationX = 0;
   cameraRotationY = 0;
   cameraRotationZ = 0;
   cameraX = -10.0;
   cameraY = 15.0;
   cameraZ = -5.0;
   lookX = 25.0;
   lookY = 5.0;
   lookZ = 0.0;
   upX = 0.0;
   upY = 1.0;
   upZ = 0.0;

   // Set our camera up
   glMatrixMode(GL_MODELVIEW);
   glLoadIdentity();

   gluLookAt(cameraX, cameraY, cameraZ,
      lookX, lookY, lookZ,
      upX, upY, upZ);

   glRotatef(cameraRotationX, 1.0, 0.0, 0.0);
   glRotatef(cameraRotationY, 0.0, 1.0, 0.0);
   glRotatef(cameraRotationZ, 0.0, 0.0, 1.0);
}

//-----------------------------------------------------------------------------
/**

*/
void drawBackdropGeometry()
{
   glPushMatrix();
   glPushAttrib(GL_ALL_ATTRIB_BITS);
   // floor
   glColor3f(.5,.5,.5);
   GLfloat matAmbDiff[] = { 0.6, 0.45, 0.25, 1.0 };
   glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, matAmbDiff);
   glBegin(GL_POLYGON);
      glVertex3fv(floorVerticies[0]);
      glVertex3fv(floorVerticies[1]);
      glVertex3fv(floorVerticies[2]);
      glVertex3fv(floorVerticies[3]);
   glEnd();

   //wall
   glColor3f(.55,.55,.55);
   GLfloat matAmbDiff2[] = { 0.6, 0.5, 0.3, 1.0 };
   glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, matAmbDiff2);
   glBegin(GL_POLYGON);
      glVertex3fv(wallVerticies[0]);
      glVertex3fv(wallVerticies[1]);
      glVertex3fv(wallVerticies[2]);
      glVertex3fv(wallVerticies[3]);
   glEnd();
   glPopAttrib();
   glPopMatrix();
}

//-----------------------------------------------------------------------------
/**
  Draw all the objects in the scene
*/
void drawScene()
{
   glPushMatrix();
   glPushAttrib(GL_ALL_ATTRIB_BITS);
   glTranslatef(objectPosition[0],objectPosition[1],objectPosition[2]);
   glRotatef(tetraTurn,1,0,0);
   glutSolidSphere (2.0, 20, 16);
   glScalef(3.0,3.0,3.0);
   glutSolidTetrahedron();
   glPopAttrib();
   glPopMatrix();
}

//-----------------------------------------------------------------------------
/**
  Show where the light is coming from.  You can turn on a vertical line to
  better see where it is located.
*/
void drawLightSource()
{
   glPushMatrix();
   glPushAttrib(GL_ALL_ATTRIB_BITS);
   glColor4f(1.0,1.0,0.0,0.75);
   GLfloat matAmbDiff[] = { 1.0, 1.0, 0.0, 0.75 };
   glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, matAmbDiff);
   glDisable(GL_LIGHTING);
   glTranslatef(lightPosition0[0],lightPosition0[1],lightPosition0[2]);
   glutSolidSphere (0.2, 20, 16);
   glTranslatef(-lightPosition0[0],-lightPosition0[1],-lightPosition0[2]);
   if (lightLineOn)
   {
      glPointSize(5.0);
      glBegin(GL_LINES);
      glVertex3f(lightPosition0[0],floorVerticies[0][1],lightPosition0[2]);
      glVertex3f(lightPosition0[0],lightPosition0[1],lightPosition0[2]);
      glVertex3f(lightPosition0[0],lightPosition0[1],lightPosition0[2]);
      glVertex3f(wallVerticies[0][0],lightPosition0[1],lightPosition0[2]);
      glEnd();
   }
   glEnable(GL_LIGHTING);
   glPopAttrib();
   glPopMatrix();
}

//-----------------------------------------------------------------------------
/**
Calculate shadows projected onto the argument plane
  We know the light position already, and the shadowMatrix is static
*/
void calculateShadows(GLfloat* projectionPlane)
{
   GLfloat dotProduct =
      projectionPlane[0] * lightPosition0[0] +
      projectionPlane[1] * lightPosition0[1] +
      projectionPlane[2] * lightPosition0[2] +
      projectionPlane[3] * lightPosition0[3];

   shadowMatrix[0][0] = dotProduct - lightPosition0[0] * projectionPlane[0];
   shadowMatrix[1][0] = 0.0 - lightPosition0[0] * projectionPlane[1];
   shadowMatrix[2][0] = 0.0 - lightPosition0[0] * projectionPlane[2];
   shadowMatrix[3][0] = 0.0 - lightPosition0[0] * projectionPlane[3];

   shadowMatrix[0][1] = 0.0 - lightPosition0[1] * projectionPlane[0];
   shadowMatrix[1][1] = dotProduct - lightPosition0[1] * projectionPlane[1];
   shadowMatrix[2][1] = 0.0 - lightPosition0[1] * projectionPlane[2];
   shadowMatrix[3][1] = 0.0 - lightPosition0[1] * projectionPlane[3];

   shadowMatrix[0][2] = 0.0 - lightPosition0[2] * projectionPlane[0];
   shadowMatrix[1][2] = 0.0 - lightPosition0[2] * projectionPlane[1];
   shadowMatrix[2][2] = dotProduct - lightPosition0[2] * projectionPlane[2];
   shadowMatrix[3][2] = 0.0 - lightPosition0[2] * projectionPlane[3];

   shadowMatrix[0][3] = 0.0 - lightPosition0[3] * projectionPlane[0];
   shadowMatrix[1][3] = 0.0 - lightPosition0[3] * projectionPlane[1];
   shadowMatrix[2][3] = 0.0 - lightPosition0[3] * projectionPlane[2];
   shadowMatrix[3][3] = dotProduct - lightPosition0[3] * projectionPlane[3];
}

//-----------------------------------------------------------------------------
/**
   Draw the shadow on the floor and wall.  This is done through
   use of the stencil buffer.
*/
void drawShadows()
{
   glPushAttrib(GL_ALL_ATTRIB_BITS);
   glEnable(GL_STENCIL_TEST);
   glStencilFunc(GL_ALWAYS, 1, 0xffffffff);
   glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);

   // this will make sure the shadowd drawing doesn't conflict with the 
   // actual scene polygons
   glEnable(GL_POLYGON_OFFSET_FILL);
   
   //glEnable(GL_BLEND);
   //glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
   
   glDisable(GL_LIGHTING);
   glColor4f(0.0, 0.0, 0.0, 0.5);

   glPushMatrix();
   // transform our drawing by our shadow matrix calculation
   glMultMatrixf((GLfloat *) shadowMatrix);
   drawScene();
   glPopMatrix();

   if (lightingOn)
      glEnable(GL_LIGHTING);

   //glDisable(GL_BLEND);
   glDisable(GL_POLYGON_OFFSET_FILL);
   glDisable(GL_STENCIL_TEST);
   glPopAttrib();
}

//-----------------------------------------------------------------------------
/**
   Find the equation of a plane from 3 points
*/
GLfloat* calculatePlaneFromPoints(GLfloat p0[3], GLfloat p1[3], GLfloat p2[3])
{
  // get vector between point1 and point0
  GLfloat vector0[3];
  vector0[0] = p1[0] - p0[0];
  vector0[1] = p1[1] - p0[1];
  vector0[2] = p1[2] - p0[2];

  // get vector between point2 and point0
  GLfloat vector1[3];
  vector1[0] = p2[0] - p0[0];
  vector1[1] = p2[1] - p0[1];
  vector1[2] = p2[2] - p0[2];

  // the cross product of the vectors gives us a plane
  GLfloat* plane = new GLfloat[4];
  plane[0] = vector0[1] * vector1[2] - vector0[2] * vector1[1];
  plane[1] = -(vector0[0] * vector1[2] - vector0[2] * vector1[0]);
  plane[2] = vector0[0] * vector1[1] - vector0[1] * vector1[0];
  plane[3] = -(plane[0] * p0[0] + plane[1] * p0[1] + plane[2] * p0[2]);

  return plane;
}