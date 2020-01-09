
#ifndef GLPROGRAM_H
#define GLPROGRAM_H

////////////////////////////////////////////////////////////////////////////////

#include <stdint.h>
#include <string>

////////////////////////////////////////////////////////////////////////////////

/**
 * @brief The GLProgram class is used to compile opengl shaders and a program.
 */
class GLProgram
{
public:
	GLProgram( const char* const vertPath, const char* const fragPath );
	~GLProgram();

	void	Use();

private:
	void		CompileShaders();

	template< int SHADER_TYPE >
	uint32_t		CompileShader( const char* const path );
	std::string		ReadFile( const char* const path );
	uint32_t		CompileProgram( uint32_t vertProgramId, uint32_t fragProgramId );

private:
	const char* const	m_vertPath;
	const char* const	m_fragPath;

	uint32_t			m_programId;
};
////////////////////////////////////////////////////////////////////////////////

#endif // OPENGLPROGRAM_H
