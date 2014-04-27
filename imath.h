#ifndef IMATH_H
#define IMATH_H



/**
 * \brief Get the integer square root of a 16 bit number
 *
 * \param x a number greater or equal to 0 and less or equal to 0xFFFF
 *
 * \return The integer square root
 */
int isqrt16( int x );

/**
 * \brief Get the 16.16 fixed point cosine of an angle in degrees
 *
 * \param x An angle in degrees
 *
 * \return The 16.16 fixed point cosine of the given angle
 */
int cosd16( int x );

/**
 * \brief Get the 16.16 fixed point sine of an angle in degrees
 *
 * \param x An angle in degrees
 *
 * \return The 16.16 fixed point sine of the given angle
 */
int sind16( int x );



#define FP16_ONE (1<<16)
#define INT_TO_FP16( x ) ((x)<<16)
#define FP16_TO_INT( x ) ((x)>>16)
#define FP16_MUL( a, b ) ((((long)(a)) * ((long)(b))) >> 16)
#define FP16_DIV( a, b ) ((((long)(a))<<16) / (b))



#endif

