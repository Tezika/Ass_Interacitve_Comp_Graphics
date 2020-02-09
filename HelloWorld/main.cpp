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
#include <glm/common.hpp>
#include "lodepng.h"

GLuint VAO;
GLuint VBO;
GLuint EBO;
GLuint TEX;

cyGLSLProgram g_shaderProgram;
cyGLSLShader g_vertexShader;
cyGLSLShader g_fragmentShader;

cyTriMesh g_triMesh;

bool bRotateLight = false;

float camera_angle_yaw = 0;
float camera_angle_pitch = 90.0f;
float camera_distance = 50.0f;

float rotation_yaw = 1.0f;
float rotation_pitch = 0.5f;

float light_angle_yaw = 0.0f;
float light_angle_pitch = 0.0f;

constexpr float Screen_Width = 640;
constexpr float Screen_Height = 480;

constexpr char const* path_vertexShader = "content/shaders/vertex.shader";
constexpr char const* path_fragmentShader = "content/shaders/fragment.shader";

constexpr char const* path_teapotResource = "content/teapot/";

bool left_mouseBtn_drag = false;
bool right_mouseBtn_drag = false;

void CompileShaders( const char* i_path_vertexShader, const char* i_path_fragementShader, cyGLSLProgram& i_glslProgram )
{
	g_vertexShader.Delete();
	g_fragmentShader.Delete();
	assert( glGetError() == GL_NO_ERROR );
	i_glslProgram.CreateProgram();
	assert( glGetError() == GL_NO_ERROR );
	if (!g_vertexShader.CompileFile( i_path_vertexShader, GL_VERTEX_SHADER ))
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
	g_shaderProgram.AttachShader( g_vertexShader );
	g_shaderProgram.AttachShader( g_fragmentShader );
	g_shaderProgram.Link();
	assert( glGetError() == GL_NO_ERROR );
}

void LoadTextureData( const char* i_textureName, std::vector<unsigned char>& io_data, unsigned int& io_width, unsigned int& io_height )
{
	std::string pathForTexture( path_teapotResource );
	pathForTexture += i_textureName;
	auto errorCode = lodepng::decode( io_data, io_width, io_height, pathForTexture );
	if (errorCode)
	{
		fprintf( stderr, "decoder error %d : %s", errorCode, lodepng_error_text( errorCode ) );
	}
	else
	{
		fprintf( stdout, "Loaded/Decoded the %s texture successfully.", i_textureName );
	}
}

void InitializeTextures()
{
	glGenTextures( 1, &TEX );
	glBindTexture( GL_TEXTURE_2D, TEX );

	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );

	unsigned int tex_width;
	unsigned int tex_height;
	std::vector<unsigned char> tex_data;
	LoadTextureData( g_triMesh.M( 0 ).map_Kd, tex_data, tex_width, tex_height );
	if (tex_data.data())
	{
		glTexImage2D( GL_TEXTURE_2D, 0, GL_RGB, tex_width, tex_height, 0, GL_RGB, GL_UNSIGNED_BYTE, reinterpret_cast<void*>(tex_data.data()) );
		glGenerateMipmap( GL_TEXTURE_2D );
	}
	g_shaderProgram.Bind();
	glUniform1i( glGetUniformLocation( g_shaderProgram.GetID(), "texture1" ), 0 );
}

void InitializeTrimesh( const char* i_objFileName )
{
	std::string path_objFile( path_teapotResource );
	path_objFile += i_objFileName;
	if (!g_triMesh.LoadFromFileObj( path_objFile.c_str(), true ))
	{
		fprintf( stderr, "Failed to load the %s.\n", i_objFileName );
		assert( false );
	}
	else
	{
		fprintf( stdout, "Loaded the %s successfully.\n", i_objFileName );
	}
	if (!g_triMesh.IsBoundBoxReady())
	{
		g_triMesh.ComputeBoundingBox();
	}
	// Load texture data

	glGenVertexArrays( 1, &VAO );
	std::vector<cyVec3f> vertices;
	std::vector<cyVec3f> vertexNormals;
	std::vector<cyVec2f> texCoords;
	for (int i = 0; i < g_triMesh.NV(); i++)
	{
		vertices.push_back( g_triMesh.V( i ) );
	}
	for (int i = 0; i < g_triMesh.NVN(); i++)
	{
		//auto curNormalFace = g_triMesh.FN( i );
		//for (int j = 0; j < 3; j++)
		//{
		//	vertexNormals.push_back( g_triMesh.VN( curNormalFace.v[j] ) );
		//}
		vertexNormals.push_back( g_triMesh.VN( i ) );
	}
	for (int i = 0; i < g_triMesh.NF(); i++)
	{
		auto curTexFace = g_triMesh.FT( i );
		for (int j = 0; j < 3; j++)
		{
			texCoords.push_back( g_triMesh.VT( curTexFace.v[j] ).XY());
		}
	}
	glBindVertexArray( VAO );
	assert( glGetError() == GL_NO_ERROR );
	const auto sizeOfVertices = static_cast<GLsizei>(vertices.size() * sizeof( cyVec3f ));
	const auto sizeOfNormals = static_cast<GLsizei>(vertexNormals.size() * sizeof( cyVec3f ));
	const auto sizeOfTexCoord = static_cast<GLsizei>(texCoords.size() * sizeof( cyVec2f ));
	// For vertex data buffer
	glGenBuffers( 1, &VBO );
	glBindBuffer( GL_ARRAY_BUFFER, VBO );
	glBufferData( GL_ARRAY_BUFFER, sizeOfVertices + sizeOfNormals + sizeOfTexCoord, NULL, GL_STATIC_DRAW );
	glBufferSubData( GL_ARRAY_BUFFER, 0, sizeOfVertices, reinterpret_cast<void*>(vertices.data()) );
	glBufferSubData( GL_ARRAY_BUFFER, sizeOfVertices, sizeOfNormals, reinterpret_cast<void*>(vertexNormals.data()) );
	glBufferSubData( GL_ARRAY_BUFFER, sizeOfVertices + sizeOfNormals, sizeOfTexCoord, reinterpret_cast<void*>(texCoords.data()) );

	glVertexAttribPointer( 0, 3, GL_FLOAT, GL_FALSE, static_cast<GLsizei>(sizeof( cyVec3f )), (const GLvoid*)(0) );
	glVertexAttribPointer( 1, 3, GL_FLOAT, GL_FALSE, static_cast<GLsizei>(sizeof( cyVec3f )), (const GLvoid*)(sizeOfVertices) );
	glVertexAttribPointer( 2, 2, GL_FLOAT, GL_FALSE, static_cast<GLsizei>(sizeof( cyVec2f )), (const GLvoid*)(sizeOfVertices + sizeOfNormals) );

	glEnableVertexAttribArray( 0 );
	glEnableVertexAttribArray( 1 );
	glEnableVertexAttribArray( 2 );

	glBindBuffer( GL_ARRAY_BUFFER, 0 );
	assert( glGetError() == GL_NO_ERROR );

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
			glClearColor( 0, 0, 0, 0 );
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
	// Bind the textures to gl.
	// Bind the shader program to gl.
	// Bind the vertex array to gl.
	{
		glActiveTexture( GL_TEXTURE0 );
		glBindTexture( GL_TEXTURE_2D, TEX );
		assert( glGetError() == GL_NO_ERROR );
		g_shaderProgram.Bind();
		assert( glGetError() == GL_NO_ERROR );
		glBindVertexArray( VAO );
		assert( glGetError() == GL_NO_ERROR );
	}

	// Draw elements
	{
		const GLvoid* const offset = 0;
		glDrawElements( GL_TRIANGLES, static_cast<GLsizei>(3 * g_triMesh.NF()), GL_UNSIGNED_INT, offset );
		//glDrawArrays( GL_TRIANGLES, 0, 3 * g_triMesh.NF());
		assert( glGetError() == GL_NO_ERROR );
		glBindVertexArray( 0 );
	}
}

void UpdateCamera()
{
	g_shaderProgram.Bind();

	auto mat_model = cyMatrix4f( 1.0f );
	auto mat_light = cyMatrix4f( 1.0f );
	auto mat_view = cyMatrix4f( 1.0f );
	auto mat_perspective = cyMatrix4f( 1.0f );
	auto mat_modelToProjection = cyMatrix4f( 1.0f );
	auto mat_modelToView = cyMatrix4f( 1.0f );

	mat_model.SetRotationXYZ( glm::radians( -camera_angle_pitch ), glm::radians( -camera_angle_yaw ), 0 );
	mat_light.SetRotationXYZ( glm::radians( light_angle_pitch ), glm::radians( light_angle_yaw ), 0 );

	auto cameraTarget = (g_triMesh.GetBoundMax() + g_triMesh.GetBoundMin()) / 2;
	auto cameraPosition = cyVec3f( 0, 0, camera_distance );
	auto cameraDir = (cameraTarget - cameraPosition).GetNormalized();
	auto worldUp = cyVec3f( 0, 1, 0 );
	auto cameraRight = worldUp.Cross( cameraDir ).GetNormalized();
	auto cameraUp = cameraDir.Cross( cameraRight ).GetNormalized();

	auto mat_cameraRotation = cyMatrix4f( 1.0f );
	mat_cameraRotation.SetRow( 0, cameraRight.x, cameraRight.y, cameraRight.z, 0 );
	mat_cameraRotation.SetRow( 1, cameraUp.x, cameraUp.y, cameraUp.z, 0 );
	mat_cameraRotation.SetRow( 2, cameraDir.x, cameraDir.y, cameraDir.z, 0 );
	mat_cameraRotation.SetRow( 3, 0, 0, 0, 1 );

	auto mat_cameraTranslation = cyMatrix4f( 1.0f );
	mat_cameraTranslation.SetTranslation( cameraPosition );
	mat_view = mat_cameraRotation * mat_cameraTranslation;

	mat_perspective.SetPerspective( glm::radians( 60.0f ), Screen_Width / Screen_Height, 0.1f, 100.0f );
	mat_modelToView = mat_view * mat_model;
	auto mat_normalMatToView = mat_modelToView.GetInverse().GetTranspose();
	mat_modelToProjection = mat_perspective * mat_view * mat_model;

	unsigned int modelToProjection = glGetUniformLocation( g_shaderProgram.GetID(), "mat_modelToProjection" );
	glUniformMatrix4fv( modelToProjection, 1, GL_FALSE, mat_modelToProjection.cell );

	unsigned int modelToView = glGetUniformLocation( g_shaderProgram.GetID(), "mat_modelToView" );
	glUniformMatrix4fv( modelToView, 1, GL_FALSE, mat_modelToView.cell );
	assert( glGetError() == GL_NO_ERROR );

	unsigned int normalModelToView = glGetUniformLocation( g_shaderProgram.GetID(), "mat_normalModelToView" );
	glUniformMatrix4fv( normalModelToView, 1, GL_FALSE, mat_normalMatToView.cell );
	assert( glGetError() == GL_NO_ERROR );

	unsigned int lightTransformation = glGetUniformLocation( g_shaderProgram.GetID(), "mat_lightTransformation" );
	glUniformMatrix4fv( lightTransformation, 1, GL_FALSE, mat_light.cell );
}

void KeyboardCallback( GLFWwindow* i_pWindow, int i_key, int i_scancode, int i_action, int i_mods )
{
	assert( i_pWindow );
	if (i_key == GLFW_KEY_F6 && i_action == GLFW_PRESS)
	{
		CompileShaders( path_vertexShader, path_fragmentShader, g_shaderProgram );
	}
	if (i_key == GLFW_KEY_LEFT_CONTROL)
	{
		if (i_action == GLFW_PRESS)
		{
			bRotateLight = true;
		}
		else if (i_action == GLFW_RELEASE)
		{
			bRotateLight = false;
		}
	}
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
		static cyVec2d _cachedRightMousePos( 0, 0 );
		double pos_x;
		double pos_y;
		glfwGetCursorPos( i_pWindow, &pos_x, &pos_y );
		auto pos_mouse = cyVec2d( pos_x, pos_y );
		auto dis = _cachedRightMousePos - pos_mouse;
		dis.Normalize();
		_cachedRightMousePos = pos_mouse;
		if (dis.Dot( cyVec2d( 0, 1 ) ) > 0)
		{
			camera_distance -= 0.3f;
		}
		else if (dis.Dot( cyVec2d( 0, 1 ) ) < 0)
		{
			camera_distance += 0.3f;
		}
	}
	// Get a new camera rotation
	if (left_mouseBtn_drag)
	{
		static cyVec2d _cachedLeftMousePos( 0, 0 );
		double pos_x;
		double pos_y;
		glfwGetCursorPos( i_pWindow, &pos_x, &pos_y );
		auto pos_mouse = cyVec2d( pos_x, pos_y );
		auto dis = _cachedLeftMousePos - pos_mouse;
		dis.Normalize();
		_cachedLeftMousePos = pos_mouse;
		// Rotate around yaw
		if (dis.Dot( cyVec2d( 1, 0 ) ) > 0)
		{
			if (!bRotateLight)
			{
				camera_angle_yaw -= rotation_yaw;
			}
			else
			{
				light_angle_yaw -= rotation_yaw;
			}

		}
		else if (dis.Dot( cyVec2d( 1, 0 ) ) < 0)
		{
			if (!bRotateLight)
			{
				camera_angle_yaw += rotation_yaw;
			}
			else
			{
				light_angle_yaw += rotation_yaw;
			}
		}
		// Rotate around the pitch
		if (dis.Dot( cyVec2d( 0, 1 ) ) > 0)
		{
			if (!bRotateLight)
			{
				camera_angle_pitch -= rotation_pitch;
			}
			else
			{
				light_angle_pitch -= rotation_pitch;
			}
		}
		else if (dis.Dot( cyVec2d( 0, 1 ) ) < 0)
		{
			if (!bRotateLight)
			{
				camera_angle_pitch += rotation_pitch;
			}
			else
			{
				light_angle_pitch += rotation_pitch;
			}
		}
	}
}

int main( int argc, char* argv[] )
{
	if (argc < 2)
	{
		fprintf( stderr, "Please enter the .obj file name as the argument.\n" );
		return -1;
	}
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
	glfwWindowHint( GLFW_DEPTH_BITS, GLFW_TRUE );
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
	glEnable( GL_DEPTH_TEST );
	glEnable( GL_TEXTURE_2D );
	CompileShaders( path_vertexShader, path_fragmentShader, g_shaderProgram );
	InitializeTrimesh( argv[1] );
	InitializeTextures();

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
		glDeleteBuffers( 1, &VBO );
		glDeleteBuffers( 1, &EBO );
		glDeleteTextures( 1, &TEX );
		assert( glGetError() == GL_NO_ERROR );
		g_vertexShader.Delete();
		g_fragmentShader.Delete();
		g_shaderProgram.Delete();
		assert( glGetError() == GL_NO_ERROR );
	}

	glfwTerminate();
	return 0;
}