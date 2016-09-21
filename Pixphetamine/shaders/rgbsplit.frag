#version 330

uniform sampler2D  image;

uniform float split;

out vec4 OutColor;

void main() {

    ivec2 windowSize = textureSize(image, 0);
    vec2 texCoord = gl_FragCoord.xy / windowSize; // Coordinate of current pixel in [0..1]
    
	float horizontalProximity;	// Will describe the proximity to the center 
	float verticalProximity;
	if (texCoord.x > 0.5) {
		horizontalProximity = 1.0 - texCoord.x;
	} else {
		horizontalProximity = texCoord.x;
	}
	if (texCoord.y > 0.5) {
		verticalProximity = 1.0 - texCoord.y;
	} else {
		verticalProximity = texCoord.y;
	}
	float centerCoef = horizontalProximity * verticalProximity;
	
	
	// TRY SOMETHING SIMPLE!!!! (like distance?) INSTEAD OF THIS "NEWBIE DUMB HOT-HACK"....
	//centerCoef = length(texCoord, vec2(0.5, 0.5)); // Length is not a GLSL function... u_u
	
	float redColor = texture(image, vec2(texCoord.x + 0.05*sin(split)*centerCoef, texCoord.y)).r;
    float greenColor = texture(image, vec2(texCoord.x, texCoord.y + 0.05*sin(split/0.25)*centerCoef)).g;
    float blueColor = texture(image, vec2(texCoord.x + 0.05*sin(split/0.5)*centerCoef, texCoord.y)).b;
    
    
    OutColor = vec4(redColor, greenColor, blueColor, 1.0);
	//OutColor = vec4(texture(image, texCoord));
	//OutColor = vec4(1.0, 1.0, 0.0, 1.0);
}