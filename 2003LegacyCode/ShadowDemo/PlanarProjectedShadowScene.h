#ifndef PLANARPROJECTEDSHADOWSCENE_H
#define PLANARPROJECTEDSHADOWSCENE_H
//-----------------------------------------------------------------------------
#include <vector>
#include "ShadowableScene.h"
#include "FTM.h"

namespace SML_CORE
{
// forward declare
class Model3D;

/**
  This class is a specalized ShadowableScene class that provides the ability
  to render projected planar shadows.

  @author Jason Dudash
*/
class PlanarProjectedShadowScene : public ShadowableScene
{
private:
   void renderModelListAsShadows(const std::vector<Model3D*> &modelList, FTM shadowMatrix);
   void drawShadows();
   FTM calculateShadowTransformation(float* projectionPlane, float* lightPosition);
   float* calculatePlaneFromPoints(float p0[3], float p1[3], float p2[3]);

public:
	PlanarProjectedShadowScene();
	virtual ~PlanarProjectedShadowScene();
};
}
#endif
