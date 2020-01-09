
#ifndef SIMD_AVX_H
#define SIMD_AVX_H

////////////////////////////////////////////////////////////////////////////////

#include <stdint.h>
#include <cassert>
#include <immintrin.h>

#include "vec3.h"

////////////////////////////////////////////////////////////////////////////////

/**
 * Code used for AVX SIMD implementation.
 */
namespace AVX
{
	static constexpr uint8_t	SIZE	= 8;

	class						AVXVec3;
	struct						float_t;
	struct						bool_t;
	using						Vec		= AVXVec3;

	/**
	 * @brief The bool_t struct is a bool type that holds multiple results.
	 */
	struct	bool_t
	{
		union {
			__m256		val;
			uint32_t	f[ SIZE ];
		};
		constexpr bool_t( __m256 v ) : val( v ) {}

		operator bool()											const
		{
			int	result	= _mm256_movemask_ps( val );
			if( result > 0 )
				return	true;
			return	false;
		}
	};

	// Const false value
	const bool_t	FALSE_VALUE	= _mm256_set1_ps( 0.0f );

	/**
	 * @brief The float_t struct is a float type that uses SIMD.
	 */
	struct	float_t
	{
		union {
			__m256	val;
			float	fval[ SIZE ];
		};

		float_t( float v ) {
			val	= _mm256_set1_ps( v );
		}
		constexpr float_t( __m256 v ) : val( v ) { }

		float_t	operator*( const float_t& rhs )						const { return	_mm256_mul_ps( val, rhs.val );   }
		float_t	operator-( const float_t& rhs )						const { return	_mm256_sub_ps( val, rhs.val );   }
		float_t	operator+( const float_t& rhs )						const { return	_mm256_add_ps( val, rhs.val );   }
		float_t	operator/( const float_t& rhs )						const { return	_mm256_div_ps( val, rhs.val );   }
		bool_t	GreaterOrEqualThan( const float_t& rhs )			const { return	_mm256_cmp_ps( val, rhs.val, _CMP_GE_OQ ); }
		bool_t	LessThan( const float_t& rhs )						const { return	_mm256_cmp_ps( val, rhs.val, _CMP_LT_OQ ); }
		bool_t	IsInRange( const float_t& min, const float_t& max )	const
		{
			__m256	minMask	= _mm256_cmp_ps( val, min.val, _CMP_GT_OQ );
			__m256	maxMask	= _mm256_cmp_ps( val, max.val, _CMP_LT_OQ );

			return	_mm256_and_ps( minMask, maxMask );
		}

		float	Extract( uint32_t index ) const
		{
			switch( index )
			{
				case	0:
				case	1:
				case	2:
				case	3:
				case	4:
				case	5:
				case	6:
				case	7:
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
	class	AVXVec3
	{
	public:
		union {
			__m256	m;
			float	f[ SIZE ];
		} val[ 3 ];

		AVXVec3(){}

		AVXVec3( Vec3 v )
		{
			val[ 0 ].m	= _mm256_set1_ps( v.x );
			val[ 1 ].m	= _mm256_set1_ps( v.y );
			val[ 2 ].m	= _mm256_set1_ps( v.z );
		}

		AVXVec3( Vec3 v[ 8 ] )
		{
			val[ 0 ].m	= _mm256_set_ps( v[ 0 ].x, v[ 1 ].x, v[ 2 ].x, v[ 3 ].x, v[ 4 ].x, v[ 5 ].x, v[ 6 ].x, v[ 7 ].x );
			val[ 1 ].m	= _mm256_set_ps( v[ 0 ].y, v[ 1 ].y, v[ 2 ].y, v[ 3 ].y, v[ 4 ].y, v[ 5 ].y, v[ 6 ].y, v[ 7 ].y );
			val[ 2 ].m	= _mm256_set_ps( v[ 0 ].z, v[ 1 ].z, v[ 2 ].z, v[ 3 ].z, v[ 4 ].z, v[ 5 ].z, v[ 6 ].z, v[ 7 ].z );
		}

		float_t	dot( const AVXVec3& rhs )	const
		{
			__m256	xs	= _mm256_mul_ps( rhs.val[ 0 ].m, val[ 0 ].m );
			__m256	ys	= _mm256_mul_ps( rhs.val[ 1 ].m, val[ 1 ].m );
			__m256	zs	= _mm256_mul_ps( rhs.val[ 2 ].m, val[ 2 ].m );

			return	_mm256_add_ps( _mm256_add_ps( xs, ys ), zs );
		}

		AVXVec3	MultiplyByFloat( const float_t& rhs )	const
		{
			AVXVec3	result;
			result.val[ 0 ].m	= _mm256_mul_ps( val[ 0 ].m, rhs.val );
			result.val[ 1 ].m	= _mm256_mul_ps( val[ 1 ].m, rhs.val );
			result.val[ 2 ].m	= _mm256_mul_ps( val[ 2 ].m, rhs.val );

			return	result;
		}

		inline
		AVXVec3	operator/( const float_t& rhs )
		{
			AVXVec3	result;
			result.val[ 0 ].m	= _mm256_div_ps( val[ 0 ].m, rhs.val );
			result.val[ 1 ].m	= _mm256_div_ps( val[ 1 ].m, rhs.val );
			result.val[ 2 ].m	= _mm256_div_ps( val[ 2 ].m, rhs.val );

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
				case	4:
				case	5:
				case	6:
				case	7:
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
		return	_mm256_or_ps( _mm256_and_ps( cond.val, f1.val ), _mm256_andnot_ps( cond.val, f2.val ) );
	}
	////////////////////////////////////////////////////////////////////////////

	/**
	 * @brief PickBasedOnCondition	A function that will pick the value of f1 or
	 * f2, based on a the condition in bool.
	 */
	inline
	AVXVec3	PickBasedOnCondition( bool_t cond, const AVXVec3& v1, const AVXVec3& v2 )
	{
		AVXVec3	result;

		result.val[ 0 ].m	= _mm256_or_ps( _mm256_and_ps( cond.val, v1.val[ 0 ].m ), _mm256_andnot_ps( cond.val, v2.val[ 0 ].m ) );
		result.val[ 1 ].m	= _mm256_or_ps( _mm256_and_ps( cond.val, v1.val[ 1 ].m ), _mm256_andnot_ps( cond.val, v2.val[ 1 ].m ) );
		result.val[ 2 ].m	= _mm256_or_ps( _mm256_and_ps( cond.val, v1.val[ 2 ].m ), _mm256_andnot_ps( cond.val, v2.val[ 2 ].m ) );

		return	result;
	}
	////////////////////////////////////////////////////////////////////////////

	/**
	 * @brief operator - A function that substracts two vectors.
	 */
	inline
	AVXVec3	operator-( const AVXVec3& lhs, const AVXVec3& rhs )
	{
		AVXVec3	result;
		result.val[ 0 ].m	= _mm256_sub_ps( lhs.val[ 0 ].m, rhs.val[ 0 ].m );
		result.val[ 1 ].m	= _mm256_sub_ps( lhs.val[ 1 ].m, rhs.val[ 1 ].m );
		result.val[ 2 ].m	= _mm256_sub_ps( lhs.val[ 2 ].m, rhs.val[ 2 ].m );

		return	result;
	}
	////////////////////////////////////////////////////////////////////////////

	/**
	 * @brief operator * A function that multiplis two vectors.
	 */
	inline
	AVXVec3	operator*( const AVXVec3& lhs, const AVXVec3& rhs )
	{
		AVXVec3	result;
		result.val[ 0 ].m	= _mm256_mul_ps( lhs.val[ 0 ].m, rhs.val[ 0 ].m );
		result.val[ 1 ].m	= _mm256_mul_ps( lhs.val[ 1 ].m, rhs.val[ 1 ].m );
		result.val[ 2 ].m	= _mm256_mul_ps( lhs.val[ 2 ].m, rhs.val[ 2 ].m );

		return	result;
	}
	////////////////////////////////////////////////////////////////////////////

	/**
	 * @brief operator + A function that adds two vectors.
	 */
	inline
	AVXVec3	operator+( const AVXVec3& lhs, const AVXVec3& rhs )
	{
		AVXVec3	result;
		result.val[ 0 ].m	= _mm256_add_ps( lhs.val[ 0 ].m, rhs.val[ 0 ].m );
		result.val[ 1 ].m	= _mm256_add_ps( lhs.val[ 1 ].m, rhs.val[ 1 ].m );
		result.val[ 2 ].m	= _mm256_add_ps( lhs.val[ 2 ].m, rhs.val[ 2 ].m );

		return	result;
	}
	////////////////////////////////////////////////////////////////////////////

	/**
	 * @brief operator + A function that executes sqrt on the vector.
	 */
	inline
	float_t	sqrtf( float_t val )
	{
		return	_mm256_sqrt_ps( val.val );
	}
	////////////////////////////////////////////////////////////////////////////
}; // namespace AVX
////////////////////////////////////////////////////////////////////////////////

#endif // SIMD_AVX_H
