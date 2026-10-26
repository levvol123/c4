RAYLIB_SRC = raylib/src
RAYLIB_LIB = $(RAYLIB_SRC)/libraylib.a

c4: c4.c draw_c4.c c4.h $(RAYLIB_LIB)
	gcc c4.c draw_c4.c -o c4 -Wextra -Wall -Wpedantic -I$(RAYLIB_SRC) -L$(RAYLIB_SRC) -lraylib -lGL -lm -lpthread -ldl -lrt -lX11

$(RAYLIB_LIB):
	$(MAKE) -C $(RAYLIB_SRC) PLATFORM=PLATFORM_DESKTOP \ GLFW_LINUX_ENABLE_WAYLAND=TRUE \ GLFW_LINUX_ENABLE_X11=TRUE

clean:
	rm -f c4
	$(MAKE) -C $(RAYLIB_SRC) clean