#include "testdraw.h"



void testdraw( canvas* cv )
{
    unsigned int x, y;

    canvas_set_line_width( cv, 1 );

    canvas_set_color( cv, 0, 0, 0, 0 );
    canvas_clear( cv );
    canvas_set_color( cv, 0xFF, 0xFF, 0xFF, 0xFF );

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
        canvas_set_color( cv, 0xFF, 0xFF, 0xFF, y );
        canvas_draw_circle( cv, 320, 240, y );
    }

    canvas_set_color( cv, 0xFF, 0x00, 0x00, 0xFF );
    canvas_draw_rect( cv, 240, 180, 400, 300 );
    canvas_fill_rect( cv,  20,  20,  50,  50 );

    canvas_set_color( cv, 0xFF, 0xFF, 0xFF, 0x80 );
    canvas_fill_circle( cv, 35, 35, 15 );

    canvas_set_color( cv, 0x00, 0xFF, 0x00, 0x80 );
    canvas_fill_triangle( cv, 10, 470, 50, 470, 30, 440 );

    canvas_set_color( cv, 0xFF, 0x00, 0x00, 0xFF );
    canvas_set_line_width( cv, 1 );
    canvas_draw_line( cv, 10, 200, 30, 100 );
    canvas_set_color( cv, 0x00, 0xFF, 0x00, 0xFF );
    canvas_set_line_width( cv, 3 );
    canvas_draw_line( cv, 25, 200, 45, 100 );
    canvas_set_color( cv, 0x00, 0x00, 0xFF, 0xFF );
    canvas_set_line_width( cv, 5 );
    canvas_draw_line( cv, 40, 200, 60, 100 );
    canvas_set_color( cv, 0xFF, 0xFF, 0xFF, 0xFF );
    canvas_set_line_width( cv, 7 );
    canvas_draw_line( cv, 55, 200, 75, 100 );

    canvas_set_line_width( cv, 5 );
    canvas_set_color( cv, 0xFF, 0xFF, 0xFF, 0x80 );
    canvas_draw_circle( cv, 45, 150, 30 );
}

