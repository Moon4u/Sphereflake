
#ifndef SIMD_AVX512_H
#define SIMD_AVX512_H

////////////////////////////////////////////////////////////////////////////////

#include <stdint.h>
#include <cassert>
#include <immintrin.h>

#include "vec3.h"

////////////////////////////////////////////////////////////////////////////////

/**
 * Code used for AVX512 SIMD implementation.
 *
 * @note This code is not tested.
 */
namespace AVX512
{
#ifdef __AVX512__

	static constexpr uint8_t	SIZE	= 16;

	class						AVX512Vec3;
	struct						float_t;
	struct						bool_t;
	using						Vec		= AVX512Vec3;

	/**
	 * @brief The bool_t struct is a bool type that holds multiple results.
	 */
	struct	bool_t
	{
		union {
			__mmask16		val;
		};
		constexpr bool_t( __mmask16 v ) : val( v ) {}

		operator bool()											const
		{
			return	val != 0;
		}
	};
	////////////////////////////////////////////////////////////////////////////

	// Const false value
	const bool_t	FALSE_VALUE	= 0;

	/**
	 * @brief The float_t struct is a float type that uses SIMD.
	 */
	struct	float_t
	{
		union {
			__m512	val;
			float	fval[ SIZE ];
		};

		float_t( float v ) {
			val	= _mm512_set1_ps( v );
		}
		constexpr float_t( __m512 v ) : val( v ) { }

		float_t	operator*( const float_t& rhs )						const { return	_mm512_mul_ps( val, rhs.val );                  }
		float_t	operator-( const float_t& rhs )						const { return	_mm512_sub_ps( val, rhs.val );                  }
		float_t	operator+( const float_t& rhs )						const { return	_mm512_add_ps( val, rhs.val );                  }
		float_t	operator/( const float_t& rhs )						const { return	_mm512_div_ps( val, rhs.val );                  }
		bool_t	GreaterOrEqualThan( const float_t& rhs )			const { return	_mm512_cmp_ps_mask( val, rhs.val, _CMP_GE_OQ ); }
		bool_t	LessThan( const float_t& rhs )						const { return	_mm512_cmp_ps_mask( val, rhs.val, _CMP_LT_OQ ); }
		bool_t	IsInRange( const float_t& min, const float_t& max )	const
		{
			__mmask16	minMask	= _mm512_cmp_ps_mask( val, min.val, _CMP_GT_OQ );
			__mmask16	maxMask	= _mm512_cmp_ps_mask( val, max.val, _CMP_LT_OQ );

			return	minMask & maxMask;
		}

		float	Extract( uint32_t index ) const
		{
			if( index >= 16 )
			{
				assert( false );
			}

			return	fval[ SIZE - index - 1 ];
		}
	};
	////////////////////////////////////////////////////////////////////////////


	/**
	 * @brief The AVXVec3 class is a vector class that uses SIMD.
	 */
	class	AVX512Vec3
	{
	public:
		union {
			__m512	m;
			float	f[ SIZE ];
		} val[ 3 ];

		AVX512Vec3(){}

		AVX512Vec3( Vec3 v )
		{
			val[ 0 ].m	= _mm512_set1_ps( v.x );
			val[ 1 ].m	= _mm512_set1_ps( v.y );
			val[ 2 ].m	= _mm512_set1_ps( v.z );
		}

		AVX512Vec3( Vec3 v[ SIZE ] )
		{
			val[ 0 ].m	= _mm512_set_ps( v[ 0 ].x, v[ 1 ].x, v[ 2 ].x, v[ 3 ].x, v[ 4 ].x, v[ 5 ].x, v[ 6 ].x, v[ 7 ].x, v[ 8 ].x, v[ 9 ].x, v[ 10 ].x, v[ 11 ].x, v[ 12 ].x, v[ 13 ].x, v[ 14 ].x, v[ 15 ].x );
			val[ 1 ].m	= _mm512_set_ps( v[ 0 ].y, v[ 1 ].y, v[ 2 ].y, v[ 3 ].y, v[ 4 ].y, v[ 5 ].y, v[ 6 ].y, v[ 7 ].y, v[ 8 ].y, v[ 9 ].y, v[ 10 ].y, v[ 11 ].y, v[ 12 ].y, v[ 13 ].y, v[ 14 ].y, v[ 15 ].y );
			val[ 2 ].m	= _mm512_set_ps( v[ 0 ].z, v[ 1 ].z, v[ 2 ].z, v[ 3 ].z, v[ 4 ].z, v[ 5 ].z, v[ 6 ].z, v[ 7 ].z, v[ 8 ].z, v[ 9 ].z, v[ 10 ].z, v[ 11 ].z, v[ 12 ].z, v[ 13 ].z, v[ 14 ].z, v[ 15 ].z );
		}

		float_t	dot( const AVX512Vec3& rhs )	const
		{
			__m512	xs	= _mm512_mul_ps( rhs.val[ 0 ].m, val[ 0 ].m );
			__m512	ys	= _mm512_mul_ps( rhs.val[ 1 ].m, val[ 1 ].m );
			__m512	zs	= _mm512_mul_ps( rhs.val[ 2 ].m, val[ 2 ].m );

			return	_mm512_add_ps( _mm512_add_ps( xs, ys ), zs );
		}

		AVX512Vec3	MultiplyByFloat( const float_t& rhs )	const
		{
			AVX512Vec3	result;
			result.val[ 0 ].m	= _mm512_mul_ps( val[ 0 ].m, rhs.val );
			result.val[ 1 ].m	= _mm512_mul_ps( val[ 1 ].m, rhs.val );
			result.val[ 2 ].m	= _mm512_mul_ps( val[ 2 ].m, rhs.val );

			return	result;
		}

		inline
		AVX512Vec3	operator/( const float_t& rhs )
		{
			AVX512Vec3	result;
			result.val[ 0 ].m	= _mm512_div_ps( val[ 0 ].m, rhs.val );
			result.val[ 1 ].m	= _mm512_div_ps( val[ 1 ].m, rhs.val );
			result.val[ 2 ].m	= _mm512_div_ps( val[ 2 ].m, rhs.val );

			return	result;
		}
		////////////////////////////////////////////////////////////////////////////

		Vec3	Extract( uint32_t index ) const
		{
			if( index >= SIZE )
				assert( false );

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
		return	_mm512_mask_blend_ps( cond.val, f1.val, f2.val );
	}
	////////////////////////////////////////////////////////////////////////////

	/**
	 * @brief PickBasedOnCondition	A function that will pick the value of f1 or
	 * f2, based on a the condition in bool.
	 */
	inline
	AVX512Vec3	PickBasedOnCondition( bool_t cond, const AVX512Vec3& v1, const AVX512Vec3& v2 )
	{
		AVX512Vec3	result;

		result.val[ 0 ].m	= _mm512_mask_blend_ps( cond.val, v1.val[ 0 ].m, v2.val[ 0 ].m );
		result.val[ 1 ].m	= _mm512_mask_blend_ps( cond.val, v1.val[ 1 ].m, v2.val[ 1 ].m );
		result.val[ 2 ].m	= _mm512_mask_blend_ps( cond.val, v1.val[ 2 ].m, v2.val[ 2 ].m );

		return	result;
	}
	////////////////////////////////////////////////////////////////////////////

	/**
	 * @brief operator - A function that substracts two vectors.
	 */
	inline
	AVX512Vec3	operator-( const AVX512Vec3& lhs, const AVX512Vec3& rhs )
	{
		AVX512Vec3	result;
		result.val[ 0 ].m	= _mm512_sub_ps( lhs.val[ 0 ].m, rhs.val[ 0 ].m );
		result.val[ 1 ].m	= _mm512_sub_ps( lhs.val[ 1 ].m, rhs.val[ 1 ].m );
		result.val[ 2 ].m	= _mm512_sub_ps( lhs.val[ 2 ].m, rhs.val[ 2 ].m );

		return	result;
	}
	////////////////////////////////////////////////////////////////////////////

	/**
	 * @brief operator - A function that multiplis two vectors.
	 */
	inline
	AVX512Vec3	operator*( const AVX512Vec3& lhs, const AVX512Vec3& rhs )
	{
		AVX512Vec3	result;
		result.val[ 0 ].m	= _mm512_mul_ps( lhs.val[ 0 ].m, rhs.val[ 0 ].m );
		result.val[ 1 ].m	= _mm512_mul_ps( lhs.val[ 1 ].m, rhs.val[ 1 ].m );
		result.val[ 2 ].m	= _mm512_mul_ps( lhs.val[ 2 ].m, rhs.val[ 2 ].m );

		return	result;
	}
	////////////////////////////////////////////////////////////////////////////

	/**
	 * @brief operator + A function that adds two vectors.
	 */
	inline
	AVX512Vec3	operator+( const AVX512Vec3& lhs, const AVX512Vec3& rhs )
	{
		AVX512Vec3	result;
		result.val[ 0 ].m	= _mm512_add_ps( lhs.val[ 0 ].m, rhs.val[ 0 ].m );
		result.val[ 1 ].m	= _mm512_add_ps( lhs.val[ 1 ].m, rhs.val[ 1 ].m );
		result.val[ 2 ].m	= _mm512_add_ps( lhs.val[ 2 ].m, rhs.val[ 2 ].m );

		return	result;
	}
	////////////////////////////////////////////////////////////////////////////

	/**
	 * @brief operator + A function that executes sqrt on the vector.
	 */
	inline
	float_t	sqrtf( float_t val )
	{
		return	_mm512_sqrt_ps( val.val );
	}
	////////////////////////////////////////////////////////////////////////////
#endif // __AVX512__

}; // namespace AVX512
////////////////////////////////////////////////////////////////////////////////

#endif // SIMD_AVX512_H
