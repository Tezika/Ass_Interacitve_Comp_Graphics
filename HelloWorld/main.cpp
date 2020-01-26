#pragma warning(disable : 4996)
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <cy/cyVector.h>
#include <cy/cyTriMesh.h>

GLuint VAO;
GLuint VBO;
cy::TriMesh triMesh;

void Initialize()
{
	if (!triMesh.LoadFromFileObj( "content/teapot.obj" ))
	{
		fprintf( stderr, "Failed to load the teapot.obj\n" );
	}
	else
	{
		fprintf( stdout, "Loaded the teapot.obj successfully\n" );
	}

	glGenVertexArrays( 1, &VAO );
	glGenBuffers( 1, &VBO );

	std::vector<cy::Vec3f> vertices;
	for (size_t i = 0; i < triMesh.NV(); i++)
	{
		vertices.push_back( triMesh.V( i ) );
	}

	glBindVertexArray( VAO );
	glEnableVertexAttribArray( 0 );
	glBindBuffer( GL_ARRAY_BUFFER, VBO );
	glBufferData( GL_ARRAY_BUFFER, static_cast <GLsizeiptr>(sizeof( cy::Vec3f ) * triMesh.NV()), reinterpret_cast<void*>(&vertices[0]), GL_STATIC_DRAW );
	glVertexAttribPointer( 0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof( GL_FLOAT ), 0 );
}

void Display()
{
	glClearColor( 0, 0, 0.2, 0 );
	glClear( GL_COLOR_BUFFER_BIT );
	glBindVertexArray( VAO );
	glDrawArrays( GL_TRIANGLES, 0, triMesh.NV() );
}

int main( void )
{
	GLFWwindow* pWindow;
	/* Initialize the library */
	if (!glfwInit())
	{
		/* Problem: glewInit failed, something is seriously wrong. */
		fprintf( stderr, "Initialized GLFW failed\n" );
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

	Initialize();

	while (!glfwWindowShouldClose( pWindow ))
	{
		Display();
		/* Swap front and back buffers */
		glfwSwapBuffers( pWindow );
		/* Poll for and process events */
		glfwPollEvents();
	}

	glfwTerminate();
	return 0;
}