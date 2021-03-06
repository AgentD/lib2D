#include "testdraw.h"
#include "pixmap.h"



void testdraw( canvas* cv )
{
    unsigned char data[64*64*4];
    unsigned char* ptr;
    unsigned int x, y;
    pixmap* pic;

    canvas_set_line_width( cv, 1 );

    canvas_set_color( cv, 0, 0, 0, 0, 0 );
    canvas_set_color( cv, 1, 0xFF, 0xFF, 0xFF, 0xFF );
    canvas_clear( cv );

    for( y=300; y>=180; y-=20 )
    {
        canvas_draw_line( cv, 320, 240, 400, y );
        canvas_draw_line( cv, 320, 240, 240, y );
    }

    for( x=400; x>=240; x-=20 )
    {
        canvas_draw_line( cv, 320, 240, x, 300 );
        canvas_draw_line( cv, 320, 240, x, 180 );
    }

    for( y=220; y>=20; y-=20 )
    {
        canvas_set_color( cv, 1, 0xFF, 0xFF, 0xFF, y );
        canvas_draw_circle( cv, 320, 240, y );
    }

    canvas_set_color( cv, 1, 0xFF, 0x00, 0x00, 0xFF );
    canvas_draw_rect( cv, 240, 180, 400, 300 );
    canvas_fill_rect( cv,  20,  20,  50,  50 );

    canvas_set_color( cv, 1, 0xFF, 0xFF, 0xFF, 0x80 );
    canvas_set_color( cv, 0, 0xFF, 0, 0, 0 );
    canvas_fill_circle( cv, 35, 35, 15 );
    canvas_set_color( cv, 0, 0, 0, 0, 0 );

    canvas_set_color( cv, 1, 0x00, 0xFF, 0x00, 0x80 );
    canvas_fill_triangle( cv, 10, 470, 50, 470, 30, 440 );

    canvas_set_color( cv, 1, 0xFF, 0x00, 0x00, 0xFF );
    canvas_set_line_width( cv, 1 );
    canvas_draw_line( cv, 10, 200, 30, 100 );
    canvas_set_color( cv, 1, 0x00, 0xFF, 0x00, 0xFF );
    canvas_set_line_width( cv, 3 );
    canvas_draw_line( cv, 25, 200, 45, 100 );
    canvas_set_color( cv, 1, 0x00, 0x00, 0xFF, 0xFF );
    canvas_set_line_width( cv, 5 );
    canvas_draw_line( cv, 40, 200, 60, 100 );
    canvas_set_color( cv, 1, 0xFF, 0xFF, 0xFF, 0xFF );
    canvas_set_line_width( cv, 7 );
    canvas_draw_line( cv, 55, 200, 75, 100 );

    canvas_set_line_width( cv, 5 );
    canvas_set_color( cv, 1, 0xFF, 0xFF, 0xFF, 0x80 );
    canvas_draw_circle( cv, 45, 150, 30 );

    pic = canvas_create_pixmap( cv, 64, 64, COLOR_RGBA8 );

    for( ptr=data, y=0; y<64; ++y )
    {
        for( x=0; x<64; ++x, ptr+=4 )
        {
            ptr[0] = x<<2;
            ptr[1] = y<<2;
            ptr[2] = 0x00;
            ptr[3] = x<<2;
        }
    }

    pixmap_load( pic, 0, 0, 0, 0, 64, 64, 64, COLOR_RGBA8, data );
    canvas_blit_pixmap( cv, pic, 10, 330 );
    canvas_blend_pixmap( cv, pic, 128, 330 );
    pixmap_destroy( pic );




    pic = canvas_create_pixmap( cv, 16, 16, COLOR_A8 );

    for( ptr=data, y=0; y<16; ++y )
    {
        for( x=0; x<16; ++x, ++ptr )
        {
            *ptr = (x % 4) ? 0x00 : 0xFF;
            *ptr = (y % 4) ? *ptr : 0x80;
        }
    }

    pixmap_load( pic, 0, 0, 0, 0, 16, 16, 16, COLOR_A8, data );
    canvas_set_color( cv, 1, 0x00, 0x00, 0xFF, 0xFF );
    canvas_stencil_blend( cv, pic, 58, 330, 0, 0, 16, 16 );
    pixmap_destroy( pic );
}

