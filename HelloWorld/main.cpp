#include <GL/freeglut.h>
#include <iostream>
#include <math.h>

void ProcessKeyboardKey( unsigned char i_key, int x, int y )
{
	// 27 is the ACSII keycode for the 'esc' key.
	if (i_key == 27)
	{
		exit( 0 );
	}
}

void Display()
{
	glClear( GL_COLOR_BUFFER_BIT );
	glutSwapBuffers();
}

void Idle()
{
	// Submit the animated background colors
	float elapsedTime = glutGet( GLUT_ELAPSED_TIME ) / 1000.0;
	float r = 1 + sinf( elapsedTime ) / 2;
	float g = 1 + cosf( elapsedTime ) / 2;
	float b = 1 + cosf( elapsedTime ) / 2;
	float a = 1 + cosf( elapsedTime ) / 2;
	glClearColor( r, g, b, a );
	glutPostRedisplay();
}

void Init()
{
	glClearColor( 1.0, 1.0, 1.0, 1.0 );
	glMatrixMode( GL_PROJECTION );
	glLoadIdentity();
	gluOrtho2D( 0.0, 499.0, 0.0, 499.0 );
}

void main( int argc, char** argv ) {
	glutInit( &argc, argv );
	glutInitDisplayMode( GLUT_SINGLE | GLUT_RGB );
	glutInitWindowSize( 500, 500 );
	glutInitWindowPosition( 0, 0 );
	glutCreateWindow( "HelloWorld" );
	glutDisplayFunc( Display );
	glutKeyboardFunc( ProcessKeyboardKey );
	glutIdleFunc( Idle );
	Init();
	glutMainLoop();
}