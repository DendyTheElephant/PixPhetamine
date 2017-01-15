#version 330

in vec3 frag_normal;
in vec3 frag_color;
in vec3 frag_type;

uniform vec4 object_type; // (constant for all the vertices of the object)

// output
layout(location=0) out vec4 outColor;
layout(location=1) out vec4 outNormal;
layout(location=2) out vec4 outType;
//layout(location=3) out vec4 outDepth;

void main() {
    // Fetch color 
    outColor = vec4(vec3(gl_FragCoord.z), 1.0f);
    outNormal = vec4(frag_normal, 1.0);
    outType = object_type;
	//outDepth = vec4(gl_FragDepth, gl_FragDepth, gl_FragDepth, 1.0);
}