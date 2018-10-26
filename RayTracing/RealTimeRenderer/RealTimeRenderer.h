#pragma once
#include "..\Math\BWPrimitive.h"
#include "glew.h"
#include "glut.h"
#include <vector>
template<typename ObjectType> 
class RealTimeRenderable;

template<typename ObjectType>
class RealTimeRenderer
{
public:
	void Init(int Width , int Height);
	void BeginFrame();
	void EndFrame();
	void SetLineColor(float R, float G, float B);
	void SetLineWidth(int Width);
	void ShowCoord();
	void UseProgram();
	void SetModelMatrix(const BWMatrix4 &ModelMatrix);
	void SetViewMatrix(const BWMatrix4 &ViewMatrix);
	void SetProjectMatrix(const BWMatrix4 &ProjectMatrix);
	void SetDrawMode(GLenum Enum);
	void DrawLine(const std::vector<BWVector3D> &Lines);
	void DrawImage(int Width, int Height, void *Data);
	void Draw();
	void AddDrawable(const ObjectType* InObject);
	
	GLuint BaseShaderProgram;
	GLint PosLoc;
	GLint NormalLoc;
	GLint UVLoc;
	GLint TagentLoc;
	GLint TextureLoc;
	GLint ModelMatrixLoc;
	GLint ViewMatrixLoc;
	GLint ProjectMatrixLoc;
	GLint LineColorLoc;

	GLuint DrawImageShader;
	GLint ImageTextureLoc;
	GLint DrawImagePosLoc;
	GLint DrawImageUVLoc;
	GLint OrthographicLoc;
	GLuint ImageTex;
	GLuint DrawImageVAO;
	GLuint DrawImageTex;

	std::vector<BWVector3D> Square;

	std::vector<RealTimeRenderable<ObjectType>*> DrawList;
private:
	void InitShaderProgram();
	void InitPiplineState(int Width, int Height);

};

#include "RealTimeRenderer.inl"