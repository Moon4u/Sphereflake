
#include <cassert>
#include <fstream>

#include <SDL2/SDL.h>
#include "GL/glew.h"

#include "config.h"
#include "GLError.h"

#include "glprogram.h"

////////////////////////////////////////////////////////////////////////////////

constexpr int	INFO_LOG_SIZE						= 512;

constexpr char	SHADER_COMPILATION_FAILED_MSG[]		= "Failed to compile shader %d. Log:\n%s";
constexpr char	PROGRAM_COMPILATION_FAILED_MSG[]	= "Failed to compile the program. Log:\n%s";

////////////////////////////////////////////////////////////////////////////////

/**
 * @brief GLProgram::GLProgram Constructor for the class
 * @param vertPath	Path to the vertex shader.
 * @param fragPath	Path to the fragment shader.
 */
GLProgram::GLProgram( const char* const vertPath, const char* const fragPath )
	: m_vertPath( vertPath )
	, m_fragPath( fragPath )
	, m_programId( 0 )
{
	CompileShaders();
}
////////////////////////////////////////////////////////////////////////////////

/**
 * @brief GLProgram::~GLProgram	Destructor for the clas.
 */
GLProgram::~GLProgram()
{
	glDeleteProgram( m_programId );
}
////////////////////////////////////////////////////////////////////////////////

/**
 * @brief GLProgram::Use	Binds the program.
 */
void	GLProgram::Use()
{
	glUseProgram( m_programId );
}
////////////////////////////////////////////////////////////////////////////////

/**
 * @brief GLProgram::CompileShaders This function compiles shaders and links
 * them in a program.
 */
void	GLProgram::CompileShaders()
{
	uint32_t	vertId	= CompileShader< GL_VERTEX_SHADER >( m_vertPath );
	uint32_t	fragId	= CompileShader< GL_FRAGMENT_SHADER >( m_fragPath );

	m_programId	= CompileProgram( vertId, fragId );

	glDeleteShader( vertId );
	glDeleteShader( fragId );

	glUseProgram( m_programId );
}
////////////////////////////////////////////////////////////////////////////////

/**
 * @brief GLProgram::CompileShader This function compiles a shader and returns
 * the id of the shader.
 * Takes path to the file and shader type (as a template parameter).
 */
template< int SHADER_TYPE >
uint32_t	GLProgram::CompileShader( const char* const path )
{
	auto	code	= ReadFile( path );
	if( code.empty() )
		return	0;

	const char*	cstrCode	= code.c_str();

	int32_t		success;
	char		info_log[ INFO_LOG_SIZE ];
	uint32_t	shaderId	= glCreateShader( SHADER_TYPE );

	glShaderSource( shaderId, 1, &cstrCode, nullptr );

	glCompileShader( shaderId );
	glGetShaderiv( shaderId, GL_COMPILE_STATUS, &success );
	GL::ErrCh;
	if( GL_TRUE != success )
	{
		glGetShaderInfoLog( shaderId, INFO_LOG_SIZE, nullptr, info_log );
		SDL_LogError( SDL_LOG_CATEGORY_RENDER, SHADER_COMPILATION_FAILED_MSG,
					  SHADER_TYPE, info_log );
		SDL_Log( SHADER_COMPILATION_FAILED_MSG, SHADER_TYPE, info_log );

		return	0;
	}

	return	shaderId;
}
////////////////////////////////////////////////////////////////////////////////

/**
 * @brief GLProgram::ReadFile	Function that reads from a file.
 * @param path	Path to the file.
 * @return	Returns the contents of the file as std::string.
 */
std::string	GLProgram::ReadFile( const char* const path )
{
	std::ifstream	fileStream;
	fileStream.open( path );
	assert( fileStream.is_open() );

	return	std::string( ( std::istreambuf_iterator< char >( fileStream ) ),
						   std::istreambuf_iterator< char >() );
}
////////////////////////////////////////////////////////////////////////////////

/**
 * @brief GLProgram::CompileProgram	Function that links two shaders to a
 * program.
 * @param vertId	Id of the vertex shader.
 * @param fragId	Id of the fragment shader.
 * @return	Returns the id of the program.
 */
uint32_t	GLProgram::CompileProgram( uint32_t vertId, uint32_t fragId )
{
	int		success;
	char	info_log[ INFO_LOG_SIZE ];
	uint32_t	programId	= glCreateProgram();
	glAttachShader( programId, vertId );
	glAttachShader( programId, fragId );
	glLinkProgram( programId );
	glGetProgramiv( programId, GL_LINK_STATUS, &success );
	if( GL_TRUE != success )
	{
		glGetProgramInfoLog( programId, INFO_LOG_SIZE, nullptr, info_log ); GL::ErrCh;
		SDL_LogError( SDL_LOG_CATEGORY_RENDER, PROGRAM_COMPILATION_FAILED_MSG,
					  info_log );
		SDL_Log( PROGRAM_COMPILATION_FAILED_MSG, info_log );

		return	0;
	}

	return	programId;
}
////////////////////////////////////////////////////////////////////////////////
