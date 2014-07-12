#ifndef MEM_CANVAS
#define MEM_CANVAS



#include "canvas.h"



/**
 * \brief Create an offscreen in memory implementation of a canvas
 *
 * \param width The width of the canvas in pixels
 * \param height The height of the canvas in pixels
 *
 * \return A pointer to a canas
 */
canvas* canvas_memory_create( unsigned int width, unsigned int height );

/**
 * \brief Get the pointer to the buffer data of a memory canvas
 *
 * \param cv A pointer to a canvas
 *
 * \return A pointer to the top left pixel
 */
unsigned char* canvas_memory_get_buffer( canvas* cv );



#endif

