#include "xrender_canvas.h"
#include "pixmap.h"
#include "imath.h"

#include <X11/extensions/Xrender.h>
#include <stdlib.h>
#include <string.h>



typedef struct
{
    canvas super;

    Display* dpy;

    XRenderColor c;
    XRenderColor bg;

    Picture pen;
    Pixmap penmap;

    Picture pic;
    Drawable target;
}
canvas_xrender;

typedef struct
{
    pixmap super;
    canvas_xrender* owner;
    Pixmap xpm;
    Picture pic;
}
pixmap_xrender;



static void pixmap_xrender_load( pixmap* super, int dstx, int dsty,
                                 unsigned int width, unsigned int height,
                                 unsigned int scan, int format,
                                 unsigned char* data )
{
    pixmap_xrender* this = (pixmap_xrender*)super;
    unsigned int x, y, bpp, R, G, B, A;
    unsigned char* src_row;
    XRenderColor c;

    if( format==COLOR_A8 && super->format==COLOR_A8 )
    {
        for( y=0; y<height; ++y, data+=scan )
        {
            for( src_row=data, x=0; x<width; ++x )
            {
                c.red = c.green = c.blue = 0xFFFF;
                c.alpha = (*(src_row++))<<8;
                XRenderFillRectangle( this->owner->dpy, PictOpSrc, this->pic,
                                      &c, dstx+x, dsty+y, 1, 1 );
            }
        }
    }
    else
    {
        bpp = format==COLOR_RGBA8 ? 4 : (format==COLOR_RGB8 ? 3 : 1);

        for( y=0; y<height; ++y, data+=scan*bpp )
        {
            for( src_row=data, x=0; x<width; ++x, src_row+=bpp )
            {
                R = src_row[0];
                G = bpp>1 ? src_row[1] : R;
                B = bpp>1 ? src_row[2] : R;
                A = bpp>3 ? src_row[3] : 0xFF;

                c.red   = R*A;
                c.green = G*A;
                c.blue  = B*A;
                c.alpha = A<<8;

                XRenderFillRectangle( this->owner->dpy, PictOpSrc, this->pic,
                                      &c, dstx+x, dsty+y, 1, 1 );
            }
        }
    }
}

static void pixmap_xrender_destroy( pixmap* super )
{
    pixmap_xrender* this = (pixmap_xrender*)super;
    XRenderFreePicture( this->owner->dpy, this->pic );
    XFreePixmap( this->owner->dpy, this->xpm );
    free( this );
}

static pixmap* canvas_xrender_create_pixmap( canvas* super,
                                             unsigned int width,
                                             unsigned int height,
                                             int format )
{
    canvas_xrender* this = (canvas_xrender*)super;
    XRenderPictFormat* fmt;
    pixmap_xrender* pix;
    unsigned int bpp;

    if( format==COLOR_RGBA8 )
        fmt = XRenderFindStandardFormat( this->dpy, PictStandardARGB32 );
    else if( format==COLOR_RGB8 )
        fmt = XRenderFindStandardFormat( this->dpy, PictStandardRGB24 );
    else
        fmt = XRenderFindStandardFormat( this->dpy, PictStandardA8 );

    if( !fmt )
        return NULL;

    pix = malloc( sizeof(pixmap_xrender) );

    if( !pix )
        return NULL;

    pix->super.width   = width;
    pix->super.height  = height;
    pix->super.format  = format;
    pix->super.load    = pixmap_xrender_load;
    pix->super.destroy = pixmap_xrender_destroy;

    bpp = format==COLOR_RGBA8 ? 32 : (format==COLOR_RGB8 ? 24 : 8);

    pix->owner = this;
    pix->xpm = XCreatePixmap(this->dpy, this->target, width, height, bpp);

    if( !pix->xpm )
    {
        free( pix );
        return NULL;
    }

    pix->pic = XRenderCreatePicture( this->dpy, pix->xpm, fmt, 0, NULL );

    if( !pix->pic )
    {
        XFreePixmap( this->dpy, pix->xpm );
        free( pix );
        return NULL;
    }

    return (pixmap*)pix;
}

static void canvas_xrender_set_color( canvas* super, int fg,
                                      unsigned char r, unsigned char g,
                                      unsigned char b, unsigned char a )
{
    canvas_xrender* this = (canvas_xrender*)super;

    if( fg )
    {
        this->c.red   = r*a;
        this->c.green = g*a;
        this->c.blue  = b*a;
        this->c.alpha = a<<8;

        XRenderFillRectangle( this->dpy, PictOpSrc, this->pen, &this->c,
                              0, 0, 1, 1 );
    }
    else
    {
        this->bg.red   = r*a;
        this->bg.green = g*a;
        this->bg.blue  = b*a;
        this->bg.alpha = a<<8;
    }
}

static void canvas_xrender_clear( canvas* super )
{
    canvas_xrender* this = (canvas_xrender*)super;
    XRenderFillRectangle( this->dpy, PictOpSrc, this->pic, &this->bg,
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

static void canvas_xrender_blit_pixmap( canvas* super, pixmap* pm,
                                        int x, int y )
{
    canvas_xrender* this = (canvas_xrender*)super;
    pixmap_xrender* pix = (pixmap_xrender*)pm;
    XRenderColor c;

    c.red = c.green = c.blue = 0;
    c.alpha = 0xFFFF;

    XRenderFillRectangle( this->dpy, PictOpSrc, this->pic, &c, x, y,
                          pm->width, pm->height );
    XRenderComposite( this->dpy, PictOpOver, pix->pic, 0,
                      this->pic, 0, 0, 0, 0, x, y, pm->width, pm->height );
}

static void canvas_xrender_blend_pixmap( canvas* super, pixmap* pm,
                                         int x, int y )
{
    canvas_xrender* this = (canvas_xrender*)super;
    pixmap_xrender* pix = (pixmap_xrender*)pm;

    XRenderComposite( this->dpy, PictOpOver, pix->pic, 0,
                      this->pic, 0, 0, 0, 0, x, y, pm->width, pm->height );
}

static void canvas_xrender_stencil( canvas* super, pixmap* pm, int x, int y,
                                    int srcx, int srcy,
                                    unsigned int width, unsigned int height )
{
    canvas_xrender* this = (canvas_xrender*)super;
    pixmap_xrender* pix = (pixmap_xrender*)pm;

    XRenderComposite( this->dpy, PictOpOver, this->pen, pix->pic, this->pic,
                      0, 0, srcx, srcy, x, y, width, height );

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
    this->target = target;
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
    super->blit_pixmap   = canvas_xrender_blit_pixmap;
    super->blend_pixmap  = canvas_xrender_blend_pixmap;
    super->stencil       = canvas_xrender_stencil;
    super->create_pixmap = canvas_xrender_create_pixmap;
    super->destroy       = canvas_xrender_destroy;

    return super;
fail:
    if( this->penmap ) XFreePixmap( this->dpy, this->penmap );
    if( this->pic    ) XRenderFreePicture( this->dpy, this->pic );
    if( this->pen    ) XRenderFreePicture( this->dpy, this->pen );
    free( this );
    return NULL;
}

