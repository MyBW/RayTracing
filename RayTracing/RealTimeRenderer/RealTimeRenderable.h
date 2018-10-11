#pragma once
template<typename ObjectType>
class RealTimeRenderer;

template<typename ObjectType>
class RealTimeRenderable 
{
public:
	virtual ~RealTimeRenderable();
	void InitRealTimeRenderable(const ObjectType *InObject, RealTimeRenderer<ObjectType> *RTRenderer);
	void Draw(RealTimeRenderer<ObjectType> *RTRenderer);
private:
	const ObjectType *Object;
	GLuint PosBuffer;
	GLuint UVBuffer;
	GLuint NormalBuffer;
	GLuint LightMapBuffer;
	GLuint IndexBuffer;
	GLuint VAO;
	GLuint TextureName;
};


#include "RealTimeRenderable.inl"