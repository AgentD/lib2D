#ifndef XRENDER_CANVAS
#define XRENDER_CANVAS



#include "canvas.h"

#include <X11/X.h>
#include <X11/Xlib.h>



/**
 * \brief Create a canvas that uses Xrender functions to draw to a Drawable
 *
 * \param dpy    A pointer to an X11 display object
 * \param target A window or pixmap to draw to
 * \param width  The width of the Drawable in pixels
 * \param height The width of the Drawable in pixels
 *
 * \return A pointer to a canvas object
 */
canvas* canvas_xrender_create( Display* dpy, Drawable target,
                               unsigned int width, unsigned int height );



#endif

