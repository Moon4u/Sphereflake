
#ifndef HIT_RECORD_H
#define HIT_RECORD_H

////////////////////////////////////////////////////////////////////////////////

#include <limits>

#include "ray.h"
#include "simd.h"

////////////////////////////////////////////////////////////////////////////////

const Vec3	HASH_CONST	= Vec3( 3.5353123f, 4.1459123f, 1.3490423f );

////////////////////////////////////////////////////////////////////////////////

/**
 * @brief The HitRecord struct is used to store hit information when doing
 * ray tracing.
 * Min and Max are used to filter hits that are too far or too close.
 * Result stores the resulting hit. Default value -1.0f, means no hit detected.
 * SphereCenter is used to store the center of the sphere that was hit.
 * Radius is used to store the radius of the hit sphere.
 */
struct HitRecord
{
	static constexpr float	DEFAULT_MIN	= -0.99f;
	static constexpr float	DEFAULT_MAX	= std::numeric_limits< float >::max();

	HitRecord( float min = DEFAULT_MIN, float max = DEFAULT_MAX )
		: result( -1.f )
		, min( min )
		, max( max )
		, radius( 0.f )
		, level( -1.0f )
	{}

	/**
	 * @brief extractColor Calculate color from the hit record. Parameter index
	 * is to get one element in cases when SIMD code is used.
	 */
	Vec3	extractColor( const Ray& ray, uint32_t index )
	{
		const float&	recordResult	= result.Extract( index );
		const float&	levelResult		= level.Extract( index );
		const Vec3&		dir				= ray.direction().Extract( index );
		const Vec3&		origin			= ray.origin().Extract( index );

		if( HitRecord::DEFAULT_MIN > recordResult )
			return	Vec3( 0.178f, 0.461f, 0.853f );

		Vec3	col( sinf( levelResult + 0 ),
					 sinf( levelResult + 1 ),
					 sinf( levelResult + 2 ) );

		col	*= HASH_CONST;

		Vec3	point	= origin + dir * recordResult;
		float	div		= STARTING_RADIUS + point.y;

		return	col / div;
	}

	SIMD::Vec		sphereCenter;
	SIMD::float_t	result;
	SIMD::float_t	min;
	SIMD::float_t	max;
	SIMD::float_t	radius;
	SIMD::float_t	level;
};
////////////////////////////////////////////////////////////////////////////////

#endif // HIT_RECORD_H
