#ifndef FTM_H
#define FTM_H
//-----------------------------------------------------------------------------
namespace SML_CORE
{
/**
  This class holds data that represents a frame transformation matrix.
*/
class FTM  
{
public:
   float _00,_01,_02,_03;
   float _10,_11,_12,_13;
   float _20,_21,_22,_23;
   float _30,_31,_32,_33;

public:
	FTM();
	virtual ~FTM();
   FTM multMatrix(FTM martix);
   void loadIdentity();
};
}
#endif
