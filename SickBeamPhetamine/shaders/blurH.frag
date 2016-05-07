#version 400

const float weight[3] = float[]( 0.2270270270, 0.3162162162, 0.0702702703 );

uniform sampler2D  image;

out vec4 OutColor;
 

void main() {
    ivec2 windowSize = textureSize(image, 0);
    vec2 texCoord = gl_FragCoord.xy / windowSize;
        
    float offset[2];
    offset[0] = 1.3846153846 / windowSize.x;
    offset[1] = 3.2307692308 / windowSize.x;
    
    OutColor = texture(image, texCoord) * weight[0];
    
    OutColor += texture(image, texCoord + vec2(offset[0], 0.0)) * weight[1];
    OutColor += texture(image, texCoord - vec2(offset[0], 0.0)) * weight[1];
    
    OutColor += texture(image, texCoord + vec2(offset[1], 0.0)) * weight[2];
    OutColor += texture(image, texCoord + vec2(offset[1], 0.0)) * weight[2];
}