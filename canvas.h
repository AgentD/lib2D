#ifndef CANVAS_H
#define CANVAS_H



typedef struct canvas canvas;

struct canvas
{
    unsigned int width, height;
    unsigned int linewidth;

    void (* set_color )( canvas* cv, unsigned char r, unsigned char g,
                                     unsigned char b, unsigned char a );


    void (* clear )( canvas* cv );
    void (* draw_point )( canvas* cv, unsigned int x, unsigned int y );
    void (* draw_line )( canvas* cv, int x0, int y0, int x1, int y1 );
    void (* draw_circle )( canvas* cv, int cx, int cy, int radius );
    void (* fill_rect )( canvas* cv, int x, int y, int x1, int y1 );
    void (* fill_circle )( canvas* cv, int cx, int cy, int radius );
    void (* fill_triangle )( canvas* cv, int x0, int y0,
                                         int x1, int y1,
                                         int x2, int y2 );

    void (* destroy )( canvas* cv );
};



/**
 * \brief Destroy a canvas and free all its memory
 *
 * \param cv A pointer to a canvas
 */
void canvas_destroy( canvas* cv );

/**
 * \brief Get the width of a canvas in pixels
 *
 * \param cv A pointer to a canvas
 *
 * \return The width in pixels
 */
unsigned int canvas_get_width( canvas* cv );

/**
 * \brief Get the height of a canvas in pixels
 *
 * \param cv A pointer to a canvas
 *
 * \return The height in pixels
 */
unsigned int canvas_get_height( canvas* cv );

/**
 * \brief Set the line drawing width in pixels
 *
 * \param cv A pointer to a canvas
 * \param width The new line width in pixels
 */
void canvas_set_line_width( canvas* cv, unsigned int width );

/**
 * \brief Get the currently set line drawing width in pixels
 *
 * \param cv A pointer to a canvas
 *
 * \return The line width in pixels
 */
unsigned int canvas_get_line_width( canvas* cv );

/**
 * \brief Set the drawing color of a canvas
 *
 * \param cv A pointer to a canvas
 * \param r The red component of the color
 * \param g The green component of the color
 * \param b The blue component of the color
 * \param a The alpha component of the color (transparency)
 */
void canvas_set_color( canvas* cv, unsigned char r, unsigned char g,
                                   unsigned char b, unsigned char a );

/**
 * \brief Clear all contents of a canvas to the currently set drawing color
 *
 * \param cv A pointer to a canvas
 */
void canvas_clear( canvas* cv );

/**
 * \brief Draw a point onto a canvas
 *
 * \param cv A pointer to a canvas
 * \param x The distance from the left of the canvas to the point
 * \param y The distance from the top of the canvas to the point
 */
void canvas_draw_point( canvas* cv, unsigned int x, unsigned int y );

/**
 * \brief Draw a line onto a canvas with the currently set line width
 *
 * \param cv A pointer to a canvas
 * \param x0 The distance from the left of the canvas to the starting point
 * \param y0 The distance from the top of the canvas to the starting point
 * \param x1 The distance from the left of the canvas to the end point
 * \param y1 The distance from the top of the canvas to the end point
 */
void canvas_draw_line( canvas* cv, int x0, int y0, int x1, int y1 );

/**
 * \brief Draw a circle onto a canvas with the currently set line width
 *
 * \param cv A pointer to a canvas
 * \param cx The distance from the left of the canvas to the circle center
 * \param cy The distance from the top of the canvas to the circle center
 * \param radius The radius of the circle
 */
void canvas_draw_circle( canvas* cv, int cx, int cy, int radius );

/**
 * \brief Draw a the outlines of a rectangle onto a canvas with the currently
 *        set line width
 *
 * \param cv A pointer to a canvas
 * \param x The distance from the left of the canvas to the starting point
 * \param y The distance from the top of the canvas to the starting point
 * \param x1 The distance from the left of the canvas to the end point
 * \param y1 The distance from the top of the canvas to the end point
 */
void canvas_draw_rect( canvas* cv, int x, int y, int x1, int y1 );

/**
 * \brief Draw a filled rectangle onto a canvas with
 *
 * \param cv A pointer to a canvas
 * \param x The distance from the left of the canvas to the starting point
 * \param y The distance from the top of the canvas to the starting point
 * \param x1 The distance from the left of the canvas to the end point
 * \param y1 The distance from the top of the canvas to the end point
 */
void canvas_fill_rect( canvas* cv, int x, int y, int x1, int y1 );

/**
 * \brief Draw a filled circle onto a canvas
 *
 * \param cv A pointer to a canvas
 * \param cx The distance from the left of the canvas to the circle center
 * \param cy The distance from the top of the canvas to the circle center
 * \param radius The radius of the circle
 */
void canvas_fill_circle( canvas* cv, int cx, int cy, int radius );

/**
 * \brief Draw a filled triangle onto a canvas
 *
 * \param cv A pointer to a canvas
 * \param x0 The distance from the left of the canvas to first vertex
 * \param y0 The distance from the top of the canvas to first vertex
 * \param x1 The distance from the left of the canvas to second vertex
 * \param y1 The distance from the top of the canvas to second vertex
 * \param x2 The distance from the left of the canvas to third vertex
 * \param y2 The distance from the top of the canvas to third vertex
 */
void canvas_fill_triangle( canvas* cv, int x0, int y0,
                                       int x1, int y1,
                                       int x2, int y2 );



#endif /* CANVAS_H */

