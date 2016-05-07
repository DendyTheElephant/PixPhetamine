#version 400

// input attributes
layout(location = 0) in vec3 in_vertex_position;
layout(location = 1) in vec3 in_vertex_normal;
layout(location = 2) in vec3 in_vertex_color;

uniform mat4 MVP; // modelviewprojection matrix (constant for all the vertices)

// output variables that will be interpolated during rasterization (equivalent to varying)
out vec3 frag_normal;
out vec3 frag_color;


void main() {
  gl_Position = MVP * vec4(in_vertex_position, 1.0);
  frag_normal = in_vertex_normal;
  frag_color = in_vertex_color;
}
