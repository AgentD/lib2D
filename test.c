#include <stdlib.h>
#include <stdio.h>

#include "canvas.h"
#include "mem_canvas.h"
#include "testdraw.h"



int main( void )
{
    unsigned char* ptr;
    unsigned int x, y;
    canvas* cv;
    FILE* f;

    cv = canvas_memory_create( 640, 480 );

    testdraw( cv );

    ptr = canvas_memory_get_buffer( cv );
    f = fopen( "out.ppm", "wb" );
    fprintf( f, "P3\n%d %d\n%d\n",
             canvas_get_width( cv ), canvas_get_height( cv ), 255 );

    for( y=0; y<canvas_get_height( cv ); ++y )
    {
        for( x=0; x<canvas_get_width( cv ); ++x, ptr+=4 )
        {
            fprintf( f, "%d %d %d ", ptr[0], ptr[1], ptr[2] );
        }
    }

    fclose( f );
    canvas_destroy( cv );
    return 0;
}

