#include <GL/glut.h>
#include "PlanarProjectedShadowScene.h"
#include "Model3D.h"

using std::vector;

namespace SML_CORE
{
//-----------------------------------------------------------------------------
/**
   Constructor
  */
PlanarProjectedShadowScene::PlanarProjectedShadowScene() : ShadowableScene()
{
   // setup the stencil buffer for our shadowing (used when blending)
   glStencilFunc(GL_ALWAYS, 1, 0xffffffff);
   glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);

   // set the polygon offset values (factor, units)
   glPolygonOffset(-1.0, -2.0);
}

//-----------------------------------------------------------------------------
/**
   Destructor
  */
PlanarProjectedShadowScene::~PlanarProjectedShadowScene()
{

}

//-----------------------------------------------------------------------------
/**
  Render the model list to the screen
*/
void PlanarProjectedShadowScene::renderModelListAsShadows(const vector<Model3D*> &modelList, FTM shadowMatrix)
{
   Model3D *aModel;
   glDisable(GL_LIGHTING);
   for (int index = 0; index < modelList.size(); index++)
   {
      aModel = modelList[index];
      glPushMatrix();
 
      // Transform the model onto the calculated receiver plane
      float tempShadowMatrix[] = 
      { 
         shadowMatrix._00,shadowMatrix._01,shadowMatrix._02,shadowMatrix._03,
         shadowMatrix._10,shadowMatrix._11,shadowMatrix._12,shadowMatrix._13,
         shadowMatrix._20,shadowMatrix._21,shadowMatrix._22,shadowMatrix._23,
         shadowMatrix._30,shadowMatrix._31,shadowMatrix._32,shadowMatrix._33,
      };
      glMultMatrixf(tempShadowMatrix);

      // Position the model
      Vector3D position(aModel->getPosition());
      glTranslatef(position.x, position.y, position.z);
      
      // Orient the model
      FTM rotations(aModel->getFTM());
      float tempMatrix[] = 
      { 
         rotations._00,rotations._01,rotations._02,rotations._03,
         rotations._10,rotations._11,rotations._12,rotations._13,
         rotations._20,rotations._21,rotations._22,rotations._23,
         rotations._30,rotations._31,rotations._32,rotations._33,
      };
      glMultMatrixf(tempMatrix);

      // draw the model
      glCallList(aModel->getCallListId());

      glPopMatrix();
   }
}

//-----------------------------------------------------------------------------
/**
   A implmentation of drawShadows uses a planar projection technique to draw
   stencil buffer shadows on a flat plane.
*/
void PlanarProjectedShadowScene::drawShadows()
{
   // loop through each receiver and draw shadows on it
   for (int index = 0; index < shadowReceiverList.size(); index++)
   {
      Model3D* aModel = shadowReceiverList[index];

      // calculate plane from points
      /// \todo get the plane from the model
      float p0[4] = {0.0, 0.0, 0.0};
      float p1[4] = {0.0, 0.0, 200.0};
      float p2[4] = {200.0, 0.0, 200.0};
      float *tempPlane = calculatePlaneFromPoints(p0,p1,p2);

      // draw shadows onto this receiver
      glPushAttrib(GL_ALL_ATTRIB_BITS);
      glEnable(GL_STENCIL_TEST);

      // this will make sure the shadow drawing doesn't conflict with the actual scene polygons
      glEnable(GL_POLYGON_OFFSET_FILL);

      // uncomment these lines to enable shadow blending
      //glEnable(GL_BLEND);
      //glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
      glDisable(GL_LIGHTING);
      glColor4f(0.0, 0.0, 0.0, 0.8);

      // draw a shadow for each light on this plane
      for (int lightIndex = 0; lightIndex < pointLightList.size(); lightIndex++)
      {
         float tempLight[4] = {
            pointLightList[lightIndex].x,
            pointLightList[lightIndex].y,
            pointLightList[lightIndex].z,
            1.0
         };
         // transform by our shadow matrix calculation and draw
         FTM shadowMatrix = calculateShadowTransformation(tempPlane, tempLight);
         renderModelListAsShadows(shadowCasterList, shadowMatrix);
      }

      glEnable(GL_LIGHTING);
      glDisable(GL_POLYGON_OFFSET_FILL);
      glDisable(GL_STENCIL_TEST);
      glPopAttrib();
   }
}

//-----------------------------------------------------------------------------
/**
  Calculate a plane that transforms objects onto the argument plane
  We also pass in a 4D light position.

  @param projectionPlane The plane to project onto
  @param lightPosition The position we are projecting
  @return Shadow frame transformation matrix
*/
FTM PlanarProjectedShadowScene::calculateShadowTransformation(float* projectionPlane, float* lightPosition)
{
   float dotProduct =
      projectionPlane[0] * lightPosition[0] +
      projectionPlane[1] * lightPosition[1] +
      projectionPlane[2] * lightPosition[2] +
      projectionPlane[3] * lightPosition[3];

   FTM shadowMatrix;

   shadowMatrix._00 = dotProduct - lightPosition[0] * projectionPlane[0];
   shadowMatrix._10 = 0.0 - lightPosition[0] * projectionPlane[1];
   shadowMatrix._20 = 0.0 - lightPosition[0] * projectionPlane[2];
   shadowMatrix._30 = 0.0 - lightPosition[0] * projectionPlane[3];

   shadowMatrix._01 = 0.0 - lightPosition[1] * projectionPlane[0];
   shadowMatrix._11 = dotProduct - lightPosition[1] * projectionPlane[1];
   shadowMatrix._21 = 0.0 - lightPosition[1] * projectionPlane[2];
   shadowMatrix._31 = 0.0 - lightPosition[1] * projectionPlane[3];

   shadowMatrix._02 = 0.0 - lightPosition[2] * projectionPlane[0];
   shadowMatrix._12 = 0.0 - lightPosition[2] * projectionPlane[1];
   shadowMatrix._22 = dotProduct - lightPosition[2] * projectionPlane[2];
   shadowMatrix._32 = 0.0 - lightPosition[2] * projectionPlane[3];

   shadowMatrix._03 = 0.0 - lightPosition[3] * projectionPlane[0];
   shadowMatrix._13 = 0.0 - lightPosition[3] * projectionPlane[1];
   shadowMatrix._23 = 0.0 - lightPosition[3] * projectionPlane[2];
   shadowMatrix._33 = dotProduct - lightPosition[3] * projectionPlane[3];

   return shadowMatrix;
}

//-----------------------------------------------------------------------------
/**
   Find the equation of a plane from 3 points

   Note: this operation has a memory leak (we loose plane), we need to
   either pass in a pointer to a plane, or create a Plane class.

  @return The plane formed by the argument points
*/
float* PlanarProjectedShadowScene::calculatePlaneFromPoints(float p0[3], float p1[3], float p2[3])
{
  // get vector between point1 and point0
  float vector0[3];
  vector0[0] = p1[0] - p0[0];
  vector0[1] = p1[1] - p0[1];
  vector0[2] = p1[2] - p0[2];

  // get vector between point2 and point0
  float vector1[3];
  vector1[0] = p2[0] - p0[0];
  vector1[1] = p2[1] - p0[1];
  vector1[2] = p2[2] - p0[2];

  // the cross product of the vectors gives us a plane
  float* plane = new GLfloat[4];
  plane[0] = vector0[1] * vector1[2] - vector0[2] * vector1[1];
  plane[1] = -(vector0[0] * vector1[2] - vector0[2] * vector1[0]);
  plane[2] = vector0[0] * vector1[1] - vector0[1] * vector1[0];
  plane[3] = -(plane[0] * p0[0] + plane[1] * p0[1] + plane[2] * p0[2]);

  return plane;
}
}