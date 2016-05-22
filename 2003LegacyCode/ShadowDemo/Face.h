#ifndef FACE_H
#define FACE_H
//-----------------------------------------------------------------------------
namespace SML_CORE
{
/**
   This class holds data used to represent a triangle or polygon face.
   (numIndices refers to the number of indicies, the other ints refer
   to the 4 vertex lookup indexes into the vertex array).
  */
class Face  
{
public:
   int numIndices;
   int one,two,three,four;
};
}
#endif
