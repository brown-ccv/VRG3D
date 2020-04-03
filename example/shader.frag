uniform sampler2D tex;

void main()
{
  float halfw = 0.25;
  float maxd = 0.005;
  vec4 fgColor = vec4(1.0, 0.0, 0.0, 1.0);
  //vec4 fgColor = vec4(gl_Color);
  vec4 bgColor = vec4(0.0, 0.0, 1.0, 0.0);
  //vec4 bgColor = vec4(1.0, 1.0, 1.0, 1.0);

  float s = abs(gl_TexCoord[0].t - 0.5);
  if(s <= halfw) {
    gl_FragColor = fgColor;
    gl_FragDepth = gl_FragCoord.z;
  } else {
    gl_FragColor = bgColor;
    //gl_FragDepth = gl_FragCoord.z + 0.1*(s*2.0);
    gl_FragDepth = gl_FragCoord.z + maxd*(s*2.0);
    //gl_FragDepth = gl_FragCoord.z + 0.008;
  }
//  gl_FragColor = vec4(gl_FragCoord.z, 0.0, 0.0, 0.0);
}
