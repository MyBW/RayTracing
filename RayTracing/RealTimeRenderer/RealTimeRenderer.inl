#include "ShaderRead.h"
#include "RealTimeRenderable.h"
template<typename ObjectType>
void RealTimeRenderer<ObjectType>::Init(int Width, int Height)
{
	InitPiplineState(Width, Height);
	InitShaderProgram();
}
template<typename ObjectType>
void RealTimeRenderer<ObjectType>::BeginFrame()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}
template<typename ObjectType>
void RealTimeRenderer<ObjectType>::EndFrame()
{
	glutSwapBuffers();
}
template<typename ObjectType>
void RealTimeRenderer<ObjectType>::SetLineColor(float R, float G, float B)
{
	glUniform3f(LineColorLoc, R, G, B);
}
template<typename ObjectType>
void RealTimeRenderer<ObjectType>::SetLineWidth(int Width)
{
	glLineWidth(Width);
}
template<typename ObjectType>
void RealTimeRenderer<ObjectType>::ShowCoord()
{
	SetLineWidth(3);

	std::vector<BWVector3D> Lines;
	Lines.push_back(BWVector3D(0, 0, 0));
	Lines.push_back(BWVector3D(100, 0, 0));
	SetLineColor(1.0, 0.0, 0.0);
	DrawLine(Lines);
	
	Lines.clear();
	Lines.push_back(BWVector3D(0, 0, 0));
	Lines.push_back(BWVector3D(0, 100, 0));
	SetLineColor(0.0, 1.0, 0.0);
	DrawLine(Lines);

	Lines.clear();
	Lines.push_back(BWVector3D(0, 0, 0));
	Lines.push_back(BWVector3D(0, 0, 100));
	SetLineColor(0.0, 0.0, 1.0);
	DrawLine(Lines);

	SetLineColor(1.0, 0.0, 0.0);
	SetLineWidth(1);
}
template<typename ObjectType>
void RealTimeRenderer<ObjectType>::UseProgram()
{
	glUseProgram(BaseShaderProgram);
}
template<typename ObjectType>
void RealTimeRenderer<ObjectType>::SetModelMatrix(const BWMatrix4 &ModelMatrix)
{
	glUniformMatrix4fv(ModelMatrixLoc, 1, GL_TRUE, ModelMatrix.M);
}
template<typename ObjectType>
void RealTimeRenderer<ObjectType>::SetViewMatrix(const BWMatrix4 &ViewMatrix)
{
	glUniformMatrix4fv(ViewMatrixLoc, 1, GL_TRUE, ViewMatrix.M);
}
template<typename ObjectType>
void RealTimeRenderer<ObjectType>::SetProjectMatrix(const BWMatrix4 &ProjectMatrix)
{
	glUniformMatrix4fv(ProjectMatrixLoc, 1, GL_TRUE, ProjectMatrix.M);
}
template<typename ObjectType>
void RealTimeRenderer<ObjectType>::SetDrawMode(GLenum Enum)
{
	glPolygonMode(GL_FRONT_AND_BACK, Enum);
}
template<typename ObjectType>
void RealTimeRenderer<ObjectType>::DrawLine(const std::vector<BWVector3D> &Lines)
{
	SetModelMatrix(BWMatrix4::IDENTITY);

	GLuint LinesBuff;
	glGenBuffers(1, &LinesBuff);
	glBindBuffer(GL_ARRAY_BUFFER, LinesBuff);
	glBufferData(GL_ARRAY_BUFFER, Lines.size() * sizeof(BWVector3D), Lines.data(), GL_STATIC_DRAW);

	GLuint VAO;
	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);

	glBindBuffer(GL_ARRAY_BUFFER, LinesBuff);
	glEnableVertexAttribArray(PosLoc);
	glVertexAttribPointer(PosLoc, 3, GL_FLOAT, GL_FALSE, 0, NULL);

	glBindVertexArray(VAO);
	glDrawArrays(GL_LINE_STRIP, 0, Lines.size() * 3);

	glDeleteBuffers(1, &LinesBuff);
	glDeleteVertexArrays(1, &VAO);
}
template<typename ObjectType>
void RealTimeRenderer<ObjectType>::InitShaderProgram()
{
	GLuint VertexShaderObj = glCreateShader(GL_VERTEX_SHADER);
	GLuint FragShaderObj = glCreateShader(GL_FRAGMENT_SHADER);
	BaseShaderProgram = glCreateProgram();
	if (BWReadShaderSource(VertexShaderObj, "Shader\\BaseVertexShader.vert") == -1)
		return;
	glCompileShader(VertexShaderObj);
	PrintShaderInfoLog(VertexShaderObj);
	
	if (BWReadShaderSource(FragShaderObj, "Shader\\BaseFragmShader.frag") == -1)
		return;
	glCompileShader(FragShaderObj);
	PrintShaderInfoLog(FragShaderObj);

	glAttachShader(BaseShaderProgram, VertexShaderObj);
	glAttachShader(BaseShaderProgram, FragShaderObj);
	glLinkProgram(BaseShaderProgram);
	PrintProgramInfoLog(BaseShaderProgram);

	PosLoc = glGetAttribLocation(BaseShaderProgram, "Position");
	NormalLoc = glGetAttribLocation(BaseShaderProgram, "Normal");
	UVLoc = glGetAttribLocation(BaseShaderProgram, "inUV");
	TagentLoc = glGetAttribLocation(BaseShaderProgram, "Tagent");
	TextureLoc = glGetAttribLocation(BaseShaderProgram, "LightMap");
	ModelMatrixLoc = glGetUniformLocation(BaseShaderProgram, "ModelMatrix");
	ViewMatrixLoc = glGetUniformLocation(BaseShaderProgram, "ViewMatrix");
	ProjectMatrixLoc = glGetUniformLocation(BaseShaderProgram, "ProjectMatrix");
	LineColorLoc = glGetUniformLocation(BaseShaderProgram, "LineColor");
}
template<typename ObjectType>
void RealTimeRenderer<ObjectType>::InitPiplineState(int Width, int Height)
{
	glewInit();
	glViewport(0, 0, Width, Height);
	glClearColor(1.0, 1.0, 1.0, 1.0);
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);

}
template<typename ObjectType>
void RealTimeRenderer<ObjectType>::Draw()
{
	for (auto Obj : DrawList)
	{
		Obj->Draw(this);
	}
}
template<typename ObjectType>
void RealTimeRenderer<ObjectType>::AddDrawable(const ObjectType* InObject)
{
	RealTimeRenderable<ObjectType> *NewRenderable = new RealTimeRenderable<ObjectType>();
	NewRenderable->InitRealTimeRenderable(InObject, this);
	DrawList.push_back(NewRenderable);
}

