#include <GL/glut.h>
#include <iostream>
#include "Model3D.h"

using namespace std;

namespace SML_CORE
{
//-----------------------------------------------------------------------------
/**
      Constructor
  */
Model3D::Model3D(string aName, int callListid, Vector3D position, bool useLight) :
   textureLoaded(false),   
   useLighting(useLight), 
   displayListCreated(false),
   myName(aName),
   callListId(callListid),
   textureIndex(-1),
   red(1.0), blue(1.0), green(1.0),
   modelPosition(position),
   transformMatrix()
{

}

//-----------------------------------------------------------------------------
/**
   Destructor
  */
Model3D::~Model3D()
{

}

//-----------------------------------------------------------------------------
/**
   Return a vector of the position

  @return The position of this model in the world
  */
Vector3D Model3D::getPosition()
{
   return modelPosition;
}

//-----------------------------------------------------------------------------
/**
   Return a FTM copy of this model's transform matrix

  @return A copy of the material properties
  */
FTM Model3D::getFTM()
{
   return transformMatrix;
}

//-----------------------------------------------------------------------------
/**
   Return a material copy of this model's material properties

  @return A copy of the material properties
  */
Material Model3D::getMaterial()
{
   return materialProps;
}

//-----------------------------------------------------------------------------
/**
   Set the position of the model

  @param x X component of new position
  @param y Y component of new position
  @param z Z component of new position
  */
void Model3D::setPosition(float x, float y, float z)
{
   modelPosition.x = x;
   modelPosition.y = y;
   modelPosition.z = z;
}

//-----------------------------------------------------------------------------
/**
   Set the position of the model

  @param newPosition The new x,y,z position of the model
  */
void Model3D::setPosition(Vector3D newPosition)
{
   modelPosition = newPosition;
}

//-----------------------------------------------------------------------------
/**
   Set the color of the model if it's not textured

  @param red Component of color
  @param green Component of color
  @param blue Component of color
  */
void Model3D::setColor(float redC, float greenC, float blueC)
{
   red = redC;
   green = greenC;
   blue = blueC;
}

//-----------------------------------------------------------------------------
/**
   Set the material properties of this model

  @param newMaterial The new material properties
  */
void Model3D::setMaterial(Material newMaterial)
{
   materialProps = newMaterial;
}

//-----------------------------------------------------------------------------
/**
   Set the FTM of this model

  @param newMatrix The new FTM
  */
void Model3D::setTransformMatrix(FTM newMatrix)
{
   transformMatrix = newMatrix;
}

//-----------------------------------------------------------------------------
/**
   Set the FTM to be used when creating the openGL display list

  @param newMatrix The new initial transform
  */
void Model3D::setInitialTransform(FTM newMatrix)
{
   initialTransform = newMatrix;
}

//-----------------------------------------------------------------------------
/**
   Set the texture id.  This assumes that the texture has been loaded into 
   memory and has an index created.

  @param id index into the texture array
  */
void Model3D::setTexture(int index, unsigned int* textureList)
{
   textureLoaded = true;
   textureIndex = index;
   textureListPtr = textureList;
}

//-----------------------------------------------------------------------------
/**
   This operation loads the vert and face data into an openGL display list.  It also
   properly sets the normals for the surface and applys a texture if one has been specified.

  @param listId The id of the display list being created
  @return true if display list creation successful, false otherwise
*/
bool Model3D::createOpenGLDisplayList()
{   
 	glNewList(callListId, GL_COMPILE);
      glPushMatrix();
		glPushAttrib(GL_ALL_ATTRIB_BITS);

      // apply our local transformation matrix
      float tempMatrix[] = 
      { 
         initialTransform._00,initialTransform._01,initialTransform._02,initialTransform._03,
         initialTransform._10,initialTransform._11,initialTransform._12,initialTransform._13,
         initialTransform._20,initialTransform._21,initialTransform._22,initialTransform._23,
         initialTransform._30,initialTransform._31,initialTransform._32,initialTransform._33,
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
      if (vertsFaceList.size() == normalsFaceList.size())
      {
         if (textureLoaded) glEnable(GL_TEXTURE_2D);
         // Loop through the face list and draw a triangle or a 4 sided polygon
         // also map the texture coord list to each vertex
         for (int faceIndex=0; faceIndex<vertsFaceList.size(); faceIndex++)
         {
            Face currentFace = vertsFaceList[faceIndex];
            Face normalFace = normalsFaceList[faceIndex];
            if (textureLoaded) glBindTexture(GL_TEXTURE_2D, textureListPtr[textureIndex]);
            glBegin(GL_POLYGON);
               if (currentFace.numIndices == 3 || currentFace.numIndices == 4)
               {
                  glNormal3f(normalList[normalFace.one].x, normalList[normalFace.one].y, normalList[normalFace.one].z);
                  glTexCoord2f(uvList[currentFace.one].u,uvList[currentFace.one].v);
                  glVertex3f(vertList[currentFace.one].x, vertList[currentFace.one].y, vertList[currentFace.one].z);
                  glNormal3f(normalList[normalFace.two].x, normalList[normalFace.two].y, normalList[normalFace.two].z);
                  glTexCoord2f(uvList[currentFace.two].u,uvList[currentFace.two].v);
                  glVertex3f(vertList[currentFace.two].x, vertList[currentFace.two].y, vertList[currentFace.two].z);
                  glNormal3f(normalList[normalFace.three].x, normalList[normalFace.three].y, normalList[normalFace.three].z);
                  glTexCoord2f(uvList[currentFace.three].u,uvList[currentFace.three].v);
                  glVertex3f(vertList[currentFace.three].x, vertList[currentFace.three].y, vertList[currentFace.three].z);
                  if (currentFace.numIndices == 4)
                  {
                     glNormal3f(normalList[normalFace.four].x, normalList[normalFace.four].y, normalList[normalFace.four].z);
                     glTexCoord2f(uvList[currentFace.four].u,uvList[currentFace.four].v);
                     glVertex3f(vertList[currentFace.four].x, vertList[currentFace.four].y, vertList[currentFace.four].z);
                  }
               }
		      glEnd();
         }
         if (textureLoaded) glDisable(GL_TEXTURE_2D);
      }
      // else we have an invalid set of normals, just do the verts
      else
      {
         for (int faceIndex=0; faceIndex<vertsFaceList.size(); faceIndex++)
         {
            Face currentFace = vertsFaceList[faceIndex];
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
   displayListCreated = true;
   return true;
}

//-----------------------------------------------------------------------------
/**
   This operation rotates the local model transform matrix.  We load the models
   transform matrix, and then we rotate it, and then we get the matrix and set
   the transform matrix to that rotated version.

  Note: there's got to be a better way to do this

  @param degrees Amount to rotate
  @param xAxis rotate along this axis
  @param yAxis rotate along this axis
  @param ZAxis rotate along this axis
*/
void Model3D::rotateModel(int degrees, int xAxis, int yAxis, int zAxis)
{
   glPushMatrix();
   float tempMatrix[] = 
   { 
      transformMatrix._00,transformMatrix._01,transformMatrix._02,transformMatrix._03,
      transformMatrix._10,transformMatrix._11,transformMatrix._12,transformMatrix._13,
      transformMatrix._20,transformMatrix._21,transformMatrix._22,transformMatrix._23,
      transformMatrix._30,transformMatrix._31,transformMatrix._32,transformMatrix._33,
   };
   // load the models matrix and rotate it
   glLoadIdentity();
   glMultMatrixf(tempMatrix);
   glRotatef(degrees, xAxis, yAxis, zAxis);

   // now get the resultant matrix and load it into the models matrix
   float matrix[16];
   glGetFloatv(GL_MODELVIEW_MATRIX, matrix);
   transformMatrix._00 = matrix[0]; transformMatrix._01 = matrix[1]; transformMatrix._02 = matrix[2]; transformMatrix._03 = matrix[3];
   transformMatrix._10 = matrix[4]; transformMatrix._11 = matrix[5]; transformMatrix._12 = matrix[6]; transformMatrix._13 = matrix[7];
   transformMatrix._20 = matrix[8]; transformMatrix._21 = matrix[9]; transformMatrix._22 = matrix[10]; transformMatrix._23 = matrix[11];
   transformMatrix._30 = matrix[12]; transformMatrix._31 = matrix[13]; transformMatrix._32 = matrix[14]; transformMatrix._33 = matrix[15];   

   glPopMatrix();
}

//-----------------------------------------------------------------------------
/**
   This operation moves the local model transform matrix.  We load the models
   transform matrix, and then we translate it, and then we get the matrix and set
   the transform matrix to that translated version.

   \todo Update the models position instead of just translating the local axis

  @param xAmount Amount to move along axis
  @param yAmount Amount to move along axis
  @param zAmount Amount to move along axis
*/
void Model3D::moveModel(float xAmount, float yAmount, float zAmount)
{
   glPushMatrix();
   float tempMatrix[] = 
   { 
      transformMatrix._00,transformMatrix._01,transformMatrix._02,transformMatrix._03,
      transformMatrix._10,transformMatrix._11,transformMatrix._12,transformMatrix._13,
      transformMatrix._20,transformMatrix._21,transformMatrix._22,transformMatrix._23,
      transformMatrix._30,transformMatrix._31,transformMatrix._32,transformMatrix._33,
   };
   // load the models matrix and move it
   glLoadIdentity();
   glMultMatrixf(tempMatrix);
   glTranslatef(xAmount, yAmount, zAmount);

   // now get the resultant matrix and load it into the models matrix
   float matrix[16];
   glGetFloatv(GL_MODELVIEW_MATRIX, matrix);
   transformMatrix._00 = matrix[0]; transformMatrix._01 = matrix[1]; transformMatrix._02 = matrix[2]; transformMatrix._03 = matrix[3];
   transformMatrix._10 = matrix[4]; transformMatrix._11 = matrix[5]; transformMatrix._12 = matrix[6]; transformMatrix._13 = matrix[7];
   transformMatrix._20 = matrix[8]; transformMatrix._21 = matrix[9]; transformMatrix._22 = matrix[10]; transformMatrix._23 = matrix[11];
   transformMatrix._30 = matrix[12]; transformMatrix._31 = matrix[13]; transformMatrix._32 = matrix[14]; transformMatrix._33 = matrix[15];   

   glPopMatrix();
}
}