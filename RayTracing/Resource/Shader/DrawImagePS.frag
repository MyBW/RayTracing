#version 430 core
in vec2 UV;
uniform sampler2D ImageTex;
void main()
{
   vec3 LightMapColor = texture2D(ImageTex, UV).xyz;
   LightMapColor = LightMapColor / (LightMapColor + 0.187) * 1.035;
   gl_FragColor = vec4(LightMapColor, 1.0);
}