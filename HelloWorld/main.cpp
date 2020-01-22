#include <GLFW/glfw3.h>
#include <glm/vec3.hpp>

int main( void )
{
	GLFWwindow* window;

	/* Initialize the library */
	if (!glfwInit())
		return -1;

	/* Create a windowed mode window and its OpenGL context */
	window = glfwCreateWindow( 640, 480, "Hello World", NULL, NULL );
	if (!window)
	{
		glfwTerminate();
		return -1;
	}
	glfwTerminate();
	return 0;
}