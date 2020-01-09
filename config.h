
#ifndef CONFIG_H
#define CONFIG_H

////////////////////////////////////////////////////////////////////////////////

#include <stdint.h>

////////////////////////////////////////////////////////////////////////////////

/**
 * @brief These are constants that are used throughout the code.
 */

// Number of type1 and type2 spheres.
#define	TYPE1_SPHERES_COUNT		6
#define	TYPE2_SPHERES_COUNT		3

// Total number of spheres.
#define TOTAL_NUMBER_OF_SPHERES	TYPE1_SPHERES_COUNT + TYPE2_SPHERES_COUNT

// Sphere Y axis position. Only between 0 and 90 degrees.
#define	TYPE1_SPHERES_DEGREE	90.0
#define	TYPE2_SPHERES_DEGREE	30.0

// Rotation on the surface of the sphere
#define	TYPE1_SPHERES_ROTATION	0.0
#define TYPE2_SPHERES_ROTATION	60.0

constexpr float		STARTING_RADIUS				= 1.f;
constexpr float		SPHERE_RATIO				= 1.0f / 3.0f;

constexpr uint32_t	SCREEN_WIDTH				= 800;
constexpr uint32_t	SCREEN_HEIGHT				= 600;

constexpr uint32_t	FPS							= 60;

constexpr float		SCREEN_RATIO				= float( SCREEN_HEIGHT ) / float( SCREEN_WIDTH );

////////////////////////////////////////////////////////////////////////////////

#endif // CONFIG_H
