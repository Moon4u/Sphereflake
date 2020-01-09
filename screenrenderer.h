
#ifndef SCREENRENDERER_H
#define SCREENRENDERER_H

////////////////////////////////////////////////////////////////////////////////

#include <thread>
#include <vector>

#include <stdint.h>

#include "vec3.h"
#include "sphereflake.h"
#include "glprogram.h"

////////////////////////////////////////////////////////////////////////////////

/**
 * @brief The ScreenRenderer class is used to render the SphereFlake to the
 * screen.
 */
class ScreenRenderer
{
public:
	ScreenRenderer();
	~ScreenRenderer();

	void	Update( const Vec3& camPos );
	void	ClearScreen();
	void	RenderFrame();

private:
	void	InitTexture();
	void	RenderBuffer();

private:
	bool			m_shouldQuit;

	Vec3			m_origin;
	uint32_t		m_textureId;
	uint32_t		m_VAO;
	uint32_t		m_VBO;
	uint32_t		m_EBO;

	Vec3*			m_buffer;
	GLProgram*		m_program;
	SphereFlake		m_sphereFlake;

	std::vector< std::shared_ptr< std::thread > >	m_threads;
};
////////////////////////////////////////////////////////////////////////////////

#endif // SCREENRENDERER_H
