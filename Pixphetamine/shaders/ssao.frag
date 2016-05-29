#version 450

const vec4 TYPE_FOX = vec4(1.0, 1.0, 0.0, 1.0);

uniform sampler2D  color_map;
uniform sampler2D  normal_map;
uniform sampler2D  type_map;
uniform sampler2D  depth_map;

uniform vec3 sun_direction; // sun light direction (constant for all the vertices)

out vec4 outColor;

vec4 readTexture(sampler2D map, vec2 coord) {
    return texelFetch(map, ivec2(coord), 0);
}

float3 normal_from_depth(float depth, vec2 coord) {
  
  const vec2 offset1 = vec2(0.0, 0.001);
  const vec2 offset2 = vec2(0.001, 0.0);
  
  float depth1 = readTexture(depth_map, coord + offset1).r;
  float depth2 = readTexture(depth_map, coord + offset2).r;
  
  vec3 p1 = vec3(offset1, depth1 - depth);
  vec3 p2 = vec3(offset2, depth2 - depth);
  
  vec3 normal = cross(p1, p2);
  normal.z = -normal.z;
  
  return normalize(normal);
}

PS_OUTPUT ps_ssao(VS_OUT_SSAO In)
{ 
  PS_OUTPUT Output;
  
  const float total_strength = 1.0;
  const float base = 0.2;
  
  const float area = 0.0075;
  const float falloff = 0.000001;
  
  const float radius = 0.0002;
  
  const int samples = 16;
  float3 sample_sphere[samples] = {
      float3( 0.5381, 0.1856,-0.4319), float3( 0.1379, 0.2486, 0.4430),
      float3( 0.3371, 0.5679,-0.0057), float3(-0.6999,-0.0451,-0.0019),
      float3( 0.0689,-0.1598,-0.8547), float3( 0.0560, 0.0069,-0.1843),
      float3(-0.0146, 0.1402, 0.0762), float3( 0.0100,-0.1924,-0.0344),
      float3(-0.3577,-0.5301,-0.4358), float3(-0.3169, 0.1063, 0.0158),
      float3( 0.0103,-0.5869, 0.0046), float3(-0.0897,-0.4940, 0.3287),
      float3( 0.7119,-0.0154,-0.0918), float3(-0.0533, 0.0596,-0.5411),
      float3( 0.0352,-0.0631, 0.5460), float3(-0.4776, 0.2847,-0.0271)
  };
  
  float3 random = normalize( tex2D(RandomTextureSampler, In.Tex0 * 4.0).rgb );
  
  float depth = tex2D(DepthTextureSampler, In.Tex0).r;
 
  float3 position = float3(In.Tex0, depth);
  float3 normal = normal_from_depth(depth, In.Tex0);
  
  float radius_depth = radius/depth;
  float occlusion = 0.0;
  for(int i=0; i < samples; i++) {
  
    float3 ray = radius_depth * reflect(sample_sphere[i], random);
    float3 hemi_ray = position + sign(dot(ray,normal)) * ray;
    
    float occ_depth = tex2D(DepthTextureSampler, saturate(hemi_ray.xy)).r;
    float difference = depth - occ_depth;
    
    occlusion += step(falloff, difference) * (1.0-smoothstep(falloff, area, difference));
  }
  
  float ao = 1.0 - total_strength * occlusion * (1.0 / samples);
  Output.RGBColor = saturate(ao + base);
  
  return Output;
}