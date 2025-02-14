#include "Shadows.H"
#include <VRBase.H>

namespace VRBase {

using namespace G3D;

static bool gShadowsOn = true;

bool getShadowsOn() {
  return gShadowsOn;
}

void setShadowsOn(bool b) {
  gShadowsOn = b;
}

void 
pushShadowState(RenderDevice *rd)
{
  rd->pushState();
  Matrix3 m(1,0,0, 0,0,0, 0,0,1);
  double y = ConfigVal("ShadowPlaneY", -0.15, false);
  Vector3 t(0,y,0);
  CoordinateFrame shadowMat(m,t);
  rd->setObjectToWorldMatrix(shadowMat * rd->objectToWorldMatrix());
  Color3 col = ConfigVal("ShadowColor", Color3::black(), false);
  rd->setColor(col);
  rd->disableLighting();
}


void popShadowState(RenderDevice *rd)
{
  rd->popState();
}


} // end namespace

