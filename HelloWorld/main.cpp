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

//#define RENDER_TO_TEXTURE
//#define RENDER_SKYBOX
//#define USE_REFlECTION_SHADER
#define RENDER_SHADOW

GLuint VAO;
GLuint VBO;

GLuint VAO_plane;
GLuint VBO_plane;

GLuint VAO_quad;
GLuint VBO_quad;
GLuint EBO_quad;

Ass_Inter_Comp_Graphics::Texture* pDiffuseTex;
Ass_Inter_Comp_Graphics::Texture* pSpecularTex;

cyGLRenderTexture2D g_rtt_mirror;
cyGLRenderDepth2D g_tex_renderDepth;

cyGLSLProgram g_skyboxShaderProgram;
cyGLSLProgram g_quadShaderProgram;
cyGLSLProgram g_shadowPassProgram;
cyGLSLProgram g_shadowMeshProgram;

cyTriMesh g_objMesh;
cyTriMesh g_planeMesh;

cyMatrix4f g_mat_model = cyMatrix4f( 1.0f );
cyMatrix4f g_mat_plane = cyMatrix4f( 1.0f );
cyMatrix4f g_mat_normalModel = cyMatrix4f( 1.0f );
cyMatrix4f g_mat_normalPlane = cyMatrix4f( 1.0f );

bool bRotateLight = false;
bool bControlTheRtt = false;

float camera_angle_yaw = 0;
float camera_angle_pitch = 0;
float camera_distance = 60.0f;

cyVec3f g_ligtPosInWorld = cyVec3f( 0.0f, 100.0f, 60.0f );

float rtt_angle_yaw = 0;
float rtt_angle_pitch = 0;
float rtt_dis = 1.0f;

float rotation_yaw = 2.0f;
float rotation_pitch = 1.5f;

float light_angle_yaw = 0.0f;
float light_angle_pitch = -90.0f;

float screen_Width = 640;
float screen_Height = 480;

float shadow_width = 1024;
float shadow_height = 1024;

constexpr char const* path_vertexShader_mesh = "content/shaders/vertex_mesh.shader";
constexpr char const* path_fragmentShader_mesh = "content/shaders/fragment_mesh.shader";

constexpr char const* path_vertexShader_shadow = "content/shaders/vertex_shadow.shader";
constexpr char const* path_fragmentShader_shadow = "content/shaders/fragment_shadow.shader";

constexpr char const* path_vertexShader_shadowPass = "content/shaders/vertex_shadowPass.shader";
constexpr char const* path_fragmentShader_shadowPass = "content/shaders/fragment_shadowPass.shader";

constexpr char const* path_vertexShader_quad = "content/shaders/vertex_quad_texture.shader";
constexpr char const* path_fragmentShader_quad = "content/shaders/fragment_quad_texture.shader";

constexpr char const* path_vertexShader_skybox = "content/shaders/vertex_skybox.shader";
constexpr char const* path_fragmentShader_skybox = "content/shaders/fragment_skybox.shader";

constexpr char const* path_vertexShader_reflection = "content/shaders/vertex_reflection.shader";
constexpr char const* path_fragmentShader_reflection = "content/shaders/fragment_reflection.shader";

constexpr char const* path_meshResource = "content/mesh/";

bool left_mouseBtn_drag = false;
bool right_mouseBtn_drag = false;

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

const GLfloat g_quad_buffer_data[] =
{
	// ndc pos         // UV
	-0.5f, -0.5f, 0.0f, 0.0f, 0.0f,
	0.5f,  -0.5f, 0.0f,  1.0f, 0.0f,
	0.5f,  0.5f,  0.0f,  1.0f, 1.0f,
	-0.5f, 0.5f,  0.0f,  0.0f, 1.0f
};

const unsigned int g_quad_indices_data[] =
{
	0,1,2,
	2,3,0
};

const unsigned int count_quad_indices = 6;

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

	cyMatrix4f GetLookAtMatrix( const cyVec3f& i_pos, const cyVec3f& i_target, const cyVec3f& i_worldUp )
	{
		auto dir = (i_pos - i_target).GetNormalized();
		auto right = i_worldUp.Cross( dir ).GetNormalized();
		auto up = dir.Cross( right ).GetNormalized();

		auto mat_lookat = cyMatrix4f( 1.0f );
		mat_lookat.SetRow( 0, right.x, right.y, right.z, -right.Dot( i_pos ) );
		mat_lookat.SetRow( 1, up.x, up.y, up.z, -up.Dot( i_pos ) );
		mat_lookat.SetRow( 2, dir.x, dir.y, dir.z, -dir.Dot( i_pos ) );
		mat_lookat.SetRow( 3, 0, 0, 0, 1 );

		return mat_lookat;
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

void InitializeRenderTexture( cyGLRenderTexture2D& i_rtt )
{
	if (!i_rtt.Initialize( true ))
	{
		fprintf( stderr, "Cannot generate the renderToTexture." );
		assert( false );
	}
	i_rtt.SetTextureFilteringMode( GL_LINEAR, GL_LINEAR );
	i_rtt.BuildTextureMipmaps();
	i_rtt.SetTextureMaxAnisotropy();
	if (!i_rtt.Resize( 4, screen_Width, screen_Height ))
	{
		fprintf( stderr, "Cannot resize the renderToTexture." );
		assert( false );
	}
}

void InitializeDepthMap( cyGLRenderDepth2D& i_renderDepth2D )
{
	// Do not initialize it as a render depth texture to test it.
	if (!i_renderDepth2D.Initialize( false ))
	{
		fprintf( stderr, "Failed to generate the render depth texture." );
		assert( false );
	}
	i_renderDepth2D.SetTextureFilteringMode( GL_NEAREST, GL_NEAREST );
	if (!i_renderDepth2D.Resize( shadow_width, shadow_height ))
	{
		fprintf( stderr, "Cannot resize the render depth." );
		assert( false );
	}
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
	i_mesh.ComputeNormals();
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

void InitializeDebugQuad( GLuint& i_VAO, GLuint& i_VBO, GLuint& i_EBO )
{
	// Generate the render to texture's vbo and vao
	glGenVertexArrays( 1, &i_VAO );
	glBindVertexArray( i_VAO );

	glGenBuffers( 1, &i_VBO );
	glBindBuffer( GL_ARRAY_BUFFER, i_VBO );
	glBufferData( GL_ARRAY_BUFFER, sizeof( g_quad_buffer_data ), g_quad_buffer_data, GL_STATIC_DRAW );
	glVertexAttribPointer( 0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof( GLfloat ), (const GLvoid*)(0) );
	glEnableVertexAttribArray( 0 );
	glVertexAttribPointer( 1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof( GLfloat ), (const GLvoid*)(3 * sizeof( GLfloat )) );
	glEnableVertexAttribArray( 1 );

	glGenBuffers( 1, &i_EBO );
	glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, i_EBO );
	glBufferData( GL_ELEMENT_ARRAY_BUFFER, static_cast<GLsizeiptr>(sizeof( unsigned int ) * count_quad_indices), &g_quad_indices_data[0], GL_STATIC_DRAW );
	assert( glGetError() == GL_NO_ERROR );

	glBindVertexArray( 0 );
	glBindBuffer( GL_ARRAY_BUFFER, 0 );
}

void RenderScene( bool i_bDrawShdow = false )
{
	glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
	g_shadowMeshProgram.Bind();
	if (i_bDrawShdow)
	{
		glUniform1i( glGetUniformLocation( g_shadowMeshProgram.GetID(), "tex_shadowMap" ), 0 );
	}
	glUniform1i( glGetUniformLocation( g_shadowMeshProgram.GetID(), "shadowing" ), 0 );
	// Draw the plane 
	{
		glUniformMatrix4fv( glGetUniformLocation( g_shadowMeshProgram.GetID(), "mat_model" ), 1, GL_FALSE, g_mat_plane.cell );
		glUniformMatrix4fv( glGetUniformLocation( g_shadowMeshProgram.GetID(), "mat_normalToView" ), 1, GL_FALSE, g_mat_normalPlane.cell );
		if (i_bDrawShdow)
		{
			glUniform1i( glGetUniformLocation( g_shadowMeshProgram.GetID(), "shadowing" ), 1 );
		}
		glBindVertexArray( VAO_plane );
		glDrawArrays( GL_TRIANGLES, 0, 3 * g_planeMesh.NF() );
		assert( glGetError() == GL_NO_ERROR );
		glBindVertexArray( 0 );
	}
	// Draw the teapot
	{
		glUniformMatrix4fv( glGetUniformLocation( g_shadowMeshProgram.GetID(), "mat_model" ), 1, GL_FALSE, g_mat_model.cell );
		glUniformMatrix4fv( glGetUniformLocation( g_shadowMeshProgram.GetID(), "mat_normalToView" ), 1, GL_FALSE, g_mat_normalModel.cell );
		glBindVertexArray( VAO );
		glDrawArrays( GL_TRIANGLES, 0, 3 * g_objMesh.NF() );
		assert( glGetError() == GL_NO_ERROR );
		glBindVertexArray( 0 );
	}

}

void GenerateShadowMap()
{
	glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
	g_shadowPassProgram.Bind();
	// Draw the teapot
	{
		glUniformMatrix4fv( glGetUniformLocation( g_shadowPassProgram.GetID(), "mat_model" ), 1, GL_FALSE, g_mat_model.cell );
		glBindVertexArray( VAO );
		glDrawArrays( GL_TRIANGLES, 0, 3 * g_objMesh.NF() );
		assert( glGetError() == GL_NO_ERROR );
		glBindVertexArray( 0 );
	}
	// Draw the plane 
	{
		glUniformMatrix4fv( glGetUniformLocation( g_shadowPassProgram.GetID(), "mat_model" ), 1, GL_FALSE, g_mat_plane.cell );
		glBindVertexArray( VAO_plane );
		glDrawArrays( GL_TRIANGLES, 0, 3 * g_planeMesh.NF() );
		assert( glGetError() == GL_NO_ERROR );
		glBindVertexArray( 0 );
	}
}

void Display()
{
	// Render the mirror mesh to a texture(rtt)
	//{
	//	g_rtt_mirror.Bind();
	//	glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
	//	glClearColor( 1.0f, 1.0f, 1.0f, 1.0f );
	//	g_meshShaderProgram.Bind();
	//	glUniform1i( glGetUniformLocation( g_meshShaderProgram.GetID(), "environment" ), 1 );
	//	glUniform1i( glGetUniformLocation( g_meshShaderProgram.GetID(), "mirroring" ), 0 );
	//	auto matFlip = cyMatrix4f::Scale( cyVec3f( 1.0f, -1.0f, 1.0f ) );
	//	auto matTranslation = cyMatrix4f( 1.0f );
	//	matTranslation.SetRotationXYZ( glm::radians( -camera_angle_pitch ), glm::radians( -camera_angle_yaw ), 0 );
	//	auto mat_reflect_model = matTranslation * matFlip;
	//	glUniformMatrix4fv( glGetUniformLocation( g_meshShaderProgram.GetID(), "mat_model" ), 1, GL_FALSE, mat_reflect_model.cell );
	//	glBindVertexArray( VAO );
	//	glDrawArrays( GL_TRIANGLES, 0, 3 * g_objMesh.NF() );
	//	assert( glGetError() == GL_NO_ERROR );
	//	glBindVertexArray( 0 );
	//	g_rtt_mirror.Unbind();
	//	assert( glGetError() == GL_NO_ERROR );
	//}
	// Draw the skybox
	//{
	//	glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
	//	//glDepthFunc(GL_LEQUAL);
	//	glDepthMask( GL_FALSE );
	//	g_skyboxShaderProgram.Bind();
	//	// ... set view and projection matrix
	//	glBindVertexArray( VAO_cubemap );
	//	glActiveTexture( GL_TEXTURE0 );
	//	glBindTexture( GL_TEXTURE_CUBE_MAP, Tex_cubemap );
	//	glDrawArrays( GL_TRIANGLES, 0, 36 );
	//	glDepthMask( GL_TRUE );
	//}

	// Render the scene to the shadow map from the light perspective
	{
		g_tex_renderDepth.Bind();
		GenerateShadowMap();
		g_tex_renderDepth.Unbind();
	}

	//{
	//	glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
	//	g_quadShaderProgram.Bind();
	//	glUniform1i( glGetUniformLocation( g_quadShaderProgram.GetID(), "tex_render" ), 0 );
	//	glActiveTexture( GL_TEXTURE0 );
	//	glBindTexture( GL_TEXTURE_2D, g_tex_renderDepth.GetTextureID() );

	//	glBindVertexArray( VAO_quad );
	//	glDrawElements( GL_TRIANGLES, static_cast<GLsizei>(count_quad_indices), GL_UNSIGNED_INT, 0 );
	//	glBindVertexArray( 0 );
	//}

	{
		RenderScene( true );
	}
	assert( glGetError() == GL_NO_ERROR );
}

void UpdateCamera()
{

}

void UpdateLight()
{
	auto mat_light_trans = cyMatrix4f( 1.0f );
	mat_light_trans.SetTranslation( g_ligtPosInWorld );
	auto mat_light_rot = cyMatrix4f( 1.0f );
	mat_light_rot.SetRotationXYZ( glm::radians( light_angle_pitch ), glm::radians( light_angle_yaw ), 0 );
	mat_light_trans = mat_light_rot * mat_light_trans;

	auto lightTarget = (g_objMesh.GetBoundMax() + g_objMesh.GetBoundMin()) / 2;

	auto worldUp = cyVec3f( 0, 1, 0 );
	auto mat_lightSpace_view = GetLookAtMatrix( g_ligtPosInWorld, lightTarget, worldUp );

	auto mat_perspective = cyMatrix4f( 1.0f );
	float near_plane = 1.0f;
	float far_plane = 200.0f;
	mat_perspective.SetPerspective( glm::radians( 60.0f ), shadow_width / shadow_height, near_plane, far_plane );

	auto mat_lightSpace = mat_perspective * mat_lightSpace_view;

	g_quadShaderProgram.Bind();
	glUniform1f( glGetUniformLocation( g_quadShaderProgram.GetID(), "near_plane" ), near_plane );
	glUniform1f( glGetUniformLocation( g_quadShaderProgram.GetID(), "far_plane" ), far_plane );

	g_shadowMeshProgram.Bind();
	glUniformMatrix4fv( glGetUniformLocation( g_shadowMeshProgram.GetID(), "mat_lightSpaceTransformation" ), 1, GL_FALSE, mat_lightSpace.cell );
	glUniform3fv( glGetUniformLocation( g_shadowMeshProgram.GetID(), "lightPos" ), 1, &g_ligtPosInWorld.elem[0] );

	g_shadowPassProgram.Bind();
	glUniformMatrix4fv( glGetUniformLocation( g_shadowPassProgram.GetID(), "mat_lightSpaceTransformation" ), 1, GL_FALSE, mat_lightSpace.cell );
}

void UpdateModels()
{
	auto mat_view = cyMatrix4f( 1.0f );
	auto mat_perspective = cyMatrix4f( 1.0f );
	auto mat_modelToView = cyMatrix4f( 1.0f );

	g_mat_plane.SetTranslation( cyVec3f( 0, 0, 0 ) );
	auto mat_model_rot = cyMatrix4f( 1.0f );
	mat_model_rot.SetRotationXYZ( glm::radians( -camera_angle_pitch ), glm::radians( -camera_angle_yaw ), 0 );

	auto mat_model_trans = cyMatrix4f( 1.0f );
	mat_model_trans.SetTranslation( cyVec3f( 0, 6.0f, 0 ) );
	g_mat_model = mat_model_rot * mat_model_trans;

	auto cameraTarget = (g_objMesh.GetBoundMax() + g_objMesh.GetBoundMin()) / 2;
	auto cameraPosition = cyVec3f( 0, camera_distance, camera_distance );
	auto worldUp = cyVec3f( 0, 1, 0 );

	auto mat_cameraView = GetLookAtMatrix( cameraPosition, cameraTarget, worldUp );

	auto mat_cameraTranslation = cyMatrix4f( 1.0f );
	mat_cameraTranslation.SetTranslation( cameraPosition );

	mat_perspective.SetPerspective( glm::radians( 60.0f ), screen_Width / screen_Height, 5.0f, 1000.0f );
	g_mat_normalPlane = (mat_view * g_mat_plane).GetInverse().GetTranspose();
	g_mat_normalModel = (mat_view * g_mat_model).GetInverse().GetTranspose();

	g_shadowMeshProgram.Bind();
	glUniformMatrix4fv( glGetUniformLocation( g_shadowMeshProgram.GetID(), "mat_view" ), 1, GL_FALSE, mat_cameraView.cell );
	glUniformMatrix4fv( glGetUniformLocation( g_shadowMeshProgram.GetID(), "mat_projection" ), 1, GL_FALSE, mat_perspective.cell );
	glUniform3fv( glGetUniformLocation( g_shadowMeshProgram.GetID(), "viewPos" ), 1, &cameraPosition.elem[0] );

	assert( glGetError() == GL_NO_ERROR );

#if defined(RENDER_SHADOW)
	g_quadShaderProgram.Bind();
	auto mat_quad_rot = cyMatrix4f( 1.0f );
	mat_quad_rot.SetRotationXYZ( glm::radians( -rtt_angle_pitch ), glm::radians( -rtt_angle_yaw ), 0 );
	glUniformMatrix4fv( glGetUniformLocation( g_quadShaderProgram.GetID(), "mat_rttRot" ), 1, GL_FALSE, mat_quad_rot.cell );
	glUniform1f( glGetUniformLocation( g_quadShaderProgram.GetID(), "dis" ), 1.0f );
#endif
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
		//CompileShaders( path_vertexShader_mesh, path_fragmentShader_mesh, g_meshShaderProgram );
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
		system( "pause" );
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
#elif defined(RENDER_SHADOW)
	CompileShaders( path_vertexShader_shadow, path_fragmentShader_shadow, g_shadowMeshProgram );
	CompileShaders( path_vertexShader_shadowPass, path_fragmentShader_shadowPass, g_shadowPassProgram );
#else
	CompileShaders( path_vertexShader_mesh, path_fragmentShader_mesh, g_meshShaderProgram );
	CompileShaders( path_vertexShader_mesh, path_fragmentShader_mesh, g_planeShaderProgram );
#endif
	InitializeMesh( argv[1], g_objMesh, VAO, VBO );
	InitializeMesh( "plane.obj", g_planeMesh, VAO_plane, VBO_plane );
	InitializeMaterial( g_objMesh, g_shadowMeshProgram );

#if defined(RENDER_SHADOW)
	CompileShaders( path_vertexShader_quad, path_fragmentShader_quad, g_quadShaderProgram );
	InitializeDebugQuad( VAO_quad, VBO_quad, EBO_quad );
	InitializeDepthMap( g_tex_renderDepth );
#endif

#if defined(RENDER_TO_TEXTURE)
	InitializeRenderTexture( g_rtt_mirror );
#endif

#if defined(RENDER_SKYBOX)
	InitializeSkyBox();
#endif

	while (!glfwWindowShouldClose( pWindow ))
	{
		UpdateMouseInput( pWindow );
		UpdateLight();
		UpdateModels();
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
		glDeleteVertexArrays( 1, &VAO_plane );
		glDeleteBuffers( 1, &VBO_plane );
		assert( glGetError() == GL_NO_ERROR );
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

#if defined(RENDER_SHADOW)
		g_tex_renderDepth.Delete();
		g_shadowPassProgram.Delete();
		g_shadowMeshProgram.Delete();
		glDeleteVertexArrays( 1, &VAO_quad );
		glDeleteBuffers( 1, &VBO_quad );
		glDeleteBuffers( 1, &EBO_quad );
#endif
		if (glGetError() != GL_NO_ERROR)
		{
			assert( false );
		}
	}
	glfwTerminate();
	return 0;
}