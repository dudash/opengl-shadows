#ifndef CAMERA_H
#define CAMERA_H
//-----------------------------------------------------------------------------
#include "Vector3D.h"
#include "FTM.h"

namespace SML_CORE
{
// forward declarations
class Model3D;

/**
   This class defines a virtual openGL camera and provides operations
   to modify and utilize it.
  */
class Camera
{
public:
   float locX;
   float locY;
   float locZ;
   float lookAtX;
   float lookAtY;
   float lookAtZ;
   float upX;
   float upY;
   float upZ;
   FTM transformMatrix;
   bool attachedToModel;
   Model3D* attachModel;

   // not used yet
   //float fieldOfView;
   //float aspect;
   //float nearClipPlane;
   //float farClipPlane;
public:
   Camera(Vector3D pos, Vector3D lookAt);
   //Camera(float xv, float yv, float zv, float uv, float, vv, float wv);  
	virtual ~Camera();

   void update();
   void attachToModel(Model3D* theModel);
   void unattach();
   bool isAttached() {return attachedToModel;};
};
}
#endif
