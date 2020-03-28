#version 330 core
layout (location = 0) in vec2 aPos;
layout (location = 1) in vec2 aTexCoords;

out vec2 TexCoords;
//does it have to be mat4?
uniform mat4 transform;

void main()
{
    //gl_Position = vec4(aPos.x, aPos.y, 0.0, 1.0); 
    gl_Position = transform * vec4(aPos.x, aPos.y, 0.0, 1.0);
    TexCoords = aTexCoords;
}