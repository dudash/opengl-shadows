#include <GL/glut.h>
#include "Camera.h"
#include "Model3D.h"

namespace SML_CORE
{
//-----------------------------------------------------------------------------
/**
      Constructor
  */
Camera::Camera(Vector3D loc, Vector3D lookAt) :
locX(loc.x), locY(loc.y), locZ(loc.z),
lookAtX(lookAt.x), lookAtY(lookAt.y), lookAtZ(lookAt.z),
upX(0.0), upY(1.0), upZ(0.0),
transformMatrix(),
attachedToModel(false),
attachModel(0)
{ }

//-----------------------------------------------------------------------------
/**
      Constructor
  *
Camera::Camera(float xv, float yv, float zv, float av, float, bv, float cv) :
locX(xv), locY(yv), locZ(zv),
lookAtX(av), lookAtY(bv), lookAtZ(cv),
upX(0.0), upY(1.0), upZ(0.0)
{ }
*/

//-----------------------------------------------------------------------------
/**
   Destructor
  */
Camera::~Camera()
{

}

//-----------------------------------------------------------------------------
/**
   Update the openGL MV-Matrix with data from this camera
  */
void Camera::update()
{
   // Set our camera up
   glMatrixMode(GL_MODELVIEW);
   glLoadIdentity();

   gluLookAt(locX, locY, locZ,
      lookAtX, lookAtY, lookAtZ,
      upX, upY, upZ);

   // if we are attached look with the model using the inverse of its FTM
   FTM tempFTM;
   if (attachedToModel)
   {
      tempFTM = attachModel->getFTM();
      transformMatrix = tempFTM; /// \todo get inverse transpose to fix camera attach

   }

   float tempMatrix[] = 
   { 
      transformMatrix._00,transformMatrix._01,transformMatrix._02,transformMatrix._03,
      transformMatrix._10,transformMatrix._11,transformMatrix._12,transformMatrix._13,
      transformMatrix._20,transformMatrix._21,transformMatrix._22,transformMatrix._23,
      transformMatrix._30,transformMatrix._31,transformMatrix._32,transformMatrix._33,
   };

   // apply our local transformation matrix
   glMultMatrixf(tempMatrix);
}

//-----------------------------------------------------------------------------
/**
   Attach the camera to a model so that it follows it around and is rotated
   when the model is rotated.
  */
void Camera::attachToModel(Model3D* theModel)
{
   attachedToModel = true;
   attachModel = theModel;
}

//-----------------------------------------------------------------------------
/**
   Unattach the camera from a model
  */
void Camera::unattach()
{
   attachedToModel = false;
   attachModel = 0;
   transformMatrix.loadIdentity();
}
}