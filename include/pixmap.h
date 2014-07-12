#ifndef PIXMAP_H
#define PIXMAP_H



#include "predef.h"



struct pixmap
{
    unsigned int width, height;
    int format;

    void (* load )( pixmap* pix, int dstx, int dsty,
                    unsigned int width, unsigned int height,
                    unsigned int scan, int format, unsigned char* data );

    void (* destroy )( pixmap* pix );
};



/**
 * \brief Load image data into a pixmap
 *
 * \param pix A pointer to a pixmap
 * \param srcx A horizontal offset into the source image data
 * \param srcy A vertical offset into the source image data
 * \param dstx A horizontal offset into the destination pixmap
 * \param dsty A vertical offset into the destination pixmap
 * \param width The width of the sub image to copy into the pixmap
 * \param height The height of the sub image to copy into the pixmap
 * \param scan The number of pixels requred to get from one scan line to
 *             the next
 * \param format The color format of the image data
 * \param data A pointer to the image data
 */
void pixmap_load( pixmap* pix, int srcx, int srcy, int dstx, int dsty,
                  unsigned int width, unsigned int height,
                  unsigned int scan, int format, unsigned char* data );

/**
 * \brief Destroy a pixmap and free all its resources
 *
 * \param pix A pointer to a pixmap
 */
void pixmap_destroy( pixmap* pix );



#endif /* PIXMAP_H */

