#version 450 core
out vec4 FragColor;

in vec2 TexCoord;
uniform sampler2D texture1;
uniform sampler2D texture2;

void main()
{
	//built-in texture() function samples a texture
	//built-in mix() function mixes 2 colors, 
	//	in this case 80% of the 1st one and 20% of the 2nd one
	FragColor = mix(texture(texture1, TexCoord), texture(texture2, vec2(1.0 - TexCoord.x, TexCoord.y)), 0.0);
}