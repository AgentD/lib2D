#include "pixmap.h"



void pixmap_load( pixmap* this, int srcx, int srcy, int dstx, int dsty,
                  unsigned int width, unsigned int height,
                  unsigned int scan, int format, unsigned char* data )
{
    if( !this || !data || format<0 || format>3 || scan<width )
        return;

    if( srcx<0 || srcy<0 || srcx>=(int)width || srcy>=(int)height )
        return;

    if( dstx>=(int)this->width || dsty>=(int)this->height )
        return;

    if( (dstx+(int)width)<0 || (dsty+(int)height)<0 )
        return;

    if( (srcx+width-1)>=scan )
        width = scan - srcx;

    if( dsty<0 ) { height += dsty; dsty = 0; }
    if( dstx<0 ) { width  += dstx; dstx = 0; }

    if( (dstx+width -1) >= this->width  ) width  = this->width  - dstx;
    if( (dsty+height-1) >= this->height ) height = this->height - dsty;

         if( format==COLOR_RGBA8 ) { data += (dsty*scan + dstx)*4; }
    else if( format==COLOR_RGB8  ) { data += (dsty*scan + dstx)*3; }
    else                           { data +=  dsty*scan + dstx;    }

    this->load( this, dstx, dsty, width, height, scan, format, data );
}

void pixmap_destroy( pixmap* this )
{
    if( this )
        this->destroy( this );
}

