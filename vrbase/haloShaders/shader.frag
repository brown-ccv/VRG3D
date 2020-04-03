uniform sampler2D tex;

uniform float halfWidth;
uniform float maxDepth;
uniform vec3 haloColor;

void main()
{
  vec4 fgColor = vec4(gl_Color);
  // TODO Figure out if there's a way to get the clearColor through GLSL
  // 2nd Option: Pass in the clear color
  vec4 bgColor = vec4(haloColor.x, haloColor.y, haloColor.z, 0.0);

  float s = abs(gl_TexCoord[0].t - 0.5);
  if(s <= halfWidth) {
    gl_FragColor = fgColor;
    gl_FragDepth = gl_FragCoord.z;
  } else {
    gl_FragColor = bgColor;
    gl_FragDepth = gl_FragCoord.z + maxDepth*(s*2.0);
    //gl_FragDepth = gl_FragCoord.z;
  }

}
