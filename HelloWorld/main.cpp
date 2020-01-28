// Close the Error 4996 in cyTriMesh.h
#pragma warning(disable : 4996)
// Fixed the wingdi 100 errors by adding the windows header file.
#include <windows.h>

#include <GL/glew.h>
#include <cy/cyGL.h>
#include <GLFW/glfw3.h>
#include <cy/cyVector.h>
#include <cy/cyTriMesh.h>
#include <cy/cyMatrix.h>
#include <glm/glm.hpp>

GLuint VAO;
GLuint VBO;
GLuint EBO;

cyGLSLProgram g_shaderProgram;
cyGLSLShader g_vertexShader;
cyGLSLShader g_fragmentShader;

cyTriMesh g_triMesh;

cyMatrix4f g_mat_model;
cyMatrix4f g_mat_view;
cyMatrix4f g_mat_perspective;

float camera_angle_yaw = 0;
float camera_angle_pitch = 90.0f;
float camera_distance = 50.0f;

constexpr float Screen_Width = 640;
constexpr float Screen_Height = 480;

bool left_mouseBtn_drag = false;
bool right_mouseBtn_drag = false;

void CompileShaders( const char* i_path_vertexShader, const char* i_path_fragementShader, cyGLSLProgram& i_glslProgram )
{
	if (!g_vertexShader.CompileFile( "content/vertex.shader", GL_VERTEX_SHADER ))
	{
		fprintf( stderr, "Failed to compile the vertex shader.\n" );
	}
	else
	{
		fprintf( stdout, "Compiled the vertex shader successfully.\n" );
	}
	if (!g_fragmentShader.CompileFile( i_path_fragementShader, GL_FRAGMENT_SHADER ))
	{
		fprintf( stderr, "Failed to compile the fragment shader.\n" );
	}
	else
	{
		fprintf( stdout, "Compiled the fragment shader successfully.\n" );
	}
	i_glslProgram.AttachShader( g_vertexShader );
	i_glslProgram.AttachShader( g_fragmentShader );
	i_glslProgram.Link();
	assert( glGetError() == GL_NO_ERROR );
}

void InitializeShaders()
{
	g_shaderProgram.CreateProgram();
	CompileShaders( "content/vertex.shader", "content/fragment.shader", g_shaderProgram );
}

void InitializeGL()
{
	if (!g_triMesh.LoadFromFileObj( "content/teapot.objt" ))
	{
		fprintf( stderr, "Failed to load the teapot.obj.\n" );
		assert( false );
	}
	else
	{
		fprintf( stdout, "Loaded the teapot.obj successfully.\n" );
	}

	glGenVertexArrays( 1, &VAO );
	std::vector<cyVec3f> vertices;
	for (size_t i = 0; i < g_triMesh.NV(); i++)
	{
		vertices.push_back( g_triMesh.V( i ) );
	}
	glBindVertexArray( VAO );
	assert( glGetError() == GL_NO_ERROR );
	// For vertex data
	glGenBuffers( 1, &VBO );
	glBindBuffer( GL_ARRAY_BUFFER, VBO );
	glBufferData( GL_ARRAY_BUFFER, static_cast <GLsizei>(sizeof( cyVec3f ) * g_triMesh.NV()), reinterpret_cast<void*>(vertices.data()), GL_STATIC_DRAW );
	glVertexAttribPointer( 0, 3, GL_FLOAT, GL_FALSE, static_cast <GLsizei>(sizeof( cyVec3f )), (void*)0 );
	glEnableVertexAttribArray( 0 );
	assert( glGetError() == GL_NO_ERROR );
	glBindBuffer( GL_ARRAY_BUFFER, 0 );

	// For element data
	glGenBuffers( 1, &EBO );
	glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, EBO );
	std::vector<unsigned int> indices;
	for (size_t i = 0; i < g_triMesh.NF(); i++)
	{
		auto triFace = g_triMesh.F( i );
		for (size_t j = 0; j < 3; j++)
		{
			indices.push_back( triFace.v[j] );
		}
	}
	glBufferData( GL_ELEMENT_ARRAY_BUFFER, static_cast <GLsizeiptr>(sizeof( unsigned int ) * 3 * g_triMesh.NF()), reinterpret_cast<void*>(&indices[0]), GL_STATIC_DRAW );
	assert( glGetError() == GL_NO_ERROR );
	glBindBuffer( GL_ARRAY_BUFFER, 0 );
	glBindVertexArray( 0 );
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
	// Bind the shader program to gl.
	// Bind the vertex array to gl.
	{
		g_shaderProgram.Bind();
		assert( glGetError() == GL_NO_ERROR );
		glBindVertexArray( VAO );
		assert( glGetError() == GL_NO_ERROR );
	}
	// Draw elements
	{
		const GLvoid* const offset = 0;
		glDrawElements( GL_TRIANGLES, static_cast<GLsizei>(3 * g_triMesh.NF()), GL_UNSIGNED_INT, offset );
		assert( glGetError() == GL_NO_ERROR );
		glBindVertexArray( 0 );
	}
}

void UpdateCamera()
{
	g_shaderProgram.Bind();

	g_mat_model = cyMatrix4f( 1.0f );
	g_mat_view = cyMatrix4f( 1.0f );
	g_mat_perspective = cyMatrix4f( 1.0f );

	g_mat_model.SetRotationXYZ( glm::radians( -camera_angle_pitch ), glm::radians( -camera_angle_yaw ), 0 );
	g_mat_view.SetTranslation( cyVec3f( 0, 0, -camera_distance ) );
	g_mat_perspective.SetPerspective( glm::radians( 45.0f ), Screen_Width / Screen_Height, 0.1f, 100.0f );

	unsigned int modelLoc = glGetUniformLocation( g_shaderProgram.GetID(), "model" );
	glUniformMatrix4fv( modelLoc, 1, GL_FALSE, g_mat_model.cell );
	assert( glGetError() == GL_NO_ERROR );

	unsigned int viewLoc = glGetUniformLocation( g_shaderProgram.GetID(), "view" );
	glUniformMatrix4fv( viewLoc, 1, GL_FALSE, g_mat_view.cell );
	assert( glGetError() == GL_NO_ERROR );

	unsigned int projectionLoc = glGetUniformLocation( g_shaderProgram.GetID(), "projection" );
	glUniformMatrix4fv( projectionLoc, 1, GL_FALSE, g_mat_perspective.cell );
	assert( glGetError() == GL_NO_ERROR );
}


void KeyboardCallback( GLFWwindow* i_pWindow, int i_key, int i_scancode, int i_action, int i_mods )
{
	assert( i_pWindow );
}

void MouseButtonCallback( GLFWwindow* i_pWindow, int i_button, int i_action, int i_mods )
{
	assert( i_pWindow );
	if (i_button == GLFW_MOUSE_BUTTON_LEFT)
	{
		if (i_action == GLFW_PRESS)
		{
			left_mouseBtn_drag = true;
		}
		else if (i_action == GLFW_RELEASE)
		{
			left_mouseBtn_drag = false;
		}
	}
	if (i_button == GLFW_MOUSE_BUTTON_RIGHT)
	{
		if (i_action == GLFW_PRESS)
		{
			right_mouseBtn_drag = true;
		}
		else if (i_action == GLFW_RELEASE)
		{
			right_mouseBtn_drag = false;
		}
	}
}

void UpdateMouseInput( GLFWwindow* i_pWindow )
{
	// Get a new camera distance
	if (right_mouseBtn_drag)
	{

	}
	// Get a new camera rotation
	if (left_mouseBtn_drag)
	{
		static cyVec2d _cachedLeftMousePos( 0, 0 );
		double pos_x;
		double pos_y;
		glfwGetCursorPos( i_pWindow, &pos_x, &pos_y );
		fprintf( stdout, "The mouse position is %.3f, %.3f\n", pos_x, pos_y );
		auto pos_mouse = cyVec2d( pos_x, pos_y );
		auto dis = _cachedLeftMousePos - pos_mouse;
		dis.Normalize();
		_cachedLeftMousePos = pos_mouse;
		// Rotate around yaw
		if (dis.Dot( cyVec2d( 1, 0 ) ) > 0)
		{
			camera_angle_yaw -= 0.2f;
		}
		else if (dis.Dot( cyVec2d( 1, 0 ) ) < 0)
		{
			camera_angle_yaw += 0.2f;
		}
		// Rotate around the pitch
		if (dis.Dot( cyVec2d( 0, 1 ) ) > 0)
		{
			camera_angle_pitch -= 0.2f;
		}
		else if (dis.Dot( cyVec2d( 0, 1 ) ) < 0)
		{
			camera_angle_pitch += 0.2f;
		}
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
	pWindow = glfwCreateWindow( Screen_Width, Screen_Height, "Hello World", NULL, NULL );
	if (!pWindow)
	{
		glfwTerminate();
		return -1;
	}
	glfwWindowHint( GLFW_DEPTH_BITS, GL_TRUE );
	// Register the mouse and keyboard callback.
	glfwSetMouseButtonCallback( pWindow, MouseButtonCallback );
	glfwSetKeyCallback( pWindow, KeyboardCallback );
	/* Make the window's context current */
	glfwMakeContextCurrent( pWindow );

	GLenum err = glewInit();
	if (GLEW_OK != err)
	{
		/* Problem: glewInit failed, something is seriously wrong. */
		fprintf( stderr, "Initialized GLEW failed, Error: %s\n", glewGetErrorString( err ) );
	}

	InitializeShaders();
	InitializeGL();

	while (!glfwWindowShouldClose( pWindow ))
	{
		UpdateMouseInput( pWindow );
		UpdateCamera();
		Display();
		/* Swap front and back buffers */
		glfwSwapBuffers( pWindow );
		/* Poll for and process events */
		glfwPollEvents();
	}
	// Release buffers and the shader program.
	{
		glDeleteVertexArrays( 1, &VAO );
		assert( glGetError() == GL_NO_ERROR );
		glDeleteBuffers( 1, &VBO );
		assert( glGetError() == GL_NO_ERROR );
		glDeleteBuffers( 1, &EBO );
		assert( glGetError() == GL_NO_ERROR );
		g_vertexShader.Delete();
		g_fragmentShader.Delete();
		g_shaderProgram.Delete();
		assert( glGetError() == GL_NO_ERROR );
	}

	glfwTerminate();
	return 0;
}