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
#include "PoissonGenerator.h"
#include <AntTweakBar/AntTweakBar.h>

#define MIN_NUM_SAMPLES 4
#define MAX_NUM_SAMPLES 32

GLuint VAO_teapot;
GLuint VBO_teapot;

GLuint VAO_sphere;
GLuint VBO_sphere;

GLuint VAO_cube;
GLuint VBO_cube;

GLuint VAO_plane;
GLuint VBO_plane;

GLuint VAO_light;
GLuint VBO_light;

GLuint VAO_quad;
GLuint VBO_quad;
GLuint EBO_quad;

cyGLRenderTexture2D g_rtt_mirror;
cyGLRenderDepth2D g_tex_renderDepth;

cyGLTexture1D g_tex_possion_distribution0;
cyGLTexture1D g_tex_possion_distribution1;

cyGLSLProgram g_sp_skybox;
cyGLSLProgram g_sp_shadowMap;
cyGLSLProgram g_sp_shadowPass;
cyGLSLProgram g_sp_shadowMesh;
cyGLSLProgram g_sp_lightMesh;

cyTriMesh g_objMesh_teapot;
cyTriMesh g_objMesh_sphere;
cyTriMesh g_objMesh_cube;
cyTriMesh g_planeMesh;
cyTriMesh g_lightMesh;
cyVec3f g_dir_targetToCamera;
cyVec3f g_dir_targetTolight;
cyVec3f g_target_camera;
cyVec3f g_target_light;

cyMatrix4f g_mat_teapot = cyMatrix4f( 1.0f );
cyMatrix4f g_mat_plane = cyMatrix4f( 1.0f );
cyMatrix4f g_mat_light = cyMatrix4f( 1.0f );
cyMatrix4f g_mat_sphere = cyMatrix4f( 1.0f );
cyMatrix4f g_mat_cube = cyMatrix4f( 1.0f );

bool bControlTheLight = false;

float camera_angle_yaw = 0;
float camera_angle_pitch = 0;

cyVec3f g_lightPosInWorld = cyVec3f( 0.0f, 0.0f, 40.0f );
cyVec3f g_cameraPosInWorld = cyVec3f( 40.0f, 100.0f, 40.0f );

float g_moveSpeed = 50;
float g_rotate_yaw = 60;
float g_rotate_pitch = 50;

float rotation_yaw = 2.0f;
float rotation_pitch = 1.5f;

float light_angle_yaw = 0.0f;
float light_angle_pitch = 0.0f;

float g_dis_lightToTarget = 60.0f;
float g_dis_cameraToTarget = 60.0f;

float g_width_screen = 640;
float g_height_screen = 480;

float g_light_size = 1.5f;

float width_shadow = 640;
float height_shadow = 480;

constexpr char const* path_vertexShader_mesh = "content/shaders/vertex_mesh.shader";
constexpr char const* path_fragmentShader_mesh = "content/shaders/fragment_mesh.shader";

constexpr char const* path_vertexShader_shadow = "content/shaders/vertex_shadow.shader";
constexpr char const* path_fragmentShader_shadow = "content/shaders/fragment_shadow.shader";

constexpr char const* path_vertexShader_shadowPass = "content/shaders/vertex_shadowPass.shader";
constexpr char const* path_fragmentShader_shadowPass = "content/shaders/fragment_shadowPass.shader";

constexpr char const* path_vertexShader_quad = "content/shaders/vertex_quad_texture.shader";
constexpr char const* path_fragmentShader_shadowMap = "content/shaders/fragment_quad_shadowMap.shader";

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

int g_num_blockerSearchSamples = 8;
int g_num_pcfFilteringSamples = 8;

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
	-1.0, -1.0f, 0.0f, 0.0f, 0.0f,
	1.0f, -1.0f, 0.0f,  1.0f, 0.0f,
	1.0f,  1.0f,  0.0f,  1.0f, 1.0f,
	-1.0f, 1.0f,  0.0f,  0.0f, 1.0f
};

const unsigned int g_quad_indices_data[] =
{
	0,1,2,
	2,3,0
};

const unsigned int count_quad_indices = 6;

namespace
{
	void CheckGLError()
	{
		auto err = glGetError();
		if (err != GL_NO_ERROR)
		{
			fprintf( stderr, "There is an error with the GL, Error: %s\n", glewGetErrorString( err ) );
			assert( false );
		}
	}
	void CreatePoissonDiscDistribution( cyGLTexture1D& io_tex, size_t numSamples )
	{
		PoissonGenerator::DefaultPRNG PRNG_0;
		PoissonGenerator::DefaultPRNG PRNG_1;
		auto points = PoissonGenerator::GeneratePoissonPoints( numSamples * 2, PRNG_0 );
		size_t attempts = 0;
		while (points.size() < numSamples && ++attempts < 100)
			points = PoissonGenerator::GeneratePoissonPoints( numSamples * 2, PRNG_1 );
		if (attempts == 100)
		{
			std::cout << "couldn't generate Poisson-disc distribution with " << numSamples << " samples" << std::endl;
			numSamples = points.size();
		}
		std::vector<float> data( numSamples * 2 );
		for (auto i = 0, j = 0; i < numSamples; i++, j += 2)
		{
			auto& point = points[i];
			data[j] = point.x;
			data[j + 1] = point.y;
		}
		io_tex.Initialize();
		io_tex.SetImageRG<float>( data.data(), numSamples, 0 );
		io_tex.SetFilteringMode( GL_NEAREST, GL_NEAREST );
		io_tex.SetWrappingMode( GL_CLAMP );
	}
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

void CompileShaders( const char* i_path_vertexShader, const char* i_path_fragementShader, cyGLSLProgram& i_glslProgram )
{
	i_glslProgram.Delete();
	i_glslProgram.CreateProgram();
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
	CheckGLError();
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
				0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, reinterpret_cast<void*>( data.data() )
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

	CompileShaders( path_vertexShader_skybox, path_fragmentShader_skybox, g_sp_skybox );
	g_sp_skybox.Bind();
	glUniform1i( glGetUniformLocation( g_sp_skybox.GetID(), "tex_cubemap" ), 0 );

	glGenBuffers( 1, &VBO_cubemap );
	glBindBuffer( GL_ARRAY_BUFFER, VBO_cubemap );
	glBufferData( GL_ARRAY_BUFFER, sizeof( g_skyboxVertices ), &g_skyboxVertices[0], GL_STATIC_DRAW );
	glVertexAttribPointer( 0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof( GLfloat ), (const GLvoid*)( 0 ) );
	glEnableVertexAttribArray( 0 );
	glBindBuffer( GL_ARRAY_BUFFER, 0 );
	glBindVertexArray( 0 );
	CheckGLError();
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
	if (!i_rtt.Resize( 4, g_width_screen, g_height_screen ))
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

void InitializeTextures( cyTriMesh& i_mesh, cyGLSLProgram& i_shaderProgram )
{
	//if (i_mesh.NM() <= 0)
	//{
	//	return;
	//}
	//if (i_mesh.M( 0 ).map_Kd != nullptr)
	//{
	//	std::string path_diffusesTex( path_meshResource );
	//	path_diffusesTex += i_mesh.M( 0 ).map_Kd;
	//	pDiffuseTex = Ass_Inter_Comp_Graphics::Texture::Create( path_diffusesTex.c_str() );
	//	if (!pDiffuseTex)
	//	{
	//		fprintf( stderr, "Failed to create the diffuse texture.\n" );
	//		assert( false );
	//	}
	//}
	//if (i_mesh.M( 0 ).map_Ks != nullptr)
	//{
	//	std::string path_specularTex( path_meshResource );
	//	path_specularTex += i_mesh.M( 0 ).map_Ks;
	//	pSpecularTex = Ass_Inter_Comp_Graphics::Texture::Create( path_specularTex.c_str() );
	//	if (!pSpecularTex)
	//	{
	//		fprintf( stderr, "Failed to create the specular texture.\n" );
	//		assert( false );
	//	}
	//}
	//i_shaderProgram.Bind();
	//glUniform1i( glGetUniformLocation( i_shaderProgram.GetID(), "tex_diff" ), 0 );
	//glUniform1i( glGetUniformLocation( i_shaderProgram.GetID(), "tex_spec" ), 1 );
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
		CheckGLError();
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
		glBindVertexArray( 0 );

		CheckGLError();
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
		CheckGLError();
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
		CheckGLError();
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
	glVertexAttribPointer( 0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof( GLfloat ), (const GLvoid*)( 0 ) );
	glEnableVertexAttribArray( 0 );
	glVertexAttribPointer( 1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof( GLfloat ), (const GLvoid*)( 3 * sizeof( GLfloat ) ) );
	glEnableVertexAttribArray( 1 );

	glGenBuffers( 1, &i_EBO );
	glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, i_EBO );
	glBufferData( GL_ELEMENT_ARRAY_BUFFER, static_cast<GLsizeiptr>( sizeof( unsigned int ) * count_quad_indices ), &g_quad_indices_data[0], GL_STATIC_DRAW );

	CheckGLError();

	glBindVertexArray( 0 );
	glBindBuffer( GL_ARRAY_BUFFER, 0 );
}

void InitializeView()
{
	g_target_camera = ( g_objMesh_teapot.GetBoundMax() + g_objMesh_teapot.GetBoundMin() ) / 2;
	g_target_light = ( g_objMesh_teapot.GetBoundMax() + g_objMesh_teapot.GetBoundMin() ) / 2;
	g_lightPosInWorld = g_target_light + cyVec3f( 0, 0, 100 );

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
		glBindVertexArray( 0 );
	}

	g_sp_shadowMesh.Bind();
	glUniform1i( glGetUniformLocation( g_sp_shadowMesh.GetID(), "tex_shadowMap" ), 0 );
	if (i_bDrawShdow)
	{
		glUniform1i( glGetUniformLocation( g_sp_shadowMesh.GetID(), "receiveShadow" ), 1 );
	}
	else
	{
		glUniform1i( glGetUniformLocation( g_sp_shadowMesh.GetID(), "receiveShadow" ), 0 );
	}
	g_tex_possion_distribution0.Bind( 1 );
	g_tex_possion_distribution1.Bind( 2 );
	glUniform1i( glGetUniformLocation( g_sp_shadowMesh.GetID(), "tex_distribution0" ), 1 );
	glUniform1i( glGetUniformLocation( g_sp_shadowMesh.GetID(), "tex_distribution1" ), 2 );
	CheckGLError();
	// Draw the plane 
	{
		glUniformMatrix4fv( glGetUniformLocation( g_sp_shadowMesh.GetID(), "mat_model" ), 1, GL_FALSE, g_mat_plane.cell );
		glBindVertexArray( VAO_plane );
		glDrawArrays( GL_TRIANGLES, 0, 3 * g_planeMesh.NF() );
		glBindVertexArray( 0 );
	}
	// Draw the teapot
	{
		glUniformMatrix4fv( glGetUniformLocation( g_sp_shadowMesh.GetID(), "mat_model" ), 1, GL_FALSE, g_mat_teapot.cell );
		glBindVertexArray( VAO_teapot );
		glDrawArrays( GL_TRIANGLES, 0, 3 * g_objMesh_teapot.NF() );
		glBindVertexArray( 0 );
	}
	// Draw the sphere
	{
		glUniformMatrix4fv( glGetUniformLocation( g_sp_shadowMesh.GetID(), "mat_model" ), 1, GL_FALSE, g_mat_sphere.cell );
		glBindVertexArray( VAO_sphere );
		glDrawArrays( GL_TRIANGLES, 0, 3 * g_objMesh_sphere.NF() );
		glBindVertexArray( 0 );
	}
	// Draw the cube
	{
		glUniformMatrix4fv( glGetUniformLocation( g_sp_shadowMesh.GetID(), "mat_model" ), 1, GL_FALSE, g_mat_cube.cell );
		glBindVertexArray( VAO_cube );
		glDrawArrays( GL_TRIANGLES, 0, 3 * g_objMesh_cube.NF() );
		glBindVertexArray( 0 );
	}
	CheckGLError();
}

void GenerateShadowMap()
{
	glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
	g_sp_shadowPass.Bind();
	// Draw the teapot
	{
		glUniformMatrix4fv( glGetUniformLocation( g_sp_shadowPass.GetID(), "mat_model" ), 1, GL_FALSE, g_mat_teapot.cell );
		glBindVertexArray( VAO_teapot );
		glDrawArrays( GL_TRIANGLES, 0, 3 * g_objMesh_teapot.NF() );
		glBindVertexArray( 0 );
	}
	// Draw the sphere
	{
		glUniformMatrix4fv( glGetUniformLocation( g_sp_shadowPass.GetID(), "mat_model" ), 1, GL_FALSE, g_mat_sphere.cell );
		glBindVertexArray( VAO_sphere );
		glDrawArrays( GL_TRIANGLES, 0, 3 * g_objMesh_sphere.NF() );
		glBindVertexArray( 0 );
	}
	// Draw the cube
	{
		glUniformMatrix4fv( glGetUniformLocation( g_sp_shadowPass.GetID(), "mat_model" ), 1, GL_FALSE, g_mat_cube.cell );
		glBindVertexArray( VAO_cube );
		glDrawArrays( GL_TRIANGLES, 0, 3 * g_objMesh_cube.NF() );
		glBindVertexArray( 0 );
	}
	// Draw the plane 
	{
		glUniformMatrix4fv( glGetUniformLocation( g_sp_shadowPass.GetID(), "mat_model" ), 1, GL_FALSE, g_mat_plane.cell );
		glBindVertexArray( VAO_plane );
		glDrawArrays( GL_TRIANGLES, 0, 3 * g_planeMesh.NF() );
		glBindVertexArray( 0 );
	}
}

void Display()
{
	glViewport( 0, 0, g_width_screen, g_height_screen );
	// Render the scene to the shadow map from the light perspective
	{
		g_tex_renderDepth.Bind();
		glCullFace( GL_FRONT );
		GenerateShadowMap();
		glCullFace( GL_BACK );
		g_tex_renderDepth.Unbind();
	}
	CheckGLError();
	{
		RenderScene( true );
	}
	CheckGLError();
}

void UpdateCamera()
{
	auto mat_camera_rot = cyMatrix4f( 1.0f );
	mat_camera_rot.SetRotationXYZ( glm::radians( camera_angle_pitch ), glm::radians( camera_angle_yaw ), 0 );
	auto dir_camera = mat_camera_rot * cyVec4f( g_dir_targetToCamera, 0.0f );
	g_cameraPosInWorld = g_target_camera + cyVec3f( dir_camera ) * g_dis_cameraToTarget;

	auto mat_cameraView = GetLookAtMatrix( g_cameraPosInWorld, g_target_camera, cyVec3f( 0, 1.0, 0 ) );
	auto mat_perspective = cyMatrix4f( 1.0f );
	mat_perspective.SetPerspective( glm::radians( 60.0f ), g_width_screen / g_height_screen, 5.0f, 1000.0f );

	g_sp_shadowMesh.Bind();
	glUniformMatrix4fv( glGetUniformLocation( g_sp_shadowMesh.GetID(), "mat_view" ), 1, GL_FALSE, mat_cameraView.cell );
	glUniformMatrix4fv( glGetUniformLocation( g_sp_shadowMesh.GetID(), "mat_projection" ), 1, GL_FALSE, mat_perspective.cell );
	glUniform3fv( glGetUniformLocation( g_sp_shadowMesh.GetID(), "viewPos" ), 1, &g_cameraPosInWorld.elem[0] );

	g_sp_lightMesh.Bind();
	glUniformMatrix4fv( glGetUniformLocation( g_sp_lightMesh.GetID(), "mat_view" ), 1, GL_FALSE, mat_cameraView.cell );
	glUniformMatrix4fv( glGetUniformLocation( g_sp_lightMesh.GetID(), "mat_projection" ), 1, GL_FALSE, mat_perspective.cell );

	CheckGLError();
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
	float near_plane = 1.5f;
	float far_plane = 500.0f;
	mat_perspective.SetPerspective( glm::radians( 45.0f ), width_shadow / height_shadow, near_plane, far_plane );

	auto mat_lightSpace = mat_perspective * mat_lightSpace_view;

	g_sp_shadowMap.Bind();
	glUniform1f( glGetUniformLocation( g_sp_shadowMap.GetID(), "near_plane" ), near_plane );
	glUniform1f( glGetUniformLocation( g_sp_shadowMap.GetID(), "far_plane" ), far_plane );

	g_sp_shadowMesh.Bind();
	glUniformMatrix4fv( glGetUniformLocation( g_sp_shadowMesh.GetID(), "mat_lightSpaceTransformation" ), 1, GL_FALSE, mat_lightSpace.cell );
	glUniform1f( glGetUniformLocation( g_sp_shadowMesh.GetID(), "near_plane" ), near_plane );
	glUniform3fv( glGetUniformLocation( g_sp_shadowMesh.GetID(), "lightPos" ), 1, &g_lightPosInWorld.elem[0] );

	g_sp_shadowPass.Bind();
	glUniformMatrix4fv( glGetUniformLocation( g_sp_shadowPass.GetID(), "mat_lightSpaceTransformation" ), 1, GL_FALSE, mat_lightSpace.cell );

	g_sp_lightMesh.Bind();
	cyVec3f lightSource( 1.0f, 1.0f, 1.0f );
	glUniform3fv( glGetUniformLocation( g_sp_lightMesh.GetID(), "lightPos" ), 1, &lightSource.elem[0] );
}

void UpdateModels()
{
	g_mat_plane.SetTranslation( cyVec3f( 0, 0, 0 ) );
	g_mat_teapot.SetTranslation( cyVec3f( 0, 20, 0 ) );
	g_mat_sphere.SetTranslation( cyVec3f( 25, 40, 0 ) );
	g_mat_cube.SetTranslation( cyVec3f( -20, 10, 0 ) );

	g_sp_shadowMap.Bind();
	auto mat_quad_rot = cyMatrix4f( 1.0f );
	mat_quad_rot.SetRotationXYZ( glm::radians( -camera_angle_pitch ), glm::radians( -camera_angle_yaw ), 0 );
	glUniformMatrix4fv( glGetUniformLocation( g_sp_shadowMap.GetID(), "mat_rttRot" ), 1, GL_FALSE, mat_quad_rot.cell );
	glUniform1f( glGetUniformLocation( g_sp_shadowMap.GetID(), "dis" ), 1.0f );
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

#pragma region Callbacks
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
}

void MouseMoveCallback( GLFWwindow* i_pWindow, double i_x, double i_y )
{
	TwMouseMotion( int( i_x ), int( i_y ) );
}

void MouseButtonCallback( GLFWwindow* i_pWindow, int i_button, int i_action, int i_mods )
{
	assert( i_pWindow );
	if (TwEventMouseButtonGLFW( i_button, i_action ))
		return;

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

void CharCallback( GLFWwindow* i_pWindow, unsigned int i_key )
{
	TwEventCharGLFW( i_key, GLFW_PRESS );
}

void WindowSizeCallback( GLFWwindow* i_pWindow, int i_width, int i_height )
{
	g_width_screen = i_width;
	g_height_screen = i_height;
	TwWindowSize( i_width, i_height );
}

void TW_CALL SetNumblockerSearchSamplesCallbck( const void* i_pValue, void* i_pClientData )
{
	g_num_blockerSearchSamples = *static_cast<const size_t*>( i_pValue );
	g_num_blockerSearchSamples = glm::clamp<size_t>( g_num_blockerSearchSamples, MIN_NUM_SAMPLES, MAX_NUM_SAMPLES );
	CreatePoissonDiscDistribution( g_tex_possion_distribution1, g_num_blockerSearchSamples );
	g_sp_shadowMesh.Bind();
	glUniform1i( glGetUniformLocation( g_sp_shadowMesh.GetID(), "numOfSample_blockerSearch" ), g_num_blockerSearchSamples );

}

void TW_CALL GetNumBlockerSearchSamplesCallback( void* i_pValue, void* i_pClientData )
{
	*static_cast<size_t*>( i_pValue ) = g_num_blockerSearchSamples;
}

void TW_CALL SetNumPCFSamplesCallback( const void* i_pValue, void* i_pClientData )
{
	g_num_pcfFilteringSamples = *static_cast<const size_t*>( i_pValue );
	g_num_pcfFilteringSamples = glm::clamp<size_t>( g_num_pcfFilteringSamples, MIN_NUM_SAMPLES, MAX_NUM_SAMPLES );
	CreatePoissonDiscDistribution( g_tex_possion_distribution0, g_num_pcfFilteringSamples );
	g_sp_shadowMesh.Bind();
	glUniform1i( glGetUniformLocation( g_sp_shadowMesh.GetID(), "numOfSample_pcfFiltering" ), g_num_pcfFilteringSamples );
}

void TW_CALL GetNumPCFSamplesCallback( void* i_pValue, void* i_pClientData )
{
	*static_cast<size_t*>( i_pValue ) = g_num_pcfFilteringSamples;
}

void TW_CALL SetLightSizeCallback( const void* i_pValue, void* i_pClientData )
{
	g_light_size = *static_cast<const float*>( i_pValue );
	g_sp_shadowMesh.Bind();
	glUniform1f( glGetUniformLocation( g_sp_shadowMesh.GetID(), "lightSize" ), g_light_size );
}

void TW_CALL GetLigthtSizeCallback( void* i_pValue, void* i_pClientData )
{
	*static_cast<float*>( i_pValue ) = g_light_size;
}

#pragma endregion Callbacks

void InitializeTweakAntBar()
{
	TwInit( TW_OPENGL, NULL );
	auto newBar = TwNewBar( "Parameters" );
	TwDefine( " Parameters position='0 0' size='300 256' " );
	TwWindowSize( g_width_screen, g_height_screen );
	std::string definitionStr = "min=" + std::to_string( MIN_NUM_SAMPLES ) + " max=" + std::to_string( MAX_NUM_SAMPLES ) + " group=PCSS";
	TwAddVarCB( newBar, "Blocker Search Samples", TW_TYPE_INT32, SetNumblockerSearchSamplesCallbck, GetNumBlockerSearchSamplesCallback, 0, definitionStr.c_str() );
	TwAddVarCB( newBar, "PCF Samples", TW_TYPE_INT32, SetNumPCFSamplesCallback, GetNumPCFSamplesCallback, 0, definitionStr.c_str() );
	definitionStr = "step=0.2 group=Lights";
	TwAddVarCB( newBar, "Light Size", TW_TYPE_FLOAT, SetLightSizeCallback, GetLigthtSizeCallback, 0, definitionStr.c_str() );
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
	pWindow = glfwCreateWindow( g_width_screen, g_height_screen, "FP_PCSS", NULL, NULL );
	if (!pWindow)
	{
		glfwTerminate();
		return -1;
	}

	glfwWindowHint( GLFW_DEPTH_BITS, GLFW_TRUE );
	// Register the mouse and keyboard callback.
	glfwSetMouseButtonCallback( pWindow, MouseButtonCallback );
	glfwSetKeyCallback( pWindow, KeyboardCallback );
	glfwSetCharCallback( pWindow, CharCallback );
	glfwSetCursorPosCallback( pWindow, MouseMoveCallback );
	glfwSetWindowSizeCallback( pWindow, WindowSizeCallback );
	/* Make the window's context current */
	glfwMakeContextCurrent( pWindow );

	GLenum err = glewInit();
	if (GLEW_OK != err)
	{
		/* Problem: glewInit failed, something is seriously wrong. */
		fprintf( stderr, "Initialized GLEW failed, Error: %s\n", glewGetErrorString( err ) );
	}

	InitializeTweakAntBar();

	glEnable( GL_DEPTH_TEST );
	glEnable( GL_TEXTURE_2D );

	CompileShaders( path_vertexShader_mesh, path_fragmentShader_mesh, g_sp_lightMesh );
	CompileShaders( path_vertexShader_shadow, path_fragmentShader_shadow, g_sp_shadowMesh );
	CompileShaders( path_vertexShader_shadowPass, path_fragmentShader_shadowPass, g_sp_shadowPass );

	InitializeMesh( argv[1], g_objMesh_teapot, VAO_teapot, VBO_teapot );
	InitializeMesh( "plane.obj", g_planeMesh, VAO_plane, VBO_plane );
	InitializeMesh( "sphere.obj", g_objMesh_sphere, VAO_sphere, VBO_sphere );
	InitializeMesh( "cube.obj", g_objMesh_cube, VAO_cube, VBO_cube );
	InitializeMaterial( g_objMesh_teapot, g_sp_shadowMesh );

	CompileShaders( path_vertexShader_quad, path_fragmentShader_shadowMap, g_sp_shadowMap );
	InitializeDebugQuad( VAO_quad, VBO_quad, EBO_quad );
	InitializeDepthMap( g_tex_renderDepth );

	InitializeMesh( "light.obj", g_lightMesh, VAO_light, VBO_light );
	InitializeMaterial( g_lightMesh, g_sp_lightMesh );

	CreatePoissonDiscDistribution( g_tex_possion_distribution0, g_num_pcfFilteringSamples );
	CreatePoissonDiscDistribution( g_tex_possion_distribution1, g_num_blockerSearchSamples );
	CheckGLError();

	g_sp_shadowMesh.Bind();
	glUniform1i( glGetUniformLocation( g_sp_shadowMesh.GetID(), "numOfSample_pcfFiltering" ), g_num_pcfFilteringSamples );
	glUniform1i( glGetUniformLocation( g_sp_shadowMesh.GetID(), "numOfSample_blockerSearch" ), g_num_blockerSearchSamples );
	glUniform1f( glGetUniformLocation( g_sp_shadowMesh.GetID(), "bias_dirLightShadowMap" ), 0.001f );
	glUniform1f( glGetUniformLocation( g_sp_shadowMesh.GetID(), "lightSize" ), g_light_size );
	CheckGLError();

	InitializeView();
	CheckGLError();

	while (!glfwWindowShouldClose( pWindow ))
	{
		UpdateMouseInput( pWindow );

		UpdateCamera();
		UpdateLight();
		UpdateModels();

		Display();

		// Draw the tw bars.
		TwDraw();
		/* Swap front and back buffers */
		glfwSwapBuffers( pWindow );
		/* Poll for and process events */
		glfwPollEvents();
	}
	TwTerminate();
	// Release buffers and the shader program.
	{
		glDeleteVertexArrays( 1, &VAO_teapot );
		glDeleteBuffers( 1, &VBO_teapot );
		glDeleteVertexArrays( 1, &VAO_sphere );
		glDeleteBuffers( 1, &VBO_sphere );
		glDeleteVertexArrays( 1, &VAO_cube );
		glDeleteBuffers( 1, &VBO_cube );
		glDeleteVertexArrays( 1, &VAO_plane );
		glDeleteBuffers( 1, &VBO_plane );
		CheckGLError();

		g_tex_renderDepth.Delete();
		g_tex_possion_distribution0.Delete();
		g_tex_possion_distribution1.Delete();

		g_sp_shadowPass.Delete();
		g_sp_shadowMesh.Delete();
		g_sp_lightMesh.Delete();
		g_sp_shadowMap.Delete();

		glDeleteVertexArrays( 1, &VAO_quad );
		glDeleteBuffers( 1, &VBO_quad );
		glDeleteBuffers( 1, &EBO_quad );

		glDeleteVertexArrays( 1, &VAO_light );
		glDeleteBuffers( 1, &VBO_light );

		CheckGLError();
	}
	glfwTerminate();
	return 0;
}