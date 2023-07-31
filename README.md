# SDL_rolling_graph
Rolling graph in SDL written by BingAI

To compile the code under Linux, you need to have the SDL2 library and its dependencies installed on your system. You can either install them using prepared packages available with most Linux distributions, or download the source code and build them from scratch. You also need to have the gcc compiler and the pkg-config tool installed.

To compile the code using gcc, you can use the following command:

gcc -Wall -Wextra -g SDLGAME.c $(pkg-config --cflags --libs sdl2) -o sdlgame-binary

This command will compile the SDLGAME.c file with some warning flags and debugging symbols, and link it with the SDL2 library and its dependencies using the pkg-config tool. The output binary will be named sdlgame-binary.

You can find more information about how to install and compile SDL2 C code on Linux/Ubuntu in [this web page](^2^) or [this web page](^3^).

To compile the helper program, you need to have the gcc compiler, the pkg-config tool, and the event and pthread libraries installed on your system. You can either install them using prepared packages available with most Linux distributions, or download the source code and build them from scratch.

To compile the helper program using gcc, you can use the following command:

gcc -Wall -Wextra -g helper.c $(pkg-config --cflags --libs event_pthreads) -o helper-binary

This command will compile the helper.c file with some warning flags and debugging symbols, and link it with the event and pthread libraries using the pkg-config tool. The output binary will be named helper-binary.

You can find more information about how to install and compile event and pthread C code on Linux/Ubuntu in [this web page] or [this web page].

Source: Conversation with Bing, 7/31/2023
(1) How to install/compile SDL2 C code on Linux/Ubuntu. https://stackoverflow.com/questions/29876411/how-to-install-compile-sdl2-c-code-on-linux-ubuntu.
(2) c - Compile an SDL project using gcc? - Stack Overflow. https://stackoverflow.com/questions/67233475/compile-an-sdl-project-using-gcc.
(3) SDL2/Installation - SDL Wiki - Simple DirectMedia Layer. https://wiki.libsdl.org/Installation.
(4) undefined. http://www.libsdl.org/download-2.0.php.
(5) undefined. https://askubuntu.com/questions/872792/what-is-xdg-runtime-dir.
(6) undefined. https://askubuntu.com/questions/456689/error-xdg-runtime-dir-not-set-in-the-environment-when-attempting-to-run-naut.
