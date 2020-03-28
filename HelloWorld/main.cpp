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

GLuint VAO;
GLuint VBO;

GLuint VAO_plane;
GLuint VBO_plane;

GLuint VAO_light;
GLuint VBO_light;

GLuint VAO_quad;
GLuint VBO_quad;
GLuint EBO_quad;

Ass_Inter_Comp_Graphics::Texture* pDiffuseTex;
Ass_Inter_Comp_Graphics::Texture* pSpecularTex;

cyGLRenderTexture2D g_rtt_mirror;
cyGLRenderDepth2D g_tex_renderDepth;

cyGLTexture2D g_tex_normalMap;
cyGLTexture2D g_tex_dispMap;

cyGLSLProgram g_sp_skybox;
cyGLSLProgram g_sp_quad;
cyGLSLProgram g_sp_shadowPass_tess;
cyGLSLProgram g_sp_shadowPass_noTess;
cyGLSLProgram g_sp_shadowMesh;
cyGLSLProgram g_sp_lightMesh;
cyGLSLProgram g_sp_tessellation;
cyGLSLProgram g_sp_outline;

cyTriMesh g_objMesh;
cyTriMesh g_planeMesh;
cyTriMesh g_lightMesh;
cyVec3f g_dir_targetToCamera;
cyVec3f g_dir_targetTolight;
cyVec3f g_target_camera;
cyVec3f g_target_light;

cyMatrix4f g_mat_model = cyMatrix4f( 1.0f );
cyMatrix4f g_mat_plane = cyMatrix4f( 1.0f );
cyMatrix4f g_mat_light = cyMatrix4f( 1.0f );
cyMatrix4f g_mat_quad = cyMatrix4f( 1.0f );

bool bControlTheLight = false;
bool bShowTessGrids = true;
int g_displacement = 0;

float camera_angle_yaw = 0;
float camera_angle_pitch = 0;
float g_level_tess = 1;
float g_tess_speed = 100;

float camera_moveSpeed_perframe = 0.5f;

cyVec3f g_lightPosInWorld = cyVec3f( 0.0f, 30.0f, 40.0f );
cyVec3f g_cameraPosInWorld = cyVec3f( 0.0, 0.0, 100.0f );

float g_moveSpeed = 20;
float g_rotate_yaw = 60;
float g_rotate_pitch = 45;

float rotation_yaw = 2.0f;
float rotation_pitch = 1.5f;

float light_angle_yaw = 0.0f;
float light_angle_pitch = 0.0f;

float g_dis_lightToTarget = 60.0f;
float g_dis_cameraToTarget = 60.0f;

float width_screen = 640;
float height_screen = 480;

float width_shadow = 1024;
float height_shadow = 1024;

constexpr char const* path_vertexShader_tessellation = "content/shaders/vertex_tessellation.shader";
constexpr char const* path_fragmentShader_tessellation = "content/shaders/fragment_tessellation.shader";

constexpr char const* path_vertexShader_mesh = "content/shaders/vertex_mesh.shader";
constexpr char const* path_fragmentShader_mesh = "content/shaders/fragment_mesh.shader";

constexpr char const* path_vertexShader_shadow = "content/shaders/vertex_shadow.shader";
constexpr char const* path_fragmentShader_shadow = "content/shaders/fragment_shadow.shader";

constexpr char const* path_vertexShader_shadowPass = "content/shaders/vertex_shadowPass.shader";
constexpr char const* path_fragmentShader_shadowPass = "content/shaders/fragment_shadowPass.shader";

constexpr char const* path_vertexShader_quad = "content/shaders/vertex_quad_texture.shader";
constexpr char const* path_fragmentShader_quad = "content/shaders/fragment_quad_texture.shader";

constexpr char const* path_fragmentShader_outline = "content/shaders/fragment_outline.shader";
constexpr char const* path_geometryShader_outline = "content/shaders/geometry_outline.shader";

constexpr char const* path_tess_control = "content/shaders/tess_control.shader";
constexpr char const* path_tess_evaulation = "content/shaders/tess_evaluation.shader";

constexpr char const* path_meshResource = "content/mesh/";
constexpr char const* path_texResource = "content/tex_mapping/";

bool left_mouseBtn_drag = false;
bool right_mouseBtn_drag = false;

const GLfloat g_quad_buffer_data[] =
{
	// ndc pos         // UV
	-0.5, -0.5f, 0.0f, 0.0f, 0.0f,
	0.5f, -0.5f, 0.0f,  1.0f, 0.0f,
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
	bool LoadTextureData( const char* i_texturePath, std::vector<unsigned char>& io_data, unsigned int& io_width, unsigned int& io_height )
	{
		io_data.clear();
		auto errorCode = lodepng::decode( io_data, io_width, io_height, i_texturePath );
		if (errorCode)
		{
			fprintf( stderr, "decoder error %d : %s.\n", errorCode, lodepng_error_text( errorCode ) );
			return false;
		}
		else
		{
			fprintf( stdout, "Loaded/Decoded the %s texture successfully.\n", i_texturePath );
			return true;
		}
	}

	cyMatrix4f GetLookAtMatrix( const cyVec3f& i_pos, const cyVec3f& i_target, const cyVec3f& i_worldUp )
	{
		auto dir = ( i_pos - i_target ).GetNormalized();
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

void CompileShaders( const char* i_path_vertexShader,
	const char* i_path_fragementShader,
	cyGLSLProgram& i_glslProgram,
	const char* i_path_geometryShader = "",
	const char* i_path_tess_control = "",
	const char* i_path_tess_evaluation = ""
)
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

	// Tessellation control shader
	if (strlen( i_path_tess_control ))
	{
		cyGLSLShader tess_control;
		if (!tess_control.CompileFile( i_path_tess_control, GL_TESS_CONTROL_SHADER ))
		{
			fprintf( stderr, "Failed to compile the tessellation control shader, %s.\n", i_path_tess_control );
		}
		else
		{
			fprintf( stdout, "Compiled the tessellation control shader, %s, successfully.\n", i_path_tess_control );
		}
		i_glslProgram.AttachShader( tess_control );
	}
	// Tessellation evaluate shader
	if (strlen( i_path_tess_evaluation ))
	{
		cyGLSLShader tess_evaluate;
		if (!tess_evaluate.CompileFile( i_path_tess_evaluation, GL_TESS_EVALUATION_SHADER ))
		{
			fprintf( stderr, "Failed to compile the tessellation evaluation shader, %s.\n", i_path_tess_evaluation );
		}
		else
		{
			fprintf( stdout, "Compiled the tessellation evaluation shader, %s, successfully.\n", i_path_tess_evaluation );
		}
		i_glslProgram.AttachShader( tess_evaluate );
	}

	// Geometry shader
	if (strlen( i_path_geometryShader ))
	{
		cyGLSLShader geometryShader;
		if (!geometryShader.CompileFile( i_path_geometryShader, GL_GEOMETRY_SHADER ))
		{
			fprintf( stderr, "Failed to compile the geometry shader, %s.\n", i_path_geometryShader );
		}
		else
		{
			fprintf( stdout, "Compiled the geometry shader, %s, successfully.\n", i_path_geometryShader );
		}
		i_glslProgram.AttachShader( geometryShader );
	}
	i_glslProgram.AttachShader( fragmentShader );
	i_glslProgram.Link();
	assert( glGetError() == GL_NO_ERROR );
}

#pragma region  
void InitializeMaterial( cyTriMesh& i_mesh, cyGLSLProgram& i_shaderProgram )
{
	//if (i_mesh.NM() > 0)
	//{
	//	i_shaderProgram.Bind();
	//	glUniform3f( glGetUniformLocation( i_shaderProgram.GetID(), "diffuseColor" ), i_mesh.M( 0 ).Kd[0], i_mesh.M( 0 ).Kd[1], i_mesh.M( 0 ).Kd[2] );
	//	glUniform3f( glGetUniformLocation( i_shaderProgram.GetID(), "ambientColor" ), i_mesh.M( 0 ).Ka[0], i_mesh.M( 0 ).Ka[1], i_mesh.M( 0 ).Ka[2] );
	//	glUniform3f( glGetUniformLocation( i_shaderProgram.GetID(), "specularColor" ), i_mesh.M( 0 ).Ks[0], i_mesh.M( 0 ).Ks[1], i_mesh.M( 0 ).Ks[2] );
	//}
	//else
	//{
	i_shaderProgram.Bind();
	assert( glGetError() == GL_NO_ERROR );
	glUniform3f( glGetUniformLocation( i_shaderProgram.GetID(), "diffuseColor" ), 0.5f, 0.5, 0.5f );
	glUniform3f( glGetUniformLocation( i_shaderProgram.GetID(), "ambientColor" ), 0.5f, 0.5f, 0.5f );
	glUniform3f( glGetUniformLocation( i_shaderProgram.GetID(), "specularColor" ), 0.80099994f, 0.80099994f, 0.80099994f );

	//}
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
	if (!i_rtt.Resize( 4, width_screen, height_screen ))
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
	if (!i_renderDepth2D.Resize( width_shadow, height_shadow ))
	{
		fprintf( stderr, "Cannot resize the render depth." );
		assert( false );
	}
}

void LoadTexture( const char* i_name_tex, cyGLTexture2D& io_tex )
{
	std::string path_texFile( path_texResource );
	path_texFile += i_name_tex;
	unsigned int width_tex;
	unsigned int height_tex;
	std::vector<unsigned char> data_tex;
	if (!LoadTextureData( path_texFile.c_str(), data_tex, width_tex, height_tex ))
	{
		assert( false );
	}
	io_tex.Initialize();
	io_tex.SetWrappingMode( GL_REPEAT, GL_REPEAT );
	io_tex.SetImage<unsigned char>( data_tex.data(), 4, width_tex, height_tex, 0 );
	io_tex.BuildMipmaps();
	fprintf_s( stdout, "Loaded the %s texture successfully.\n", i_name_tex );
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
		//for (int i = 0; i < i_mesh.NV(); i++)
		//{
		//	vertices.push_back( i_mesh.V( i ) );
		//}
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
		//for (int i = 0; i < i_mesh.NVT(); i++)
		//{
		//	texCoords.push_back( i_mesh.VT( i ).XY() );
		//}
		glGenVertexArrays( 1, &i_VAO );
		glBindVertexArray( i_VAO );
		assert( glGetError() == GL_NO_ERROR );
		const auto sizeOfVertices = static_cast<GLsizei>( vertices.size() * sizeof( cyVec3f ) );
		const auto sizeOfNormals = static_cast<GLsizei>( vertexNormals.size() * sizeof( cyVec3f ) );
		const auto sizeOfTexCoord = static_cast<GLsizei>( texCoords.size() * sizeof( cyVec2f ) );
		// For vertex data buffer
		glGenBuffers( 1, &i_VBO );
		glBindBuffer( GL_ARRAY_BUFFER, i_VBO );
		glBufferData( GL_ARRAY_BUFFER, sizeOfVertices + sizeOfNormals + sizeOfTexCoord, NULL, GL_STATIC_DRAW );
		glBufferSubData( GL_ARRAY_BUFFER, 0, sizeOfVertices, reinterpret_cast<void*>( vertices.data() ) );
		glBufferSubData( GL_ARRAY_BUFFER, sizeOfVertices, sizeOfNormals, reinterpret_cast<void*>( vertexNormals.data() ) );
		glBufferSubData( GL_ARRAY_BUFFER, sizeOfVertices + sizeOfNormals, sizeOfTexCoord, reinterpret_cast<void*>( texCoords.data() ) );

		glVertexAttribPointer( 0, 3, GL_FLOAT, GL_FALSE, static_cast<GLsizei>( sizeof( cyVec3f ) ), (const GLvoid*)( 0 ) );
		glVertexAttribPointer( 1, 3, GL_FLOAT, GL_FALSE, static_cast<GLsizei>( sizeof( cyVec3f ) ), (const GLvoid*)( sizeOfVertices ) );
		glVertexAttribPointer( 2, 2, GL_FLOAT, GL_FALSE, static_cast<GLsizei>( sizeof( cyVec2f ) ), (const GLvoid*)( sizeOfVertices + sizeOfNormals ) );

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
		const auto sizeOfVertices = static_cast<GLsizei>( vertices.size() * sizeof( cyVec3f ) );
		const auto sizeOfNormals = static_cast<GLsizei>( vertexNormals.size() * sizeof( cyVec3f ) );
		// For vertex data buffer
		glGenBuffers( 1, &i_VBO );
		glBindBuffer( GL_ARRAY_BUFFER, i_VBO );
		glBufferData( GL_ARRAY_BUFFER, sizeOfVertices + sizeOfNormals, NULL, GL_STATIC_DRAW );
		glBufferSubData( GL_ARRAY_BUFFER, 0, sizeOfVertices, reinterpret_cast<void*>( vertices.data() ) );
		glBufferSubData( GL_ARRAY_BUFFER, sizeOfVertices, sizeOfNormals, reinterpret_cast<void*>( vertexNormals.data() ) );

		glVertexAttribPointer( 0, 3, GL_FLOAT, GL_FALSE, static_cast<GLsizei>( sizeof( cyVec3f ) ), (const GLvoid*)( 0 ) );
		glVertexAttribPointer( 1, 3, GL_FLOAT, GL_FALSE, static_cast<GLsizei>( sizeof( cyVec3f ) ), (const GLvoid*)( sizeOfVertices ) );

		glEnableVertexAttribArray( 0 );
		glEnableVertexAttribArray( 1 );

		glBindBuffer( GL_ARRAY_BUFFER, 0 );
		glBindBuffer( GL_ARRAY_BUFFER, 0 );
		glBindVertexArray( 0 );
		assert( glGetError() == GL_NO_ERROR );
	}
}

void InitializeSimpleQuad( const char* i_objFileName, cyTriMesh& i_mesh, GLuint& i_VAO, GLuint& i_VBO )
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
		for (int i = 0; i < i_mesh.NV(); i++)
		{
			vertices.push_back( i_mesh.V( i ) );
		}
		for (int i = 0; i < i_mesh.NVN(); i++)
		{
			vertexNormals.push_back( i_mesh.VN( i ) );
		}
		std::vector<cyVec2f> texCoords;
		for (int i = 0; i < i_mesh.NVT(); i++)
		{
			texCoords.push_back( i_mesh.VT( i ).XY() );
		}
		glGenVertexArrays( 1, &i_VAO );
		glBindVertexArray( i_VAO );
		assert( glGetError() == GL_NO_ERROR );
		const auto sizeOfVertices = static_cast<GLsizei>( vertices.size() * sizeof( cyVec3f ) );
		const auto sizeOfNormals = static_cast<GLsizei>( vertexNormals.size() * sizeof( cyVec3f ) );
		const auto sizeOfTexCoord = static_cast<GLsizei>( texCoords.size() * sizeof( cyVec2f ) );
		// For vertex data buffer
		glGenBuffers( 1, &i_VBO );
		glBindBuffer( GL_ARRAY_BUFFER, i_VBO );
		glBufferData( GL_ARRAY_BUFFER, sizeOfVertices + sizeOfNormals + sizeOfTexCoord, NULL, GL_STATIC_DRAW );
		glBufferSubData( GL_ARRAY_BUFFER, 0, sizeOfVertices, reinterpret_cast<void*>( vertices.data() ) );
		glBufferSubData( GL_ARRAY_BUFFER, sizeOfVertices, sizeOfNormals, reinterpret_cast<void*>( vertexNormals.data() ) );
		glBufferSubData( GL_ARRAY_BUFFER, sizeOfVertices + sizeOfNormals, sizeOfTexCoord, reinterpret_cast<void*>( texCoords.data() ) );

		glVertexAttribPointer( 0, 3, GL_FLOAT, GL_FALSE, static_cast<GLsizei>( sizeof( cyVec3f ) ), (const GLvoid*)( 0 ) );
		glVertexAttribPointer( 1, 3, GL_FLOAT, GL_FALSE, static_cast<GLsizei>( sizeof( cyVec3f ) ), (const GLvoid*)( sizeOfVertices ) );
		glVertexAttribPointer( 2, 2, GL_FLOAT, GL_FALSE, static_cast<GLsizei>( sizeof( cyVec2f ) ), (const GLvoid*)( sizeOfVertices + sizeOfNormals ) );

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
		for (int i = 0; i < i_mesh.NV(); i++)
		{
			vertices.push_back( i_mesh.V( i ) );
		}
		for (int i = 0; i < i_mesh.NVN(); i++)
		{
			vertexNormals.push_back( i_mesh.VN( i ) );
		}
		glGenVertexArrays( 1, &i_VAO );
		glBindVertexArray( i_VAO );
		assert( glGetError() == GL_NO_ERROR );
		const auto sizeOfVertices = static_cast<GLsizei>( vertices.size() * sizeof( cyVec3f ) );
		const auto sizeOfNormals = static_cast<GLsizei>( vertexNormals.size() * sizeof( cyVec3f ) );
		// For vertex data buffer
		glGenBuffers( 1, &i_VBO );
		glBindBuffer( GL_ARRAY_BUFFER, i_VBO );
		glBufferData( GL_ARRAY_BUFFER, sizeOfVertices + sizeOfNormals, NULL, GL_STATIC_DRAW );
		glBufferSubData( GL_ARRAY_BUFFER, 0, sizeOfVertices, reinterpret_cast<void*>( vertices.data() ) );
		glBufferSubData( GL_ARRAY_BUFFER, sizeOfVertices, sizeOfNormals, reinterpret_cast<void*>( vertexNormals.data() ) );

		glVertexAttribPointer( 0, 3, GL_FLOAT, GL_FALSE, static_cast<GLsizei>( sizeof( cyVec3f ) ), (const GLvoid*)( 0 ) );
		glVertexAttribPointer( 1, 3, GL_FLOAT, GL_FALSE, static_cast<GLsizei>( sizeof( cyVec3f ) ), (const GLvoid*)( sizeOfVertices ) );

		glEnableVertexAttribArray( 0 );
		glEnableVertexAttribArray( 1 );

		glBindBuffer( GL_ARRAY_BUFFER, 0 );
		glBindBuffer( GL_ARRAY_BUFFER, 0 );
		glBindVertexArray( 0 );
		assert( glGetError() == GL_NO_ERROR );
	}
}

void InitializeView()
{
	g_target_camera = ( g_planeMesh.GetBoundMax() + g_planeMesh.GetBoundMin() ) / 2;
	g_target_light = ( g_planeMesh.GetBoundMax() + g_planeMesh.GetBoundMin() ) / 2;

	g_dir_targetToCamera = ( g_cameraPosInWorld - g_target_camera ).GetNormalized();
	g_dir_targetTolight = ( g_lightPosInWorld - g_target_light ).GetNormalized();

	g_dis_cameraToTarget = ( g_cameraPosInWorld - g_target_camera ).Length();
	g_dis_lightToTarget = ( g_lightPosInWorld - g_target_light ).Length();
}

#pragma endregion Initialization

#pragma region
void RenderScene( bool i_bDrawShdow = false )
{
	glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
	// Draw the light source
	{
		g_sp_lightMesh.Bind();
		glUniformMatrix4fv( glGetUniformLocation( g_sp_lightMesh.GetID(), "mat_model" ), 1, GL_FALSE, g_mat_light.cell );
		glBindVertexArray( VAO_light );
		glDrawArrays( GL_TRIANGLES, 0, 3 * g_lightMesh.NF() );
		assert( glGetError() == GL_NO_ERROR );
		glBindVertexArray( 0 );
	}
	g_tex_normalMap.Bind( 0 );
	if (g_displacement)
	{
		g_tex_dispMap.Bind( 1 );
	}
	g_tex_renderDepth.BindTexture();
	//Display the plane
	{
		g_sp_tessellation.Bind();
		glUniform1i( glGetUniformLocation( g_sp_tessellation.GetID(), "tex_normal" ), 0 );
		glUniform1i( glGetUniformLocation( g_sp_tessellation.GetID(), "tex_disp" ), 1 );
		glUniform1i( glGetUniformLocation( g_sp_tessellation.GetID(), "tex_shadowMap" ), 2 );
		glUniform1f( glGetUniformLocation( g_sp_tessellation.GetID(), "level_tess" ), g_level_tess );
		glUniform1i( glGetUniformLocation( g_sp_tessellation.GetID(), "displacement" ), g_displacement );
		glUniform1i( glGetUniformLocation( g_sp_tessellation.GetID(), "shadowing" ), 1 );
		glUniformMatrix4fv( glGetUniformLocation( g_sp_tessellation.GetID(), "mat_model" ), 1, GL_FALSE, g_mat_plane.cell );
		glBindVertexArray( VAO_plane );
		glPatchParameteri( GL_PATCH_VERTICES, 4 );
		glDrawArrays( GL_PATCHES, 0, g_planeMesh.NV() );
		glBindVertexArray( 0 );
	}
	if (bShowTessGrids)
	{
		// Draw the outline of the plane separately
		{
			g_sp_outline.Bind();
			glUniformMatrix4fv( glGetUniformLocation( g_sp_outline.GetID(), "mat_model" ), 1, GL_FALSE, g_mat_plane.cell );
			glUniform1i( glGetUniformLocation( g_sp_outline.GetID(), "displacement" ), g_displacement );
			glUniform1f( glGetUniformLocation( g_sp_outline.GetID(), "level_tess" ), g_level_tess );
			glUniform1i( glGetUniformLocation( g_sp_outline.GetID(), "tex_normal" ), 0 );
			glUniform1i( glGetUniformLocation( g_sp_outline.GetID(), "tex_disp" ), 1 );
			glBindVertexArray( VAO_plane );
			glPatchParameteri( GL_PATCH_VERTICES, 4 );
			glDrawArrays( GL_PATCHES, 0, g_planeMesh.NV() );
			assert( glGetError() == GL_NO_ERROR );
			glBindVertexArray( 0 );
		}
	}
}

void GenerateShadowMap()
{
	glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
	{
		g_sp_shadowPass_tess.Bind();
		//Display the plane
		{
			g_sp_tessellation.Bind();
			glUniform1i( glGetUniformLocation( g_sp_tessellation.GetID(), "tex_normal" ), 0 );
			glUniform1i( glGetUniformLocation( g_sp_tessellation.GetID(), "tex_disp" ), 1 );
			glUniform1f( glGetUniformLocation( g_sp_tessellation.GetID(), "level_tess" ), g_level_tess );
			glUniform1i( glGetUniformLocation( g_sp_tessellation.GetID(), "displacement" ), g_displacement );
			assert( glGetError() == GL_NO_ERROR );
			glUniformMatrix4fv( glGetUniformLocation( g_sp_tessellation.GetID(), "mat_model" ), 1, GL_FALSE, g_mat_plane.cell );
			glBindVertexArray( VAO_plane );
			glPatchParameteri( GL_PATCH_VERTICES, 4 );
			glDrawArrays( GL_PATCHES, 0, g_planeMesh.NV() );
			glBindVertexArray( 0 );
		}
	}
}

void Display()
{
	// Render the scene to the shadow map from the light perspective
	{
		g_tex_renderDepth.Bind();
		glCullFace( GL_FRONT );
		GenerateShadowMap();
		glCullFace( GL_BACK );
		g_tex_renderDepth.Unbind();
	}

	{
		RenderScene( true );
	}
	assert( glGetError() == GL_NO_ERROR );
}

void UpdateCamera()
{
	auto mat_camera_rot = cyMatrix4f( 1.0f );
	mat_camera_rot.SetRotationXYZ( glm::radians( camera_angle_pitch ), glm::radians( camera_angle_yaw ), 0 );
	auto dir_camera = mat_camera_rot * cyVec4f( g_dir_targetToCamera, 0.0f );
	g_cameraPosInWorld = g_target_camera + cyVec3f( dir_camera ) * g_dis_cameraToTarget;

	auto mat_cameraView = GetLookAtMatrix( g_cameraPosInWorld, g_target_camera, cyVec3f( 0, 1.0, 0 ) );
	auto mat_perspective = cyMatrix4f( 1.0f );
	mat_perspective.SetPerspective( glm::radians( 60.0f ), width_screen / height_screen, 5.0f, 1000.0f );

	g_sp_lightMesh.Bind();
	glUniformMatrix4fv( glGetUniformLocation( g_sp_lightMesh.GetID(), "mat_view" ), 1, GL_FALSE, mat_cameraView.cell );
	glUniformMatrix4fv( glGetUniformLocation( g_sp_lightMesh.GetID(), "mat_projection" ), 1, GL_FALSE, mat_perspective.cell );

	g_sp_tessellation.Bind();
	glUniformMatrix4fv( glGetUniformLocation( g_sp_tessellation.GetID(), "mat_view" ), 1, GL_FALSE, mat_cameraView.cell );
	glUniformMatrix4fv( glGetUniformLocation( g_sp_tessellation.GetID(), "mat_projection" ), 1, GL_FALSE, mat_perspective.cell );

	g_sp_outline.Bind();
	glUniformMatrix4fv( glGetUniformLocation( g_sp_outline.GetID(), "mat_view" ), 1, GL_FALSE, mat_cameraView.cell );
	glUniformMatrix4fv( glGetUniformLocation( g_sp_outline.GetID(), "mat_projection" ), 1, GL_FALSE, mat_perspective.cell );

	assert( glGetError() == GL_NO_ERROR );
}

void UpdateLight()
{
	auto mat_light_rot = cyMatrix4f( 1.0f );
	mat_light_rot.SetRotationXYZ( glm::radians( light_angle_pitch ), glm::radians( light_angle_yaw ), 0 );
	auto dir_light = mat_light_rot * cyVec4f( g_dir_targetTolight, 0.0f );
	g_lightPosInWorld = g_target_light + cyVec3f( dir_light ) * g_dis_lightToTarget;

	auto mat_trans = cyMatrix4f( 1.0 );
	mat_trans.SetTranslation( g_lightPosInWorld );
	g_mat_light = mat_trans * mat_light_rot;

	auto worldUp = cyVec3f( 0, 1, 0 );
	auto mat_lightSpace_view = GetLookAtMatrix( g_lightPosInWorld, g_target_light, worldUp );

	auto mat_perspective = cyMatrix4f( 1.0f );
	float near_plane = 1.0f;
	float far_plane = 200.0f;
	mat_perspective.SetPerspective( glm::radians( 60.0f ), width_shadow / height_shadow, near_plane, far_plane );

	auto mat_lightSpace = mat_perspective * mat_lightSpace_view;

	g_sp_lightMesh.Bind();
	cyVec3f lightSource( 1.0f, 1.0f, 1.0f );
	glUniform3fv( glGetUniformLocation( g_sp_lightMesh.GetID(), "lightPos" ), 1, &lightSource.elem[0] );

	g_sp_tessellation.Bind();
	glUniform3fv( glGetUniformLocation( g_sp_tessellation.GetID(), "worldPos_light" ), 1, &g_lightPosInWorld.elem[0] );
	glUniformMatrix4fv( glGetUniformLocation( g_sp_tessellation.GetID(), "mat_light" ), 1, GL_FALSE, g_mat_light.cell );

	//g_sp_shadowPass_tess.Bind();
	//glUniformMatrix4fv( glGetUniformLocation( g_sp_tessellation.GetID(), "mat_lightSpaceTransformation" ), 1, GL_FALSE, mat_lightSpace.cell );
}

void UpdateModels()
{
	g_mat_plane.SetTranslation( cyVec3f( 0, 0, 0 ) );
	g_mat_model.SetTranslation( cyVec3f( 0, 10, 0 ) );

#if defined(RENDER_SHADOW)
	g_sp_quad.Bind();
	auto mat_quad_rot = cyMatrix4f( 1.0f );
	mat_quad_rot.SetRotationXYZ( glm::radians( -camera_angle_pitch ), glm::radians( -camera_angle_yaw ), 0 );
	glUniformMatrix4fv( glGetUniformLocation( g_sp_quad.GetID(), "mat_rttRot" ), 1, GL_FALSE, mat_quad_rot.cell );
	glUniform1f( glGetUniformLocation( g_sp_quad.GetID(), "dis" ), 1.0f );
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
			bControlTheLight = true;
		}
		else if (i_action == GLFW_RELEASE)
		{
			bControlTheLight = false;
		}
	}
	if (i_key == GLFW_KEY_LEFT)
	{
		if (i_action == GLFW_RELEASE)
		{
			if (g_level_tess > 1.0f)
			{
				g_level_tess -= 1.0f;
			}
		}
	}
	if (i_key == GLFW_KEY_RIGHT)
	{
		if (i_action == GLFW_RELEASE)
		{
			g_level_tess += 1.0f;

		}
	}
	if (i_key == GLFW_KEY_SPACE)
	{
		if (i_action == GLFW_RELEASE)
		{
			bShowTessGrids = !bShowTessGrids;
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

void UpdateKeyboardIput()
{
	//	if (bKeyWPressed)
	//	{
	//		camera_distance_z -= camera_moveSpeed_perframe;
	//	}
	//	if (bKeyAPressed)
	//	{
	//		camera_distance_x += camera_moveSpeed_perframe;
	//	}
	//	if (bKeySPressed)
	//	{
	//		camera_distance_z += camera_moveSpeed_perframe;
	//	}
	//	if (bKeyDPressed)
	//	{
	//		camera_distance_x -= camera_moveSpeed_perframe;
	//	}
}

void UpdateMouseInput( GLFWwindow* i_pWindow, float i_frameTime = 0.016 )
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
		auto moveDistance = g_moveSpeed * i_frameTime;
		if (dis.Dot( cyVec2d( 0, 1 ) ) > 0)
		{
			if (bControlTheLight)
			{
				g_dis_lightToTarget -= moveDistance;
			}
			else
			{
				g_dis_cameraToTarget -= moveDistance;
			}

		}
		else if (dis.Dot( cyVec2d( 0, 1 ) ) < 0)
		{
			if (bControlTheLight)
			{
				g_dis_lightToTarget += moveDistance;
			}
			else
			{
				g_dis_cameraToTarget += moveDistance;
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
			if (bControlTheLight)
			{
				light_angle_yaw -= g_rotate_yaw * i_frameTime;
			}
			else
			{
				camera_angle_yaw -= g_rotate_yaw * i_frameTime;
			}
		}
		else if (dis.Dot( cyVec2d( 1, 0 ) ) < 0)
		{
			if (bControlTheLight)
			{
				light_angle_yaw += g_rotate_yaw * i_frameTime;
			}
			else
			{
				camera_angle_yaw += g_rotate_yaw * i_frameTime;
			}
		}

		// Rotate around the pitch
		if (dis.Dot( cyVec2d( 0, 1 ) ) > 0)
		{
			if (bControlTheLight)
			{
				light_angle_pitch -= g_rotate_pitch * i_frameTime;
			}
			else
			{
				camera_angle_pitch -= g_rotate_pitch * i_frameTime;
			}
		}
		else if (dis.Dot( cyVec2d( 0, 1 ) ) < 0)
		{
			if (bControlTheLight)
			{
				light_angle_pitch += g_rotate_pitch * i_frameTime;
			}
			else
			{
				camera_angle_pitch += g_rotate_pitch * i_frameTime;
			}
		}
	}
}
#pragma endregion Update

int main( int argc, char* argv[] )
{
	if (argc < 2)
	{
		fprintf( stderr, "Please enter the normal and disply mapping file name as the argument.\n" );
		system( "pause" );
		return -1;
	}
	if (argc < 3)
	{
		g_displacement = 0;
	}
	else
	{
		g_displacement = 1;
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
	pWindow = glfwCreateWindow( width_screen, height_screen, "Hello World", NULL, NULL );
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

	CompileShaders( path_vertexShader_mesh, path_fragmentShader_mesh, g_sp_lightMesh );
	InitializeMesh( "light.obj", g_lightMesh, VAO_light, VBO_light );
	InitializeMaterial( g_lightMesh, g_sp_lightMesh );

	// tessellation shader
	CompileShaders(
		path_vertexShader_tessellation,
		path_fragmentShader_tessellation,
		g_sp_tessellation,
		"",
		path_tess_control,
		path_tess_evaulation
	);

	assert( glGetError() == GL_NO_ERROR );
	InitializeSimpleQuad( "plane.obj", g_planeMesh, VAO_plane, VBO_plane );
	InitializeMaterial( g_planeMesh, g_sp_tessellation );

	// outline shader
	CompileShaders(
		path_vertexShader_tessellation,
		path_fragmentShader_outline,
		g_sp_outline,
		path_geometryShader_outline,
		path_tess_control,
		path_tess_evaulation
	);

	CompileShaders(
		path_vertexShader_tessellation,
		path_fragmentShader_shadowPass,
		g_sp_shadowPass_tess,
		"",
		path_tess_control,
		path_tess_evaulation
	);
	assert( glGetError() == GL_NO_ERROR );

	// Load the flip .png to resolve the texture upside down bug.
	LoadTexture( argv[1], g_tex_normalMap );
	if (g_displacement)
	{
		LoadTexture( argv[2], g_tex_dispMap );
	}
	InitializeView();
	InitializeDepthMap( g_tex_renderDepth );

	while (!glfwWindowShouldClose( pWindow ))
	{
		UpdateMouseInput( pWindow );

		UpdateCamera();
		UpdateLight();

		Display();
		/* Swap front and back buffers */
		glfwSwapBuffers( pWindow );
		/* Poll for and process events */
		glfwPollEvents();
	}
	// Release buffers and the shader program.
	{
		glDeleteVertexArrays( 1, &VAO_plane );
		glDeleteBuffers( 1, &VBO_plane );

		glDeleteVertexArrays( 1, &VAO_light );
		glDeleteBuffers( 1, &VBO_light );

		g_tex_normalMap.Delete();
		g_tex_dispMap.Delete();
		g_sp_lightMesh.Delete();
		g_sp_quad.Delete();
		g_sp_shadowPass_tess.Delete();

		if (glGetError() != GL_NO_ERROR)
		{
			assert( false );
		}
	}
	glfwTerminate();
	return 0;
}