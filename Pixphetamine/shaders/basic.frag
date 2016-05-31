#version 330

in vec3 frag_normal;
in vec3 frag_color;
in vec3 frag_type;

uniform vec4 object_type; // (constant for all the vertices of the object)


// output
layout(location=0) out vec4 outColor;
layout(location=1) out vec4 outNormal;
layout(location=2) out vec4 outType;

void main() {
    // Fetch color 
    
    outColor = vec4(frag_color, 1.0);
    outNormal = vec4(frag_normal, 1.0);
    outType = object_type;
}