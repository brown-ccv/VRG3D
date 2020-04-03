/**
 * \author Nicholas Malbraaten
 *
 * \file  HaloLine.H
 * \brief A line utilizing depth-dependent halos
 */

#ifndef HALOLINE_H
#define HALOLINE_H

#include <VRG3D.h>
#include <VRBase.H>

typedef G3D::ReferenceCountedPointer<class HaloLineSet> HaloLineSetRef;

class HaloLineSet : public G3D::ReferenceCountedObject
{
public:
  HaloLineSet();
  // Creates a HaloLine from a vertex array
  HaloLineSet(const G3D::Array<G3D::Array<G3D::Vector3> > &verts);
  // Creates a HaloLine from a vertex array using given colors
  HaloLineSet(const G3D::Array<G3D::Array<G3D::Vector3> > &verts, const G3D::Array<G3D::Array<G3D::Color3> > &colors);
  HaloLineSet(const G3D::Array<G3D::Array<G3D::Vector3> > &verts, const G3D::Array<G3D::Array<G3D::Color3> > &colors, const G3D::Color3 &haloColor);
  HaloLineSet(const G3D::Array<G3D::Array<G3D::Vector3> > &verts, const G3D::Array<G3D::Array<G3D::Array<G3D::Color3> > > &colors);
  HaloLineSet(const G3D::Array<G3D::Array<G3D::Vector3> > &verts, const G3D::Array<G3D::Array<G3D::Array<G3D::Color3> > > &colors, const G3D::Color3 &haloColor);
  virtual ~HaloLineSet();

  virtual void draw(G3D::RenderDevice *rd);
  virtual void drawSubset(G3D::RenderDevice *rd, G3D::Array<G3D::Array<int> > subsetIndices);
  virtual void drawLine(G3D::RenderDevice *rd, int lineIndex);
  virtual void drawMotionLine(G3D::RenderDevice *rd, int lineIndex, int pointIndex, int lineLength=7);
  virtual void setupHaloShaders(G3D::RenderDevice *rd);
  virtual void tearDownHaloShaders(G3D::RenderDevice *rd);
  virtual void setLineColors(G3D::Array<G3D::Color3> &colors);
  virtual void setHaloColor(G3D::Color3 color);
  virtual void setHaloWidth(double width);

  virtual void incrementLineWidth();
  virtual void incrementLineWidth(double incr);
  virtual void decrementLineWidth();
  virtual void decrementLineWidth(double decr);
  virtual void incrementHalfWidth();
  virtual void incrementHalfWidth(double incr);
  virtual void decrementHalfWidth();
  virtual void decrementHalfWidth(double decr);
  virtual void incrementMaxDepth();
  virtual void incrementMaxDepth(double incr);
  virtual void decrementMaxDepth();
  virtual void decrementMaxDepth(double decr);

  virtual void setMaxDepth(double depth);
  virtual void setLineWidth(double width);
  virtual void setHalfWidth(double width);
  virtual double getMaxDepth();
  virtual double getLineWidth();
  virtual double getHalfWidth();

  virtual void incrementColorArray(G3D::RenderDevice *rd);
  virtual void setColorArray(G3D::RenderDevice *rd, int index);

private:
  G3D::Array<G3D::Vector3> m_vertices;
  G3D::Array<G3D::Array<G3D::Color3> > m_colors;
  G3D::Array<G3D::Array<G3D::Color3> > m_colorArr;
  G3D::Array<G3D::Vector3> m_normals;
  G3D::Array<G3D::Vector2> m_texCoords;
  G3D::Array<G3D::Array<int> > m_indices;
  G3D::VertexBufferRef m_varArea;
  G3D::VAR m_vertexVAR;
  G3D::VAR m_colorVAR;
  G3D::Array<G3D::VAR> m_colorVARArr;
  int m_colorArrIndex;
  G3D::VAR m_normalVAR;
  G3D::VAR m_texCoordVAR;
  G3D::Array<G3D::VAR> m_texCoordArr;
  G3D::ShaderRef m_shader;
  bool _shadersSet;
  bool _haloEnabled;

  // Shader parameters
  float _lineWidth;
  float _halfWidth;
  float _maxDepth;
  G3D::Color3 _haloColor;

  void computeNormal(G3D::Vector3 &normal, G3D::Vector3 vert1, G3D::Vector3 vert2, G3D::Vector3 vert3, G3D::Vector3 vert4);
  void buildLines(const G3D::Array<G3D::Array<G3D::Vector3> > &verts, const G3D::Array<G3D::Array<G3D::Color3> > &colors);
  void buildLines(const G3D::Array<G3D::Array<G3D::Vector3> > &verts, const G3D::Array<G3D::Array<G3D::Array<G3D::Color3> > > &colors);
  void setupShaders();
  void drawSetup(G3D::RenderDevice *rd);
  void drawTearDown(G3D::RenderDevice *rd);
};


#endif
