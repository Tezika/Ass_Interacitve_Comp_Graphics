// Close the Error 4996 in cyTriMesh.h
#pragma warning(disable : 4996)

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <cy/cyVector.h>
#include <cy/cyTriMesh.h>

GLuint VAO;
GLuint VBO;
GLuint EBO;
cyTriMesh triMesh;
void Display();

void Initialize()
{
	if (!triMesh.LoadFromFileObj( "content/teapot.obj" ))
	{
		fprintf( stderr, "Failed to load the teapot.obj.\n" );
	}
	else
	{
		fprintf( stdout, "Loaded the teapot.obj successfully.\n" );
	}

	glGenVertexArrays( 1, &VAO );
	std::vector<cyVec3f> vertices;
	for (size_t i = 0; i < triMesh.NV(); i++)
	{
		vertices.push_back( triMesh.V( i ) );
	}

	glBindVertexArray( VAO );
	assert( glGetError() == GL_NO_ERROR );
	// For vertex data
	glGenBuffers( 1, &VBO );
	glBindBuffer( GL_ARRAY_BUFFER, VBO );
	glBufferData( GL_ARRAY_BUFFER, static_cast <GLsizeiptr>(sizeof( cyVec3f ) * triMesh.NV()), reinterpret_cast<void*>(vertices.data()), GL_STATIC_DRAW );
	glVertexAttribPointer( 0, 3, GL_FLOAT, GL_FALSE, static_cast<GLsizei>(sizeof( cyVec3f )), reinterpret_cast<GLvoid*>(offsetof( cyVec3f, x )) );
	glEnableVertexAttribArray( 0 );
	assert( glGetError() == GL_NO_ERROR );
	// For element data
	glGenBuffers( 1, &EBO );
	glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, EBO );
	std::vector<unsigned int> indices;
	for (size_t i = 0; i < triMesh.NF(); i++)
	{
		auto triFace = triMesh.F( i );
		for (size_t j = 0; j < 3; j++)
		{
			indices.push_back( triFace.v[j] );
		}
	}
	glBufferData( GL_ELEMENT_ARRAY_BUFFER, static_cast <GLsizeiptr>(sizeof( unsigned int ) * 3 * triMesh.NF()), reinterpret_cast<void*>(indices.data()), GL_STATIC_DRAW );
	assert( glGetError() == GL_NO_ERROR );
}

void Display()
{
	// Every frame an entirely new image will be created.
	// Before drawing anything, then, the previous image will be erased
	// by "clearing" the image buffer (filling it with a solid color)
	{
		// Black is usually used
		{
			glClearColor( 0, 0, 0.2, 0 );
			assert( glGetError() == GL_NO_ERROR );
		}
		{
			constexpr GLbitfield clearColor = GL_COLOR_BUFFER_BIT;
			glClear( clearColor );
			assert( glGetError() == GL_NO_ERROR );
		}
	}
	// In addition to the color buffer there is also a hidden image called the "depth buffer"
	// which is used to make it less important which order draw calls are made.
	// It must also be "cleared" every frame just like the visible color buffer.
	{
		{
			glDepthMask( GL_TRUE );
			assert( glGetError() == GL_NO_ERROR );
			constexpr GLclampd clearToFarDepth = 1.0;
			glClearDepth( clearToFarDepth );
			assert( glGetError() == GL_NO_ERROR );
		}
		{
			constexpr GLbitfield clearDepth = GL_DEPTH_BUFFER_BIT;
			glClear( clearDepth );
			assert( glGetError() == GL_NO_ERROR );
		}
	}
	// Bind the vertex array to gl.
	{
		glBindVertexArray( VAO );
		assert( glGetError() == GL_NO_ERROR );
	}
	// Draw elements
	{
		const GLvoid* const offset = 0;
		glDrawElements( GL_TRIANGLES, static_cast<GLsizei>(3 * triMesh.NF()), GL_UNSIGNED_INT, offset );
		assert( glGetError() == GL_NO_ERROR );
	}
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