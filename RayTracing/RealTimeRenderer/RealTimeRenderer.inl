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
	glDrawArrays(GL_LINES, 0, Lines.size() * 3);

	glDeleteBuffers(1, &LinesBuff);
	glDeleteVertexArrays(1, &VAO);
}
template<typename ObjectType>
void RealTimeRenderer<ObjectType>::InitShaderProgram()
{
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
	{
		GLuint VertexShaderObj = glCreateShader(GL_VERTEX_SHADER);
		GLuint FragShaderObj = glCreateShader(GL_FRAGMENT_SHADER);
		DrawImageShader = glCreateProgram();
		if (BWReadShaderSource(VertexShaderObj, "Shader\\DrawImageVS.vert") == -1)
			return;
		glCompileShader(VertexShaderObj);
		PrintShaderInfoLog(VertexShaderObj);

		if (BWReadShaderSource(FragShaderObj, "Shader\\DrawImagePS.frag") == -1)
			return;
		glCompileShader(FragShaderObj);
		PrintShaderInfoLog(FragShaderObj);

		glAttachShader(DrawImageShader, VertexShaderObj);
		glAttachShader(DrawImageShader, FragShaderObj);
		glLinkProgram(DrawImageShader);
		PrintProgramInfoLog(DrawImageShader);

		DrawImagePosLoc = glGetAttribLocation(DrawImageShader, "Position");
		DrawImageUVLoc = glGetAttribLocation(DrawImageShader, "inUV");
		ImageTextureLoc = glGetUniformLocation(DrawImageShader, "ImageTex");
		OrthographicLoc = glGetUniformLocation(DrawImageShader, "Orthographic");



		Square.clear();
		std::vector<BWPoint2DD> UVs;
		Square.push_back(BWVector3D(-10, 10, -10));
		UVs.push_back(BWPoint2DD(0, 1));
		Square.push_back(BWVector3D(-10, -10, -10));
		UVs.push_back(BWPoint2DD(0, 0));
		Square.push_back(BWVector3D(10, -10, -10));
		UVs.push_back(BWPoint2DD(1, 0));

		Square.push_back(BWVector3D(-10, 10, -10));
		UVs.push_back(BWPoint2DD(0, 1));
		Square.push_back(BWVector3D(10, -10, -10));
		UVs.push_back(BWPoint2DD(1, 0));
		Square.push_back(BWVector3D(10, 10, -10));
		UVs.push_back(BWPoint2DD(1, 1));

		GLuint LinesBuff;
		glGenBuffers(1, &LinesBuff);
		glBindBuffer(GL_ARRAY_BUFFER, LinesBuff);
		glBufferData(GL_ARRAY_BUFFER, Square.size() * sizeof(BWVector3D), Square.data(), GL_STATIC_DRAW);

		GLuint UVBuff;
		glGenBuffers(1, &UVBuff);
		glBindBuffer(GL_ARRAY_BUFFER, UVBuff);
		glBufferData(GL_ARRAY_BUFFER, UVs.size() * sizeof(BWPoint2DD), UVs.data(), GL_STATIC_DRAW);


		glGenVertexArrays(1, &DrawImageVAO);
		glBindVertexArray(DrawImageVAO);

		glBindBuffer(GL_ARRAY_BUFFER, LinesBuff);
		glEnableVertexAttribArray(DrawImagePosLoc);
		glVertexAttribPointer(DrawImagePosLoc, 3, GL_FLOAT, GL_FALSE, 0, NULL);

		glBindBuffer(GL_ARRAY_BUFFER, UVBuff);
		glEnableVertexAttribArray(DrawImageUVLoc);
		glVertexAttribPointer(DrawImageUVLoc, 2, GL_FLOAT, GL_FALSE, 0, NULL);

		glBindVertexArray(0);
	}
	
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


template<typename ObjectType>
void RealTimeRenderer<ObjectType>::DrawImage(int Width, int Height, void *Data)
{
	
	glPolygonMode(GL_FRONT_AND_BACK,GL_FILL);
	glUseProgram(DrawImageShader);
	glGenTextures(1, &DrawImageTex);
	glBindTexture(GL_TEXTURE_2D, DrawImageTex);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, Width, Height, 0,GL_RGB, GL_UNSIGNED_BYTE, Data);


	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, DrawImageTex);
	glUniform1i(ImageTextureLoc, 0);

	BWMatrix4 Orthographic;
	Orthographic.SetZero();
	Orthographic.M00 = 2.0 / 20;
	Orthographic.M11 = 2.0 / 20;
	Orthographic.M22 = -2.0 / 20;
	Orthographic.M33 = 1;
	glUniformMatrix4fv(OrthographicLoc, 1, GL_TRUE, Orthographic.M);
	glBindVertexArray(DrawImageVAO);
	glDrawArrays(GL_TRIANGLES, 0, Square.size() * 3);

	//glDeleteTextures(1, &DrawImageTex);
}
