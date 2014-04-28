CFLAGS:=-ansi -pedantic -Wall -Wextra -ggdb

.PHONY: all
all: test test_x11 lib2D.a

test_x11: test_x11.o testdraw.o lib2D.a
	$(CC) $^ -lX11 -lXrender -o $@

test: test.o testdraw.o lib2D.a
	$(CC) $^ -o $@

lib2D.a: imath.o pixmap.o canvas.o mem_canvas.o xlib_canvas.o xrender_canvas.o
	$(AR) rcs $@ $^

imath.o: imath.c imath.h
pixmap.o: pixmap.c pixmap.h predef.h
canvas.o: canvas.c canvas.h imath.h predef.h pixmap.h
mem_canvas.o: mem_canvas.c mem_canvas.h canvas.h predef.h pixmap.h
xlib_canvas.o: xlib_canvas.c xlib_canvas.h canvas.h predef.h pixmap.h
xrender_canvas.o: xrender_canvas.c xrender_canvas.h canvas.h predef.h pixmap.h

test.o: test.c canvas.h mem_canvas.h testdraw.h
testdraw.o: testdraw.c testdraw.h canvas.h pixmap.h
test_x11.o: test_x11.c xrender_canvas.h canvas.h imath.h testdraw.h

.PHONY: clean
clean:
	$(RM) test test_x11 *.o *.a out.ppm

