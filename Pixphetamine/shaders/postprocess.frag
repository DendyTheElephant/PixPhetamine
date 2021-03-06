#version 330

const vec4 TYPE_FOX = vec4(1.0, 0.0, 0.0, 1.0);

uniform sampler2D  color_map;
uniform sampler2D  normal_map;
uniform sampler2D  type_map;
uniform sampler2D  depth_map;
uniform sampler2D  blurred_color_map;

uniform vec3 sun_direction; // sun light direction (constant for all the vertices)

out vec4 outColor;

void main() {  
    ivec2 windowSize = textureSize(depth_map, 0);
    vec2 texCoord = gl_FragCoord.xy / windowSize;
    vec2 centerCoord = windowSize / 2;

    vec3 lightDirection = normalize(sun_direction);

	vec4 color = texture(color_map, texCoord);
    vec4 normal = texture(normal_map, texCoord);
    vec4 type = texture(type_map, texCoord);
	
    float far = 100.0;
    float near = 0.1;
    float depth = 1 - ((2 * near) / (far + near - texture(depth_map,texCoord).r * (far - near)));
    
    //float centerDepth = 1 - ((2 * near) / (far + near - texture(depth_map,centerCoord).r * (far - near)));
  
    float diffuse = max(dot(lightDirection,normal.xyz), 0.0);
    
	color =  color - ((0.0 + diffuse*0.5))*(0.5, 0.2, 0.2, 1.0)*type.r;
    
	//color = color + depth*texture(blurred_color_map, texCoord);
    
    outColor = color;
    //outColor = texture(color_map, texCoord);
    //outColor = texture(normal_map, texCoord);
    //outColor = texture(type_map, texCoord);
	//outColor = texture(blurred_color_map, texCoord);
    //outColor = vec4(depth,depth,depth,1.0);
	//outColor = texture(depth_map, texCoord);
}