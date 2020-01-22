#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/vec3.hpp>

int main( void )
{
	GLFWwindow* pWindow;
	/* Initialize the library */
	if (!glfwInit())
	{
		/* Problem: glewInit failed, something is seriously wrong. */
		fprintf( stderr, "Initialized GLFW failed" );
		return -1;
	}
	/* Create a windowed mode window and its OpenGL context */
	pWindow = glfwCreateWindow( 640, 480, "Hello World", NULL, NULL );
	if (!pWindow)
	{
		glfwTerminate();
		return -1;
	}
	/* Make the window's context current */
	glfwMakeContextCurrent( pWindow );

	GLenum err = glewInit();
	if (GLEW_OK != err)
	{
		/* Problem: glewInit failed, something is seriously wrong. */
		fprintf( stderr, "Initialized GLEW failed, Error: %s\n", glewGetErrorString( err ) );
	}

	while (!glfwWindowShouldClose( pWindow ))
	{
		/* Render here */
		glClear( GL_COLOR_BUFFER_BIT );
		GLuint VBO;
		// Added the simple test code to draw a single point in the middle of the screen.
		glm::vec3 vertices[1];
		vertices[0] = glm::vec3( 0.0f, 0.0f, 0.0f );
		glGenBuffers( 1, &VBO );
		glBindBuffer( GL_ARRAY_BUFFER, VBO );
		glBufferData( GL_ARRAY_BUFFER, sizeof( vertices ), vertices, GL_STATIC_DRAW );
		glEnableVertexAttribArray( 0 );
		glBindBuffer( GL_ARRAY_BUFFER, VBO );
		glVertexAttribPointer( 0, 3, GL_FLOAT, GL_FALSE, 0, 0 );
		glDrawArrays( GL_POINTS, 0, 1 );
		glDisableVertexAttribArray( 0 );
		// ........

		/* Swap front and back buffers */
		glfwSwapBuffers( pWindow );
		/* Poll for and process events */
		glfwPollEvents();
	}

	glfwTerminate();
	return 0;
}