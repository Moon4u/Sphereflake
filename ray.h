
#ifndef RAY_H
#define RAY_H

////////////////////////////////////////////////////////////////////////////////

#include "config.h"

#include "simd.h"
#include "vec3.h"

////////////////////////////////////////////////////////////////////////////////

/**
 * @brief The Ray class	This class describes an ray. Takes ro as ray origin
 * and rd as ray direction. Both parameters will use SIMD if they are enabled.
 */
class	Ray
{
public:
	Ray( SIMD::Vec ro, SIMD::Vec rd )
		: m_ro( ro )
		, m_rd( rd )
	{}

	const SIMD::Vec&	origin()			const { return	m_ro; }
	const SIMD::Vec&	direction()			const { return	m_rd; }

	/**
	 * @brief castRays	Constructs rays for each SIMD instruction.
	 */
	static Ray	castRays( Vec3 ro, uint32_t x, uint32_t y )
	{
		float	u;
		float	v;
		Vec3	dir[ SIMD::SIZE ];

		for( uint32_t k = 0; k < SIMD::SIZE; ++k )
		{
			u		= float( x + k ) / float( SCREEN_WIDTH  );
			v		= float( y ) / float( SCREEN_HEIGHT );
			v		*= SCREEN_RATIO;
			u		= ( u - 0.5f ) * 2.0f;
			v		= ( v - 0.5f ) * 2.0f;

			dir[ k ]	= ( Vec3( u, v, -1.f ) ).Normalized();
		}

		return	Ray( ro, dir );
	}


private:
	SIMD::Vec	m_ro;
	SIMD::Vec	m_rd;
};
////////////////////////////////////////////////////////////////////////////////

#endif // RAY_H
