#version 450 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;

out vec3 FragPos;
out vec2 TexCoords;
out vec3 Normal;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform mat3 normal_mat;

void main()
{
    vec4 worldPos = model * vec4(aPos, 1.0);
    FragPos = worldPos.xyz; 
    //TexCoords = aTexCoords * vec2(1.0, -1.0) + vec2(0.0, 1.0);
    TexCoords = aTexCoords;
    
    Normal = normal_mat * aNormal;

    gl_Position = projection * view * worldPos;
}