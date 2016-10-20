#version 330

uniform sampler2D image;
uniform float scale;

out vec4 OutColor;

void main() {
    ivec2 windowSize = textureSize(image, 0);
    vec2 texCoord = gl_FragCoord.xy / windowSize;
	
    OutColor = texture(image, scale*texCoord);
}