CFLAGS:=-ansi -pedantic -Wall -Wextra -ggdb

.PHONY: all
all: test test_x11 test_x11_old

test_x11: test_x11.o canvas.o imath.o xrender_canvas.o testdraw.o
	$(CC) $^ -lX11 -lXrender -o $@

test_x11_old: test_x11_old.o canvas.o imath.o xlib_canvas.o testdraw.o
	$(CC) $^ -lX11 -o $@

test: test.o canvas.o imath.o mem_canvas.o testdraw.o
	$(CC) $^ -o $@

test.o: test.c canvas.h mem_canvas.h testdraw.h
imath.o: imath.c imath.h
canvas.o: canvas.c canvas.h imath.h
testdraw.o: testdraw.c testdraw.h canvas.h
test_x11.o: test_x11.c xrender_canvas.h canvas.h imath.h testdraw.h
mem_canvas.o: mem_canvas.c mem_canvas.h canvas.h
xlib_canvas.o: xlib_canvas.c xlib_canvas.h canvas.h
test_x11_old.o: test_x11_old.c xlib_canvas.h canvas.h imath.h testdraw.h
xrender_canvas.o: xrender_canvas.c xrender_canvas.h canvas.h

.PHONY: clean
clean:
	$(RM) test test_x11 test_x11_old *.o out.ppm

