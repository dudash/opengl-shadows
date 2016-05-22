#ifndef MATERIAL_H
#define MATERIAL_H
//-----------------------------------------------------------------------------
namespace SML_CORE
{
/**
   This class defines material properties for openGL to use
   when determining how to shade a lit object
  */
class Material  
{
public:
	Material();
	virtual ~Material();

   float specularRed;
   float specularBlue;
   float specularGreen;
   float specularAlpha;
   float shininess;
   float ambientDiffuseRed;
   float ambientDiffuseGreen;
   float ambientDiffuseBlue;
   float ambientDiffuseAlpha;
};
}
#endif
