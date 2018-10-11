#include "ShaderRead.h"
#include "glew.h"
#include "glut.h"
#include "glaux.h"





void InitAllGLShaderFunc()
{
	glGetProgramInfoLog = (PFNGLGETPROGRAMINFOLOGPROC)wglGetProcAddress("glGetProgramInfoLog");

	glGetShaderInfoLog = (PFNGLGETPROGRAMINFOLOGPROC)wglGetProcAddress("glGetShaderInfoLog");

	glShaderSource = (PFNGLSHADERSOURCEPROC)wglGetProcAddress("glShaderSource");

	glAttachShader = (PFNGLATTACHSHADERPROC)wglGetProcAddress("glAttachShader");

	glCompileShader = (PFNGLCOMPILESHADERPROC)wglGetProcAddress("glCompileShader");

	glCreateProgram = (PFNGLCREATEPROGRAMPROC)wglGetProcAddress("glCreateProgram");

	glCreateShader = (PFNGLCREATESHADERPROC)wglGetProcAddress("glCreateShader");

	glDeleteProgram = (PFNGLDELETEPROGRAMPROC)wglGetProcAddress("glDeleteProgram");

	glLinkProgram = (PFNGLLINKPROGRAMPROC)wglGetProcAddress("glLinkProgram");

	glUseProgram = (PFNGLUSEPROGRAMPROC)wglGetProcAddress("glUseProgram");

	glGetShaderiv = (PFNGLGETPROGRAMIVPROC)wglGetProcAddress("glGetShaderiv");

	glGetProgramiv = (PFNGLGETPROGRAMIVPROC)wglGetProcAddress("glGetProgramiv");

	glUniform3f = (PFNGLUNIFORM3FPROC)wglGetProcAddress("glUniform3f");

	glUniform4f = (PFNGLUNIFORM4FPROC)wglGetProcAddress("glUniform4f");

	glCheckFramebufferStatus = (PFNGLCHECKFRAMEBUFFERSTATUSPROC)wglGetProcAddress("glCheckFramebufferStatus");

	glGetAttribLocation = (PFNGLGETATTRIBLOCATIONPROC)wglGetProcAddress("glGetAttribLocation");

	glGetUniformLocation = (PFNGLGETUNIFORMLOCATIONPROC)wglGetProcAddress("glGetUniformLocation");

	glVertexAttrib3dv = (PFNGLVERTEXATTRIB3DVPROC)wglGetProcAddress("glVertexAttrib3dv");

	glUniform1i = (PFNGLUNIFORM1IPROC)wglGetProcAddress("glUniform1i");

	glUniform1f = (PFNGLUNIFORM1FPROC)wglGetProcAddress("glUniform1f");

	glUniformMatrix4fv = (PFNGLUNIFORMMATRIX4FVPROC)wglGetProcAddress("glUniformMatrix4fv");

	glActiveTexture = (PFNGLACTIVETEXTUREPROC)wglGetProcAddress("glActiveTexture");

	glGenFramebuffers = (PFNGLGENFRAMEBUFFERSEXTPROC)wglGetProcAddress("glGenFramebuffersEXT");

	glBindFramebuffer = (PFNGLBINDFRAMEBUFFEREXTPROC)wglGetProcAddress("glBindFramebufferEXT");

	glGenRenderbuffers = (PFNGLGENRENDERBUFFERSEXTPROC)wglGetProcAddress("glGenRenderbuffersEXT");

	glBindRenderbuffer = (PFNGLBINDRENDERBUFFEREXTPROC)wglGetProcAddress("glBindRenderbufferEXT");

	glRenderbufferStorage = (PFNGLRENDERBUFFERSTORAGEEXTPROC)wglGetProcAddress("glRenderbufferStorageEXT");

	glFramebufferRenderbuffer = (PFNGLFRAMEBUFFERRENDERBUFFEREXTPROC)wglGetProcAddress("glFramebufferRenderbufferEXT");

	glFramebufferTexture2D = (PFNGLFRAMEBUFFERTEXTURE2DEXTPROC)wglGetProcAddress("glFramebufferTexture2DEXT");

	glActiveTextureARB = (PFNGLACTIVETEXTUREARBPROC)wglGetProcAddress("glActiveTextureARB");


}

int BWReadShaderSource(GLuint shader, const GLchar *filename)
{

	FILE *file;
	const GLchar *source;
	GLsizei fileSize;
	int ret;
	file = fopen(filename, "rb");
	if (file == NULL)
	{
		cout << "open " << filename << " fial......" << endl;
		perror(filename);
		return -1;
	}
	fseek(file, 0L, SEEK_END);
	fileSize = ftell(file);
	fseek(file, 0L, SEEK_SET);
	source = new GLchar[fileSize];
	if (source == NULL) {
		cout << "there is no enough memery !" << endl;
		return  -1;
	}

	ret = fread((void*)source, sizeof(GLchar), fileSize, file) != fileSize;
	//source[fileSize] = '\0' ;
#ifdef SHOW_SHADER_CONTENT_
	cout << "--------------shader file name is :" << filename << "----------------------------" << endl;
	cout << source << endl;
	cout << "--------------------------------------------------------------------------" << endl << endl;
#endif
	fclose(file);
	if (ret)
	{
		cout << "can not read shader file !" << endl;
		return  -1;
	}

	glShaderSource(shader, 1, &source, &fileSize);
	GLenum tmpEnum = glGetError();

	delete[] source;
	return ret;
}
void PrintShaderInfoLog(GLuint shaderObj)
{
	GLsizei bufSize;
	GLchar *info;
	GLsizei length;
	glGetShaderiv(shaderObj, GL_INFO_LOG_LENGTH, &bufSize);
	if (bufSize > 0) {
		info = new GLchar[bufSize];
		if (info != NULL)
		{
			glGetShaderInfoLog(shaderObj, bufSize, &length, info);
			cout << info << endl;
			delete[] info;
		}
		else
		{
			cout << "there is not have many memory to allocate !!" << endl;
		}
	}
}
void PrintProgramInfoLog(GLuint glslProgram)
{
	GLsizei bufsize;
	GLchar* infoLog;
	GLsizei length;
	glGetProgramiv(glslProgram, GL_INFO_LOG_LENGTH, &bufsize);
	if (bufsize > 0) {
		infoLog = new GLchar[bufsize];
		if (infoLog != NULL)
		{
			glGetProgramInfoLog(glslProgram, bufsize, &length, infoLog);
			cout << infoLog << endl;
			delete[] infoLog;
		}
		else
		{
			cout << "there is not have many memory to allocate !!" << endl;
		}
	}
}
GLuint BWLoadShaders(SHADERINFO * shaderInfo, int shaderPrgmNum)
{
	if (!shaderInfo) {
		return  0;
	}
	GLuint shaderObj;
	GLuint glslProgram;
	glewInit();
	glslProgram = glCreateProgram();
	for (int i = 0; i < shaderPrgmNum; i++)
	{
		shaderObj = glCreateShader(shaderInfo[i].shaderType);
		BWReadShaderSource(shaderObj, shaderInfo[i].name.c_str());
		glCompileShader(shaderObj);
		PrintShaderInfoLog(shaderObj);
		glAttachShader(glslProgram, shaderObj);
	}
	glLinkProgram(glslProgram);
	PrintProgramInfoLog(glslProgram);
	return glslProgram;
}
GLuint BWLoadShadersDontLink(SHADERINFO * shaderInfo, int shaderPrgmNum)
{
	if (!shaderInfo) {
		return  0;
	}
	GLuint shaderObj;
	GLuint glslProgram;
	glewInit();
	glslProgram = glCreateProgram();
	for (int i = 0; i < shaderPrgmNum; i++)
	{
		shaderObj = glCreateShader(shaderInfo[i].shaderType);
		BWReadShaderSource(shaderObj, shaderInfo[i].name.c_str());
		glCompileShader(shaderObj);
		PrintShaderInfoLog(shaderObj);
		glAttachShader(glslProgram, shaderObj);
	}
	return glslProgram;
}
void DeleteProgram(GLuint program)
{
	glDeleteProgram(program);
}

