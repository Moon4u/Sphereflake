
#ifndef WINDOW_H
#define WINDOW_H

////////////////////////////////////////////////////////////////////////////////

#include <stdint.h>

#include "simd_base.h"

////////////////////////////////////////////////////////////////////////////////

typedef void*	SDL_GLContext;
struct			SDL_Window;
class			ScreenRenderer;
class			GLProgram;

////////////////////////////////////////////////////////////////////////////////

/**
 * @brief The Window class is used to initialize SDL2 and to open a window.
 */
class Window
{
public:
	Window( const char* const appName );
	~Window();

	void	run();

private:
	void	HandleEvents();

private:
	bool			m_shouldQuit;
	Vec3			m_cameraPos;

	SDL_Window*		m_window;
	SDL_GLContext	m_context;
	ScreenRenderer*	m_screenRenderer;
};
////////////////////////////////////////////////////////////////////////////////

#endif // WINDOW_H
