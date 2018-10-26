#version 430 core

in vec3 Position;
in vec2 inUV;
uniform mat4  Orthographic;

out vec2  UV; 
void main()
{
   gl_Position = Orthographic * vec4(Position , 1.0) ;
   UV = inUV;
}

