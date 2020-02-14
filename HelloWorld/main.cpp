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
#include "Texture.h"

#define RENDER_TO_TEXTURE

GLuint VAO;
GLuint VBO;
GLuint EBO;

GLuint VAO_renderTex;
GLuint VBO_renderTex;

Ass_Inter_Comp_Graphics::Texture* pDiffuseTex;
Ass_Inter_Comp_Graphics::Texture* pSpecularTex;

cyGLRenderTexture2D g_renderToTex2D;

cyGLSLProgram g_teapotShaderProgram;
cyGLSLProgram g_renderTexShaderProgram;

cyTriMesh g_triMesh;

bool bRotateLight = false;

float camera_angle_yaw = 0;
float camera_angle_pitch = 90.0f;
float camera_distance = 50.0f;

float rotation_yaw = 2.0f;
float rotation_pitch = 1.5f;

float light_angle_yaw = 0.0f;
float light_angle_pitch = 0.0f;

constexpr float Screen_Width = 640;
constexpr float Screen_Height = 480;

constexpr char const* path_vertexShader_teapot = "content/shaders/vertex_teapot.shader";
constexpr char const* path_fragmentShader_teapot = "content/shaders/fragment_teapot.shader";

constexpr char const* path_vertexShader_renderTex = "content/shaders/vertex_renderTex.shader";
constexpr char const* path_fragmentShader_renderTex = "content/shaders/fragment_renderTex.shader";

constexpr char const* path_teapotResource = "content/teapot/";

bool left_mouseBtn_drag = false;
bool right_mouseBtn_drag = false;

const GLfloat g_quad_vertex_buffer_data[] = {
-1.0f, -1.0f, 0.0f,
1.0f, -1.0f, 0.0f,
-1.0f,  1.0f, 0.0f,
-1.0f,  1.0f, 0.0f,
1.0f, -1.0f, 0.0f,
1.0f,  1.0f, 0.0f,
};

void CompileShaders( const char* i_path_vertexShader, const char* i_path_fragementShader, cyGLSLProgram& i_glslProgram )
{
	i_glslProgram.Delete();
	assert( glGetError() == GL_NO_ERROR );
	i_glslProgram.CreateProgram();
	assert( glGetError() == GL_NO_ERROR );
	cyGLSLShader vertexShader;
	cyGLSLShader fragmentShader;
	if (!vertexShader.CompileFile( i_path_vertexShader, GL_VERTEX_SHADER ))
	{
		fprintf( stderr, "Failed to compile the vertex shader, %s.\n", i_path_vertexShader );
	}
	else
	{
		fprintf( stdout, "Compiled the vertex shader, %s, successfully.\n", i_path_vertexShader);
	}
	if (!fragmentShader.CompileFile( i_path_fragementShader, GL_FRAGMENT_SHADER ))
	{
		fprintf( stderr, "Failed to compile the fragment shader, %s.\n", i_path_fragementShader);
	}
	else
	{
		fprintf( stdout, "Compiled the fragment shader, %s, successfully.\n", i_path_fragementShader );
	}
	i_glslProgram.AttachShader(vertexShader);
	i_glslProgram.AttachShader(fragmentShader);
	i_glslProgram.Link();
	assert( glGetError() == GL_NO_ERROR );
}

void InitializeMaterial()
{
	g_teapotShaderProgram.Bind();
	glUniform3f( glGetUniformLocation( g_teapotShaderProgram.GetID(), "diffuseColor" ), g_triMesh.M( 0 ).Kd[0], g_triMesh.M( 0 ).Kd[1], g_triMesh.M( 0 ).Kd[2] );
	glUniform3f( glGetUniformLocation( g_teapotShaderProgram.GetID(), "ambientColor" ), g_triMesh.M( 0 ).Ka[0], g_triMesh.M( 0 ).Ka[1], g_triMesh.M( 0 ).Ka[2] );
	glUniform3f( glGetUniformLocation( g_teapotShaderProgram.GetID(), "specularColor"), g_triMesh.M( 0 ).Ks[0], g_triMesh.M( 0 ).Ks[1], g_triMesh.M( 0 ).Ks[2] );
}

void InitializeRenderTexture()
{
	if (!g_renderToTex2D.Initialize(true))
	{
		fprintf(stderr, "Cannot generate the renderToTexture.");
		assert(false);
	}

	g_renderToTex2D.Resize(3, Screen_Width, Screen_Height);
	// Generate the render to texture's vbo and vao
	glGenVertexArrays(1, &VAO_renderTex);
	glBindVertexArray(VAO_renderTex);
	
	glGenBuffers(1, &VBO_renderTex);
	glBindBuffer(GL_ARRAY_BUFFER, VBO_renderTex);
	glBufferData(GL_ARRAY_BUFFER, sizeof(g_quad_vertex_buffer_data), g_quad_vertex_buffer_data, GL_STATIC_DRAW);

	CompileShaders(path_vertexShader_renderTex, path_fragmentShader_renderTex, g_renderTexShaderProgram);
	g_renderTexShaderProgram.Bind();
	glUniform1i(glGetUniformLocation(g_renderTexShaderProgram.GetID(), "tex_render"), 0);

	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER,0);
}

void InitializeTextures()
{
	std::string path_diffusesTex( path_teapotResource );
	path_diffusesTex += g_triMesh.M( 0 ).map_Kd;
	pDiffuseTex = Ass_Inter_Comp_Graphics::Texture::Create( path_diffusesTex.c_str());
	if (!pDiffuseTex)
	{
		fprintf( stderr, "Failed to create the diffuse texture.\n" );
		assert( false );
	}
	std::string path_specularTex( path_teapotResource );
	path_specularTex += g_triMesh.M( 0 ).map_Ks;
	pSpecularTex = Ass_Inter_Comp_Graphics::Texture::Create( path_specularTex.c_str());
	if (!pSpecularTex)
	{
		fprintf( stderr, "Failed to create the specular texture.\n" );
		assert( false );
	}
	g_teapotShaderProgram.Bind();
	glUniform1i( glGetUniformLocation( g_teapotShaderProgram.GetID(), "tex_diff" ), 0 );
	glUniform1i( glGetUniformLocation( g_teapotShaderProgram.GetID(), "tex_spec" ), 1 );
}

void InitializeMesh( const char* i_objFileName )
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
	for (int i = 0; i < g_triMesh.NF(); i++)
	{
		auto triFace = g_triMesh.F( i );
		for (int j = 0; j < 3; j++)
		{
			vertices.push_back( g_triMesh.V( triFace.v[j] ) );
		}
	}
	for (int i = 0; i < g_triMesh.NF(); i++)
	{
		auto curNormalFace = g_triMesh.FN( i );
		for (int j = 0; j < 3; j++)
		{
			vertexNormals.push_back( g_triMesh.VN( curNormalFace.v[j] ) );
		}
	}
	for (int i = 0; i < g_triMesh.NF(); i++)
	{
		auto curTexFace = g_triMesh.FT( i );
		for (int j = 0; j < 3; j++)
		{
			texCoords.push_back( g_triMesh.VT( curTexFace.v[j] ).XY() );
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
	// Bind the shader program to gl.
	// Bind the vertex array to gl.
	// Bind the textures to gl.
	{
		pDiffuseTex->Bind(GL_TEXTURE0, GL_TEXTURE_2D);
		pSpecularTex->Bind(GL_TEXTURE1, GL_TEXTURE_2D);
		g_teapotShaderProgram.Bind();
		glBindVertexArray(VAO);
		const GLvoid* const offset = 0;
		glDrawArrays( GL_TRIANGLES, 0, 3 * g_triMesh.NF() );
		assert( glGetError() == GL_NO_ERROR );
		glBindVertexArray( 0 );
#if defined(RENDER_TO_TEXTURE)
		g_renderToTex2D.Bind();
#endif
		// Render the render texture;
		g_renderTexShaderProgram.Bind();
		glBindVertexArray(VAO_renderTex);
		glDrawArrays(GL_TRIANGLES, 0, sizeof(g_quad_vertex_buffer_data));
		assert(glGetError() == GL_NO_ERROR);
		g_renderToTex2D.Unbind();
	}
}

void UpdateCamera()
{
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

	g_teapotShaderProgram.Bind();

	glUniformMatrix4fv(glGetUniformLocation(g_teapotShaderProgram.GetID(), "mat_modelToProjection"), 1, GL_FALSE, mat_modelToProjection.cell );
	glUniformMatrix4fv(glGetUniformLocation(g_teapotShaderProgram.GetID(), "mat_modelToView"), 1, GL_FALSE, mat_modelToView.cell );
	glUniformMatrix4fv(glGetUniformLocation(g_teapotShaderProgram.GetID(), "mat_normalModelToView"), 1, GL_FALSE, mat_normalMatToView.cell );
	glUniformMatrix4fv(glGetUniformLocation(g_teapotShaderProgram.GetID(), "mat_lightTransformation"), 1, GL_FALSE, mat_light.cell);
	assert( glGetError() == GL_NO_ERROR );

	g_renderTexShaderProgram.Bind();
	glUniformMatrix4fv(glGetUniformLocation(g_renderTexShaderProgram.GetID(), "mat_modelToProjection"), 1, GL_FALSE, mat_modelToProjection.cell);
	glUniformMatrix4fv(glGetUniformLocation(g_renderTexShaderProgram.GetID(), "mat_modelToView"), 1, GL_FALSE, mat_modelToView.cell);
	glUniformMatrix4fv(glGetUniformLocation(g_renderTexShaderProgram.GetID(), "mat_normalModelToView"), 1, GL_FALSE, mat_normalMatToView.cell);
	assert(glGetError() == GL_NO_ERROR);
}

void KeyboardCallback( GLFWwindow* i_pWindow, int i_key, int i_scancode, int i_action, int i_mods )
{
	assert( i_pWindow );
	if (i_key == GLFW_KEY_F6 && i_action == GLFW_PRESS)
	{
		CompileShaders( path_vertexShader_teapot, path_fragmentShader_teapot, g_teapotShaderProgram );
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
	CompileShaders( path_vertexShader_teapot, path_fragmentShader_teapot, g_teapotShaderProgram );

	InitializeMesh( argv[1] );
	InitializeMaterial();
	InitializeTextures();

#if defined(RENDER_TO_TEXTURE)
	InitializeRenderTexture();
#endif

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
		assert( glGetError() == GL_NO_ERROR );
		g_teapotShaderProgram.Delete();
		assert( glGetError() == GL_NO_ERROR );
		delete pDiffuseTex;
		pDiffuseTex = nullptr;
		delete pSpecularTex;
		pSpecularTex = nullptr;
		assert(glGetError() == GL_NO_ERROR);

#if defined(RENDER_TO_TEXTURE)
		glDeleteVertexArrays(1, &VAO_renderTex);
		glDeleteBuffers(1, &VBO_renderTex);
		g_renderTexShaderProgram.Delete();
		g_renderToTex2D.Delete();
		assert(glGetError() == GL_NO_ERROR);
#endif
	}

	glfwTerminate();
	return 0;
}