
#ifndef GLERROR_H
#define GLERROR_H

////////////////////////////////////////////////////////////////////////////////

#include <string>

#include "GL/glew.h"

////////////////////////////////////////////////////////////////////////////////

/**
 * @brief These functions are used for opengl error reporting.
 *
 * \note Used while developing the code only.
 */

#define ErrCh CheckError_(__FILE__, __LINE__)

////////////////////////////////////////////////////////////////////////////////

namespace GL
{
	const char* const	GL_CALLBACK_ERROR_FORMAT	= "GL CALLBACK(%d): %s, severity = %s, source = %s, message = %s\n";
	const char* const	GL_CHECK_ERROR_FORMAT		= "Type: %s, file: %s, line: %d\n";

static void GLAPIENTRY
MessageCallback( GLenum source,
				 GLenum type,
				 GLuint id,
				 GLenum severity,
				 GLsizei length,
				 const GLchar* message,
				 const void* userParam )
{
	( void )( userParam );
	( void )( length );

	std::string	errorType;
	std::string	severityType;
	std::string	sourceType;

	switch( type )
	{
		case	GL_DEBUG_TYPE_ERROR:				errorType	= "** ERROR ** "; break;
		case	GL_DEBUG_TYPE_OTHER:				errorType	= "OTHER"; break;
		case	GL_DEBUG_TYPE_MARKER:				errorType	= "MARKER"; break;
		case	GL_DEBUG_TYPE_POP_GROUP:			errorType	= "POP_GROUP"; break;
		case	GL_DEBUG_TYPE_PUSH_GROUP:			errorType	= "PUSH_GROUP"; break;
		case	GL_DEBUG_TYPE_PERFORMANCE:			errorType	= "PERFORMANCE"; break;
		case	GL_DEBUG_TYPE_PORTABILITY:			errorType	= "PORTABILITY"; break;
		case	GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR:	errorType	= "UNDEFINED_BEHAVIOR"; break;
		case	GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR:	errorType	= "DEPRECATED_BEHAVIOR"; break;
		default:									errorType	= "Unknown";break;
	}

	switch( severity )
	{
		case	GL_DEBUG_SEVERITY_LOW:			severityType	= "LOW"; break;
		case	GL_DEBUG_SEVERITY_HIGH:			severityType	= "HIGH"; break;
		case	GL_DEBUG_SEVERITY_MEDIUM:		severityType	= "MEDIUM"; break;
		case	GL_DEBUG_SEVERITY_NOTIFICATION:	return; // ignore notifications
		default:								severityType	= "Unknown"; break;
	}

	switch( source )
	{
		case	GL_DEBUG_SOURCE_API:				sourceType	= "API"; break;
		case	GL_DEBUG_SOURCE_WINDOW_SYSTEM:		sourceType	= "Window System"; break;
		case	GL_DEBUG_SOURCE_SHADER_COMPILER:	sourceType	= "Shader Compiler"; break;
		case	GL_DEBUG_SOURCE_THIRD_PARTY:		sourceType	= "Third Party"; break;
		case	GL_DEBUG_SOURCE_APPLICATION:		sourceType	= "Application"; break;
		case	GL_DEBUG_SOURCE_OTHER:				sourceType	= "Other"; break;
		default:									sourceType	= "Unknown"; break;
	}

	fprintf( stderr, GL_CALLBACK_ERROR_FORMAT, id, errorType.c_str(),
			 severityType.c_str(), sourceType.c_str(), message );
}
////////////////////////////////////////////////////////////////////////////////

static
void	CheckError_( const char *file, int line )
{
	GLenum	errorCode;
	while( ( errorCode = glGetError() ) != GL_NO_ERROR )
	{
		std::string		error;
		switch( errorCode )
		{
			case	GL_INVALID_ENUM:					error	= "INVALID_ENUM"; break;
			case	GL_INVALID_VALUE:					error	= "INVALID_VALUE"; break;
			case	GL_INVALID_OPERATION:				error	= "INVALID_OPERATION"; break;
			case	GL_STACK_OVERFLOW:					error	= "STACK_OVERFLOW"; break;
			case	GL_STACK_UNDERFLOW:					error	= "STACK_UNDERFLOW"; break;
			case	GL_OUT_OF_MEMORY:					error	= "OUT_OF_MEMORY"; break;
			case	GL_INVALID_FRAMEBUFFER_OPERATION:	error	= "INVALID_FRAMEBUFFER_OPERATION"; break;
			default:									error	= "UNKNOWN"; break;
		}

		fprintf( stderr, GL_CHECK_ERROR_FORMAT, error.c_str(), file, line );
	}
}
////////////////////////////////////////////////////////////////////////////////

} // namespace GL
////////////////////////////////////////////////////////////////////////////////

#endif // GLERROR_H
