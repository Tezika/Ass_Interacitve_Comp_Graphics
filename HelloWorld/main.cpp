#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/vec3.hpp>

GLuint VAO;
GLuint VBO;

void Initialize()
{
	glm::vec3 vertices[3];
	vertices[0] = glm::vec3( 0.0f, 0.0f, 0.0f );
	vertices[1] = glm::vec3( 3.0f, -1.0f, 0.0f );
	vertices[2] = glm::vec3( -3.0f, -1.0f, 0.0f );

	glGenVertexArrays( 1, &VAO );
	glBindVertexArray( VAO );
	glGenBuffers( 2, &VBO );
	glBindBuffer( GL_ARRAY_BUFFER, VBO );
	glBufferData( GL_ARRAY_BUFFER, sizeof( vertices ), vertices, GL_STATIC_DRAW );
	glVertexAttribPointer( 0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof( GLfloat ), 0 );
	glEnableVertexAttribArray( 0 );
	glBindBuffer( GL_ARRAY_BUFFER, 0 );
	glBindVertexArray( 0 );
}

void Render()
{
	glClearColor( 0, 0, 0.2, 0 );
	glClear( GL_COLOR_BUFFER_BIT );
	glBindVertexArray( VAO );
	glDrawArrays( GL_TRIANGLES, 0, 3 );
}

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
	pWindow = glfwCreateWindow( 1920, 1080, "Hello World", NULL, NULL );
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

	Initialize();

	while (!glfwWindowShouldClose( pWindow ))
	{
		Render();
		/* Swap front and back buffers */
		glfwSwapBuffers( pWindow );
		/* Poll for and process events */
		glfwPollEvents();
	}

	glfwTerminate();
	return 0;
}