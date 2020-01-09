
#ifndef SPHEREFLAKE_H
#define SPHEREFLAKE_H

////////////////////////////////////////////////////////////////////////////////

#include <stdint.h>

#include "simd.h"

#include "hitrecord.h"
#include "config.h"
#include "ray.h"
#include "vec3.h"

////////////////////////////////////////////////////////////////////////////////

constexpr float	SIN_HALF_FOV		= 0.4794255386f;
constexpr float	PIXEL_AT_DISTANCE	= 2.0f * SIN_HALF_FOV * SCREEN_HEIGHT;

////////////////////////////////////////////////////////////////////////////////

/**
 * @brief GetMaxDepth	Returns the maximum depth at which the square of the
 * radius will become 0. For starting radius of 1.0f this works out as ~46.
 */
constexpr uint8_t	GetMaxDepth()
{
	float	radius		= STARTING_RADIUS;
	uint8_t	maxDepth	= 1;
	while( radius * radius > 0.0f )
	{
		radius	/= 3.0f;
		maxDepth++;
	}

	return	maxDepth;
}
////////////////////////////////////////////////////////////////////////////////

/**
 * @brief A struct that contantains the radius for each depth.
 */
template< int depth >
struct rad
{
	constexpr static float	r	= rad< depth - 1 >::r * SPHERE_RATIO;
};

template<>
struct rad< 0 >
{
	constexpr static float	r	= STARTING_RADIUS;
};
////////////////////////////////////////////////////////////////////////////////

/**
 * @brief The Sphere struct A struct containing the center of the sphere and
 * the direction.
 */
struct Sphere
{
	Vec3	center;
	Vec3	direction;
};
////////////////////////////////////////////////////////////////////////////////

constexpr float	angleToRads( float rad )
{
	// Some compilers don't provide the pi constant.
	constexpr float	PI	= 3.14159265359f;

	return	rad * PI / 180.0f;
}
////////////////////////////////////////////////////////////////////////////////

/**
 * @brief The SphereFlake class is used to implement the ray intersecting with
 * the sphereflake.
 */
class SphereFlake
{
public:
	SphereFlake()
	{
		float	angle1	= angleToRads( 360.0f / float( TYPE1_SPHERES_COUNT ) );
		float	angle2	= angleToRads( 360.0f / float( TYPE2_SPHERES_COUNT ) );

		float	t1Rads	= angleToRads( TYPE1_SPHERES_ROTATION );
		float	t2Rads	= angleToRads( TYPE2_SPHERES_ROTATION );
		for( int k = 0; k < TYPE1_SPHERES_COUNT; ++k )
		{
			m_rotateSin[				   0 + k ]	= sinf( t1Rads + angle1 * k );
			m_rotateCos[				   0 + k ]	= cosf( t1Rads + angle1 * k );
		}

		for( int k = 0; k < TYPE2_SPHERES_COUNT; ++k )
		{
			m_rotateSin[ TYPE1_SPHERES_COUNT + k ]	= sinf( t2Rads + angle2 * k );
			m_rotateCos[ TYPE1_SPHERES_COUNT + k ]	= cosf( t2Rads + angle2 * k );
		}

		float	t1sin	= sinf( angleToRads( TYPE1_SPHERES_DEGREE ) );
		float	t1cos	= cosf( angleToRads( TYPE1_SPHERES_DEGREE ) );
		float	t2sin	= sinf( angleToRads( TYPE2_SPHERES_DEGREE ) );
		float	t2cos	= cosf( angleToRads( TYPE2_SPHERES_DEGREE ) );
		for( int k = 0; k < TYPE1_SPHERES_COUNT; ++k )
		{
			m_yAxisRotSines[				   0 + k ]		= t1sin;
			m_yAxisRotCosines[				   0 + k ]		= t1cos;
		}

		for( int k = 0; k < TYPE2_SPHERES_COUNT; ++k )
		{
			m_yAxisRotSines[ TYPE1_SPHERES_COUNT   + k ]	= t2sin;
			m_yAxisRotCosines[ TYPE1_SPHERES_COUNT + k ]	= t2cos;
		}
	}

	void	Intersect( const Ray& ray, HitRecord& records );

private:
	template< uint8_t DEPTH = 0 >
	void	IntersectRecurs( const Ray& ray,
							 const Sphere& current,
							 const Vec3& parDir,
							 HitRecord& records );

	template< uint8_t DEPTH, bool testOnly >
	SIMD::bool_t	SphereIntersect( const Ray& ray,
									  const SIMD::Vec& sphereCenter,
									  HitRecord& hit );

private:
	float				m_rotateSin[ TOTAL_NUMBER_OF_SPHERES ];
	float				m_rotateCos[ TOTAL_NUMBER_OF_SPHERES ];

	float				m_yAxisRotSines[ TOTAL_NUMBER_OF_SPHERES ];
	float				m_yAxisRotCosines[ TOTAL_NUMBER_OF_SPHERES ];
};
////////////////////////////////////////////////////////////////////////////////

/**
 * @brief SphereFlake::Intersect	This functions checks if a 'ray' is
 * intersecting the sphereflake and stores the result in 'records.'
 */
inline
void	SphereFlake::Intersect( const Ray& ray, HitRecord& records )
{
	Sphere	current;
	Vec3	parDir;

	current.center		= Vec3( 0.0f, 0.0f, 0.0f );
	current.direction	= Vec3( 0.0f, 1.0f, 0.0f );
	parDir				= Vec3( 0.0f, 0.0f, 1.0f );

	IntersectRecurs( ray, current, parDir, records );
}
////////////////////////////////////////////////////////////////////////////////

/**
 * @brief This is a function that checks for intersection of a sphere from the
 * sphereflake. The function is called recursively and checks each child sphere
 * for intersection.
 *
 * @note This function is using DEPTH, as template parameter which means that
 * the code will generate new function for each depth. This still worksout
 * faster than having single function.
 */
template< uint8_t DEPTH >
inline
void	SphereFlake::IntersectRecurs( const Ray& ray, const Sphere& current,
									  const Vec3& parDir, HitRecord& records )
{
	constexpr float	currentRadius	= rad< DEPTH >::r;
	constexpr float	childRadius		= rad< DEPTH + 1 >::r;

	if( ! SphereIntersect< DEPTH, true >( ray, current.center, records ) )
		return;

	SphereIntersect< DEPTH, false >( ray, current.center, records );

	const Vec3&	curDir	= current.direction;
	const Vec3&	curCntr	= current.center;

	constexpr float	radDist	= currentRadius + childRadius;
	for( int i = 0; i < TOTAL_NUMBER_OF_SPHERES; ++i )
	{
		const Vec3	perp1	= curDir.cross( parDir ).Normalized();
		const Vec3	perp2	= curDir.cross( perp1  ).Normalized();

		const Vec3	rot		= m_yAxisRotSines[ i ] * m_rotateCos[ i ] * perp1
							+ m_yAxisRotSines[ i ] * m_rotateSin[ i ] * perp2;

		const Vec3	newDir	= rot + curDir * m_yAxisRotCosines[ i ];

		Sphere		child;
		child.direction		= newDir.Normalized();
		child.center		= newDir * radDist + curCntr;

		const Vec3&	ro		= ray.origin().Extract( 0 );

		// Discard spheres that have radius smaller than 1 pixel.
		const float	dist	= ( ro - child.center ).len();
		const float	result	= PIXEL_AT_DISTANCE * childRadius / dist;
		if( result < 1.0f || dist < childRadius )
			continue;

		IntersectRecurs< DEPTH + 1 >( ray, child, current.direction, records );
	}
}
////////////////////////////////////////////////////////////////////////////////

/**
 * @brief This is an empty function to end the recursive generation of the
 * function IntersectRecurs. This saves a little bit compilation time.
 *
 * The last function is at depth calculated by GetMaxDepth().
 */
template<>
inline
void	SphereFlake::IntersectRecurs< GetMaxDepth() >( const Ray&,
													   const Sphere&,
													   const Vec3&,
													   HitRecord& )
{
}
////////////////////////////////////////////////////////////////////////////////

/**
 * @brief This function checks for intersection with a single sphere.
 * The radius is generated from the depth template parameter.
 * The testOnly argument is used when checking if the ray hits the sphere or
 * any of it's children.
 *
 * @note The function uses SIMD (if the code is compiled with them).
 */
template< uint8_t DEPTH, bool testOnly >
inline
SIMD::bool_t	SphereFlake::SphereIntersect( const Ray& ray,
											  const SIMD::Vec& sphereCenter,
											  HitRecord& hit )
{
	constexpr float	ra		= rad< DEPTH >::r;
	SIMD::float_t	radiusSqr( ra * ra * ( ( testOnly ) ? 4.0f : 1.0f ) );

	SIMD::Vec		deltap		= sphereCenter - ray.origin();

	SIMD::float_t	ddp			= ray.direction().dot( deltap );

	SIMD::Vec		remedyTerm	= deltap - ray.direction().MultiplyByFloat( ddp );
	SIMD::float_t	discrim		= radiusSqr- remedyTerm.dot( remedyTerm );

	SIMD::bool_t	compareRes	= discrim.GreaterOrEqualThan( 0.0f );

	if( testOnly )
		return	compareRes;

	if( ! compareRes )
		return	SIMD::FALSE_VALUE;

	// William H., Saul A. Teukolsky, William T. Vetterling, and Brian P.
	// Flannery, "Numerical Recipes in C," Cambridge University Press, 1992.
	SIMD::float_t	sqrtVal		= sqrtf( discrim );

	SIMD::bool_t	ddpCmpGE	= ddp.GreaterOrEqualThan( 0.0f );
	auto			result		= PickBasedOnCondition( ddpCmpGE, ddp + sqrtVal, ddp - sqrtVal );

	SIMD::bool_t	cmpRange	= result.IsInRange( hit.min, hit.max );

	hit.max				= PickBasedOnCondition( cmpRange, result, hit.max );
	hit.result			= PickBasedOnCondition( cmpRange, result, hit.result );
	hit.sphereCenter	= PickBasedOnCondition( cmpRange, sphereCenter, hit.sphereCenter );
	hit.radius			= PickBasedOnCondition( cmpRange, radiusSqr, hit.radius );
	hit.level			= PickBasedOnCondition( cmpRange, DEPTH, hit.level );

	return	compareRes;
}
////////////////////////////////////////////////////////////////////////////////

#endif // SPHEREFLAKE_H
