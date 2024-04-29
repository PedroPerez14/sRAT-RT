#version 450 core

out vec2 fTexcoords;
uniform int flipY = 0;          // Acts as a bool, is int for possible compatibility issues

void main()
{
    // Generate the UV coordinates for our in-screen quad (2 triangles iirc)
    vec2 pos = vec2(gl_VertexID % 2, gl_VertexID / 2) * 4.0 - 1.0;
    fTexcoords = pos * 0.5 + 0.5;

    //Account for flipY
    if(flipY != 0)
    {
        fTexcoords.y = 1.0 - fTexcoords.y;
    }
    gl_Position = vec4(pos, 0.0, 1.0);
}