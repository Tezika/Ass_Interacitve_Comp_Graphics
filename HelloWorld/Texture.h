#pragma once
#include <vector>
#include <GL/glew.h>
#include <GL/GL.h>

namespace Ass_Inter_Comp_Graphics
{
	class Texture
	{
	public:
		static Texture* Create( const char* i_pathForTex );
		inline GLuint GetTexId() const { return m_texId; };
		void Bind( GLenum i_texUnit, GLenum i_texType );
		~Texture();
	private:
		Texture() = default;
		std::vector<unsigned char> m_data;
		unsigned int m_width;
		unsigned int m_height;
		GLuint  m_texId;
	};
}