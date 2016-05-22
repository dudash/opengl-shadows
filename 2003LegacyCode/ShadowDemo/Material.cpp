#include "Material.h"

namespace SML_CORE
{
//-----------------------------------------------------------------------------
/**
      Constructor
  */
Material::Material() :
   specularRed(1.0),
   specularBlue(1.0),
   specularGreen(1.0),
   specularAlpha(1.0),
   shininess(50.0),
   ambientDiffuseRed(0.6),
   ambientDiffuseGreen(0.5),
   ambientDiffuseBlue(0.4),
   ambientDiffuseAlpha(1.0)
{
}

//-----------------------------------------------------------------------------
/**
   Destructor
  */
Material::~Material()
{

}
}