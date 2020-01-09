
#include <SDL2/SDL.h>
#include <GL/glew.h>

#include "config.h"
#include "GLError.h"
#include "glprogram.h"
#include "screenrenderer.h"

#include "window.h"

////////////////////////////////////////////////////////////////////////////////

static constexpr float	VELOCITY	= 0.1f;

////////////////////////////////////////////////////////////////////////////////

/**
 * @brief Window::Window	Constructor for the Window class.
 * @param appName	The name the APP that is going to be displayed.
 */
Window::Window( const char* const appName )
	: m_shouldQuit( false )
	, m_cameraPos( 0., 0., 5.f )
{
	auto	result	= SDL_Init( SDL_INIT_VIDEO );
	if( result )
		return;

	m_window	= SDL_CreateWindow( appName, SDL_WINDOWPOS_CENTERED,
									SDL_WINDOWPOS_CENTERED,
									SCREEN_WIDTH, SCREEN_HEIGHT,
									SDL_WINDOW_OPENGL );

	if( nullptr == m_window )
		return;

	SDL_GL_SetAttribute( SDL_GL_CONTEXT_FLAGS, SDL_GL_CONTEXT_DEBUG_FLAG );
	SDL_GL_SetAttribute( SDL_GL_CONTEXT_MAJOR_VERSION, 3 );
	SDL_GL_SetAttribute( SDL_GL_CONTEXT_MINOR_VERSION, 2 );
	SDL_GL_SetAttribute( SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE );

	m_context	= SDL_GL_CreateContext( m_window );
	if( nullptr == m_context )
		return;

	GLenum	glewError	= glewInit();
	if( GLEW_OK != glewError )
		return;

	glEnable( GL_DEBUG_OUTPUT );
	glDebugMessageCallback( GL::MessageCallback, nullptr );
	m_screenRenderer	= new ScreenRenderer;
}
////////////////////////////////////////////////////////////////////////////////

/**
 * @brief Window::~Window	Destructor for the class.
 */
Window::~Window()
{
	SDL_GL_DeleteContext( m_context );
	SDL_DestroyWindow( m_window );
	SDL_Quit();
}
////////////////////////////////////////////////////////////////////////////////

/**
 * @brief Window::run	The main loop for the program.
 */
void	Window::run()
{
	constexpr uint32_t	delay	= 1000 / FPS;

	do
	{
		m_screenRenderer->Update( m_cameraPos );

		m_screenRenderer->ClearScreen();
		m_screenRenderer->RenderFrame();

		SDL_GL_SwapWindow( m_window );

		HandleEvents();
		SDL_Delay( delay );
	} while( ! m_shouldQuit );
}
////////////////////////////////////////////////////////////////////////////////

/**
 * @brief Window::HandleEvents	This function will handle OS events.
 */
void	Window::HandleEvents()
{
	SDL_Event	event;
	while( SDL_PollEvent( &event ) )
	{
		switch( event.type )
		{
			case	SDL_KEYDOWN:
			{

				switch( event.key.keysym.scancode )
				{
					case	SDL_SCANCODE_ESCAPE:
						m_shouldQuit	= true;
						break;

					case	SDL_SCANCODE_A:
						m_cameraPos.x	-= VELOCITY;
						break;

					case	SDL_SCANCODE_Q:
						m_cameraPos.y	+= VELOCITY;
						break;

					case	SDL_SCANCODE_E:
						m_cameraPos.y	-= VELOCITY;
						break;

					case	SDL_SCANCODE_D:
						m_cameraPos.x	+= VELOCITY;
						break;

					case	SDL_SCANCODE_W:
						m_cameraPos.z	-= VELOCITY;
						break;

					case	SDL_SCANCODE_S:
						m_cameraPos.z	+= VELOCITY;
						break;

					case	SDL_SCANCODE_R:
						if( event.key.keysym.mod & KMOD_CTRL )
						{
							m_cameraPos.x	= 0.f;
							m_cameraPos.y	= 0.f;
							m_cameraPos.z	= 5.f;
						}

						break;

					default:
						break;

				}

				break;
			}

			case	SDL_QUIT:
			{
				m_shouldQuit	= true;
				break;
			}
		}
	}
}
////////////////////////////////////////////////////////////////////////////////
