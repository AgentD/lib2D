#include "canvas.h"
#include "imath.h"



void canvas_destroy( canvas* this )
{
    if( this )
        this->destroy( this );
}

void canvas_set_line_width( canvas* this, unsigned int width )
{
    if( this )
        this->linewidth = width>1 ? width : 1;
}

unsigned int canvas_get_line_width( canvas* this )
{
    return this ? this->linewidth : 0;
}

unsigned int canvas_get_width( canvas* this )
{
    return this ? this->width : 0;
}

unsigned int canvas_get_height( canvas* this )
{
    return this ? this->height : 0;
}

void canvas_set_color( canvas* this, unsigned char r, unsigned char g,
                                     unsigned char b, unsigned char a )
{
    if( this )
        this->set_color( this, r, g, b, a );
}

void canvas_clear( canvas* this )
{
    if( this )
        this->clear( this );
}

void canvas_draw_point( canvas* this, unsigned int x, unsigned int y )
{
    if( !this || x>=this->width || y>=this->height )
        return;

    this->draw_point( this, x, y );
}

void canvas_draw_line( canvas* this, int x0, int y0, int x1, int y1 )
{
    if( !this || (x0<0 && x1<0) )
        return;

    if( x0>=(int)this->width && x1>=(int)this->width )
        return;

    if( (y0<0 && y1<0) || (y0>=(int)this->height && y1>=(int)this->height) )
        return;

    this->draw_line( this, x0, y0, x1, y1 );
}

void canvas_draw_circle( canvas* this, int cx, int cy, int radius )
{
    if( !this || !radius || radius<0 )
        return;

    if( (cx+radius)<0 || ((cx-radius)>=(int)this->width) )
        return;

    if( (cy+radius)<0 || ((cy-radius)>=(int)this->height) )
        return;

    this->draw_circle( this, cx, cy, radius );
}

void canvas_draw_rect( canvas* this, int x, int y, int x1, int y1 )
{
    int temp;

    if( x>x1 ) { temp=x1; x1=x; x=temp; }
    if( y>y1 ) { temp=y1; y1=y; y=temp; }

    if( !this || x>=(int)this->width || y>=(int)this->height || x1<0 || y1<0 )
        return;

    canvas_draw_line( this, x, y, x1, y );
    canvas_draw_line( this, x, y1, x1, y1 );
    canvas_draw_line( this, x, y+1, x, y1-1 );
    canvas_draw_line( this, x1, y+1, x1, y1-1 );
}

void canvas_fill_rect( canvas* this, int x, int y, int x1, int y1 )
{
    int temp;

    if( x>x1 ) { temp=x1; x1=x; x=temp; }
    if( y>y1 ) { temp=y1; y1=y; y=temp; }

    if( !this || x>=(int)this->width || y>=(int)this->height || x1<0 || y1<0 )
        return;

    x  = x <0 ? 0 : (x >=(int)this->width  ? (int)this->width-1  : x);
    y  = y <0 ? 0 : (y >=(int)this->height ? (int)this->height-1 : y);
    x1 = x1<0 ? 0 : (x1>=(int)this->width  ? (int)this->width-1  : x1);
    y1 = y1<0 ? 0 : (y1>=(int)this->height ? (int)this->height-1 : y1);

    this->fill_rect( this, x, y, x1, y1 );
}

void canvas_fill_circle( canvas* this, int cx, int cy, int radius )
{
    if( !this || !radius || radius<0 )
        return;

    if( (cx+radius)<0 || ((cx-radius)>=(int)this->width) )
        return;

    if( (cy+radius)<0 || ((cy-radius)>=(int)this->height) )
        return;

    this->fill_circle( this, cx, cy, radius );
}

void canvas_fill_triangle( canvas* this, int x0, int y0,
                                         int x1, int y1,
                                         int x2, int y2 )
{
    if( !this || (x0<0 && x1<0 && x2<0) || (y0<0 && y1<0 && y2<0) )
        return;

    if( x0>=(int)this->width && x1>=(int)this->width && x2>=(int)this->width )
        return;

    if( y0>=(int)this->height&&y1>=(int)this->height&&y2>=(int)this->height )
        return;

    this->fill_triangle( this, x0, y0, x1, y1, x2, y2 );
}

