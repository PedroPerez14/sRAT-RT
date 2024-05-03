#version 450 core
layout (location = 0) out vec4 gPosition;       //.a component is mat_id
layout (location = 1) out vec4 gNormal;         //.a component is metallic
layout (location = 2) out vec4 gAlbedo;         //.a component is roughness

in vec3 FragPos;
in vec2 TexCoords;
in vec3 Normal;

layout (binding = 0) uniform sampler2D diff_texture;
layout (binding = 1) uniform sampler2D normal_texture;
layout (binding = 2) uniform sampler2D metallic_texture;
layout (binding = 3) uniform sampler2D roughness_texture;
//layout (binding = 4) uniform sampler2D ao_texture;
uniform int mat_id;

vec3 getNormalFromMap()
{
    vec3 tangentNormal = texture(normal_texture, TexCoords).xyz * 2.0 - 1.0;

    vec3 Q1  = dFdx(FragPos.xyz);
    vec3 Q2  = dFdy(FragPos.xyz);
    vec2 st1 = dFdx(TexCoords);
    vec2 st2 = dFdy(TexCoords);

    vec3 N   = normalize(Normal);
    vec3 T  = normalize(Q1*st2.t - Q2*st1.t);
    vec3 B  = -normalize(cross(N, T));
    mat3 TBN = mat3(T, B, N);

    return normalize(TBN * tangentNormal);
}

void main()
{    
    // Sample both pbr textures
    float metallic = texture(metallic_texture, TexCoords).r;
    float roughness = texture(roughness_texture, TexCoords).r;

    vec3 map_normal = getNormalFromMap();

    // store the fragment position vector in the first gbuffer texture + mat_id
    gPosition.rgba = vec4(FragPos, mat_id);
    // also store the per-fragment normals into the gbuffer + metallic
    gNormal.rgba = vec4(map_normal, metallic);
    // and the diffuse per-fragment color + roughness
    gAlbedo.rgba = vec4(texture(diff_texture, TexCoords).rgb, roughness);
}