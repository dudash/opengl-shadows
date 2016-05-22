#ifndef SHADOWABLESCENE_H
#define SHADOWABLESCENE_H
//-----------------------------------------------------------------------------
#include <vector>
#include <string>
#include "Vector3D.h"
#include "FTM.h"

namespace SML_CORE
{
// forward declarations
class Model3D;

/**
  This class is the base class for rendering shadowed scenes.  It provdies 
  common functionality that can be utilized by specalizing classes.
  Defined casting models will render shadows onto defined receiver models.

  @author Jason Dudash
*/
class ShadowableScene  
{
protected:
   static const int MAX_LIGHTS;
   bool drawLightsFlag;
   bool drawShadowsFlag;
   std::vector<Model3D*> shadowCasterList;
   std::vector<Model3D*> shadowReceiverList;
   std::vector<Model3D*> normalList;
   std::vector<Vector3D> pointLightList;

   bool removeModelFromList(std::string modelName, std::vector<Model3D*> &list);
   void renderModelList(const std::vector<Model3D*> &modelList);
   virtual void drawShadows() = 0;
   void updateLights();

public:
	ShadowableScene();
	virtual ~ShadowableScene();
   void addModel(Model3D *model, int shadowMode);
   bool removeModel(std::string modelName);
   void addPointLightSource(float x, float y, float z);
   void addDirectionalLightSource(float x, float y, float z);
   void render();
   void drawLights(bool mode) {drawLightsFlag = mode;};
   void turnOnShadows() {drawShadowsFlag = true;};
   void turnOffShadows() {drawShadowsFlag = false;};
   bool isDrawLightsOn() {return drawLightsFlag;};

   /** Every object added to the scene must have a ModelShadowMode,
       it determines which model list it is a part of.
   */
   enum ModelShadowMode
   {
      RECEIVES_SHADOWS,
      CASTS_SHADOWS,
      NONE
   };
};
}
#endif
