#include <sstream>
#include <GL/glut.h>
#include "XFileLoader.h"
#include "Model3D.h"

using namespace std;

namespace SML_CORE
{
//-----------------------------------------------------------------------------
/**
   Constructor
*/
XFileLoader::XFileLoader() :
fileLoaded(false),
faceList(0),
vertList(0),
normalList(0),
uvList(0)
{

}

//-----------------------------------------------------------------------------
/**
   Destructor
*/
XFileLoader::~XFileLoader()
{

}

//-----------------------------------------------------------------------------
/**
   Handle section data of type header that is passed in as an argument

  @param headerData The data that we are dealing with
*/
void XFileLoader::handleHeader(string &headerData)
{
   string temp = "";
   temp = readTo(headerData,';');
   istringstream isX(temp);
   isX >> ourAxis.upX;
 
   temp = readTo(headerData,';');
   istringstream isY(temp);
   isY >> ourAxis.upY;
 
   temp = readTo(headerData,';');
   istringstream isZ(temp);
   isZ >> ourAxis.upZ;
}

//-----------------------------------------------------------------------------
/**
   Handle section data of type frame that is passed in as an argument

  @param dataFrame The data frame that we are dealing with
*/
void XFileLoader::handleFrame(string &dataFrame)
{
   // A frame usually contains nested templates so read the next section
   while(!dataFrame.empty())
   {
      XFileLoader::XFileDataSection dataSection = readDataSection(dataFrame);

      // recursive call to this function to handle nested frames
      if (dataSection.identifier == "Frame")
      {
         cout << "XFileLoader - handling Frame \"" << dataSection.name << "\"" << endl;
         handleFrame(dataSection.dataBuffer);
      }
      // transform this mesh and all child meshes
      else if (dataSection.identifier == "FrameTransformMatrix")
      {
         cout << "XFileLoader - handling FrameTransformMatrix \"" << dataSection.name << "\"" << endl;
         FTM tempFtm = readTransformMatrix(dataSection.dataBuffer);
         tempFtm.multMatrix(frameTransform);
         frameTransform = tempFtm;
      }
      // handle the mesh
      else if (dataSection.identifier == "Mesh")
      {
         cout << "XFileLoader - handling Mesh \"" << dataSection.name << "\"" << endl;
         handleMesh(dataSection.dataBuffer);
      }
   }
}

//-----------------------------------------------------------------------------
/**
   Handle the argument Mesh section.  The mesh section consists of 
   an array of verticies and an array of faces, it also has subsections

  @param meshData a reference to the data string we are readin from
*/
void XFileLoader::handleMesh(string &meshData)
{
   vertList = readVertexData(meshData);
   faceList = readFaceData(meshData);

   // A mesh usually contains nested templates so read the next section
   while(!meshData.empty())
   {
      XFileLoader::XFileDataSection dataSection = readDataSection(meshData);

      // handle mesh normal data
      if (dataSection.identifier == "MeshNormals")
      {
         cout << "XFileLoader - handling MeshNormals \"" << dataSection.name << "\"" << endl;
         handleMeshNormals(dataSection.dataBuffer);
      }
      // handle mesh material list
      else if (dataSection.identifier == "MeshMaterialList")
      {
         cout << "XFileLoader - cannot handle MeshMaterialList \"" << dataSection.name << "\"" << endl;
         /// \todo handle meshMaterialList (contains Material section)
      }
      // handle mesh texture coordinates
      else if (dataSection.identifier == "MeshTextureCoords")
      {
         cout << "XFileLoader - handling MeshTextureCoords \"" << dataSection.name << "\"" << endl;
         handleMeshUVs(dataSection.dataBuffer);
      }
   }
}

//-----------------------------------------------------------------------------
/**
   Read and handle the UV texture mapping coordinates for the argument
   mesh data.

   @param uvData Reference to the string of UV data
*/
void XFileLoader::handleMeshUVs(string &uvData)
{
   string numUVsStr = "";
   int c = readChar(uvData, true);
   while(c != ';' && c != -1)
   {
      numUVsStr +=(char)c;
      c = readChar(uvData);
   }

   istringstream is(numUVsStr);
   int numUVs = 0;
   is >> numUVs;

   string temp;
   UV tempUV;

   // read the verts
   for(int i = 0;i<numUVs;i++)
   {
      // read u
      temp = readTo(uvData,';');
      istringstream isU(temp);
      isU >> tempUV.u;

      // read v
      temp = readTo(uvData,';');
      istringstream isV(temp);
      isV >> tempUV.v;

      if(i != numUVs-1)
      {
         // read trailing ','
         temp = readTo(uvData,',');
      }
      else
      {
         // read a trailing ; at the very end
         temp = readTo(uvData,';');
      }
      uvList.push_back(tempUV);
   }
}

//-----------------------------------------------------------------------------
/**
   Handle the normals processing for the argument mesh data

  @param normalData A reference to the data we are reading from
*/
void XFileLoader::handleMeshNormals(string &normalData)
{
   normalList = readVertexData(normalData);
   faceNormalsList = readFaceData(normalData);
}

//-----------------------------------------------------------------------------
/**
   Read in a data section from the loaded .x file file stream.

   Note: for efficency this should probably not return by value 

   @return The next data section in the file
*/
XFileLoader::XFileDataSection XFileLoader::readDataSection()
{ 
   XFileDataSection tempSection;
   tempSection.identifier = "";
   tempSection.name = "";
   tempSection.dataBuffer = "";

   int c = inFile.get();
   bool found = false;
   while((!found || c != ' ') && !inFile.eof())
   {
      if(c != ' ' && c != '\n' && c != 13)
      {
         found = true;
         tempSection.identifier += (char)c;
      }
      c = inFile.get();
   }

   // read the section name
   while(c != '{' && !inFile.eof())
   {
      if(c != ' ' && c != '\n' && c != 13)
      {
         tempSection.name += (char)c;
      }
      c = inFile.get();
   }

   // read the section data between the {}'s
   int bracecount = 1;
   c = inFile.get();
   while(bracecount > 0 && !inFile.eof())
   {
      // handle counting brace depth
      if(c == '{') bracecount += 1;
      else if(c == '}') bracecount -= 1;

      // ignore last brace
      if(bracecount > 0)
      {
         tempSection.dataBuffer += (char)c;
         c = inFile.get();
      }
   }

   return tempSection;
}

//-----------------------------------------------------------------------------
/**
   Read the argument data section into its parts

   Note: for efficency this should probably not return by value

  @param dataFrame The data section to read
  @return The next data section from the given frame
*/
XFileLoader::XFileDataSection XFileLoader::readDataSection(string& dataFrame)
{ 
   XFileDataSection tempSection;
   tempSection.identifier = "";
   tempSection.name = "";
   tempSection.dataBuffer = "";

   int c = readChar(dataFrame);
   bool found = false;
   while((!found || c != ' ') && !dataFrame.empty())
   {
      if(c != ' ' && c != '\n' && c != 13)
      {
         found = true;
         tempSection.identifier += (char)c;
      }
      c = readChar(dataFrame);
   }

   // read the section name
   while(c != '{' && !dataFrame.empty())
   {
      if(c != ' ' && c != '\n' && c != 13)
      {
         tempSection.name += (char)c;
      }
      c = readChar(dataFrame);
   }

   // read the section data between the {}'s
   int bracecount = 1;
   c = readChar(dataFrame);
   while(bracecount > 0 && !dataFrame.empty())
   {
      // handle counting brace depth
      if(c == '{') bracecount += 1;
      else if(c == '}') bracecount -= 1;

      // ignore last brace
      if(bracecount > 0)
      {
         tempSection.dataBuffer += (char)c;
         c = readChar(dataFrame);
      }
   }
   return tempSection;
}

//-----------------------------------------------------------------------------
/**
   Read the vert list from the argument string data.  The vector data is assumed
   to be the first section of the string, and in the format: number of points
   followed by array of points (p1;p2;p3,p1;p2;p3,p1;p2...etc).

   Note: for efficency this should probably not return a vector by value

  @param theString a reference to the string to read
  @return The list of verts
*/
vector<Vector3D> XFileLoader::readVertexData(string& vertexData)
{
   string numVertsStr = "";
   int c = readChar(vertexData, true);
   while(c != ';' && c != -1)
   {
      numVertsStr += (char)c;
      c = readChar(vertexData);
   }

   istringstream is(numVertsStr);
   int numVerts = 0;
   is >> numVerts;
   vector<Vector3D> tempVertList;
   string temp;
   Vector3D tempVector;

   // read the verts
   for(int i = 0;i<numVerts;i++)
   {
      // read x
      temp = readTo(vertexData,';');
      istringstream isx(temp);
      isx >> tempVector.x;

      // read y
      temp = readTo(vertexData,';');
      istringstream isy(temp);
      isy >> tempVector.y;

      // read z
      temp = readTo(vertexData,';');
      istringstream isz(temp);
      isz >> tempVector.z;

      if(i != numVerts-1)
      {
         // read trailing ','
         temp = readTo(vertexData,',');
      }
      else
      {
         // read a trailing ; at the very end
         temp = readTo(vertexData,';');
      }
      tempVertList.push_back(tempVector);
   }

   return tempVertList;
}

//-----------------------------------------------------------------------------
/**
   Read the vert list from the argument string data.  The face data is assumed
   to be the first section of the string, and in the format: number of faces
   followed by array of faces (f1;f2;f3,f1;f2;f3,f1;f2...etc).

   Note: for efficency this should probably not return a vector by value

  @param theString pointer to the string to read
  @return A List of faces (indices into the vert list)
*/
vector<Face> XFileLoader::readFaceData(std::string& faceData)
{
   string numFacesStr = "";
   int c = readChar(faceData, true);
   while(c != ';' && c != -1)
   {
      numFacesStr += (char)c;
      c = readChar(faceData);
   }

   int numFaces = 0;
   istringstream is(numFacesStr);
   is >> numFaces;
   vector<Face> tempFaceList;
   string temp;
   Face tempFace;

   // read the faces
   for(int i = 0;i<numFaces;i++)
   {
      // read index count
      temp = readTo(faceData,';');
      istringstream isNum(temp);
      isNum >> tempFace.numIndices;
      if(tempFace.numIndices != 3 && tempFace.numIndices != 4)
      {
         cout << "XFileLoader - ERROR! reader doesn't support file (too face faces in mesh)" << endl;
         /// \todo add exception handling and throw error
      }

      // read indices
      temp = readTo(faceData,',');
      istringstream isOne(temp);
      isOne >> tempFace.one;

      temp = readTo(faceData,',');
      istringstream isTwo(temp);
      isTwo >> tempFace.two;

      // 3rd vert index might be capped with a ','  or a ';'
      if(tempFace.numIndices == 4)
      {
         temp = readTo(faceData,',');
      }
      else
      {
         temp = readTo(faceData,';');
      }
      istringstream isThree(temp);
      isThree >> tempFace.three;

      // optional 4th vert capped with a ;
      if(tempFace.numIndices == 4)
      {
         temp = readTo(faceData,';');
         istringstream isFour(temp);
         isFour >> tempFace.four;
      }

      if(i != numFaces-1)
      {
         // read a trailing ','
         temp = readTo(faceData,',');
      }
      else
      {
         // read a trailing ';' at the very end
         temp = readTo(faceData,';');
      }

      tempFaceList.push_back(tempFace);
   }

   return tempFaceList;
}

//-----------------------------------------------------------------------------
/**
   Read a FTM from the argument string data.  The FTM data is assumed
   to be the first section of the string.

   Note: for efficency this should probably not return a vector by value

  @param dataBuffer The reference to the data to read from
  @return The frame transformation matrix read in
*/
FTM XFileLoader::readTransformMatrix(std::string& dataBuffer)
{
   FTM tempFTM;
   string temp = "";

   temp = readTo(dataBuffer,',');
   istringstream is1(temp);
   is1 >> frameTransform._00;
   temp = readTo(dataBuffer,',');
   istringstream is2(temp);
   is2 >> frameTransform._01;
   temp = readTo(dataBuffer,',');
   istringstream is3(temp);
   is3 >> frameTransform._02;
   temp = readTo(dataBuffer,',');
   istringstream is4(temp);
   is4 >> frameTransform._03;

   temp = readTo(dataBuffer,',');
   istringstream is5(temp);
   is5 >> frameTransform._10;
   temp = readTo(dataBuffer,',');
   istringstream is6(temp);
   is6 >> frameTransform._11;
   temp = readTo(dataBuffer,',');
   istringstream is7(temp);
   is7 >> frameTransform._12;
   temp = readTo(dataBuffer,',');
   istringstream is8(temp);
   is8 >> frameTransform._13;

   temp = readTo(dataBuffer,',');
   istringstream is9(temp);
   is9 >> frameTransform._20;
   temp = readTo(dataBuffer,',');
   istringstream is10(temp);
   is10 >> frameTransform._21;
   temp = readTo(dataBuffer,',');
   istringstream is11(temp);
   is11 >> frameTransform._22;
   temp = readTo(dataBuffer,',');
   istringstream is12(temp);
   is12 >> frameTransform._23;

   temp = readTo(dataBuffer,',');
   istringstream is13(temp);
   is13 >> frameTransform._30;
   temp = readTo(dataBuffer,',');
   istringstream is14(temp);
   is14 >> frameTransform._31;
   temp = readTo(dataBuffer,',');
   istringstream is15(temp);
   is15 >> frameTransform._32;
   temp = readTo(dataBuffer,';');
   istringstream is16(temp);
   is16 >> frameTransform._33;

   return tempFTM;
}

//-----------------------------------------------------------------------------
/**
   Remove and return the first character of the string.  returns -1 if the
   string is empty.  The optional skip whitespace argument allows us to 
   read until a non-white char is found.

  @param theString Pointer to the string to read
  @param skipWhitespace if true skip all whitespace (default=false)
  @return The first character in the string
*/
char XFileLoader::readChar(string& theString, bool skipWhitespace)
{ 
   if (theString.empty())
      return -1;
   char c = theString[0];
   theString.erase(0, 1);

   // recursively call this function until we get a non-white char
   while ((c == ' ' || c == '\n' || c == 13)&&skipWhitespace)
   {
      c = readChar(theString, true);
   }
   return c;
}

//-----------------------------------------------------------------------------
/**
  Read through a string until we find the argument char, then return the
  string that we read.

  @param theString Pointer to the string to read
  @param target What to read to
  @param skipWhitespace if true skip all whitespace (default=true)
  @return The string we have read in
*/
string XFileLoader::readTo(string& theString, char target, bool skipWhitespace)
{ 
   int c = readChar(theString);
   string buffer = "";
   while(c != target && c != -1)
   {
      if(!skipWhitespace || (c!= ' ' && c != '\n' && c != 13))
      {
         buffer+=(char)c;
      }
      c = readChar(theString);
   }   
   return buffer;
}

//-----------------------------------------------------------------------------
/**
   This is the main operation of the class.  It is called with an argument
   string XFile filename to open/load/and store in the class.

  @param filename The location of the .x file to load
  @return true if successful, false otherwise
*/
bool XFileLoader::loadXFile(string filename)
{
   // clean out the member data in case this is another load
   //fileLoaded = false;
   //faceList.clear();
   //vertList.clear();
   //normalList.clear();
   //uvList.clear();
   //frameTransform.loadIdentity();

   inFile.open(filename.c_str(), ifstream::in);
   if (!inFile)
   {
      cout << "XFileLoader - could not open file \"" << filename << "\"" << endl;
      return false;
   }

   // check the header to see if it is a supported file
   char* header = new char[17];
   string validHeader = "xof 0302txt 0032";
   inFile.getline(header, 17);
   if (header!=validHeader)
   {
      cout << "XFileLoader - invalid xfile header" << endl;
      return false;
   }
   cout << "XFileLoader - file is valid" << endl;

   while(!inFile.eof())
   {
      // load in a data section of the file
      XFileLoader::XFileDataSection dataSection = readDataSection();

      // handle a header section
      if (dataSection.identifier == "Header")
      {
         cout << "XFileLoader - handling top level Header \"" << dataSection.name << "\"" << endl;
         handleHeader(dataSection.dataBuffer);
      }

      // handle a frame section
      else if (dataSection.identifier == "Frame")
      {
         cout << "XFileLoader - handling top level Frame \"" << dataSection.name << "\"" << endl;
         handleFrame(dataSection.dataBuffer);
      }
   }
   inFile.close();
   fileLoaded = true;
   cout << "XFileLoader - sucessfully loaded \"" << filename << "\"" << endl << endl;;
   return fileLoaded;
}

//-----------------------------------------------------------------------------
/**
   This operation loads the x file data into an openGL display list.
   If there hasn't been a successful load then it returns false.

  @param listId The id of the display list being created
  @return true if display list creation successful, false otherwise
*/
bool XFileLoader::createOpenGLDisplayList(int listId)
{
   if (!fileLoaded) return false;
   
 	glNewList(listId, GL_COMPILE);
      glPushMatrix();
		glPushAttrib(GL_ALL_ATTRIB_BITS);

      // apply our local transformation matrix
      float tempMatrix[] = 
      { 
         frameTransform._00,frameTransform._01,frameTransform._02,frameTransform._03,
         frameTransform._10,frameTransform._11,frameTransform._12,frameTransform._13,
         frameTransform._20,frameTransform._21,frameTransform._22,frameTransform._23,
         frameTransform._30,frameTransform._31,frameTransform._32,frameTransform._33,
      };
      glMultMatrixf(tempMatrix);

      //KLUDGE - Align the axis here to fit with my hardcoded "Y is up" world
      /// \todo replace this with a dynamic method of orienting the model to my world.
      // Assuming 1x-0y-1z model, since thats all I have right now
      //translate to center the model
      glTranslatef(0,1.5,-7);
      // rotate axis
      glRotatef(-90,0,1,0);
      glRotatef(-90,1,0,0);
      // END KLUDGE

      // test to see if normals match up with the verts
      if (faceList.size() == faceNormalsList.size())
      {
         for (int faceIndex=0; faceIndex<faceList.size(); faceIndex++)
         {
            Face currentFace = faceList[faceIndex];
            Face normalFace = faceNormalsList[faceIndex];
            glBegin(GL_POLYGON);
               if (currentFace.numIndices == 3 || currentFace.numIndices == 4)
               {
                  glNormal3f(normalList[normalFace.one].x, normalList[normalFace.one].y, normalList[normalFace.one].z);
                  glVertex3f(vertList[currentFace.one].x, vertList[currentFace.one].y, vertList[currentFace.one].z);
                  glNormal3f(normalList[normalFace.two].x, normalList[normalFace.two].y, normalList[normalFace.two].z);
                  glVertex3f(vertList[currentFace.two].x, vertList[currentFace.two].y, vertList[currentFace.two].z);
                  glNormal3f(normalList[normalFace.three].x, normalList[normalFace.three].y, normalList[normalFace.three].z);
                  glVertex3f(vertList[currentFace.three].x, vertList[currentFace.three].y, vertList[currentFace.three].z);
                  if (currentFace.numIndices == 4)
                  {
                     glNormal3f(normalList[normalFace.four].x, normalList[normalFace.four].y, normalList[normalFace.four].z);
                     glVertex3f(vertList[currentFace.four].x, vertList[currentFace.four].y, vertList[currentFace.four].z);
                  }
               }
		      glEnd();
         }
      }
      // else we have an invalid set of normals
      else
      {
         cout << "XFileLoader - Face normals do not match the verts! "
              << "Faces="<<faceList.size()<<", Normals="<<faceNormalsList.size()
              << ". Ignoring normal data." << endl;
         for (int faceIndex=0; faceIndex<faceList.size(); faceIndex++)
         {
            Face currentFace = faceList[faceIndex];
            glBegin(GL_POLYGON);
               if (currentFace.numIndices == 3 || currentFace.numIndices == 4)
               {
                  glVertex3f(vertList[currentFace.one].x, vertList[currentFace.one].y, vertList[currentFace.one].z);
                  glVertex3f(vertList[currentFace.two].x, vertList[currentFace.two].y, vertList[currentFace.two].z);
                  glVertex3f(vertList[currentFace.three].x, vertList[currentFace.three].y, vertList[currentFace.three].z);
                  if (currentFace.numIndices == 4)
                     glVertex3f(vertList[currentFace.four].x, vertList[currentFace.four].y, vertList[currentFace.four].z);
               }
		      glEnd();
         }
      }

		glPopAttrib();
		glPopMatrix();
	glEndList();  
   return true;
}

//-----------------------------------------------------------------------------
/**
   This operation loads the x file data into a Model3D object.
   If there hasn't been a successful load then it returns false

  @param theModel A reference to the model we are loading
  @return true if display list creation successful, false otherwise
*/
bool XFileLoader::createModel3D(Model3D* theModel)
{
   if (!fileLoaded) return false;
   
   theModel->setInitialTransform(frameTransform);
   theModel->setVertList(vertList);
   theModel->setNormalList(normalList);
   theModel->setVertexFaceList(faceList);
   theModel->setNormalFaceList(faceNormalsList);
   theModel->setUVsList(uvList);

   return true;
}
}