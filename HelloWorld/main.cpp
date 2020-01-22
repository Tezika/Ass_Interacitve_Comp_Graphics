#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/vec3.hpp>


int main( void )
{
	GLFWwindow* pWindow;

	/* Initialize the library */
	if (!glfwInit())
	{
		return -1;
	}
	if (!glewInit())
	{
		return -1;
	}
	/* Create a windowed mode window and its OpenGL context */
	pWindow = glfwCreateWindow( 640, 480, "Hello World", NULL, NULL );
	if (!pWindow)
	{
		glfwTerminate();
		return -1;
	}
	glm::vec3 vertices[1];
	vertices[0] = glm::vec3( 0.0f, 0.0f, 0.0f );
	GLuint VBO;
	glGenBuffers( 1, &VBO );
	glBindBuffer( GL_ARRAY_BUFFER, VBO );
	glBufferData( GL_ARRAY_BUFFER, sizeof( vertices ), vertices, GL_STATIC_DRAW );
	glfwTerminate();
	return 0;
}