
#ifndef SIMD_BASE_H
#define SIMD_BASE_H

////////////////////////////////////////////////////////////////////////////////

#include <stdint.h>

#include "vec3.h"

////////////////////////////////////////////////////////////////////////////////

/**
 *	This is implementation for cases when there is no SIMD available.
 * These are mostly wrappers around base types. The existing Vec3 is used
 * instead of another vector class.
 */
namespace NO_SIMD
{
	static constexpr uint8_t	SIZE		= 1;
	// Const false value
	static constexpr bool		FALSE_VALUE	= false;
	using						Vec			= Vec3;
	struct						bool_t;

	/**
	 * @brief The bool_t struct is a bool type that holds multiple results.
	 */
	struct	bool_t
	{
		constexpr bool_t( bool v ) : val( v ) {}
		constexpr bool_t() : val( ) {}

		constexpr float_t	PickOne( const float_t& f1, const float_t& f2 )	const;

		constexpr			operator bool()	const { return	val; }

		bool	val;
	};
	////////////////////////////////////////////////////////////////////////////

	/**
	 * @brief The float_t struct is a float type that uses SIMD.
	 */
	struct	float_t
	{
		constexpr float_t( float v ) : val( v ) {}

		constexpr			operator float()									const { return	val;                    }
		constexpr float_t	operator+( const float_t& rhs )						const { return	val + rhs.val;          }
		constexpr bool_t	operator<( const float_t& rhs )						const { return	val < rhs.val;          }
		constexpr bool_t	GreaterOrEqualThan( const float_t& rhs )			const { return	val >= rhs.val;         }
		constexpr bool_t	LessThan( const float_t& rhs )						const { return	val < rhs.val;          }
		constexpr bool_t	IsInRange( const float_t& min, const float_t max )	const { return	val > min && val < max; }
		constexpr float		Extract( uint32_t )									const { return	val;                    }

		float	val;
	};
	////////////////////////////////////////////////////////////////////////////

	/**
	 * @brief The AVXVec3 class is a vector class that uses SIMD.
	 */
	inline
	float_t	PickBasedOnCondition( bool_t cond, const float_t& f1, const float& f2 )
	{
		if( cond )
			return	f1;

		return	f2;
	}
	////////////////////////////////////////////////////////////////////////////

	inline
	Vec	PickBasedOnCondition( bool_t cond, const Vec& v1, const Vec& v2 )
	{
		if( cond )
			return	v1;

		return	v2;
	}
	////////////////////////////////////////////////////////////////////////////
}
////////////////////////////////////////////////////////////////////////////////

#endif // SIMD_BASE_H
