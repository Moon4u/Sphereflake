
#ifndef VEC3_H
#define VEC3_H

////////////////////////////////////////////////////////////////////////////////

#include <stdint.h>
#include <math.h>

////////////////////////////////////////////////////////////////////////////////

/**
 * @brief	This is the vector class implementation.
 */
template< typename T >
struct Vec
{
public:
	constexpr Vec()
		: x( 0.f )
		, y( 0.f )
		, z( 0.f )
	{}

	constexpr Vec( float v )
		: x( v )
		, y( v )
		, z( v )
	{}

	constexpr Vec( Vec o[ 1 ])
		: x( o[ 0 ].x )
		, y( o[ 0 ].y )
		, z( o[ 0 ].z )
	{}

	constexpr Vec( T x, T y, T z )
		: x( x )
		, y( y )
		, z( z )
	{}

	constexpr Vec	operator+( const Vec& rhs )		const
	{
		return	Vec( x + rhs.x, y + rhs.y, z + rhs.z );
	}

	constexpr Vec	operator+( float k )			const
	{
		return	Vec( x + k, y + k, z + k );
	}

	constexpr Vec	operator-( const Vec& rhs )		const
	{
		return	Vec( x - rhs.x, y - rhs.y, z - rhs.z );
	}

	constexpr Vec	operator-( float k )			const
	{
		return	Vec( x - k, y - k, z - k );
	}

	constexpr Vec	operator*( float k )			const
	{
		return	Vec( x * k, y * k, z * k );
	}

	constexpr Vec	MultiplyByFloat( float k )		const
	{
		return	Vec( x * k, y * k, z * k );
	}

	constexpr Vec	operator*( const Vec& rhs )		const
	{
		return	Vec( x * rhs.x, y * rhs.y, z * rhs.z );
	}

	constexpr Vec	operator/( float k )			const
	{
		return	Vec( x / k, y / k, z / k );
	}

	constexpr float	len()							const
	{
		return	sqrtf( x * x + y * y + z * z );
	}

	constexpr float	dot( const Vec& rhs )			const
	{
		return	x * rhs.x + y * rhs.y + z * rhs.z;
	}

	constexpr Vec	Normalized()					const
	{
		float	length	= len();
		if( length == 0.0f )
		{
			return	Vec( *this );
		}
		return	*this / length;
	}

	constexpr Vec	cross( const Vec& v2 )			const
	{
		return	Vec( (     y * v2.z - z * v2.y   ),
					 ( - ( x * v2.z - z * v2.x ) ),
					 (     x * v2.y - y * v2.x   ) );
	}

	constexpr Vec&	operator*=( const Vec &v )
	{
		x	*= v.x;
		y	*= v.y;
		z	*= v.z;

		return	*this;
	}

	constexpr Vec&	operator*=( float v )
	{
		x	*= v;
		y	*= v;
		z	*= v;

		return	*this;
	}

	constexpr Vec&	operator+=( const Vec &v )
	{
		x	+= v.x;
		y	+= v.y;
		z	+= v.z;

		return	*this;
	}

	constexpr Vec&	operator+=( float v )
	{
		x	+= v;
		y	+= v;
		z	+= v;

		return	*this;
	}

	/**
	 * @brief Extract	For compability with SIMD.
	 */
	const Vec&	Extract( uint32_t ) const
	{
		return	*this;
	}

	T	x;
	T	y;
	T	z;
};
////////////////////////////////////////////////////////////////////////////////

template< typename T >
inline
Vec< T >	operator*( float t, const Vec< T >& v )
{
	return	Vec< T >( t * v.x, t * v.y, t * v.z );
}
////////////////////////////////////////////////////////////////////////////////

using	Vec3	= Vec< float >;

////////////////////////////////////////////////////////////////////////////////

#endif // VEC3_H
