

#ifndef SHADOWS_H
#define SHADOWS_H

#include <CommonInc.H>

namespace VRBase {

void pushShadowState(G3D::RenderDevice *rd);
void popShadowState(G3D::RenderDevice *rd);

bool getShadowsOn();
void setShadowsOn(bool b);

} // end namespace

#endif
