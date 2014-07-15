#ifndef MEM_CANVAS
#define MEM_CANVAS



#include "canvas.h"



/**
 * \brief Create an offscreen in memory implementation of a canvas
 *
 * \param width  The width of the canvas in pixels
 * \param height The height of the canvas in pixels
 * \param swaprb If non-zero, the canvas will swap the red and blue channel,
 *               i.e. it generates BGRA instead of RGBA values in memory
 *
 * \return A pointer to a canvas
 */
canvas* canvas_memory_create( unsigned int width, unsigned int height,
                              int swaprb );

/**
 * \brief Create an offscreen in memory implementation of a canvas that draws
 *        to an external buffer
 *
 * \param buffer A pointer to a buffer with width*height*4 bytes to draw to
 * \param width  The width of the drawing surface in pixels
 * \param height The hieght of the drawing surface in pixels
 * \param swaprb If non-zero, the canvas will swap the red and blue channel,
 *               i.e. it generates BGRA instead of RGBA values in memory
 *
 * \return A pointer to a canvas
 */
canvas* canvas_memory_create_ext( unsigned char* buffer,
                                  unsigned int width, unsigned int height,
                                  int swaprb );

/**
 * \brief Get the pointer to the buffer data of a memory canvas
 *
 * \param cv A pointer to a canvas
 *
 * \return A pointer to the top left pixel
 */
unsigned char* canvas_memory_get_buffer( canvas* cv );



#endif

