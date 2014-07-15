#include <stdlib.h>

#include "mem_canvas.h"
#include "pixmap.h"
#include "imath.h"



#define BLEND( dst, src, a, ia )\
        dst[0] = (src[0]*a + dst[0]*ia)>>8;\
        dst[1] = (src[1]*a + dst[1]*ia)>>8;\
        dst[2] = (src[2]*a + dst[2]*ia)>>8;\
        dst[3] = ((a<<8)   + dst[3]*ia)>>8



typedef struct
{
    canvas super;
    unsigned char* data;
    unsigned int stride;
    unsigned char pen[ 4 ];
    unsigned char bg[ 4 ];
    int my_buffer;
    int swaprb;
}
canvas_memory;

typedef struct
{
    pixmap super;
    unsigned int bpp;
    unsigned char* data;
}
pixmap_memory;



static void pixmap_mem_load_rgba( pixmap* super, int dstx, int dsty,
                                  unsigned int width, unsigned int height,
                                  unsigned int scan, int format,
                                  unsigned char* data )
{
    pixmap_memory* this = (pixmap_memory*)super;
    unsigned int x, y, bpp, R, G, B, A;
    unsigned char* dst_row;
    unsigned char* src_row;
    unsigned char* dst;

    dst = this->data + (dsty*super->width + dstx)*this->bpp;
    bpp = format==COLOR_RGBA8 ? 4 : (format==COLOR_RGB8 ? 3 : 1);

    for( y=0; y<height; ++y, data+=scan*bpp, dst+=super->width*this->bpp )
    {
        dst_row = dst;
        src_row = data;

        for( x=0; x<width; ++x )
        {
            R = *(src_row++);
            G = bpp>1 ? *(src_row++) : R;
            B = bpp>1 ? *(src_row++) : R;
            A = bpp>3 ? *(src_row++) : 0xFF;

                              *(dst_row++) = (R*A) >> 8;
            if( this->bpp>1 ) *(dst_row++) = (G*A) >> 8;
            if( this->bpp>1 ) *(dst_row++) = (B*A) >> 8;
            if( this->bpp>3 ) *(dst_row++) = A;
        }
    }
}

static void pixmap_mem_load_bgra( pixmap* super, int dstx, int dsty,
                                  unsigned int width, unsigned int height,
                                  unsigned int scan, int format,
                                  unsigned char* data )
{
    pixmap_memory* this = (pixmap_memory*)super;
    unsigned int x, y, bpp, R, G, B, A;
    unsigned char* dst_row;
    unsigned char* src_row;
    unsigned char* dst;

    dst = this->data + (dsty*super->width + dstx)*this->bpp;
    bpp = format==COLOR_RGBA8 ? 4 : (format==COLOR_RGB8 ? 3 : 1);

    for( y=0; y<height; ++y, data+=scan*bpp, dst+=super->width*this->bpp )
    {
        dst_row = dst;
        src_row = data;

        for( x=0; x<width; ++x )
        {
            R = *(src_row++);
            G = bpp>1 ? *(src_row++) : R;
            B = bpp>1 ? *(src_row++) : R;
            A = bpp>3 ? *(src_row++) : 0xFF;

                              *(dst_row++) = (B*A) >> 8;
            if( this->bpp>1 ) *(dst_row++) = (G*A) >> 8;
            if( this->bpp>1 ) *(dst_row++) = (R*A) >> 8;
            if( this->bpp>3 ) *(dst_row++) = A;
        }
    }
}

static void pixmap_mem_destroy( pixmap* this )
{
    free( ((pixmap_memory*)this)->data );
    free( this );
}

static pixmap* canvas_mem_create_pixmap( canvas* this, unsigned int width,
                                         unsigned int height, int format )
{
    pixmap_memory* pix;
    (void)this;

    pix = malloc( sizeof(pixmap_memory) );

    if( pix )
    {
        pix->super.width   = width;
        pix->super.height  = height;
        pix->super.format  = format;
        pix->super.load    = ((canvas_memory*)this)->swaprb ?
                             pixmap_mem_load_bgra :
                             pixmap_mem_load_rgba;
        pix->super.destroy = pixmap_mem_destroy;

        pix->bpp = format==COLOR_RGBA8 ? 4 : (format==COLOR_RGB8 ? 3 : 1);
        pix->data = malloc( width*height*pix->bpp );

        if( !pix->data )
        {
            free( pix );
            pix = NULL;
        }
    }

    return (pixmap*)pix;
}

static void canvas_mem_destroy( canvas* this )
{
    if( ((canvas_memory*)this)->my_buffer )
        free( ((canvas_memory*)this)->data );

    free( this );
}

static void canvas_mem_set_color( canvas* super, int fg,
                                  unsigned char r, unsigned char g,
                                  unsigned char b, unsigned char a )
{
    canvas_memory* this = (canvas_memory*)super;

    if( fg )
    {
        this->pen[ 0 ] = this->swaprb ? b : r;
        this->pen[ 1 ] = g;
        this->pen[ 2 ] = this->swaprb ? r : b;
        this->pen[ 3 ] = a;
    }
    else
    {
        this->bg[ 0 ] = this->swaprb ? b : r;
        this->bg[ 1 ] = g;
        this->bg[ 2 ] = this->swaprb ? r : b;
        this->bg[ 3 ] = a;
    }
}

static void canvas_mem_clear( canvas* this )
{
    unsigned char* ptr = ((canvas_memory*)this)->data;
    unsigned int x, y;

    for( y=0; y<this->height; ++y )
    {
        for( x=0; x<this->width; ++x, ptr+=4 )
        {
            ptr[ 0 ] = ((canvas_memory*)this)->bg[ 0 ];
            ptr[ 1 ] = ((canvas_memory*)this)->bg[ 1 ];
            ptr[ 2 ] = ((canvas_memory*)this)->bg[ 2 ];
            ptr[ 3 ] = ((canvas_memory*)this)->bg[ 3 ];
        }
    }
}

static void canvas_mem_fill_rect( canvas* super, int x, int y,
                                                 int x1, int y1 )
{
    canvas_memory* this = (canvas_memory*)super;
    unsigned char *p0, *p1;
    int a = this->pen[3], ia = 0xFF-a;
    int i, j, temp;

    if( x>x1 ) { temp=x; x=x1; x1=temp; }
    if( y>y1 ) { temp=y; y=y1; y1=temp; }

    p0 = this->data + y*this->stride + x*4;

    for( i=y; i<=y1; ++i, p0+=this->stride )
    {
        for( p1=p0, j=x; j<=x1; ++j, p1+=4 )
        {
            BLEND( p1, this->pen, a, ia );
        }
    }
}

static void canvas_mem_draw_point( canvas* super,
                                   unsigned int x, unsigned int y )
{
    canvas_memory* this = (canvas_memory*)super;
    unsigned char* ptr = this->data + y*this->stride + x*4;
    int a = this->pen[3], ia = 0xFF-a;

    BLEND( ptr, this->pen, a, ia );
}

static void canvas_mem_fill_triangle( canvas* super, int x0, int y0,
                                                     int x1, int y1,
                                                     int x2, int y2 )
{
    canvas_memory* this = (canvas_memory*)super;
    int f0, f1, f2, f3, f4, f5, f6, f7, f8;
    int a, b, c, x, y, bl, br, bt, bb;
    int A = this->pen[3], iA = 0xFF-A;
    unsigned char *scan, *ptr;

    /* compute bounding rectangle */
    bl = x0<x1 ? (x0<x2 ? x0 : x2) : (x1<x2 ? x1 : x2);
    br = x0>x1 ? (x0>x2 ? x0 : x2) : (x1>x2 ? x1 : x2);
    bt = y0<y1 ? (y0<y2 ? y0 : y2) : (y1<y2 ? y1 : y2);
    bb = y0>y1 ? (y0>y2 ? y0 : y2) : (y1>y2 ? y1 : y2);

    /* clamp to drawing area */
    bl =  bl<0                    ?                     0  : bl;
    br = (br>=(int)super->width)  ? ((int)super->width -1) : br;
    bt =  bt<0                    ?                     0  : bt;
    bb = (bb>=(int)super->height) ? ((int)super->height-1) : bb;

    /* stop if the bounding rect is invalid or outside screen area */
    if( bl>=br || bt>=bb )
        return;

    if( bb<0 || br<0 || bt>=(int)super->height || bl>=(int)super->width )
        return;

    /* precompute factors for baricentric interpolation */
    f0  = x1*y2 - x2*y1;
    f1  = x2*y0 - x0*y2;
    f2  = x0*y1 - x1*y0;
    f3  = y1 - y2;
    f4  = y2 - y0;
    f5  = y0 - y1;
    f6  = x2 - x1;
    f7  = x0 - x2;
    f8  = x1 - x0;

    if( (f3*x0 + f6*y0 + f0)<0 ) { f3=-f3; f6=-f6; f0=-f0; }
    if( (f4*x1 + f7*y1 + f1)<0 ) { f4=-f4; f7=-f7; f1=-f1; }
    if( (f5*x2 + f8*y2 + f2)<0 ) { f5=-f5; f8=-f8; f2=-f2; }

    /* for each scanline in the triangle */
    scan = this->data + (bt*super->width + bl)*4;

    for( y=bt; y<=bb; ++y, scan+=this->stride )
    {
        /* for each pixel in the current scanline */
        for( ptr=scan, x=bl; x<=br; ++x, ptr+=4 )
        {
            /* check if denormalized baricentric coordinates are valid */
            a = f3*x + f6*y + f0;
            b = f4*x + f7*y + f1;
            c = f5*x + f8*y + f2;

            if( a<0 || b<0 || c<0 )
                continue;

            BLEND( ptr, this->pen, A, iA );
        }
    }
}

static void canvas_mem_draw_line( canvas* super, int x0, int y0,
                                                 int x1, int y1 )
{
    canvas_memory* this = (canvas_memory*)super;
    int dx, dy, sx, spx, sy, spy, a, ia, err, e2;
    unsigned char* ptr;

    if( super->linewidth>1 )
    {
        dx = x1 - x0;
        dy = y1 - y0;
        a = isqrt16( dx*dx + dy*dy );
        sx = (INT_TO_FP16(dy)>>1) / a;
        sy = (INT_TO_FP16(dx)>>1) / a;
        sx *= super->linewidth;
        sy *= super->linewidth;

        x0 = INT_TO_FP16(x0);
        y0 = INT_TO_FP16(y0);
        x1 = INT_TO_FP16(x1);
        y1 = INT_TO_FP16(y1);

        canvas_mem_fill_triangle( super,
                                  FP16_TO_INT(x0+sx), FP16_TO_INT(y0-sy),
                                  FP16_TO_INT(x1+sx), FP16_TO_INT(y1-sy),
                                  FP16_TO_INT(x0-sx), FP16_TO_INT(y0+sy) );

        canvas_mem_fill_triangle( super,
                                  FP16_TO_INT(x0-sx), FP16_TO_INT(y0+sy),
                                  FP16_TO_INT(x1+sx), FP16_TO_INT(y1-sy),
                                  FP16_TO_INT(x1-sx), FP16_TO_INT(y1+sy) );
    }
    else
    {
        ptr = this->data + y0*this->stride + x0*4;
        dx = x1<x0 ? x0-x1 : x1-x0;
        dy = y1<y0 ? y0-y1 : y1-y0;
        sx = x0<x1 ? 1 : -1, spx = sx*4;
        sy = y0<y1 ? 1 : -1, spy = sy*this->stride;
        a = this->pen[3];
        ia = 0xFF-a;
        err = dx-dy;

        while( x0<(int)super->width && y0<(int)super->height )
        {
            if( x0>0 && y0>0 )
            {
                BLEND( ptr, this->pen, a, ia );
            }

            if( x0==x1 && y0==y1 )
                break;

            e2 = err*2;

            if( e2 > -dy )
            {
                err -= dy;
                x0 += sx;
                ptr += spx;
            }

            if( x0==x1 && y0==y1 )
            {
                if( x0>0 && y0>0 )
                {
                    BLEND( ptr, this->pen, a, ia );
                }
                break;
            }

            if( e2 < dx )
            {
                err += dx;
                y0 += sy;
                ptr += spy;
            }
        }
    }
}

static void canvas_mem_draw_circle( canvas* super, int cx, int cy,
                                                   int radius )
{
    canvas_memory* this = (canvas_memory*)super;
    unsigned char* p0 = this->data + (cy-radius)*this->stride + (cx-radius)*4;
    unsigned char* p1;
    int a = this->pen[3], ia = 0xFF-a;
    int x, y, d2;

    for( y=-radius; y<=radius; ++y, p0+=this->stride )
    {
        if( (cy+y)<0 )
            continue;

        if( (cy+y)>=(int)super->height )
            break;

        for( p1=p0, x=-radius; x<=radius; ++x, p1+=4 )
        {
            if( (cx+x)<0 )
                continue;

            if( (cx+x)>=(int)super->width )
                break;

            d2 = x*x + y*y;
            d2 = isqrt16( d2 );

            if( d2>(radius-(int)super->linewidth) && d2<=radius )
            {
                BLEND( p1, this->pen, a, ia );
            }
        }
    }
}

static void canvas_mem_fill_circle( canvas* super, int cx, int cy,
                                                   int radius )
{
    canvas_memory* this = (canvas_memory*)super;
    unsigned char* p0 = this->data + (cy-radius)*this->stride + (cx-radius)*4;
    int x, y, a = this->pen[3], ia = 0xFF-a;
    unsigned char* p1;

    for( y=-radius; y<=radius; ++y, p0+=this->stride )
    {
        if( (cy+y)<0 )
            continue;

        if( (cy+y)>=(int)super->height )
            break;

        for( p1=p0, x=-radius; x<=radius; ++x, p1+=4 )
        {
            if( (cx+x)<0 )
                continue;

            if( (cx+x)>=(int)super->width )
                break;

            if( isqrt16( x*x + y*y ) <= radius )
            {
                BLEND( p1, this->pen, a, ia );
            }
        }
    }
}

static void canvas_mem_blit_pixmap( canvas* super, pixmap* pm, int x, int y )
{
    canvas_memory* this = (canvas_memory*)super;
    pixmap_memory* pix = (pixmap_memory*)pm;
    unsigned int X, Y, R, G, B;
    unsigned char* dst_row;
    unsigned char* src;
    unsigned char* dst;

    dst = this->data + (y*super->width + x)*4;
    src = pix->data;

    for( Y=0; Y<pm->height; ++Y, dst+=super->width*4 )
    {
        if( (y+(int)Y)<0 )
            continue;
        if( (y+Y)>=super->height )  
            break;

        for( dst_row=dst, X=0; X<pm->width; ++X, src+=pix->bpp )
        {
            if( (x+(int)X)<0 )
                continue;
            if( (x+X)>=super->width )  
                break;

            R = src[0];
            G = pix->bpp>1 ? src[1] : R;
            B = pix->bpp>1 ? src[2] : R;

            *(dst_row++) = R;
            *(dst_row++) = G;
            *(dst_row++) = B;
            *(dst_row++) = 0xFF;
        }
    }
}

static void canvas_memory_blend_pixmap( canvas* super, pixmap* pm,
                                        int x, int y )
{
    canvas_memory* this = (canvas_memory*)super;
    pixmap_memory* pix = (pixmap_memory*)pm;
    unsigned int X, Y, R, G, B, A;
    unsigned char* dst_row;
    unsigned char* src;
    unsigned char* dst;

    dst = this->data + (y*super->width + x)*4;
    src = pix->data;

    for( Y=0; Y<pm->height; ++Y, dst+=super->width*4 )
    {
        if( (y+(int)Y)<0 )
            continue;
        if( (y+Y)>=super->height )
            break;

        for( dst_row=dst, X=0; X<pm->width; ++X, src+=pix->bpp, dst_row+=4 )
        {
            if( (x+(int)X)<0 )
                continue;
            if( (x+X)>=super->width )
                break;

            R = src[0];
            G = pix->bpp>1 ? src[1] : R;
            B = pix->bpp>1 ? src[2] : R;
            A = pix->bpp>3 ? src[3] : A;

            dst_row[0] = R + ((dst_row[0]*(0xFF-A))>>8);
            dst_row[1] = G + ((dst_row[1]*(0xFF-A))>>8);
            dst_row[2] = B + ((dst_row[2]*(0xFF-A))>>8);
            dst_row[3] = ((A<<8) + (dst_row[3]*(0xFF-A)))>>8;
        }
    }
}

static void canvas_memory_stencil( canvas* super, pixmap* pm, int x, int y,
                                   int srcx, int srcy,
                                   unsigned int width, unsigned int height )
{
    canvas_memory* this = (canvas_memory*)super;
    pixmap_memory* pix = (pixmap_memory*)pm;
    unsigned int X, Y, A;
    unsigned char* dst_row;
    unsigned char* src_row;
    unsigned char* src;
    unsigned char* dst;

    dst = this->data + (y*super->width + x)*4;
    src = pix->data + (srcy*pm->width + srcx);

    for( Y=0; Y<height; ++Y, dst+=super->width*4, src+=pm->width )
    {
        if( (y+(int)Y)<0 )
            continue;
        if( (y+Y)>=super->height )
            break;

        dst_row = dst;
        src_row = src;

        for( X=0; X<width; ++X, dst_row+=4 )
        {
            if( (x+(int)X)<0 )
                continue;
            if( (x+X)>=super->width )
                break;

            A = *src_row++;
            dst_row[0] = (this->pen[0]*A + dst_row[0]*(0xFF-A))>>8;
            dst_row[1] = (this->pen[1]*A + dst_row[1]*(0xFF-A))>>8;
            dst_row[2] = (this->pen[2]*A + dst_row[2]*(0xFF-A))>>8;
            dst_row[3] = ((A<<8) + (dst_row[3]*(0xFF-A)))>>8;
        }
    }
}

canvas* canvas_memory_create( unsigned int width, unsigned int height,
                              int swaprb )
{
    unsigned char* buffer;
    canvas* this;

    buffer = malloc( width * height * 4 );
    this = canvas_memory_create_ext( buffer, width, height, swaprb );
    ((canvas_memory*)this)->my_buffer = 1;

    return this;
}

canvas* canvas_memory_create_ext( unsigned char* buffer,
                                  unsigned int width, unsigned int height,
                                  int swaprb )
{
    canvas_memory* this = malloc( sizeof(canvas_memory) );
    canvas* super = (canvas*)this;

    this->my_buffer = 0;
    this->swaprb = swaprb;
    this->data = buffer;
    this->stride = width * 4;
    this->pen[ 0 ] = 0x00;
    this->pen[ 1 ] = 0x00;
    this->pen[ 2 ] = 0x00;
    this->pen[ 3 ] = 0xFF;
    this->bg[ 0 ] = 0x00;
    this->bg[ 1 ] = 0x00;
    this->bg[ 2 ] = 0x00;
    this->bg[ 3 ] = 0xFF;
    super->width = width;
    super->height = height;
    super->linewidth = 1;

    super->set_color = canvas_mem_set_color;
    super->clear = canvas_mem_clear;
    super->draw_point = canvas_mem_draw_point;
    super->draw_line = canvas_mem_draw_line;
    super->draw_circle = canvas_mem_draw_circle;
    super->fill_rect = canvas_mem_fill_rect;
    super->fill_circle = canvas_mem_fill_circle;
    super->fill_triangle = canvas_mem_fill_triangle;
    super->blit_pixmap = canvas_mem_blit_pixmap;
    super->blend_pixmap = canvas_memory_blend_pixmap;
    super->stencil = canvas_memory_stencil;
    super->create_pixmap = canvas_mem_create_pixmap;
    super->destroy = canvas_mem_destroy;

    return super;
}

unsigned char* canvas_memory_get_buffer( canvas* cv )
{
    return ((canvas_memory*)cv)->data;
}

