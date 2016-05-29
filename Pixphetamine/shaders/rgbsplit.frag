#version 400

uniform sampler2D  image;

uniform float split;

out vec4 OutColor;

void main() {
    
    vec2 delta = vec2(split, 0.0);

    ivec2 windowSize = textureSize(image, 0);
    vec2 texCoord = gl_FragCoord.xy / windowSize;
    
    vec4 redColor;
    vec4 greenColor;
    vec4 blueColor;
    vec2 dir;
    
    if (texCoord.x < 0.5) { // The pixel is on the left
        dir = vec2(-0.1,0.0);
        redColor = texture(image, texCoord + delta);
        greenColor = texture(image, texCoord);
        blueColor = texture(image, texCoord);
    } else {                // The pixel is on the right
        redColor = texture(image, texCoord);
        greenColor = texture(image, texCoord);
        blueColor = texture(image, texCoord + delta);
    }
    
    OutColor = vec4(redColor.r, greenColor.g, blueColor.b, 1.0);
}