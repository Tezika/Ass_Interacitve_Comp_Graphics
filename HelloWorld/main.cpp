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
#include <vector>
#include "lodepng.h"
#include "Texture.h"

#define RENDER_TO_TEXTURE
#define RENDER_SKYBOX
#define USE_REFlECTION_SHADER

GLuint VAO;
GLuint VBO;

GLuint VAO_plane;
GLuint VBO_plane;

Ass_Inter_Comp_Graphics::Texture* pDiffuseTex;
Ass_Inter_Comp_Graphics::Texture* pSpecularTex;

cyGLRenderTexture2D g_rtt_mirror;

cyGLSLProgram g_meshShaderProgram;
cyGLSLProgram g_skyboxShaderProgram;
cyGLSLProgram g_planeShaderProgram;

cyTriMesh g_objMesh;
cyTriMesh g_planeMesh;

bool bRotateLight = false;
bool bControlTheRtt = false;

float camera_angle_yaw = 0;
float camera_angle_pitch = 0;
float camera_distance = -100.0f;

float rtt_angle_yaw = 0;
float rtt_angle_pitch = 0;
float rtt_dis = 1.0f;

float rotation_yaw = 2.0f;
float rotation_pitch = 1.5f;

float light_angle_yaw = 0.0f;
float light_angle_pitch = 0.0f;

float screen_Width = 640;
float screen_Height = 480;

constexpr char const* path_vertexShader_mesh = "content/shaders/vertex_mesh.shader";
constexpr char const* path_fragmentShader_mesh = "content/shaders/fragment_mesh.shader";

constexpr char const* path_vertexShader_rtt = "content/shaders/vertex_rtt.shader";
constexpr char const* path_fragmentShader_rtt = "content/shaders/fragment_rtt.shader";

constexpr char const* path_vertexShader_skybox = "content/shaders/vertex_skybox.shader";
constexpr char const* path_fragmentShader_skybox = "content/shaders/fragment_skybox.shader";

constexpr char const* path_vertexShader_reflection = "content/shaders/vertex_reflection.shader";
constexpr char const* path_fragmentShader_reflection = "content/shaders/fragment_reflection.shader";

constexpr char const* path_meshResource = "content/mesh/";

bool left_mouseBtn_drag = false;
bool right_mouseBtn_drag = false;

const unsigned int count_indices = 6;

GLuint VAO_cubemap;
GLuint VBO_cubemap;
GLuint Tex_cubemap;

const float g_skyboxVertices[] = {
	// positions          
	-1.0f,  1.0f, -1.0f,
	-1.0f, -1.0f, -1.0f,
	 1.0f, -1.0f, -1.0f,
	 1.0f, -1.0f, -1.0f,
	 1.0f,  1.0f, -1.0f,
	-1.0f,  1.0f, -1.0f,

	-1.0f, -1.0f,  1.0f,
	-1.0f, -1.0f, -1.0f,
	-1.0f,  1.0f, -1.0f,
	-1.0f,  1.0f, -1.0f,
	-1.0f,  1.0f,  1.0f,
	-1.0f, -1.0f,  1.0f,

	 1.0f, -1.0f, -1.0f,
	 1.0f, -1.0f,  1.0f,
	 1.0f,  1.0f,  1.0f,
	 1.0f,  1.0f,  1.0f,
	 1.0f,  1.0f, -1.0f,
	 1.0f, -1.0f, -1.0f,

	-1.0f, -1.0f,  1.0f,
	-1.0f,  1.0f,  1.0f,
	 1.0f,  1.0f,  1.0f,
	 1.0f,  1.0f,  1.0f,
	 1.0f, -1.0f,  1.0f,
	-1.0f, -1.0f,  1.0f,

	-1.0f,  1.0f, -1.0f,
	 1.0f,  1.0f, -1.0f,
	 1.0f,  1.0f,  1.0f,
	 1.0f,  1.0f,  1.0f,
	-1.0f,  1.0f,  1.0f,
	-1.0f,  1.0f, -1.0f,

	-1.0f, -1.0f, -1.0f,
	-1.0f, -1.0f,  1.0f,
	 1.0f, -1.0f, -1.0f,
	 1.0f, -1.0f, -1.0f,
	-1.0f, -1.0f,  1.0f,
	 1.0f, -1.0f,  1.0f
};

namespace
{
	void LoadTextureData( const char* i_texturePath, std::vector<unsigned char>& io_data, unsigned int& io_width, unsigned int& io_height )
	{
		io_data.clear();
		auto errorCode = lodepng::decode( io_data, io_width, io_height, i_texturePath );
		if (errorCode)
		{
			fprintf( stderr, "decoder error %d : %s.\n", errorCode, lodepng_error_text( errorCode ) );
		}
		else
		{
			fprintf( stdout, "Loaded/Decoded the %s texture successfully.\n", i_texturePath );
		}
	}
}

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
		fprintf( stdout, "Compiled the vertex shader, %s, successfully.\n", i_path_vertexShader );
	}
	if (!fragmentShader.CompileFile( i_path_fragementShader, GL_FRAGMENT_SHADER ))
	{
		fprintf( stderr, "Failed to compile the fragment shader, %s.\n", i_path_fragementShader );
	}
	else
	{
		fprintf( stdout, "Compiled the fragment shader, %s, successfully.\n", i_path_fragementShader );
	}
	i_glslProgram.AttachShader( vertexShader );
	i_glslProgram.AttachShader( fragmentShader );
	i_glslProgram.Link();
	assert( glGetError() == GL_NO_ERROR );
}

unsigned int LoadCubemap( std::vector<std::string>& i_faces )
{
	unsigned int textureID;
	glGenTextures( 1, &textureID );
	glBindTexture( GL_TEXTURE_CUBE_MAP, textureID );

	unsigned int width;
	unsigned int height;
	std::vector<unsigned char> data;
	for (unsigned int i = 0; i < i_faces.size(); i++)
	{
		LoadTextureData( i_faces[i].c_str(), data, width, height );
		if (data.data())
		{
			glTexImage2D( GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
				0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, reinterpret_cast<void*>(data.data())
			);
		}
		else
		{
			std::cout << "Cubemap texture failed to load at path: " << i_faces[i] << std::endl;
		}
	}
	glTexParameteri( GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
	glTexParameteri( GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
	glTexParameteri( GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE );
	glTexParameteri( GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE );
	glTexParameteri( GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE );
	return textureID;
}

void InitializeSkyBox()
{
	std::vector<std::string> faces
	{
		"content/cubemap/cubemap_posx.png",
		"content/cubemap/cubemap_negx.png",
		"content/cubemap/cubemap_posy.png",
		"content/cubemap/cubemap_negy.png",
		"content/cubemap/cubemap_posz.png",
		"content/cubemap/cubemap_negz.png"
	};
	Tex_cubemap = LoadCubemap( faces );
	// VAO, VBO
	glGenVertexArrays( 1, &VAO_cubemap );
	glBindVertexArray( VAO_cubemap );

	CompileShaders( path_vertexShader_skybox, path_fragmentShader_skybox, g_skyboxShaderProgram );
	g_skyboxShaderProgram.Bind();
	glUniform1i( glGetUniformLocation( g_skyboxShaderProgram.GetID(), "tex_cubemap" ), 0 );

	glGenBuffers( 1, &VBO_cubemap );
	glBindBuffer( GL_ARRAY_BUFFER, VBO_cubemap );
	glBufferData( GL_ARRAY_BUFFER, sizeof( g_skyboxVertices ), &g_skyboxVertices[0], GL_STATIC_DRAW );
	glVertexAttribPointer( 0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof( GLfloat ), (const GLvoid*)(0) );
	glEnableVertexAttribArray( 0 );
	glBindBuffer( GL_ARRAY_BUFFER, 0 );
	glBindVertexArray( 0 );
	assert( glGetError() == GL_NO_ERROR );
}

void InitializeMaterial( cyTriMesh& i_mesh, cyGLSLProgram& i_shaderProgram )
{
	if (i_mesh.NM() > 0)
	{
		i_shaderProgram.Bind();
		glUniform3f( glGetUniformLocation( i_shaderProgram.GetID(), "diffuseColor" ), i_mesh.M( 0 ).Kd[0], i_mesh.M( 0 ).Kd[1], i_mesh.M( 0 ).Kd[2] );
		glUniform3f( glGetUniformLocation( i_shaderProgram.GetID(), "ambientColor" ), i_mesh.M( 0 ).Ka[0], i_mesh.M( 0 ).Ka[1], i_mesh.M( 0 ).Ka[2] );
		glUniform3f( glGetUniformLocation( i_shaderProgram.GetID(), "specularColor" ), i_mesh.M( 0 ).Ks[0], i_mesh.M( 0 ).Ks[1], i_mesh.M( 0 ).Ks[2] );
	}
	else
	{
		i_shaderProgram.Bind();
		glUniform3f( glGetUniformLocation( i_shaderProgram.GetID(), "diffuseColor" ), 0.5f, 0.5, 0.5f );
		glUniform3f( glGetUniformLocation( i_shaderProgram.GetID(), "ambientColor" ), 0.5f, 0.5f, 0.5f );
		glUniform3f( glGetUniformLocation( i_shaderProgram.GetID(), "specularColor" ), 0.80099994f, 0.80099994f, 0.80099994f );
	}
}

void InitializeRenderTexture(cyGLRenderTexture2D& i_rtt)
{
	if (!i_rtt.Initialize( true ))
	{
		fprintf( stderr, "Cannot generate the renderToTexture." );
		assert( false );
	}
	i_rtt.SetTextureFilteringMode( GL_LINEAR, GL_LINEAR );
	assert( i_rtt.Resize( 4, screen_Width, screen_Height ) );
}

void InitializeTextures( cyTriMesh& i_mesh, cyGLSLProgram& i_shaderProgram )
{
	if (i_mesh.NM() <= 0)
	{
		return;
	}
	if (i_mesh.M( 0 ).map_Kd != nullptr)
	{
		std::string path_diffusesTex( path_meshResource );
		path_diffusesTex += i_mesh.M( 0 ).map_Kd;
		pDiffuseTex = Ass_Inter_Comp_Graphics::Texture::Create( path_diffusesTex.c_str() );
		if (!pDiffuseTex)
		{
			fprintf( stderr, "Failed to create the diffuse texture.\n" );
			assert( false );
		}
	}
	if (i_mesh.M( 0 ).map_Ks != nullptr)
	{
		std::string path_specularTex( path_meshResource );
		path_specularTex += i_mesh.M( 0 ).map_Ks;
		pSpecularTex = Ass_Inter_Comp_Graphics::Texture::Create( path_specularTex.c_str() );
		if (!pSpecularTex)
		{
			fprintf( stderr, "Failed to create the specular texture.\n" );
			assert( false );
		}
	}
	i_shaderProgram.Bind();
	glUniform1i( glGetUniformLocation( i_shaderProgram.GetID(), "tex_diff" ), 0 );
	glUniform1i( glGetUniformLocation( i_shaderProgram.GetID(), "tex_spec" ), 1 );
}

void InitializeMesh( const char* i_objFileName, cyTriMesh& i_mesh, GLuint& i_VAO, GLuint& i_VBO )
{
	std::string path_objFile( path_meshResource );
	path_objFile += i_objFileName;
	if (!i_mesh.LoadFromFileObj( path_objFile.c_str(), true ))
	{
		fprintf( stderr, "Failed to load the %s.\n", i_objFileName );
		assert( false );
	}
	else
	{
		fprintf( stdout, "Loaded the %s successfully.\n", i_objFileName );
	}
	if (!i_mesh.IsBoundBoxReady())
	{
		i_mesh.ComputeBoundingBox();
	}
	if (i_mesh.NVT() > 0)
	{
		std::vector<cyVec3f> vertices;
		std::vector<cyVec3f> vertexNormals;
		for (int i = 0; i < i_mesh.NF(); i++)
		{
			auto triFace = i_mesh.F( i );
			for (int j = 0; j < 3; j++)
			{
				vertices.push_back( i_mesh.V( triFace.v[j] ) );
			}
		}
		for (int i = 0; i < i_mesh.NF(); i++)
		{
			auto curNormalFace = i_mesh.FN( i );
			for (int j = 0; j < 3; j++)
			{
				vertexNormals.push_back( i_mesh.VN( curNormalFace.v[j] ) );
			}
		}
		std::vector<cyVec2f> texCoords;
		for (int i = 0; i < i_mesh.NF(); i++)
		{
			auto curTexFace = i_mesh.FT( i );
			for (int j = 0; j < 3; j++)
			{
				texCoords.push_back( i_mesh.VT( curTexFace.v[j] ).XY() );
			}
		}
		glGenVertexArrays( 1, &i_VAO );
		glBindVertexArray( i_VAO );
		assert( glGetError() == GL_NO_ERROR );
		const auto sizeOfVertices = static_cast<GLsizei>(vertices.size() * sizeof( cyVec3f ));
		const auto sizeOfNormals = static_cast<GLsizei>(vertexNormals.size() * sizeof( cyVec3f ));
		const auto sizeOfTexCoord = static_cast<GLsizei>(texCoords.size() * sizeof( cyVec2f ));
		// For vertex data buffer
		glGenBuffers( 1, &i_VBO );
		glBindBuffer( GL_ARRAY_BUFFER, i_VBO );
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

		glBindBuffer( GL_ARRAY_BUFFER, 0 );
		glBindVertexArray( 0 );
	}
	else
	{
		std::vector<cyVec3f> vertices;
		std::vector<cyVec3f> vertexNormals;
		for (int i = 0; i < i_mesh.NF(); i++)
		{
			auto triFace = i_mesh.F( i );
			for (int j = 0; j < 3; j++)
			{
				vertices.push_back( i_mesh.V( triFace.v[j] ) );
			}
		}
		for (int i = 0; i < i_mesh.NF(); i++)
		{
			auto curNormalFace = i_mesh.FN( i );
			for (int j = 0; j < 3; j++)
			{
				vertexNormals.push_back( i_mesh.VN( curNormalFace.v[j] ) );
			}
		}
		glGenVertexArrays( 1, &i_VAO );
		glBindVertexArray( i_VAO );
		assert( glGetError() == GL_NO_ERROR );
		const auto sizeOfVertices = static_cast<GLsizei>(vertices.size() * sizeof( cyVec3f ));
		const auto sizeOfNormals = static_cast<GLsizei>(vertexNormals.size() * sizeof( cyVec3f ));
		// For vertex data buffer
		glGenBuffers( 1, &i_VBO );
		glBindBuffer( GL_ARRAY_BUFFER, i_VBO );
		glBufferData( GL_ARRAY_BUFFER, sizeOfVertices + sizeOfNormals, NULL, GL_STATIC_DRAW );
		glBufferSubData( GL_ARRAY_BUFFER, 0, sizeOfVertices, reinterpret_cast<void*>(vertices.data()) );
		glBufferSubData( GL_ARRAY_BUFFER, sizeOfVertices, sizeOfNormals, reinterpret_cast<void*>(vertexNormals.data()) );

		glVertexAttribPointer( 0, 3, GL_FLOAT, GL_FALSE, static_cast<GLsizei>(sizeof( cyVec3f )), (const GLvoid*)(0) );
		glVertexAttribPointer( 1, 3, GL_FLOAT, GL_FALSE, static_cast<GLsizei>(sizeof( cyVec3f )), (const GLvoid*)(sizeOfVertices) );

		glEnableVertexAttribArray( 0 );
		glEnableVertexAttribArray( 1 );

		glBindBuffer( GL_ARRAY_BUFFER, 0 );
		glBindBuffer( GL_ARRAY_BUFFER, 0 );
		glBindVertexArray( 0 );
		assert( glGetError() == GL_NO_ERROR );
	}
}

void Display()
{
	// Render the mirror mesh to a texture(rtt)
	{
		g_rtt_mirror.Bind();
		glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
		glClearColor( 1.0f, 1.0f, 1.0f, 1.0f );
		g_meshShaderProgram.Bind();
		glUniform1i( glGetUniformLocation( g_meshShaderProgram.GetID(), "environment" ), 1 );
		glUniform1i( glGetUniformLocation( g_meshShaderProgram.GetID(), "mirroring" ), 0 );
		auto matFlip = cyMatrix4f::Scale( cyVec3f( 1.0f, 1.0f, -1.0f ) );
		auto matTranslation = cyMatrix4f( 1.0f );
		matTranslation.SetRotationXYZ(glm::radians( -camera_angle_pitch ), glm::radians( -camera_angle_yaw ), 0);
		auto mat_reflect_model = matTranslation.GetInverse() * matFlip * matTranslation;
		glUniformMatrix4fv( glGetUniformLocation( g_meshShaderProgram.GetID(), "mat_model" ), 1, GL_FALSE, mat_reflect_model.cell );
		glBindVertexArray( VAO );
		glDrawArrays( GL_TRIANGLES, 0, 3 * g_objMesh.NF() );
		assert(glGetError() == GL_NO_ERROR);
		glBindVertexArray( 0 );
		g_rtt_mirror.Unbind();
		assert( glGetError() == GL_NO_ERROR );
	}
	// Draw the skybox
	{
		glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
		//glDepthFunc(GL_LEQUAL);
		glDepthMask( GL_FALSE );
		g_skyboxShaderProgram.Bind();
		// ... set view and projection matrix
		glBindVertexArray( VAO_cubemap );
		glActiveTexture( GL_TEXTURE0 );
		glBindTexture( GL_TEXTURE_CUBE_MAP, Tex_cubemap );
		glDrawArrays( GL_TRIANGLES, 0, 36 );
		glDepthMask( GL_TRUE );
	}
	 // Render the scene
	{
		// Draw the scene
#if not defined(USE_REFlECTION_SHADER)
		pDiffuseTex->Bind( GL_TEXTURE0, GL_TEXTURE_2D );
		pSpecularTex->Bind( GL_TEXTURE1, GL_TEXTURE_2D );
#endif
		g_meshShaderProgram.Bind();
		glUniform1i( glGetUniformLocation( g_meshShaderProgram.GetID(), "environment" ), 1 );
		glUniform1i( glGetUniformLocation( g_meshShaderProgram.GetID(), "mirroring" ), 0 );
		auto matTranslation = cyMatrix4f( 1.0f );
		matTranslation.SetRotationXYZ( glm::radians( -camera_angle_pitch ), glm::radians( -camera_angle_yaw ), 0 );
		glUniformMatrix4fv( glGetUniformLocation( g_meshShaderProgram.GetID(), "mat_model" ), 1, GL_FALSE, matTranslation.cell );
		glBindVertexArray( VAO );
		glDrawArrays( GL_TRIANGLES, 0, 3 * g_objMesh.NF() );
		assert( glGetError() == GL_NO_ERROR );
		glBindVertexArray( 0 );
	}
	// Draw the plane for reflection
	{
		g_planeShaderProgram.Bind();
		glUniform1i( glGetUniformLocation( g_planeShaderProgram.GetID(), "environment" ), 1 );
		glUniform1i( glGetUniformLocation( g_planeShaderProgram.GetID(), "mirroring" ), 1 );
		glUniform1i( glGetUniformLocation( g_planeShaderProgram.GetID(), "tex_mirror" ), 1 );
		glActiveTexture( GL_TEXTURE1 );
		glBindTexture( GL_TEXTURE_2D, g_rtt_mirror.GetTextureID() );
		glBindVertexArray( VAO_plane );
		glDrawArrays( GL_TRIANGLES, 0, 3 * g_planeMesh.NF() );
		assert( glGetError() == GL_NO_ERROR );
		glBindVertexArray( 0 );
	}
}

void UpdateView()
{
	auto mat_model = cyMatrix4f( 1.0f );
	auto mat_light = cyMatrix4f( 1.0f );
	auto mat_view = cyMatrix4f( 1.0f );
	auto mat_perspective = cyMatrix4f( 1.0f );
	auto mat_modelToProjection = cyMatrix4f( 1.0f );
	auto mat_modelToView = cyMatrix4f( 1.0f );
	auto mat_rttTranslation = cyMatrix4f( 1.0f );
	auto mat_rttRotation = cyMatrix4f( 1.0f );

	//auto mat_flip = cyMatrix4f( 1.0f ).Scale( cyVec3f( 1.0f, -1.0f, 1.0f ) );
	//mat_model.SetRotationXYZ( glm::radians( -camera_angle_pitch ), glm::radians( -camera_angle_yaw ), 0 );
	//mat_model = mat_model * mat_flip;
	mat_model.SetRotationXYZ( glm::radians( -camera_angle_pitch ), glm::radians( -camera_angle_yaw ), 0 );
	mat_light.SetRotationXYZ( glm::radians( light_angle_pitch ), glm::radians( light_angle_yaw ), 0 );

	auto cameraTarget = (g_objMesh.GetBoundMax() + g_objMesh.GetBoundMin()) / 2;
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

	mat_perspective.SetPerspective( glm::radians( 60.0f ), screen_Width / screen_Height, 0.1f, 1000.0f );
	mat_modelToView = mat_view * mat_model;
	auto mat_normalMatToView = mat_modelToView.GetInverse().GetTranspose();
	mat_modelToProjection = mat_perspective * mat_view * mat_model;

	g_meshShaderProgram.Bind();
	glUniformMatrix4fv( glGetUniformLocation( g_meshShaderProgram.GetID(), "mat_model" ), 1, GL_FALSE, mat_model.cell );
	glUniformMatrix4fv( glGetUniformLocation( g_meshShaderProgram.GetID(), "mat_view" ), 1, GL_FALSE, mat_view.cell );
	glUniformMatrix4fv( glGetUniformLocation( g_meshShaderProgram.GetID(), "mat_projection" ), 1, GL_FALSE, mat_perspective.cell );
	glUniformMatrix4fv( glGetUniformLocation( g_meshShaderProgram.GetID(), "mat_normalToView" ), 1, GL_FALSE, mat_normalMatToView.cell );
	glUniformMatrix4fv( glGetUniformLocation( g_meshShaderProgram.GetID(), "mat_lightTransformation" ), 1, GL_FALSE, mat_light.cell );
	assert( glGetError() == GL_NO_ERROR );

	g_planeShaderProgram.Bind();
	glUniformMatrix4fv( glGetUniformLocation( g_planeShaderProgram.GetID(), "mat_model" ), 1, GL_FALSE, mat_model.cell );
	glUniformMatrix4fv( glGetUniformLocation( g_planeShaderProgram.GetID(), "mat_view" ), 1, GL_FALSE, mat_view.cell );
	glUniformMatrix4fv( glGetUniformLocation( g_planeShaderProgram.GetID(), "mat_projection" ), 1, GL_FALSE, mat_perspective.cell );
	glUniformMatrix4fv( glGetUniformLocation( g_planeShaderProgram.GetID(), "mat_normalToView" ), 1, GL_FALSE, mat_normalMatToView.cell );
	glUniformMatrix4fv( glGetUniformLocation( g_planeShaderProgram.GetID(), "mat_lightTransformation" ), 1, GL_FALSE, mat_light.cell );
	assert( glGetError() == GL_NO_ERROR );

#if defined(RENDER_SKYBOX)
	g_skyboxShaderProgram.Bind();
	// Removed the translation from the view matrix
	auto mat4_view = cyMatrix4f( cyMatrix3f( mat_view ) );
	glUniformMatrix4fv( glGetUniformLocation( g_skyboxShaderProgram.GetID(), "mat_view" ), 1, GL_FALSE, mat4_view.cell );
	glUniformMatrix4fv( glGetUniformLocation( g_skyboxShaderProgram.GetID(), "mat_proj" ), 1, GL_FALSE, mat_perspective.cell );
	assert( glGetError() == GL_NO_ERROR );
#endif
}

void KeyboardCallback( GLFWwindow* i_pWindow, int i_key, int i_scancode, int i_action, int i_mods )
{
	assert( i_pWindow );
	if (i_key == GLFW_KEY_F6 && i_action == GLFW_PRESS)
	{
		CompileShaders( path_vertexShader_mesh, path_fragmentShader_mesh, g_meshShaderProgram );
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
	if (i_key == GLFW_KEY_LEFT_ALT)
	{
		if (i_action == GLFW_PRESS)
		{
			bControlTheRtt = true;
		}
		else if (i_action == GLFW_RELEASE)
		{
			bControlTheRtt = false;
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
			if (bControlTheRtt)
			{
				rtt_dis -= 0.05f;
			}
			else
			{
				camera_distance -= 0.3f;
			}

		}
		else if (dis.Dot( cyVec2d( 0, 1 ) ) < 0)
		{
			if (bControlTheRtt)
			{
				rtt_dis += 0.05f;
			}
			else
			{
				camera_distance += 0.3f;
			}
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
			if (bControlTheRtt)
			{
				rtt_angle_yaw -= rotation_yaw;
			}
			else
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
		}
		else if (dis.Dot( cyVec2d( 1, 0 ) ) < 0)
		{
			if (bControlTheRtt)
			{
				rtt_angle_yaw += rotation_yaw;
			}
			else
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
		}

		// Rotate around the pitch
		if (dis.Dot( cyVec2d( 0, 1 ) ) > 0)
		{
			if (bControlTheRtt)
			{
				rtt_angle_pitch -= rotation_pitch;
			}
			else
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
		}
		else if (dis.Dot( cyVec2d( 0, 1 ) ) < 0)
		{
			if (bControlTheRtt)
			{
				rtt_angle_pitch += rotation_pitch;
			}
			else
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
	pWindow = glfwCreateWindow( screen_Width, screen_Height, "Hello World", NULL, NULL );
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
#if defined(USE_REFlECTION_SHADER)
	CompileShaders( path_vertexShader_reflection, path_fragmentShader_reflection, g_meshShaderProgram );
	CompileShaders( path_vertexShader_reflection, path_fragmentShader_reflection, g_planeShaderProgram );
#else
	CompileShaders( path_vertexShader_mesh, path_fragmentShader_mesh, g_meshShaderProgram );
	CompileShaders( path_vertexShader_mesh, path_fragmentShader_mesh, g_planeShaderProgram );
#endif

	InitializeMesh( argv[1], g_objMesh, VAO, VBO );
	InitializeMaterial( g_objMesh, g_meshShaderProgram );
	//InitializeTextures( g_objMesh, g_meshShaderProgram );

	InitializeMesh( "plane.obj", g_planeMesh, VAO_plane, VBO_plane );
	InitializeMaterial( g_planeMesh, g_planeShaderProgram );

#if defined(RENDER_TO_TEXTURE)
	InitializeRenderTexture(g_rtt_mirror);
#endif

#if defined(RENDER_SKYBOX)
	InitializeSkyBox();
#endif

	while (!glfwWindowShouldClose( pWindow ))
	{
		UpdateMouseInput( pWindow );
		UpdateView();
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
		glDeleteBuffers( 1, &VAO_plane );
		glDeleteBuffers( 1, &VBO_plane );
		assert( glGetError() == GL_NO_ERROR );
		g_meshShaderProgram.Delete();
		g_planeShaderProgram.Delete();
		assert( glGetError() == GL_NO_ERROR );
		delete pDiffuseTex;
		pDiffuseTex = nullptr;
		delete pSpecularTex;
		pSpecularTex = nullptr;
		assert( glGetError() == GL_NO_ERROR );

#if defined(RENDER_TO_TEXTURE)
		g_rtt_mirror.Delete();
		assert( glGetError() == GL_NO_ERROR );
#endif

#if defined(RENDER_SKYBOX)
		glDeleteVertexArrays( 1, &VAO_cubemap );
		glDeleteBuffers( 1, &VBO_cubemap );
		g_skyboxShaderProgram.Delete();
		assert( glGetError() == GL_NO_ERROR );
#endif
	}
	glfwTerminate();
	return 0;
}