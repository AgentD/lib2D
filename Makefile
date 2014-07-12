CFLAGS:=-ansi -pedantic -Wall -Wextra -ggdb -Iinclude

.PHONY: all
all: test test_x11 lib2D.a

%.o: src/%.c include/%.h
	$(CC) $(CFLAGS) -c $< -o $@

test_x11: test_x11.o testdraw.o lib2D.a
	$(CC) $^ -lX11 -lXrender -o $@

test: test.o testdraw.o lib2D.a
	$(CC) $^ -o $@

lib2D.a: imath.o pixmap.o canvas.o mem_canvas.o xlib_canvas.o xrender_canvas.o
	$(AR) rcs $@ $^

imath.o: src/imath.c include/imath.h
pixmap.o: src/pixmap.c include/pixmap.h include/predef.h
canvas.o: src/canvas.c include/canvas.h include/imath.h include/predef.h include/pixmap.h
mem_canvas.o: src/mem_canvas.c include/mem_canvas.h include/canvas.h include/predef.h include/pixmap.h
xlib_canvas.o: src/xlib_canvas.c include/xlib_canvas.h include/canvas.h include/predef.h include/pixmap.h
xrender_canvas.o: src/xrender_canvas.c include/xrender_canvas.h include/canvas.h include/predef.h include/pixmap.h

test.o: test.c include/canvas.h include/mem_canvas.h testdraw.h
testdraw.o: testdraw.c testdraw.h include/canvas.h include/pixmap.h
test_x11.o: test_x11.c include/xrender_canvas.h include/canvas.h include/imath.h testdraw.h

.PHONY: clean
clean:
	$(RM) test test_x11 *.o *.a out.ppm

