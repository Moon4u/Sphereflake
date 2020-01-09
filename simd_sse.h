#ifndef SIMD_SSE_H
#define SIMD_SSE_H

////////////////////////////////////////////////////////////////////////////////

#include <stdint.h>
#include <cassert>
#include <immintrin.h>
#include <xmmintrin.h>
#include <smmintrin.h>

#include "vec3.h"

////////////////////////////////////////////////////////////////////////////////

/**
 * Code used for SSE SIMD implementation.
 */
namespace SSE
{
	static constexpr uint8_t	SIZE	= 4;

	class						SSEDVec3;
	struct						float_t;
	struct						bool_t;
	using						Vec		= SSEDVec3;

	/**
	 * @brief The bool_t struct is a bool type that holds multiple results.
	 */
	struct	bool_t
	{
		union {
			__m128		val;
			uint32_t	f[ SIZE ];
		};
		constexpr bool_t( __m128 v ) : val( v ) {}

		operator bool()											const
		{
			return	_mm_movemask_ps( val );
		}
	};
	////////////////////////////////////////////////////////////////////////////

	// Const false value
	const bool_t	FALSE_VALUE	= _mm_castsi128_ps( _mm_set1_epi32( 0 ) );

	/**
	 * @brief The float_t struct is a float type that uses SIMD.
	 */
	struct	float_t
	{
		union {
			__m128	val;
			float	fval[ SIZE ];
		};

		float_t( float v ) {
			val	= _mm_set1_ps( v );
		}
		constexpr float_t( __m128 v ) : val( v ) { }

		float_t	operator*( const float_t& rhs )						const { return	_mm_mul_ps( val, rhs.val );   }
		float_t	operator-( const float_t& rhs )						const { return	_mm_sub_ps( val, rhs.val );   }
		float_t	operator+( const float_t& rhs )						const { return	_mm_add_ps( val, rhs.val );   }
		float_t	operator/( const float_t& rhs )						const { return	_mm_div_ps( val, rhs.val );   }
		bool_t	GreaterOrEqualThan( const float_t& rhs )			const { return	_mm_cmpge_ps( val, rhs.val ); }
		bool_t	LessThan( const float_t& rhs )						const { return	_mm_cmplt_ps( val, rhs.val ); }
		bool_t	IsInRange( const float_t& min, const float_t& max )	const
		{
			_mm_cmp_ps( val, min.val, _CMP_GT_OQ );

			__m128	minMask	= _mm_cmpgt_ps( val, min.val );
			__m128	maxMask	= _mm_cmplt_ps( val, max.val );

			return	_mm_and_ps( minMask, maxMask );
		}

		float	Extract( uint32_t index ) const
		{
			switch( index )
			{
				case	0:
				case	1:
				case	2:
				case	3:
					break;

				default:
					assert( false );
			}

			return	fval[ SIZE - index - 1 ];
		}
	};
	////////////////////////////////////////////////////////////////////////////

	/**
	 * @brief The AVXVec3 class is a vector class that uses SIMD.
	 */
	class	SSEDVec3
	{
	public:
		union {
			__m128	m;
			float	f[ SIZE ];
		} val[ 3 ];

		SSEDVec3(){}

		SSEDVec3( Vec3 v )
		{
			val[ 0 ].m	= _mm_set1_ps( v.x );
			val[ 1 ].m	= _mm_set1_ps( v.y );
			val[ 2 ].m	= _mm_set1_ps( v.z );
		}

		SSEDVec3( Vec3 v1, Vec3 v2, Vec3 v3, Vec3 v4 )
		{
			val[ 0 ].m	= _mm_set_ps( v1.x, v2.x, v3.x, v4.x );
			val[ 1 ].m	= _mm_set_ps( v1.y, v2.y, v3.y, v4.y );
			val[ 2 ].m	= _mm_set_ps( v1.z, v2.z, v3.z, v4.z );
		}

		SSEDVec3( Vec3 v[4] )
		{
			val[ 0 ].m	= _mm_set_ps( v[ 0 ].x, v[ 1 ].x, v[ 2 ].x, v[ 3 ].x );
			val[ 1 ].m	= _mm_set_ps( v[ 0 ].y, v[ 1 ].y, v[ 2 ].y, v[ 3 ].y );
			val[ 2 ].m	= _mm_set_ps( v[ 0 ].z, v[ 1 ].z, v[ 2 ].z, v[ 3 ].z );
		}

		float_t	dot( const SSEDVec3& rhs )	const
		{
			__m128	xs	= _mm_mul_ps( rhs.val[ 0 ].m, val[ 0 ].m );
			__m128	ys	= _mm_mul_ps( rhs.val[ 1 ].m, val[ 1 ].m );
			__m128	zs	= _mm_mul_ps( rhs.val[ 2 ].m, val[ 2 ].m );

			return	_mm_add_ps( _mm_add_ps( xs, ys ), zs );
		}

		SSEDVec3	MultiplyByFloat( const float_t& rhs )	const
		{
			SSEDVec3	result;
			result.val[ 0 ].m	= _mm_mul_ps( val[ 0 ].m, rhs.val );
			result.val[ 1 ].m	= _mm_mul_ps( val[ 1 ].m, rhs.val );
			result.val[ 2 ].m	= _mm_mul_ps( val[ 2 ].m, rhs.val );

			return	result;
		}

		inline
		SSEDVec3	operator/( const float_t& rhs )
		{
			SSEDVec3	result;
			result.val[ 0 ].m	= _mm_div_ps( val[ 0 ].m, rhs.val );
			result.val[ 1 ].m	= _mm_div_ps( val[ 1 ].m, rhs.val );
			result.val[ 2 ].m	= _mm_div_ps( val[ 2 ].m, rhs.val );

			return	result;
		}
		////////////////////////////////////////////////////////////////////////////

		Vec3	Extract( uint32_t index ) const
		{
			switch( index )
			{
				case	0:
				case	1:
				case	2:
				case	3:
					break;

				default:
					assert( false );
			}

			uint32_t	simdIndex	= SIZE - index - 1;
			return	Vec3( val[ 0 ].f[ simdIndex ], val[ 1 ].f[ simdIndex ], val[ 2 ].f[ simdIndex ] );
		}

	};
	////////////////////////////////////////////////////////////////////////////

	/**
	 * @brief PickBasedOnCondition	A function that will pick the value of f1 or
	 * f2, based on a the condition in bool.
	 */
	inline
	float_t	PickBasedOnCondition( bool_t cond, const float_t& f1, const float_t& f2 )
	{
		return	_mm_or_ps( _mm_and_ps( cond.val, f1.val ), _mm_andnot_ps( cond.val, f2.val ) );
	}
	////////////////////////////////////////////////////////////////////////////

	/**
	 * @brief PickBasedOnCondition	A function that will pick the value of f1 or
	 * f2, based on a the condition in bool.
	 */
	inline
	SSEDVec3	PickBasedOnCondition( bool_t cond, const SSEDVec3& v1, const SSEDVec3& v2 )
	{
		SSEDVec3	result;

		result.val[ 0 ].m	= _mm_or_ps( _mm_and_ps( cond.val, v1.val[ 0 ].m ), _mm_andnot_ps( cond.val, v2.val[ 0 ].m ) );
		result.val[ 1 ].m	= _mm_or_ps( _mm_and_ps( cond.val, v1.val[ 1 ].m ), _mm_andnot_ps( cond.val, v2.val[ 1 ].m ) );
		result.val[ 2 ].m	= _mm_or_ps( _mm_and_ps( cond.val, v1.val[ 2 ].m ), _mm_andnot_ps( cond.val, v2.val[ 2 ].m ) );

		return	result;
	}
	////////////////////////////////////////////////////////////////////////////

	/**
	 * @brief operator - A function that substracts two vectors.
	 */
	inline
	SSEDVec3	operator-( const SSEDVec3& lhs, const SSEDVec3& rhs )
	{
		SSEDVec3	result;
		result.val[ 0 ].m	= _mm_sub_ps( lhs.val[ 0 ].m, rhs.val[ 0 ].m );
		result.val[ 1 ].m	= _mm_sub_ps( lhs.val[ 1 ].m, rhs.val[ 1 ].m );
		result.val[ 2 ].m	= _mm_sub_ps( lhs.val[ 2 ].m, rhs.val[ 2 ].m );

		return	result;
	}
	////////////////////////////////////////////////////////////////////////////

	/**
	 * @brief operator * A function that multiplis two vectors.
	 */
	inline
	SSEDVec3	operator*( const SSEDVec3& lhs, const SSEDVec3& rhs )
	{
		SSEDVec3	result;
		result.val[ 0 ].m	= _mm_mul_ps( lhs.val[ 0 ].m, rhs.val[ 0 ].m );
		result.val[ 1 ].m	= _mm_mul_ps( lhs.val[ 1 ].m, rhs.val[ 1 ].m );
		result.val[ 2 ].m	= _mm_mul_ps( lhs.val[ 2 ].m, rhs.val[ 2 ].m );

		return	result;
	}
	////////////////////////////////////////////////////////////////////////////

	/**
	 * @brief operator + A function that adds two vectors.
	 */
	inline
	SSEDVec3	operator+( const SSEDVec3& lhs, const SSEDVec3& rhs )
	{
		SSEDVec3	result;
		result.val[ 0 ].m	= _mm_add_ps( lhs.val[ 0 ].m, rhs.val[ 0 ].m );
		result.val[ 1 ].m	= _mm_add_ps( lhs.val[ 1 ].m, rhs.val[ 1 ].m );
		result.val[ 2 ].m	= _mm_add_ps( lhs.val[ 2 ].m, rhs.val[ 2 ].m );

		return	result;
	}
	////////////////////////////////////////////////////////////////////////////

	/**
	 * @brief operator + A function that executes sqrt on the vector.
	 */
	inline
	float_t	sqrtf( float_t val )
	{
		return	_mm_sqrt_ps( val.val );
	}
	////////////////////////////////////////////////////////////////////////////
}; // namespace SSE
////////////////////////////////////////////////////////////////////////////////

#endif // SIMD_SSE_H
