
#include <random>

#include "SDL2/SDL.h"
#include <GL/glew.h>

#include "simd_base.h"

#include "hitrecord.h"
#include "screenrenderer.h"

////////////////////////////////////////////////////////////////////////////////

constexpr char		FRAG_SHADER_FILE_PATH[]		= "bin/shaders/fragment.glsl";
constexpr char		VERT_SHADER_FILE_PATH[]		= "bin/shaders/vertex.glsl";

constexpr size_t	BUFFER_SIZE					= SCREEN_WIDTH * SCREEN_HEIGHT;

////////////////////////////////////////////////////////////////////////////////

/**
 * @brief ScreenRenderer::ScreenRenderer	Constructor for hte class
 */
ScreenRenderer::ScreenRenderer()
	: m_shouldQuit( false )
	, m_origin( 0., 0., 5. )
{
	m_buffer	= new Vec3[ BUFFER_SIZE ];
	m_program	= new GLProgram( VERT_SHADER_FILE_PATH, FRAG_SHADER_FILE_PATH );

	InitTexture();

	// leave one thread for the gl calls.
	auto	threadCount	= std::thread::hardware_concurrency() - 1;

	for( uint32_t i = 0; i < threadCount; ++i )
	{
		m_threads.push_back( std::make_shared< std::thread >( &ScreenRenderer::RenderBuffer, this ) );
	}
}
////////////////////////////////////////////////////////////////////////////////

/**
 * @brief ScreenRenderer::~ScreenRenderer	Destructor for the class.
 */
ScreenRenderer::~ScreenRenderer()
{
	m_shouldQuit	= true;
	for( auto& t : m_threads )
		t->join();

	delete[]	m_buffer;
}
////////////////////////////////////////////////////////////////////////////////

/**
 * @brief ScreenRenderer::Update	Update method. Called every frame.
 * @param cam	The camera position.
 */
void	ScreenRenderer::Update( const Vec3& camPos )
{
	m_origin	= camPos;
}
////////////////////////////////////////////////////////////////////////////////

/**
 * @brief ScreenRenderer::ClearScreen	Function that clears the screen.
 */
void	ScreenRenderer::ClearScreen()
{
	glViewport( 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT );
	glClearColor( 1.f, 1.f, 1.f, 0.f );
	glClear( GL_COLOR_BUFFER_BIT );
}
////////////////////////////////////////////////////////////////////////////////

/**
 * @brief ScreenRenderer::RenderFrame	Function that renders the frame.
 * Uploads the new screen data from m_buffer to a texture and then displays the
 * said texture.
 */
void	ScreenRenderer::RenderFrame()
{
	glBindTexture( GL_TEXTURE_2D, m_textureId );
	glTexImage2D( GL_TEXTURE_2D, 0, GL_RGB, SCREEN_WIDTH, SCREEN_HEIGHT, 0,
				  GL_RGB, GL_FLOAT, m_buffer );

	glBindVertexArray( m_VAO );
	glBindTexture( GL_TEXTURE_2D, m_textureId );
	glDrawElements( GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr );
}
////////////////////////////////////////////////////////////////////////////////

/**
 * @brief ScreenRenderer::InitTexture	Function that initiliazies the buffer,
 * the texture, the VAO, EBO, VBO, etc. Everything neccessary to render a
 * texture.
 */
void	ScreenRenderer::InitTexture()
{
	float	vertices[]	=
	{
		 1.f,  1.f,  // top right
		 1.f, -1.f,  // bottom right
		-1.f, -1.f,  // bottom left
		-1.f,  1.f,  // top left

		1.0f, 1.0f,  // top right
		1.0f, 0.0f,  // bottom right
		0.0f, 0.0f,  // bottom left
		0.0f, 1.0f,  // top left
	};

	uint32_t	indices[]	=
	{
		0, 1, 3,  // first Triangle
		1, 2, 3   // second Triangle
	};

	glGenVertexArrays( 1, &m_VAO );
	glGenBuffers( 1, &m_VBO );
	glGenBuffers( 1, &m_EBO );
	glBindVertexArray( m_VAO );

	glBindBuffer( GL_ARRAY_BUFFER, m_VBO );
	glBufferData( GL_ARRAY_BUFFER, sizeof( vertices ), vertices, GL_STATIC_DRAW );

	glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, m_EBO );
	glBufferData( GL_ELEMENT_ARRAY_BUFFER, sizeof( indices ), indices, GL_STATIC_DRAW );

	glVertexAttribPointer( 0, 2, GL_FLOAT, GL_FALSE, 0, nullptr );
	glEnableVertexAttribArray( 0 );

	glVertexAttribPointer( 1, 2, GL_FLOAT, GL_FALSE, 0,
						   reinterpret_cast< void* >( 8 * sizeof( float ) ) );
	glEnableVertexAttribArray( 1 );

	glGenTextures( 1, &m_textureId );
	glBindTexture( GL_TEXTURE_2D, m_textureId );

	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST );

	glActiveTexture( GL_TEXTURE0 );

	for( uint32_t i = 0; i < BUFFER_SIZE; ++i )
	{
		m_buffer[ i ]	= Vec3( 0.178f, 0.461f, 0.853f );
	}
}
////////////////////////////////////////////////////////////////////////////////

/**
 * @brief ScreenRenderer::RenderBuffer	This function does the ray tracing.
 * It is called from multiple threads. And will continue execute, while the
 * program is running.
 */
using	dist	= std::uniform_int_distribution< uint32_t >;
void	ScreenRenderer::RenderBuffer()
{
	auto			s	= std::chrono::system_clock::now().time_since_epoch();

	std::mt19937	mt;
	mt.seed( static_cast< uint32_t >( s.count() ) );

	dist	rndWidth( 0, SCREEN_WIDTH / SIMD::SIZE - 1 );
	dist	rndHeight( 0, SCREEN_HEIGHT - 1 );

	for(;;)
	{
		HitRecord	records;
		uint32_t	x		= rndWidth( mt ) * SIMD::SIZE;
		uint32_t	y		= rndHeight( mt );
		uint32_t	index	= y * SCREEN_WIDTH + x;
		Ray			ray		= Ray::castRays( m_origin, x, y );

		m_sphereFlake.Intersect( ray, records );

		for( uint32_t k = 0; k < SIMD::SIZE; ++k )
			m_buffer[ index + k ]	= records.extractColor( ray, k );

		if( m_shouldQuit )
			return;
	}
}
////////////////////////////////////////////////////////////////////////////////
