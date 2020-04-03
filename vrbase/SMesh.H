/**
 * \author Daniel Keefe (dfk) / Trevor O'Brien
 *
 * \file  SMesh.H
 * \brief A simple triangle mesh
 */

#ifndef SMESH_H
#define SMESH_H


#include <VRG3D.h>
#include "GfxMgr.H"


typedef G3D::ReferenceCountedPointer<class SMesh> SMeshRef;
/**
 */
class SMesh : public G3D::ReferenceCountedObject
{
public:

  SMesh() {};

  /// Creates a mesh with no color info
  /// pass false to initVAR when overriding this constructor if you need to add
  /// data to the VARArea.
  /// If you pass false to initVAR you must, however, initialize the following:
  /// m_varArea
  /// m_vertexVAR
  /// m_normalVAR
  SMesh(const G3D::Array<G3D::Vector3> &verts, const G3D::Array<G3D::Vector3> &normals, 
        const G3D::Array<int> &indices, bool initVAR = true);

  //creates a textured mesh
  SMesh(const G3D::Array<G3D::Vector3> &verts, const G3D::Array<G3D::Vector3> &normals, 
        const G3D::Array<int> &indices, const G3D::Array<G3D::Vector2> &textureCoord, bool initVAR = true);

  /// Creates a mesh with per-vertex color info
  SMesh(const G3D::Array<G3D::Vector3> &verts, const G3D::Array<G3D::Vector3> &normals, 
        const G3D::Array<G3D::Color3> &colors, const G3D::Array<int> &indices);
        
    /// Creates a mesh with per-vertex color info and texture coords
  SMesh(const G3D::Array<G3D::Vector3> &verts, const G3D::Array<G3D::Vector3> &normals, 
        const G3D::Array<G3D::Color3> &colors, const G3D::Array<G3D::Vector2> &textureCoord, const G3D::Array<int> &indices);

  virtual ~SMesh();

  virtual void draw(G3D::RenderDevice *rd, int frame=0, VRBase::GfxMgrRef gfxMgr = NULL, G3D::RenderDevice::ShadeMode shadeMode = G3D::RenderDevice::SHADE_SMOOTH, bool outline=false, G3D::Color3 outlineColor=G3D::Color3::black(), bool ignoreMaterial = false);
  void drawWireFrame(G3D::RenderDevice *rd, int frame=0, VRBase::GfxMgrRef gfxMgr = NULL, bool ignoreMaterial = false);
  void drawFlatGeometry(G3D::RenderDevice *rd);

  void GetIndices(G3D::Array<int> &indices);
  void GetVertices(G3D::Array<G3D::Vector3> &vertices);
  void GetNormals(G3D::Array<G3D::Vector3> &normals);

  void GetAdjacencyArray(G3D::Array<G3D::MeshAlg::Face> &faces, G3D::Array<G3D::MeshAlg::Edge> &edges,
			 G3D::Array<G3D::MeshAlg::Vertex> &vertices);

  void GetMeshStats(double &minEdge, double &meanEdge, double &medianEdge,double &maxEdge,
		    double &minFaceArea, double &meanFaceArea, double &medianFaceArea,
		    double &maxFaceArea);

  G3D::Box GetBoundingBox(void);
  G3D::AABox GetAABoundingBox(void);
  G3D::Sphere GetBoundingSphere(void);

  double GetSurfaceArea(void);

  void SetVertices(const G3D::Array<G3D::Vector3> &newVerts);

  void PerformPCA(void);

  void GetCenter(G3D::Vector3 &center) {
    center = m_center;
  }

  void GetPrincComponent(double &mag, G3D::Vector3 &dir) {
    mag = m_princCompMag;
    dir = m_princCompDir;
  }
  
  /// Applies a coordinateframe transformation to the mesh.
  /// The transformation is done in object space with rotations around the object origin
  void transformMesh(G3D::CoordinateFrame f);

  bool Intersection(G3D::Ray r, float &iTime, G3D::Vector3 &iPoint);
  bool Intersection(G3D::Ray r, float &iTime, G3D::Vector3 &iPoint, G3D::Vector3 &iNormal);
  bool BacksideIntersection(G3D::Ray r, float &iTime, G3D::Vector3 &iPoint, G3D::Vector3 &iNormal);
  
  /// Returns negative distances for intersections in the -rayDir
  /// direction, returns 0.0 when there is no intersection.
  /// Applies the coordinate frames to the 2 meshes before making
  /// distance calculations, rayDir should be specified in world
  /// space (same coords as after applying the myFrame transformation).
  G3D::Array<double> CalcVertexDistsToOtherMesh(G3D::CoordinateFrame myFrame, 
                                           SMeshRef m2, G3D::CoordinateFrame frame2,
                                           G3D::Vector3 rayDir);

  void ApplyColoring(G3D::Array<G3D::Color3> &colors);
  /// returns true if per vertex colors are enabled and sets colors to the color array
  bool GetColoring(G3D::Array<G3D::Color3> &colors);
  void ApplyTexturing(G3D::Texture::Ref texture, G3D::Array<G3D::Vector2> &textureCoord, int
                      textureImageUnit=0);
  void ApplyTexturing(G3D::Texture::Ref texture, int textureImageUnit=0);
  void SetTexture(G3D::Texture::Ref texture, int textureImageUnit=0);
  void EnableTexture();
  void DisableTexture();


  G3D::Array<int>      m_indices;
  G3D::Array<G3D::Vector3>  m_vertices;
  G3D::Array<G3D::Vector3>  m_normals;
  G3D::Array<G3D::Color3>  m_colors;

  G3D::Table<int, G3D::Array<G3D::Vector2> >  m_textureCoord;

protected:

  G3D::AABox             *m_boundingBox;
  G3D::Sphere          *m_boundingSphere;
  G3D::Vector3         m_center;
  float           m_eigenVals[3];
  G3D::Vector3         m_eigenVecs[3];
  double          m_princCompMag;
  G3D::Vector3         m_princCompDir;

  bool            m_perVertexColor;
  bool		  m_bTextured;
  G3D::VertexBufferRef      m_varArea;
  G3D::VAR             m_normalVAR;
  G3D::VAR             m_colorVAR;
  G3D::VAR             m_vertexVAR;
  G3D::Table<int, G3D::VAR>             m_textureCoordVAR;
  G3D::Table<int, G3D::Texture::Ref>    m_textureRefs;
  bool           m_triTreeDirty;
  
  // For intersection tests
  G3D::Array<G3D::Tri>      m_triArray, m_backsideTriArray;
  G3D::TriTree         m_triTree, m_backsideTriTree;
  void BuildTriTree();
  
};

#endif

