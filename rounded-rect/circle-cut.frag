#version 330 core
out vec4 FragColor;
in vec4 verts;
#define rect_center    vec2(0, 0)
#define rect_size      vec2(2, 2)
#define rect_radius    0.2
vec4 sdfCircle(vec2 uv, float r) {
  //采用sdf绘制大圆
  float x = uv.x;
  float y = uv.y;

  float d = length(vec2(x, y)) - r;

  //return d > 0. ? vec4(1.0f, 0.5f, 0.2f, 0.5f) : vec4(0., 0., 1., 1.);
  return d > 0. ? vec4(1.0f, 0.5f, 0.2f, 0.2f) : vec4(0., 0., 1., 1.);
}

void main()
{
  vec2 uv = verts.xy;
  //uv = uv.xy - vec2(0.5, 0.5);
  //uv.x = uv.x * (800. / 600.);
  vec4 color = sdfCircle(uv, 1.2);
  //FragColor = vec4(1.0f, 0.5f, 0.2f, 1.0);
  FragColor = color;

}
