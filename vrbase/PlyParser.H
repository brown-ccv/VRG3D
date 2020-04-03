/**
   \author Dane Coffey
   \file   PlyParser.H
   
   \a simple PLY file format parser, reads in geometry and texture coordinates
*/

#ifndef PLYPARSER_H
#define PLYPARSER_H

#include <VRBase.H>
#include "SMesh.H"
#include <fstream>
#include "ply.h"


typedef G3D::ReferenceCountedPointer<class PlyParser> PlyParserRef;

/**
*/
class PlyParser : public G3D::ReferenceCountedObject
{
public:
  static bool loadFromFile(const std::string &filename, 
                           G3D::Array<G3D::Vector3> &vertices, 
                           G3D::Array<int> &indices,
						   G3D::Array<G3D::Vector2> &textCoord,
                           double uniformScale=1.0);
  
  static SMeshRef createSMeshFromFile(const std::string &filename,
                                      double uniformScale=1.0);
                                             
};


#endif
