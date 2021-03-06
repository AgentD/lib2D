#ifndef XLIB_CANVAS_H
#define XLIB_CANVAS_H



#include "canvas.h"

#include <X11/X.h>
#include <X11/Xlib.h>



/**
 * \brief Create a canvas that uses legacy xlib drawing functions to draw
 *        to a Drawable
 *
 * \param dpy    A pointer to an X11 display object
 * \param target A window or pixmap to draw to
 * \param width  The width of the Drawable in pixels
 * \param height The width of the Drawable in pixels
 *
 * \return A pointer to a canvas object
 */
canvas* canvas_xlib_create( Display* dpy, Drawable target,
                            unsigned int width, unsigned int height );



#endif /* XLIB_CANVAS_H */

