#include "xrender_canvas.h"
#include "imath.h"

#include <X11/extensions/Xrender.h>
#include <stdlib.h>
#include <string.h>



typedef struct
{
    canvas super;

    Display* dpy;

    XRenderColor c;

    Picture pen;
    Pixmap penmap;

    Picture pic;
}
canvas_xrender;


static void canvas_xrender_set_color( canvas* super,
                                      unsigned char r, unsigned char g,
                                      unsigned char b, unsigned char a )
{
    canvas_xrender* this = (canvas_xrender*)super;

    this->c.red   = r*a;
    this->c.green = g*a;
    this->c.blue  = b*a;
    this->c.alpha = a<<8;

    XRenderFillRectangle( this->dpy, PictOpSrc, this->pen, &this->c,
                          0, 0, 1, 1 );
}

static void canvas_xrender_clear( canvas* super )
{
    canvas_xrender* this = (canvas_xrender*)super;
    XRenderFillRectangle( this->dpy, PictOpSrc, this->pic, &this->c,
                          0, 0, super->width, super->height );
}

static void canvas_xrender_draw_point( canvas* super, unsigned int x,
                                                      unsigned int y )
{
    canvas_xrender* this = (canvas_xrender*)super;
    XRenderFillRectangle( this->dpy, PictOpSrc, this->pic, &this->c,
                          x, y, 1, 1 );
}

static void canvas_xrender_draw_line( canvas* super, int x0, int y0,
                                                     int x1, int y1 )
{
    canvas_xrender* this = (canvas_xrender*)super;
    int len, ldx, ldy, dx, dy;
    XPointFixed points[4];

    dx = x1 - x0;
    dy = y1 - y0;

    if( x0==x1 )
    {
        dy = dy<0 ? -dy : dy;
        XRenderFillRectangle( this->dpy, PictOpOver, this->pic, &this->c,
                              x0 - super->linewidth/2, y0<y1 ? y0 : y1,
                              super->linewidth, dy );
    }
    else if( y0==y1 )
    {
        dx = dx<0 ? -dx : dx;
        XRenderFillRectangle( this->dpy, PictOpOver, this->pic, &this->c,
                              x0<x1 ? x0 : x1, y0 - super->linewidth/2,
                              dx, super->linewidth );
    }
    else
    {
        len = isqrt16( dx*dx + dy*dy );
        ldx = FP16_DIV( INT_TO_FP16(dy), INT_TO_FP16(len) )>>1;
        ldy = FP16_DIV( INT_TO_FP16(dx), INT_TO_FP16(len) )>>1;
        ldx *= super->linewidth;
        ldy *= super->linewidth;

        x0 = INT_TO_FP16(x0);
        y0 = INT_TO_FP16(y0);
        x1 = INT_TO_FP16(x1);
        y1 = INT_TO_FP16(y1);

        points[0].x = x0 + ldx;
        points[0].y = y0 - ldy;
        points[1].x = x1 + ldx;
        points[1].y = y1 - ldy;
        points[2].x = x0 - ldx;
        points[2].y = y0 + ldy;
        points[3].x = x1 - ldx;
        points[3].y = y1 + ldy;

        XRenderCompositeTriStrip( this->dpy, PictOpOver, this->pen, this->pic,
                                  0, 0, 0, points, 4 );
    }
}

static void canvas_xrender_draw_circle( canvas* super, int cx, int cy,
                                                       int radius )
{
    canvas_xrender* this = (canvas_xrender*)super;
    int x, y, x2, y2, ox, oy, ox2, oy2, i, inner;
    XPointFixed points[4];

    cx = INT_TO_FP16(cx);
    cy = INT_TO_FP16(cy);
    radius = INT_TO_FP16(radius);
    inner = radius - INT_TO_FP16(super->linewidth);

    ox = cx + radius;
    oy = cy;
    ox2 = cx + inner;
    oy2 = cy;

    for( i=10; i<=360; i+=10, ox=x, oy=y, ox2=x2, oy2=y2 )
    {
        x = cosd16( i );
        y = sind16( i );

        x2 = cx + FP16_MUL(x, inner);
        y2 = cy + FP16_MUL(y, inner);
        x = cx + FP16_MUL(x, radius);
        y = cy + FP16_MUL(y, radius);

        points[0].x = ox2;
        points[0].y = oy2;
        points[1].x = x2;
        points[1].y = y2;
        points[2].x = ox;
        points[2].y = oy;
        points[3].x = x;
        points[3].y = y;

        XRenderCompositeTriStrip( this->dpy, PictOpOver, this->pen, this->pic,
                                  0, 0, 0, points, 4 );
    }
}

static void canvas_xrender_fill_rect( canvas* super, int x, int y,
                                                     int x1, int y1 )
{
    canvas_xrender* this = (canvas_xrender*)super;
    XRenderFillRectangle( this->dpy, PictOpOver, this->pic, &this->c,
                          x, y, x1-x, y1-y );
}

static void canvas_xrender_fill_triangle( canvas* super, int x0, int y0,
                                                         int x1, int y1,
                                                         int x2, int y2 )
{
    canvas_xrender* this = (canvas_xrender*)super;
    XTriangle t;

    t.p1.x = INT_TO_FP16( x0 );
    t.p1.y = INT_TO_FP16( y0 );
    t.p2.x = INT_TO_FP16( x1 );
    t.p2.y = INT_TO_FP16( y1 );
    t.p3.x = INT_TO_FP16( x2 );
    t.p3.y = INT_TO_FP16( y2 );

    XRenderCompositeTriangles( this->dpy, PictOpOver, this->pen, this->pic,
                               0, 0, 0, &t, 1 );
}

static void canvas_xrender_fill_circle( canvas* super, int cx, int cy,
                                                       int radius )
{
    canvas_xrender* this = (canvas_xrender*)super;
    XPointFixed points[ 38 ];
    int i;

    cx = INT_TO_FP16(cx);
    cy = INT_TO_FP16(cy);
    radius = INT_TO_FP16(radius);

    points[0].x = cx;
    points[0].y = cy;

    for( i=0; i<=360; i+=10 )
    {
        points[ (i/10) + 1 ].x = cx + FP16_MUL(cosd16( i ), radius);
        points[ (i/10) + 1 ].y = cy + FP16_MUL(sind16( i ), radius);
    }

    XRenderCompositeTriFan( this->dpy, PictOpOver, this->pen, this->pic,
                            0, 0, 0,
                            points, sizeof(points)/sizeof(points[0]) );
}

static void canvas_xrender_destroy( canvas* super )
{
    canvas_xrender* this = (canvas_xrender*)super;
    XRenderFreePicture( this->dpy, this->pic );
    XRenderFreePicture( this->dpy, this->pen );
    XFreePixmap( this->dpy, this->penmap );
    free( this );
}

/****************************************************************************/

canvas* canvas_xrender_create( Display* dpy, Drawable target,
                               unsigned int width, unsigned int height )
{
    XRenderPictureAttributes attr;
    XRenderPictFormat* rgba;
    XRenderPictFormat* rgb;
    canvas_xrender* this;
    int base, error;
    canvas* super;

    /* sanity check */
    if( !dpy || !XRenderQueryExtension( dpy, &base, &error ) )
        return NULL;

    /* create and initialize */
    this = malloc( sizeof(canvas_xrender) );
    super = (canvas*)this;

    if( !this )
        return NULL;

    memset( this, 0, sizeof(canvas_xrender) );

    /* get color format */
    rgba = XRenderFindStandardFormat( dpy, PictStandardARGB32 );
    rgb = XRenderFindStandardFormat( dpy, PictStandardRGB24 );

    if( !rgba || !rgb )
        goto fail;

    /* create picture */
    attr.poly_edge = PolyEdgeSmooth;
    this->pic = XRenderCreatePicture( dpy, target, rgb, CPPolyEdge, &attr );

    if( !this->pic )
        goto fail;

    /* create pen */
    this->penmap = XCreatePixmap( dpy, target, 1, 1, 32 );

    if( !this->penmap )
        goto fail;

    attr.repeat = RepeatNormal;
    this->pen = XRenderCreatePicture( dpy, this->penmap, rgba,
                                      CPRepeat, &attr );

    if( !this->pen )
        goto fail;

    this->dpy = dpy;
    super->width = width;
    super->height = height;
    super->linewidth = 1;

    super->set_color     = canvas_xrender_set_color;
    super->clear         = canvas_xrender_clear;
    super->draw_point    = canvas_xrender_draw_point;
    super->draw_line     = canvas_xrender_draw_line;
    super->draw_circle   = canvas_xrender_draw_circle;
    super->fill_rect     = canvas_xrender_fill_rect;
    super->fill_circle   = canvas_xrender_fill_circle;
    super->fill_triangle = canvas_xrender_fill_triangle;
    super->destroy       = canvas_xrender_destroy;

    return super;
fail:
    if( this->penmap ) XFreePixmap( this->dpy, this->penmap );
    if( this->pic    ) XRenderFreePicture( this->dpy, this->pic );
    if( this->pen    ) XRenderFreePicture( this->dpy, this->pen );
    free( this );
    return NULL;
}

