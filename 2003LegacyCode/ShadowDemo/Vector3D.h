#ifndef VECTOR_3D
#define VECTOR_3D
//-----------------------------------------------------------------------------

namespace SML_CORE
{
/** 
  This class holds 3 floats and represents a position in space.

  @author Jason Dudash
*/
class Vector3D  
{
public:
   float x, y, z;

public:
   Vector3D();
	Vector3D(float x, float y, float z);
	virtual ~Vector3D();
};
}
#endif
