c4: c4.c draw_c4.c c4.h
	gcc c4.c draw_c4.c -o c4 -Wextra -Wall -lraylib -lGL -lm -lpthread -ldl -lrt -lX11