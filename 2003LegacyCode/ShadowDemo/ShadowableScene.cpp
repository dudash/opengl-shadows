#include <GL/glut.h>
#include "ShadowableScene.h"
#include "Model3D.h"

using std::string;
using std::vector;

namespace SML_CORE
{
// static constants defined
const int ShadowableScene::MAX_LIGHTS = 1;

//-----------------------------------------------------------------------------
/**
      Constructor
  */
ShadowableScene::ShadowableScene() :
drawLightsFlag(false),
drawShadowsFlag(true),
shadowCasterList(0),
shadowReceiverList(0),
normalList(0),
pointLightList(0)
{
   // the default light level
   float lmodelAmbient[] = { 0.4, 0.4, 0.4, 1.0 };
   glLightModelfv(GL_LIGHT_MODEL_AMBIENT, lmodelAmbient);
   glEnable(GL_LIGHTING);
}

//-----------------------------------------------------------------------------
/**
   Destructor
  */
ShadowableScene::~ShadowableScene()
{
}

//-----------------------------------------------------------------------------
/**
   Add a call list number to the scene, but I need to add with a unique id, 
   vector positon, and call list id.
  */
void ShadowableScene::addModel(Model3D* model, int mode)
{
   switch(mode)
   {
   case (NONE):
      normalList.push_back(model);
      break;
   case (RECEIVES_SHADOWS):
      shadowReceiverList.push_back(model);
      break;
   case (CASTS_SHADOWS):
      shadowCasterList.push_back(model);
      break;
   }
}

//-----------------------------------------------------------------------------
/**
   Remove a call list
  */
bool ShadowableScene::removeModel(string modelName)
{
   /// \todo loop through all the display lists and try to find the modelName
   if (removeModelFromList(modelName, shadowCasterList))
      return true;
   else if (removeModelFromList(modelName, shadowReceiverList))
      return true;
   else if (removeModelFromList(modelName, normalList))
      return true;

   return false;
}

//-----------------------------------------------------------------------------
/**
   Remove the argument model from the argument list
  */
bool ShadowableScene::removeModelFromList(string modelName, vector<Model3D*> &list)
{
   /// \todo loop through all the display lists and try to find the modelName
   Model3D *aModel;
   for (int index = 0; index < list.size(); index++)
   {
      aModel = list[index];
      if (aModel->getName() == modelName)
      {
         shadowCasterList.erase(&list[index]);
         return true;
      }
   }

   return false;
}

//-----------------------------------------------------------------------------
/**
  Render the scene
*/
void ShadowableScene::render()
{
   // update light positions and properties
   updateLights();

   // iterate through the normal list and display the geomerty
   renderModelList(normalList);

   // iterate  through the receiver list and display the geometry
   renderModelList(shadowReceiverList);

   // iterate  through the caster list and display the geometry
   renderModelList(shadowCasterList);

   // Draw the shadows
   if (drawShadowsFlag) drawShadows();
}

//-----------------------------------------------------------------------------
/**
  Render the model list to the screen
*/
void ShadowableScene::renderModelList(const vector<Model3D*> &modelList)
{
   Model3D *aModel;
   for (int index = 0; index < modelList.size(); index++)
   {
      aModel = modelList[index];
      glPushMatrix();

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

      if (!aModel->isLit())
      {
         // Set the color (for non lit scenes)
         glColor3f(aModel->getRed(),aModel->getGreen(),aModel->getBlue());
         glDisable(GL_LIGHTING);
      }
      else
      {
         // Set the material props (for lit scenes)
         Material tempMaterial = aModel->getMaterial();
         float matSpecular[] = {tempMaterial.specularRed, tempMaterial.specularGreen, tempMaterial.specularBlue, tempMaterial.specularAlpha};
         glMaterialfv(GL_FRONT, GL_SPECULAR, matSpecular);
         float matShininess[] = {tempMaterial.shininess};
         glMaterialfv(GL_FRONT, GL_SHININESS, matShininess);
         float matAmbDiff[] = { tempMaterial.ambientDiffuseRed, tempMaterial.ambientDiffuseGreen, tempMaterial.ambientDiffuseBlue, tempMaterial.ambientDiffuseAlpha };         
         glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, matAmbDiff);
         glEnable(GL_LIGHTING);
      }

      // draw the model
      glCallList(aModel->getCallListId());
      
      glPopMatrix();
   }
}

//-----------------------------------------------------------------------------
/**
   Add a point light source to the scene.

  @param x The x position of the light
  @param y The y position of the light
  @param z The z position of the light
*/
void ShadowableScene::addPointLightSource(float x, float y, float z)
{
   if (pointLightList.size() == MAX_LIGHTS)
      return;
   
   /// \todo determine how to do this for multiple sources
   float lightAmbient[] = { 0.3, 0.3, 0.3, 1.0};
   glLightfv(GL_LIGHT0, GL_AMBIENT, lightAmbient);
   float lightDiffuse[] = { 0.7, 0.5, 0.5, 1.0};
   glLightfv(GL_LIGHT0, GL_DIFFUSE, lightDiffuse);

   // add the light to our list of lights
   Vector3D *tempVector = new Vector3D(x,y,z);
   pointLightList.push_back(*tempVector);
   delete tempVector;

   /// \todo determine how to do this for multiple sources
   glEnable(GL_LIGHT0);
}

//-----------------------------------------------------------------------------
/**
   Add a point directional light source to the scene.

  @param x The x component of the dircetion vector
  @param y The y component of the dircetion vector
  @param z The z component of the dircetion vector
*/
void ShadowableScene::addDirectionalLightSource(float x, float y, float z)
{

}

//-----------------------------------------------------------------------------
/**
   Goto through all the light lists and set the light properties.
*/
void ShadowableScene::updateLights()
{
   /// \todo determine how to update an entire scene of lights
   for (int lightIndex=0; lightIndex<pointLightList.size(); lightIndex++)
   {
      Vector3D tempPosition = pointLightList[lightIndex];
      float pos[] = {tempPosition.x, tempPosition.y, tempPosition.z, 1.0};
      glLightfv(GL_LIGHT0, GL_POSITION, pos);

      // this draws a small yellow sphere at the light source
      if (drawLightsFlag)
      {
         glPushMatrix();
         glPushAttrib(GL_ALL_ATTRIB_BITS);
         glColor3f(1.0,1.0,0.0);
         glLineWidth(2.0);
         glDisable(GL_LIGHTING);
         glBegin(GL_LINES);
           glVertex3f(tempPosition.x, tempPosition.y, tempPosition.z);
           glVertex3f(tempPosition.x, 0.0, tempPosition.z);
         glEnd();
         glTranslatef(tempPosition.x, tempPosition.y, tempPosition.z);
         glColor4f(1.0,1.0,0.0,0.75);
         glutSolidSphere (1.0, 20, 16);
         glPopAttrib();
         glPopMatrix();
      }
   }
}

//-----------------------------------------------------------------------------
/**
   Routine that draws the models in the caster list onto the receiver list
   using a shadow projection algorithm.  Pure virtual and must be overridden
   in derrived classes.
*/
void ShadowableScene::drawShadows() {}
}