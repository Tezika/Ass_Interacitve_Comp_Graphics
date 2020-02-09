#include "Texture.h"
#include "assert.h"
#include "lodepng.h"

namespace
{
	void LoadTextureData( const char* i_texturePath, std::vector<unsigned char>& io_data, unsigned int& io_width, unsigned int& io_height )
	{
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

Ass_Inter_Comp_Graphics::Texture* Ass_Inter_Comp_Graphics::Texture::Create( const char* i_pathForTex )
{
	auto* newTex = new Ass_Inter_Comp_Graphics::Texture();
	static GLuint s_texId = 0;
	newTex->m_texId = s_texId;
	s_texId++;
	glGenTextures( 1, &newTex->m_texId );
	glBindTexture( GL_TEXTURE_2D, newTex->m_texId );

	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );

	LoadTextureData( i_pathForTex, newTex->m_data, newTex->m_width, newTex->m_height );
	if (newTex->m_data.data())
	{
		glTexImage2D( GL_TEXTURE_2D, 0, GL_RGB, newTex->m_width, newTex->m_height, 0, GL_RGB, GL_UNSIGNED_BYTE, reinterpret_cast<void*>(newTex->m_data.data()) );
		glGenerateMipmap( GL_TEXTURE_2D );
	}
	return newTex;
}

Ass_Inter_Comp_Graphics::Texture::~Texture()
{
	glDeleteTextures( 1, &m_texId );
	assert( glGetError() == GL_NO_ERROR );
}

void Ass_Inter_Comp_Graphics::Texture::Bind( GLenum i_texUnit, GLenum i_texType )
{
	glActiveTexture( i_texUnit );
	glBindTexture( i_texType, m_texId );
	assert( glGetError() == GL_NO_ERROR );
}