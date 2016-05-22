#ifndef MODEL3D_H
#define MODEL3D_H
//-----------------------------------------------------------------------------
#include <string>
#include <vector>
#include "Vector3D.h"
#include "Material.h"
#include "FTM.h"
#include "Face.h"
#include "UV.h"

namespace SML_CORE
{
/**
  This class holds all the information needed for the display and manipulation
  of a 3D Model.
*/
class Model3D  
{
private:
   bool textureLoaded;
   bool displayListCreated;
   bool useLighting;
   std::string myName;
   int callListId;
   int textureIndex;
   unsigned int* textureListPtr;
   float red, blue, green;
   Vector3D modelPosition;
   Material materialProps;
   FTM transformMatrix;
   FTM initialTransform;
   std::vector<Vector3D> vertList;   
   std::vector<Face> vertsFaceList;
   std::vector<Vector3D> normalList;
   std::vector<Face> normalsFaceList;
   std::vector<UV> uvList;

public:
   Model3D(std::string myName, int callListId, Vector3D position, bool useLight=true);
	virtual ~Model3D(); 
   int getCallListId() {return callListId;};
   bool isLit() {return useLighting;};
   float getRed() {return red;};
   float getGreen() {return green;};
   float getBlue() {return blue;};
   float* getPositionArray();
   std::string getName() {return myName;};
   Vector3D getPosition();
   FTM getFTM();
   Material getMaterial();
   void setPosition(Vector3D newPosition);
   void setPosition(float x, float y, float z);
   void setColor(float red, float green, float blue);
   void setMaterial(Material newMaterialProps);
   void setTransformMatrix(FTM newMatrix);
   void setInitialTransform(FTM newMatrix);
   void setTexture(int index, unsigned int* textureList);
   bool createOpenGLDisplayList();
   void moveModel(float xAmount, float yAmount, float zAmount);
   void rotateModel(int degrees, int xAxis, int yAxis, int zAxis); 
   void setVertList(std::vector<Vector3D> list) {vertList = list;};
   void setNormalList(std::vector<Vector3D> list) {normalList = list;};
   void setVertexFaceList(std::vector<Face> list) {vertsFaceList = list;};
   void setNormalFaceList(std::vector<Face> list) {normalsFaceList = list;};
   void setUVsList(std::vector<UV> list) {uvList = list;};
};
}
#endif
