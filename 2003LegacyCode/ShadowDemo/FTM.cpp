#include "FTM.h"

namespace SML_CORE
{
//-----------------------------------------------------------------------------
/**
   Construct
*/
FTM::FTM()
{
   loadIdentity();
}

//-----------------------------------------------------------------------------
/**
   Destroy
*/
FTM::~FTM()
{

}

//-----------------------------------------------------------------------------
/**
   Transform this matrix by the argument matrix

  @param matrix The transformation matrix
  @return The newly transformed matrix
*/
FTM FTM::multMatrix(FTM matrix)
{
   FTM temp;
   temp._00 = _00*matrix._00 + _01*matrix._10 + _02*matrix._20 + _03*matrix._30;
   temp._01 = _00*matrix._01 + _01*matrix._11 + _02*matrix._21 + _03*matrix._31;
   temp._02 = _00*matrix._02 + _01*matrix._12 + _02*matrix._22 + _03*matrix._32;
   temp._03 = _00*matrix._03 + _01*matrix._13 + _02*matrix._23 + _03*matrix._33;

   temp._10 = _10*matrix._00 + _11*matrix._10 + _12*matrix._20 + _13*matrix._30;
   temp._11 = _10*matrix._01 + _11*matrix._11 + _12*matrix._21 + _13*matrix._31;
   temp._12 = _10*matrix._02 + _11*matrix._12 + _12*matrix._22 + _13*matrix._32;
   temp._13 = _10*matrix._03 + _11*matrix._13 + _12*matrix._23 + _13*matrix._33;

   temp._20 = _20*matrix._00 + _21*matrix._10 + _22*matrix._20 + _23*matrix._30;
   temp._21 = _20*matrix._01 + _21*matrix._11 + _22*matrix._21 + _23*matrix._31;
   temp._22 = _20*matrix._02 + _21*matrix._12 + _22*matrix._22 + _23*matrix._32;
   temp._23 = _20*matrix._03 + _21*matrix._13 + _22*matrix._23 + _23*matrix._33;

   temp._30 = _30*matrix._00 + _31*matrix._10 + _32*matrix._20 + _33*matrix._30;
   temp._31 = _30*matrix._01 + _31*matrix._11 + _32*matrix._21 + _33*matrix._31;
   temp._32 = _30*matrix._02 + _31*matrix._12 + _32*matrix._22 + _33*matrix._32;
   temp._33 = _30*matrix._03 + _31*matrix._13 + _32*matrix._23 + _33*matrix._33;
  
   return temp;
}

//-----------------------------------------------------------------------------
/**
   Set the matrix to the identity matrix
*/
void FTM::loadIdentity()
{
   // Setup the default matrix
   _00 = 1; _01 = 0; _02 = 0; _03 = 0;
   _10 = 0; _11 = 1; _12 = 0; _13 = 0;
   _20 = 0; _21 = 0; _22 = 1; _23 = 0;
   _30 = 0; _31 = 0; _32 = 0; _33 = 1;
}
}