#include "xlib_canvas.h"
#include "pixmap.h"
#include "imath.h"

#include <stdlib.h>
#include <string.h>



typedef struct
{
    canvas super;

    unsigned long fgcolor;
    unsigned long bgcolor;
    Drawable target;
    Display* dpy;
    GC gc;
}
canvas_xlib;

typedef struct
{
    pixmap super;
    canvas_xlib* owner;
    Pixmap xpm;
}
pixmap_xlib;



static void pixmap_xlib_load( pixmap* super, int dstx, int dsty,
                              unsigned int width, unsigned int height,
                              unsigned int scan, int format,
                              unsigned char* data )
{
    pixmap_xlib* this = (pixmap_xlib*)super;
    unsigned int x, y, bpp, R, G, B, A;
    unsigned char* src_row;
    unsigned long color;

    bpp = format==COLOR_RGBA8 ? 4 : (format==COLOR_RGB8 ? 3 : 1);

    for( y=0; y<height; ++y, data+=scan*bpp )
    {
        for( src_row=data, x=0; x<width; ++x, src_row+=bpp )
        {
            A = bpp>3 ? src_row[3] : 0xFF;
            R = ((src_row[0] * A) >> 8) & 0xFF;
            G = (((bpp>1 ? src_row[1] : R)*A)>>8) & 0xFF;
            B = (((bpp>1 ? src_row[2] : R)*A)>>8) & 0xFF;
            color = (R<<16) | (G<<8) | B;
            XSetForeground( this->owner->dpy, this->owner->gc, color );
            XDrawPoint( this->owner->dpy, this->xpm, this->owner->gc,
                        dstx+x, dsty+y );
        }
    }

    XSetForeground( this->owner->dpy, this->owner->gc, this->owner->fgcolor );
}

static void pixmap_xlib_destroy( pixmap* super )
{
    pixmap_xlib* this = (pixmap_xlib*)super;
    XFreePixmap( this->owner->dpy, this->xpm );
    free( this );
}

static pixmap* canvas_xlib_create_pixmap( canvas* super, unsigned int width,
                                          unsigned int height, int format )
{
    canvas_xlib* this = (canvas_xlib*)super;
    pixmap_xlib* pix;

    pix = malloc( sizeof(pixmap_xlib) );

    if( pix )
    {
        pix->super.width   = width;
        pix->super.height  = height;
        pix->super.format  = format;
        pix->super.load    = pixmap_xlib_load;
        pix->super.destroy = pixmap_xlib_destroy;

        pix->owner = this;
        pix->xpm = XCreatePixmap(this->dpy, this->target, width, height, 24);

        if( !pix->xpm )
        {
            free( pix );
            pix = NULL;
        }
    }

    return (pixmap*)pix;
}

static void canvas_xlib_set_color( canvas* super, int fg,
                                   unsigned char r, unsigned char g,
                                   unsigned char b, unsigned char a )
{
    canvas_xlib* this = (canvas_xlib*)super;
    unsigned int br, bg, bb;

    if( fg )
    {
        br = (this->bgcolor >> 16) & 0xFF;
        bg = (this->bgcolor >>  8) & 0xFF;
        bb = (this->bgcolor      ) & 0xFF;

        r = (r*a + br*(0xFF-a))>>8;
        g = (g*a + bg*(0xFF-a))>>8;
        b = (b*a + bb*(0xFF-a))>>8;

        this->fgcolor = (r<<16) | (g<<8) | b;
        XSetForeground( this->dpy, this->gc, this->fgcolor );
    }
    else
    {
        this->bgcolor = (r<<16) | (g<<8) | b;
    }
}

static void canvas_xlib_clear( canvas* super )
{
    canvas_xlib* this = (canvas_xlib*)super;

    XSetForeground( this->dpy, this->gc, this->bgcolor );
    XFillRectangle( this->dpy, this->target, this->gc,
                    0, 0, super->width, super->height );
    XSetForeground( this->dpy, this->gc, this->fgcolor );
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

static void canvas_xlib_blit_pixmap( canvas* super, pixmap* pm, int x, int y )
{
    canvas_xlib* this = (canvas_xlib*)super;
    pixmap_xlib* pix = (pixmap_xlib*)pm;

    XCopyArea( this->dpy, pix->xpm, this->target, this->gc, 0, 0,
               pm->width, pm->height, x, y );
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
    this->fgcolor = 0;
    this->bgcolor = 0;
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
    super->blit_pixmap   = canvas_xlib_blit_pixmap;
    super->create_pixmap = canvas_xlib_create_pixmap;
    super->destroy       = canvas_xlib_destroy;

    return super;
}

