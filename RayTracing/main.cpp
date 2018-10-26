#include "glew.h"
#include "glut.h"
#include "Test/AppTest.h"


AppTest AppForTest;
void ProcessKeyboard(unsigned char key, int x, int y)
{
	AppForTest.ProcessKeyboard(key, x, y);
}
void display()
{
	AppForTest.Update();
}
void mouse(int x, int y)
{	
	AppForTest.MouseMove(x, y);
}
void moueMoveFunction(int button, int state, int x, int y)
{
	AppForTest.MouseStateChange(button, state, x, y);
}
int main(int argc, char **argv)
{

	int Width, Height;
	Width = 800;
	Height = 400;
	glutInit(&argc, argv);
	glutInitWindowSize(Width, Height);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);
	glutCreateWindow("RayTracing");
	AppForTest.Init(Width, Height);
	glutKeyboardFunc(&ProcessKeyboard);
	glutMotionFunc(mouse);
	glutMouseFunc(moueMoveFunction);
	glutDisplayFunc(display);
	glutMainLoop();
	
	return 0;
}