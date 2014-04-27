#include "xlib_canvas.h"
#include "imath.h"

#include <stdlib.h>
#include <string.h>



typedef struct
{
    canvas super;

    Drawable target;
    Display* dpy;
    GC gc;
}
canvas_xlib;


static void canvas_xlib_set_color( canvas* super,
                                      unsigned char r, unsigned char g,
                                      unsigned char b, unsigned char a )
{
    canvas_xlib* this = (canvas_xlib*)super;
    unsigned long color;
    (void)a;

    color = (r<<16) | (g<<8) | b;
    XSetForeground( this->dpy, this->gc, color );
}

static void canvas_xlib_clear( canvas* super )
{
    canvas_xlib* this = (canvas_xlib*)super;

    XFillRectangle( this->dpy, this->target, this->gc,
                    0, 0, super->width, super->height );
}

static void canvas_xlib_draw_point( canvas* super, unsigned int x,
                                                   unsigned int y )
{
    canvas_xlib* this = (canvas_xlib*)super;

    XDrawPoint( this->dpy, this->target, this->gc, x, y );
}

static void canvas_xlib_draw_line( canvas* super, int x0, int y0,
                                                  int x1, int y1 )
{
    canvas_xlib* this = (canvas_xlib*)super;

    XSetLineAttributes( this->dpy, this->gc, super->linewidth,
                        LineSolid, CapNotLast, JoinMiter );
    XDrawLine( this->dpy, this->target, this->gc, x0, y0, x1, y1 );
}

static void canvas_xlib_draw_circle( canvas* super, int cx, int cy,
                                                    int radius )
{
    canvas_xlib* this = (canvas_xlib*)super;

    XSetLineAttributes( this->dpy, this->gc, super->linewidth,
                        LineSolid, CapNotLast, JoinMiter );
    XDrawArc( this->dpy, this->target, this->gc, cx-radius, cy-radius,
              radius*2, radius*2, 0, 360*64 );
}

static void canvas_xlib_fill_rect( canvas* super, int x, int y,
                                                  int x1, int y1 )
{
    canvas_xlib* this = (canvas_xlib*)super;

    XFillRectangle( this->dpy, this->target, this->gc,
                    x, y, x1-x+1, y1-y+1 );
}

static void canvas_xlib_fill_triangle( canvas* super, int x0, int y0,
                                                      int x1, int y1,
                                                      int x2, int y2 )
{
    canvas_xlib* this = (canvas_xlib*)super;
    XPoint points[3];

    points[0].x = x0;
    points[0].y = y0;
    points[1].x = x1;
    points[1].y = y1;
    points[2].x = x2;
    points[2].y = y2;

    XFillPolygon( this->dpy, this->target, this->gc, points, 3,
                  Nonconvex, CoordModeOrigin );
}

static void canvas_xlib_fill_circle( canvas* super, int cx, int cy,
                                                    int radius )
{
    canvas_xlib* this = (canvas_xlib*)super;

    XFillArc( this->dpy, this->target, this->gc, cx-radius-1, cy-radius-1,
              radius*2+2, radius*2+2, 0, 360*64 );
}

static void canvas_xlib_destroy( canvas* super )
{
    canvas_xlib* this = (canvas_xlib*)super;
    XFreeGC( this->dpy, this->gc );
    free( this );
}

/****************************************************************************/

canvas* canvas_xlib_create( Display* dpy, Drawable target,
                            unsigned int width, unsigned int height )
{
    canvas_xlib* this;
    canvas* super;

    /* sanity check */
    if( !dpy )
        return NULL;

    /* create and initialize */
    this = malloc( sizeof(canvas_xlib) );
    super = (canvas*)this;

    if( !this )
        return NULL;

    memset( this, 0, sizeof(canvas_xlib) );

    this->dpy = dpy;
    this->target = target;
    this->gc = XCreateGC( dpy, target, 0, NULL );

    if( !this->gc )
    {
        free( this );
        return NULL;
    }

    super->width = width;
    super->height = height;
    super->linewidth = 1;

    super->set_color     = canvas_xlib_set_color;
    super->clear         = canvas_xlib_clear;
    super->draw_point    = canvas_xlib_draw_point;
    super->draw_line     = canvas_xlib_draw_line;
    super->draw_circle   = canvas_xlib_draw_circle;
    super->fill_rect     = canvas_xlib_fill_rect;
    super->fill_circle   = canvas_xlib_fill_circle;
    super->fill_triangle = canvas_xlib_fill_triangle;
    super->destroy       = canvas_xlib_destroy;

    return super;
}

