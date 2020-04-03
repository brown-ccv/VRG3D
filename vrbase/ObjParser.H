/**
   \author Marcus Ewert/Vamsi Konchada
   \file   ObjParser.H
   \brief A home brew OBJ parser that handles a subset of the OBJ spec.  
   This assumes each file has one model specified in it and 
   extracts the model's geometry only.
*/

#ifndef OBJPARSER_H
#define OBJPARSER_H

#include <VRBase.H>
#include "SMesh.H"
#include <fstream>
#include <vector>
#include <string>

typedef G3D::ReferenceCountedPointer<class ObjParser> ObjParserRef;

/**
*/
class ObjParser : public G3D::ReferenceCountedObject
{
public:
  static bool loadFromFile(const std::string &filename, 
                           G3D::Array<G3D::Vector3> &vertices, 
                           G3D::Array<int> &indices,
						   G3D::Array<G3D::Vector2> &textCoord,
                           G3D::Array<G3D::Color3> &vertColors,
                           double uniformScale=1.0);
  
  /// This function is for loading mesh files that contain normals. Note that it creates duplicate vertices
  /// because the file format specifies normals for each triangle. It is less efficient, but allows true flat shading.
  static bool loadFromFileWithNormals(const std::string &filename, 
                           G3D::Array<G3D::Vector3> &vertices, 
						   G3D::Array<G3D::Vector3> &normals,
                           G3D::Array<int> &indices,
						   G3D::Array<G3D::Vector2> &textCoord,
                           G3D::Array<G3D::Color3> &vertColors,
                           double uniformScale=1.0);

  static SMeshRef createSMeshFromFile(const std::string &filename,
                                      double uniformScale=1.0);
                                      
  static SMeshRef createSMeshFromFileWithNormals(const std::string &filename,
                                      double uniformScale=1.0);
                                      
  static SMeshRef createColoredSMeshFromFile(const std::string &filename,
				      G3D::Vector3 critVertex,
                                      double uniformScale=1.0);
private:
 static int tokenizeLine(const string& str,
                      vector<string>& tokens,
                      const string& delimiters = " ");
 
 // Currently only loads materials and diffuse color from a .mtl file
 static G3D::Table<std::string, G3D::Color3> loadMaterialsFromFile(const std::string &filename);
                                           
};


#endif

