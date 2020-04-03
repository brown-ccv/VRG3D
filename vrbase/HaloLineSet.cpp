#include "HaloLineSet.H"

using namespace G3D;

#define LINE_WIDTH_INCREMENT 0.001
#define HALF_WIDTH_INCREMENT 0.01
#define MAX_DEPTH_INCREMENT 0.000001

HaloLineSet::HaloLineSet()
{

}

HaloLineSet::HaloLineSet(const Array<Array<Vector3> > &verts)
{
  Array<Array<Color3> > colors;
  for(int i = 0; i < verts.size(); i++) {
    colors.append(Array<Color3>());
    Color3 color((float)rand() / RAND_MAX, (float)rand() / RAND_MAX, (float)rand() / RAND_MAX);
    for(int j = 0; j < verts[i].size(); j++) {
      colors.last().append(color);
    }
  }
  buildLines(verts, colors);
  setupShaders();
  setHaloColor(Color3(0.0, 0.0, 0.0));
  _haloEnabled = true;
}

HaloLineSet::HaloLineSet(const Array<Array<Vector3> > &verts, const Array<Array<Color3> > &colors)
{
  buildLines(verts, colors);
  setupShaders();
  setHaloColor(Color3(0.0, 0.0, 0.0));
  _haloEnabled = true;
}

HaloLineSet::HaloLineSet(const Array<Array<Vector3> > &verts, const Array<Array<Color3> > &colors, const Color3 &haloColor)
{
  buildLines(verts, colors);
  setupShaders();
  setHaloColor(haloColor);
  _haloEnabled = true;
}

HaloLineSet::HaloLineSet(const Array<Array<Vector3> > &verts, const Array<Array<Array<Color3> > > &colors) {
  buildLines(verts, colors);
  setupShaders();
  setHaloColor(Color3(0.0, 0.0, 0.0));
  _haloEnabled = true;
}

HaloLineSet::HaloLineSet(const Array<Array<Vector3> > &verts, const Array<Array<Array<Color3> > > &colors, const Color3 &haloColor) {
  buildLines(verts, colors);
  setupShaders();
  setHaloColor(haloColor);
  _haloEnabled = true;
}

HaloLineSet::~HaloLineSet()
{
  VARArea::cleanupAllVARAreas();
}

void HaloLineSet::setupShaders() {
  string defaultShaderDir = G3D::System::getEnv("G");
  defaultShaderDir += "/src/vrbase/haloShaders/";
  string shaderDir = ConfigVal("shaderDir", defaultShaderDir, false);

  m_shader = Shader::fromFiles(shaderDir + "shader.vert", shaderDir + "shader.frag");
  cout << m_shader->messages();

  // original line width
  // _lineWidth = 0.005;
  _lineWidth = 0.011;
  _halfWidth = 0.25;
  _maxDepth = 0.000001;

  m_shader->args.set("lineWidth", _lineWidth);
  m_shader->args.set("halfWidth", _halfWidth);
  m_shader->args.set("maxDepth", _maxDepth);
}

void HaloLineSet::setHaloColor(Color3 haloColor) {
  _haloColor = haloColor;
}


void
HaloLineSet::buildLines(const Array<Array<Vector3> > &verts, const Array<Array<Color3> > &colors )
{
  int count = 0;
  for(int i = 0; i < verts.size(); i++) {
    count += verts[i].size();
  }

  m_colorArr.append(Array<Color3>());

  for(int i = 0; i < verts.size(); i++) {
    //m_points.append(Array<DLPt3>());
    int numPoints = verts[i].size();
    for(int j = 0; j < numPoints; j++) {
      // We have to duplicate each point for use in the vertex and fragment shaders
      m_vertices.append(verts[i][j]);
      m_vertices.append(verts[i][j]);
      //m_points.last().append(point);
      
      // append empty vectors to the normal array to pass into computeNormal below
      m_normals.append(Vector3());
      m_normals.append(Vector3());

      // append line colors
      m_colorArr[0].append(colors[i][j]);
      m_colorArr[0].append(colors[i][j]);

      // Append texture coordinates (used in the fragment shader to determine depth and color)
      m_texCoords.append(Vector2(0.0, 0.0));
      m_texCoords.append(Vector2(0.0, 1.0));
    }
  }

  // Create the index array.  
  int vertCount = 0;
  for(int i = 0; i < verts.size(); i++) {
    int numPoints = verts[i].size();
    //cout << "numPoints = " << numPoints << endl;

    computeNormal(m_normals[vertCount], m_vertices[vertCount], m_vertices[vertCount+2], m_vertices[vertCount+2], m_vertices[vertCount+4]);
    computeNormal(m_normals[vertCount+1], m_vertices[vertCount+1], m_vertices[vertCount+3], m_vertices[vertCount+3], m_vertices[vertCount+5]);
    computeNormal(m_normals[vertCount+2], m_vertices[vertCount], m_vertices[vertCount+2], m_vertices[vertCount+2], m_vertices[vertCount+4]);
    computeNormal(m_normals[vertCount+3], m_vertices[vertCount+1], m_vertices[vertCount+3], m_vertices[vertCount+3], m_vertices[vertCount+5]);
    for(int j = 4; j < 2*numPoints - 4; j += 2) {
      computeNormal(m_normals[vertCount+j], m_vertices[vertCount+j-2], m_vertices[vertCount+j], m_vertices[vertCount+j], m_vertices[vertCount+j+2]);
      computeNormal(m_normals[vertCount+j+1], m_vertices[vertCount+j-2], m_vertices[vertCount+j], m_vertices[vertCount+j], m_vertices[vertCount+j+2]);
    }
    computeNormal(m_normals[vertCount+numPoints - 2], m_vertices[vertCount+numPoints - 6], m_vertices[vertCount+numPoints-4], m_vertices[vertCount+numPoints-4], m_vertices[vertCount+numPoints-2]);
    computeNormal(m_normals[vertCount+numPoints - 1], m_vertices[vertCount+numPoints - 5], m_vertices[vertCount+numPoints-3], m_vertices[vertCount+numPoints-3], m_vertices[vertCount+numPoints-1]);
 
    m_indices.append(Array<int>());
    for(int j = 0; j < 2*numPoints; j++) {
      m_indices[i].append(vertCount+j);
    }
    vertCount += 2*numPoints;
  } 

  size_t sizeNeeded = 8 + sizeof(Vector3)*m_vertices.size() + 8 + sizeof(G3D::Color3)*m_colorArr[0].size() + 8 + sizeof(G3D::Vector3)*m_normals.size() + 8 + sizeof(G3D::Vector2)*m_texCoords.size();


  //if(m_varArea.isNull()) {
    m_varArea = VARArea::create(sizeNeeded, VARArea::WRITE_ONCE);
    if (m_varArea.isNull()) {
      cerr << "Error: Out of VARArea room!" << endl;
    }
  //}

  //if(!m_varArea.isNull()) {
    m_varArea->reset();
    m_vertexVAR = VAR(m_vertices, m_varArea);
    m_colorVARArr.append(VAR(m_colorArr[0], m_varArea));
    m_normalVAR = VAR(m_normals, m_varArea);
    m_texCoordVAR = VAR(m_texCoords, m_varArea);
    m_texCoordArr.push(m_texCoordVAR);
  //}
    m_colorArrIndex = 0;
}

//Handles multiple color arrays
void
HaloLineSet::buildLines(const Array<Array<Vector3> > &verts, const Array<Array<Array<Color3> > > &colors )
{
  int count = 0;
  for(int i = 0; i < verts.size(); i++) {
    count += verts[i].size();
  }

  for(int i = 0; i < colors.size(); i++) {
    m_colorArr.append(Array<Color3>());
  }

  for(int i = 0; i < verts.size(); i++) {
    //m_points.append(Array<DLPt3>());
    int numPoints = verts[i].size();
    for(int j = 0; j < numPoints; j++) {
      // We have to duplicate each point for use in the vertex and fragment shaders
      m_vertices.append(verts[i][j]);
      m_vertices.append(verts[i][j]);
      //m_points.last().append(point);
      
      // append empty vectors to the normal array to pass into computeNormal below
      m_normals.append(Vector3());
      m_normals.append(Vector3());

      // append line colors
      for(int k = 0; k < colors.size(); k++) {
	m_colorArr[k].append(colors[k][i][j]);
	m_colorArr[k].append(colors[k][i][j]);
      }

      // Append texture coordinates (used in the fragment shader to determine depth and color)
      m_texCoords.append(Vector2(0.0, 0.0));
      m_texCoords.append(Vector2(0.0, 1.0));
    }
  }

  // Create the index array.  
  int vertCount = 0;
  for(int i = 0; i < verts.size(); i++) {
    int numPoints = verts[i].size();
    //cout << "numPoints = " << numPoints << endl;

    computeNormal(m_normals[vertCount], m_vertices[vertCount], m_vertices[vertCount+2], m_vertices[vertCount+2], m_vertices[vertCount+4]);
    computeNormal(m_normals[vertCount+1], m_vertices[vertCount+1], m_vertices[vertCount+3], m_vertices[vertCount+3], m_vertices[vertCount+5]);
    computeNormal(m_normals[vertCount+2], m_vertices[vertCount], m_vertices[vertCount+2], m_vertices[vertCount+2], m_vertices[vertCount+4]);
    computeNormal(m_normals[vertCount+3], m_vertices[vertCount+1], m_vertices[vertCount+3], m_vertices[vertCount+3], m_vertices[vertCount+5]);
    for(int j = 4; j < 2*numPoints - 4; j += 2) {
      computeNormal(m_normals[vertCount+j], m_vertices[vertCount+j-2], m_vertices[vertCount+j], m_vertices[vertCount+j], m_vertices[vertCount+j+2]);
      computeNormal(m_normals[vertCount+j+1], m_vertices[vertCount+j-2], m_vertices[vertCount+j], m_vertices[vertCount+j], m_vertices[vertCount+j+2]);
    }
    computeNormal(m_normals[vertCount+numPoints - 2], m_vertices[vertCount+numPoints - 6], m_vertices[vertCount+numPoints-4], m_vertices[vertCount+numPoints-4], m_vertices[vertCount+numPoints-2]);
    computeNormal(m_normals[vertCount+numPoints - 1], m_vertices[vertCount+numPoints - 5], m_vertices[vertCount+numPoints-3], m_vertices[vertCount+numPoints-3], m_vertices[vertCount+numPoints-1]);
 
    m_indices.append(Array<int>());
    for(int j = 0; j < 2*numPoints; j++) {
      m_indices[i].append(vertCount+j);
    }
    vertCount += 2*numPoints;
  } 

  size_t sizeNeeded = 8 + sizeof(Vector3)*m_vertices.size() + 8 + sizeof(G3D::Vector3)*m_normals.size() + 8 + sizeof(G3D::Vector2)*m_texCoords.size();
  for(int i = 0; i < m_colorArr.size(); i++) {
    sizeNeeded += 8 + sizeof(G3D::Color3)*m_colorArr[i].size();
  }

  //if(m_varArea.isNull()) {
    m_varArea = VARArea::create(sizeNeeded, VARArea::WRITE_ONCE);
    if (m_varArea.isNull()) {
      cerr << "Error: Out of VARArea room!" << endl;
    }
  //}

  //if(!m_varArea.isNull()) {
    m_varArea->reset();
    m_vertexVAR = VAR(m_vertices, m_varArea);
    for(int i = 0; i < m_colorArr.size(); i++) {
      m_colorVARArr.append(VAR(m_colorArr[i], m_varArea));
    }
    m_normalVAR = VAR(m_normals, m_varArea);
    m_texCoordVAR = VAR(m_texCoords, m_varArea);
    m_texCoordArr.push(m_texCoordVAR);
  //}

    m_colorArrIndex = 0;
}


void HaloLineSet::computeNormal(Vector3 &normal, Vector3 vert1, Vector3 vert2, Vector3 vert3, Vector3 vert4) {
  Vector3 vec1(vert2.x - vert1.x, vert2.y - vert1.y, vert2.z - vert1.z);
  float mag = sqrt(vec1.x*vec1.x + vec1.y*vec1.y + vec1.z*vec1.z);
  if(mag != 0) {
    vec1.x = vec1.x/mag;
    vec1.y = vec1.y/mag;
    vec1.z = vec1.z/mag;
  }

  Vector3 vec2(vert4.x - vert3.x, vert4.y - vert3.y, vert4.z - vert3.z);
  mag = sqrt(vec2.x*vec2.x + vec2.y*vec2.y + vec2.z*vec2.z);
  if(mag != 0) {
    vec2.x = vec2.x/mag;
    vec2.y = vec2.y/mag;
    vec2.z = vec2.z/mag;
  }

  normal.x = (vec1.x + vec2.x)/2;
  normal.y = (vec1.y + vec2.y)/2;
  normal.z = (vec1.z + vec2.z)/2;
}

void HaloLineSet::draw(RenderDevice *rd)
{
  drawSetup(rd);
  // m_indices.size() may not be the right limit, but this should work
  for(int i = 0; i < m_indices.size(); i++) {
    rd->sendIndices(PrimitiveType::TRIANGLE_STRIP, m_indices[i]);
  } 
  drawTearDown(rd);
}

// Takes an array of ints, representing the indices of lines to be drawn,
// and draws the lines corresponsing to those indices
void HaloLineSet::drawSubset(RenderDevice *rd, Array<Array<int> > subsetIndices)
{
  drawSetup(rd);
  for(int i = 0; i < subsetIndices.size(); i++) {
    rd->sendIndices(PrimitiveType::TRIANGLE_STRIP, subsetIndices[i]);
  } 
  drawTearDown(rd);
}

void HaloLineSet::drawLine(RenderDevice *rd, int lineIndex)
{
  drawSetup(rd);
  rd->sendIndices(PrimitiveType::TRIANGLE_STRIP, m_indices[lineIndex]);
  drawTearDown(rd);
}

void HaloLineSet::drawMotionLine(RenderDevice *rd, int lineIndex, int pointIndex, int lineLength)
{
  Array<int> indices;
  for(int i = pointIndex * 2; i >= 0 && i >= 2*(pointIndex - lineLength); i = i-2) {
    indices.append(m_indices[lineIndex][i]);
    indices.append(m_indices[lineIndex][i+1]);
  }
  
  drawSetup(rd);
  rd->sendIndices(PrimitiveType::TRIANGLE_STRIP, indices);
  drawTearDown(rd);
}

void HaloLineSet::drawSetup(RenderDevice *rd) {
  rd->setShader(m_shader);
  m_shader->args.set("haloColor", _haloColor);

  rd->beginIndexedPrimitives();
  rd->setVARs(m_vertexVAR, m_normalVAR, m_colorVARArr[m_colorArrIndex], m_texCoordArr);
}

void HaloLineSet::drawTearDown(RenderDevice *rd) {
  rd->endIndexedPrimitives();
  rd->setShader(NULL);
}

void HaloLineSet::setupHaloShaders(RenderDevice *rd) {
  _shadersSet = true; 
  drawSetup(rd);
}

void HaloLineSet::tearDownHaloShaders(RenderDevice *rd) {
  drawTearDown(rd);
  _shadersSet = false;
}

void HaloLineSet::setLineColors(Array<Color3> &colors) {
    m_colors = colors;

    m_varArea->reset();
    m_vertexVAR = VAR(m_vertices, m_varArea);
    m_colorVAR = VAR(m_colors, m_varArea);
    m_normalVAR = VAR(m_normals, m_varArea);
    m_texCoordVAR = VAR(m_texCoords, m_varArea);
    m_texCoordArr.clear();
    m_texCoordArr.push(m_texCoordVAR);
}

void HaloLineSet::incrementLineWidth() {
  _lineWidth += LINE_WIDTH_INCREMENT;
  m_shader->args.set("lineWidth", _lineWidth);
  cout << "_lineWidth == " << _lineWidth << endl;
}

void HaloLineSet::incrementLineWidth(double incr) {
  _lineWidth += incr;
  m_shader->args.set("lineWidth", _lineWidth);
  cout << "_lineWidth == " << _lineWidth << endl;
}

void HaloLineSet::decrementLineWidth() {
  _lineWidth -= LINE_WIDTH_INCREMENT;
  m_shader->args.set("lineWidth", _lineWidth);
  cout << "_lineWidth == " << _lineWidth << endl;
}

void HaloLineSet::decrementLineWidth(double decr) {
  _lineWidth -= decr;
  m_shader->args.set("lineWidth", _lineWidth);
  cout << "_lineWidth == " << _lineWidth << endl;
}

void HaloLineSet::incrementHalfWidth() {
  _halfWidth += HALF_WIDTH_INCREMENT;
  m_shader->args.set("halfWidth", _halfWidth);
  cout << "_halfWidth == " << _halfWidth << endl;
}

void HaloLineSet::incrementHalfWidth(double incr) {
  _halfWidth += incr;
  m_shader->args.set("halfWidth", _halfWidth);
  cout << "_halfWidth == " << _halfWidth << endl;
}

void HaloLineSet::decrementHalfWidth() {
  _halfWidth -= HALF_WIDTH_INCREMENT;
  m_shader->args.set("halfWidth", _halfWidth);
  cout << "_halfWidth == " << _halfWidth << endl;
}

void HaloLineSet::decrementHalfWidth(double decr) {
  _halfWidth -= decr;
  m_shader->args.set("halfWidth", _halfWidth);
  cout << "_halfWidth == " << _halfWidth << endl;
}

void HaloLineSet::incrementMaxDepth() {
  _maxDepth += MAX_DEPTH_INCREMENT;
  m_shader->args.set("maxDepth", _maxDepth);
  cout << "_maxDepth == " << _maxDepth << endl;
}

void HaloLineSet::incrementMaxDepth(double incr) {
  _maxDepth += incr;
  m_shader->args.set("maxDepth", _maxDepth);
  cout << "_maxDepth == " << _maxDepth << endl;
}

void HaloLineSet::decrementMaxDepth() {
  _maxDepth -= MAX_DEPTH_INCREMENT;
  m_shader->args.set("maxDepth", _maxDepth);
  cout << "_maxDepth == " << _maxDepth << endl;
}

void HaloLineSet::decrementMaxDepth(double decr) {
  _maxDepth -= decr;
  m_shader->args.set("maxDepth", _maxDepth);
  cout << "_maxDepth == " << _maxDepth << endl;
}

void HaloLineSet::setMaxDepth(double depth) {
  _maxDepth = depth;
  m_shader->args.set("maxDepth", _maxDepth);
}

double HaloLineSet::getMaxDepth() {
  return _maxDepth;
}

void HaloLineSet::setLineWidth(double width) {
  _lineWidth = width;
  m_shader->args.set("lineWidth", _lineWidth);
}

double HaloLineSet::getLineWidth() {
  return _lineWidth;
}

void HaloLineSet::setHalfWidth(double width) {
  _halfWidth = width;
  m_shader->args.set("halfWidth", _halfWidth);
}

double HaloLineSet::getHalfWidth() {
  return _halfWidth;
}

void HaloLineSet::incrementColorArray(RenderDevice *rd) {
  m_colorArrIndex = (m_colorArrIndex + 1) % m_colorVARArr.size();
  //rd->setColorArray(m_colorVARArr[m_colorArrIndex]);
}

void HaloLineSet::setColorArray(RenderDevice *rd, int index) {
  if(index < m_colorVARArr.size()) {
    m_colorArrIndex = index;
    //rd->setColorArray(m_colorVARArr[index]);
  }
}

// TODO This function currently extends the size of the line rather than
// decreasing the size of the halo.  Currently working around this by
// setting lineWidth and halfWidth directly.  
void HaloLineSet::setHaloWidth(double width) {
  //_halfWidth = 0.5;
  if(width > _lineWidth/2) {
    width = _lineWidth/2;
  }
  _halfWidth = 0.5 - (width/_lineWidth);
  cout << "_halfWidth = " << _halfWidth << endl;
  m_shader->args.set("halfWidth", _halfWidth);
}

