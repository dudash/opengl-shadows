#ifndef XFILELOADER_H
#define XFILELOADER_H
//-----------------------------------------------------------------------------
#include <vector>
#include <string>
#include <iostream>
#include <fstream>
#include "Vector3D.h"
#include "FTM.h"
#include "Face.h"
#include "UV.h"

namespace SML_CORE
{
// forward declares
class Model3D;

/**
  This class contains the ability to load a Direct X file
  into memory.  It also provides aility to access the data
  of that file.

  This XFileLoader supports the following templates:
   Header, Frame, Mesh, MeshMaterialList, Material,
   MeshNormals, MeshTextureCoords, FrameTransformMatrix

  @author Jason Dudash
*/
class XFileLoader  
{
private:
   /**
      This class represents the orientation of the x,y,and z axis.
   */
   class Axis
   {  
   public:
      int upX;
      int upY;
      int upZ;
   };

   /**
      This class represents a section to the XFile.  The id is the type of
      section, the name is the name of the section (optional), and the dataBuffer
      is the remaining file that has been unparsed.
   */
   class XFileDataSection
   {
   public:
      std::string identifier;
      std::string name;
      std::string dataBuffer;
   };

   std::ifstream inFile;
   bool fileLoaded;
   std::vector<Vector3D> vertList;   
   std::vector<Face> faceList;
   std::vector<Vector3D> normalList;
   std::vector<Face> faceNormalsList;
   std::vector<UV> uvList;
   FTM frameTransform;
   Axis ourAxis;

   XFileDataSection readDataSection();
   XFileDataSection readDataSection(std::string &frameData);
   std::vector<Vector3D> readVertexData(std::string& vectorData);
   std::vector<Face> readFaceData(std::string& faceData);
   FTM readTransformMatrix(std::string& dataBuffer);
   char readChar(std::string &theString, bool skipWhitespace=false);
   std::string readTo(std::string &theString, char toThis, bool skipWhitepace=true);
   void handleHeader(std::string& headerData);
   void handleFrame(std::string& frameData);
	void handleMesh(std::string& meshData);
	void handleMeshUVs(std::string& uvData);
	void handleMeshNormals(std::string& normalData);

public:
	XFileLoader();
	virtual ~XFileLoader();
   bool loadXFile(std::string filename);
   bool createOpenGLDisplayList(int listId);
   bool createModel3D(Model3D* theModel);
};
}
#endif
