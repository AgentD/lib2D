#include <X11/X.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>

#include "xlib_canvas.h"
#include "testdraw.h"
#include "canvas.h"



int main( void )
{
    unsigned int width = 640, height = 480;
    Atom atom_wm_delete;
    XSizeHints hints;
    Display* dpy;
    int run = 1;
    canvas* cv;
    Window wnd;
    XEvent e;

    /********** open X11 display connection **********/
    dpy = XOpenDisplay( 0 );

    if( !dpy )
        return -1;

    /********** create a window **********/
    wnd = XCreateSimpleWindow( dpy, DefaultRootWindow( dpy ),
                               0, 0, width, height, 0, 0, 0 );

    if( !wnd )
    {
        XCloseDisplay( dpy );
        return -1;
    }

    /********** make the window non resizeable **********/
    hints.flags = PSize | PMinSize | PMaxSize;
    hints.min_width = hints.max_width = hints.base_width = width;
    hints.min_height = hints.max_height = hints.base_height = height;

    XSetWMNormalHints( dpy, wnd, &hints );

    /********** tell the X server what events we will handle **********/
    atom_wm_delete = XInternAtom( dpy, "WM_DELETE_WINDOW", True );

    XSelectInput( dpy, wnd, ExposureMask | StructureNotifyMask );
    XSetWMProtocols( dpy, wnd, &(atom_wm_delete), 1 );
    XFlush( dpy );

    /********** make the window visible **********/
    XStoreName( dpy, wnd, "X11 test" );
    XMapWindow( dpy, wnd );

    /********** create canvas **********/
    cv = canvas_xlib_create( dpy, wnd, width, height );

    if( !cv )
    {
        XDestroyWindow( dpy, wnd );
        XCloseDisplay( dpy );
        return -1;
    }

    while( run )
    {
        XNextEvent( dpy, &e );

        switch( e.type )
        {
        case Expose:
            testdraw( cv );
            XFlush( dpy );
            break;
        case ClientMessage:
            if( e.xclient.data.l[0] == (long)atom_wm_delete )
            {
                XUnmapWindow( dpy, wnd );
                run = 0;
            }
            break;
        }
    }

    canvas_destroy( cv );
    XDestroyWindow( dpy, wnd );
    XCloseDisplay( dpy );
    return 0;
}

