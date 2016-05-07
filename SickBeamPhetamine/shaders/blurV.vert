#version 450

// input attributes
layout(location = 0) in vec3 in_vertex_position;


void main(void) {
  gl_Position = vec4(in_vertex_position, 1.0);
}