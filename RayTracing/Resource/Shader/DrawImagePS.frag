#version 430 core
in vec2 UV;
uniform sampler2D ImageTex;
void main()
{
   vec3 LightMapColor = texture2D(ImageTex, UV).xyz;
   //LightMapColor = vec3(1.0, 0.0, 0.0);
   gl_FragColor = vec4(LightMapColor, 1.0);
}