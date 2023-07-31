// Include the necessary headers
#include <stdio.h>
#include <stdlib.h>
#include <SDL2/SDL.h>
#include <math.h> // Added math.h header

// Define some constants
#define WINDOW_WIDTH 800
#define WINDOW_HEIGHT 600
#define GRAPH_COLOR 0xFF0000FF // Red
#define BACKGROUND_COLOR 0xFFFFFFFF // White
#define SLIDER_COLOR 0x000000FF // Black
#define SLIDER_ACTIVE_COLOR 0x00FF00FF // Green
#define SLIDER_WIDTH 20 // The width of each slider
#define SLIDER_PADDING 10 // The padding between the sliders and the window edges

// A structure to store a point on the graph
typedef struct {
    int x; // The x coordinate
    float y; // The y coordinate (changed from int to float)
} Point;

// A global pointer to store the points on the graph
Point *points = NULL;

// A global variable to store the current index of the points array
int index = 0;

// A global variable to store the current number of points on the graph
int num_points = 0;

// A global variable to store the minimum y value in the points array
float min_y = 0.0f;

// A global variable to store the maximum y value in the points array
float max_y = 0.0f;

// A global variable to store the scale factor for mapping y values to window height
float scale = 1.0f;

// A global variable to store the offset for adjusting vertical position of graph
float offset = 0.0f;

// A global variable to store which slider is active (0 for none, 1 for offset, 2 for zoom)
int active_slider = 0;

// A function to initialize the SDL library and create a window and a renderer
int init(SDL_Window **window, SDL_Renderer **renderer) {
    // Initialize SDL
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        fprintf(stderr, "SDL could not initialize! SDL_Error: %s\n", SDL_GetError());
        return 0;
    }

    // Create a window
    *window = SDL_CreateWindow("Rolling Graph", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, WINDOW_WIDTH, WINDOW_HEIGHT, SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE);
    if (*window == NULL) {
        fprintf(stderr, "Window could not be created! SDL_Error: %s\n", SDL_GetError());
        return 0;
    }

    // Create a renderer
    *renderer = SDL_CreateRenderer(*window, -1, SDL_RENDERER_ACCELERATED);
    if (*renderer == NULL) {
        fprintf(stderr, "Renderer could not be created! SDL_Error: %s\n", SDL_GetError());
        return 0;
    }

    // Return success
    return 1;
}

// A function to close the SDL library and destroy the window and the renderer
void close(SDL_Window *window, SDL_Renderer *renderer) {
    // Destroy the window and the renderer
    SDL_DestroyWindow(window);
    SDL_DestroyRenderer(renderer);

    // Free the points array
    free(points);

    // Quit SDL
    SDL_Quit();
}

// A function to read a line from the standard input and parse it as a CSV value
float read_value() { // Changed return type from int to float
    // Declare a buffer to store the line
    char buffer[256];

    // Read a line from the standard input
    if (fgets(buffer, sizeof(buffer), stdin) == NULL) {
        fprintf(stderr, "Failed to read from stdin!\n");
        return -1.0f;
    }

    // Parse the line as a CSV value and return it as a float
    return atof(buffer); // Changed atoi to atof
}

// A function to update the points array with a new value and shift the existing values to the left
void update_points(float value, int width, int height) { // Changed value type from int to float
    // Shift the existing values to the left by one position
    for (int i = 0; i < num_points - 1; i++) {
        points[i] = points[i + 1];
    }

    // Add the new value to the end of the array
    points[num_points - 1].x = width - 1;
    points[num_points - 1].y = value;

    // Update the index of the array
    index = (index + 1) % num_points;

    // Update the minimum and maximum y values in the array
    min_y = points[0].y;
    max_y = points[0].y;
    for (int i = 1; i < num_points; i++) {
        if (points[i].y < min_y) min_y = points[i].y;
        if (points[i].y > max_y) max_y = points[i].y;
    }

    // Update the scale factor based on the minimum and maximum y values and the window height
    // Add some padding to avoid drawing on the edges of the window
    scale = (height - SLIDER_WIDTH - SLIDER_PADDING * 2) / (max_y - min_y);
}

// A function to draw the graph on the renderer using the points array
void draw_graph(SDL_Renderer *renderer, int width, int height) {
    // Set the draw color to the background color
    SDL_SetRenderDrawColor(renderer, (BACKGROUND_COLOR >> 24) & 0xFF, (BACKGROUND_COLOR >> 16) & 0xFF, (BACKGROUND_COLOR >> 8) & 0xFF, BACKGROUND_COLOR & 0xFF);

    // Clear the renderer with the background color
    SDL_RenderClear(renderer);

    // Set the draw color to the graph color
    SDL_SetRenderDrawColor(renderer, (GRAPH_COLOR >> 24) & 0xFF, (GRAPH_COLOR >> 16) & 0xFF, (GRAPH_COLOR >> 8) & 0xFF, GRAPH_COLOR & 0xFF);

    // Draw a line for each pair of consecutive points in the array
    // Use the scale factor and offset to map y values to window height
    // Skip drawing lines that involve NAN values
    for (int i = index; i < num_points - 1; i++) {
        if (!isnan(points[i].y) && !isnan(points[i + 1].y)) { // Added check for NAN values
            SDL_RenderDrawLine(renderer, points[i].x, height - SLIDER_WIDTH - SLIDER_PADDING - (int)((points[i].y - min_y) * scale + offset), points[i + 1].x, height - SLIDER_WIDTH - SLIDER_PADDING - (int)((points[i + 1].y - min_y) * scale + offset)); // Cast y values to int
        }
    }
    
     for (int i = 0; i < index - 1; i++) {
        if (!isnan(points[i].y) && !isnan(points[i + 1].y)) { // Added check for NAN values
            SDL_RenderDrawLine(renderer, points[i].x - width, height - SLIDER_WIDTH - SLIDER_PADDING - (int)((points[i].y - min_y) * scale + offset), points[i + 1].x - width, height - SLIDER_WIDTH - SLIDER_PADDING - (int)((points[i + 1].y - min_y) * scale + offset)); // Cast y values to int
        }
    }

    // Set the draw color to the slider color or slider active color depending on which slider is active
    if (active_slider == 1) {
        SDL_SetRenderDrawColor(renderer, (SLIDER_ACTIVE_COLOR >> 24) & 0xFF, (SLIDER_ACTIVE_COLOR >> 16) & 0xFF, (SLIDER_ACTIVE_COLOR >> 8) & 0xFF, SLIDER_ACTIVE_COLOR & 0xFF);
    }
    else {
        SDL_SetRenderDrawColor(renderer, (SLIDER_COLOR >> 24) & 0xFF, (SLIDER_COLOR >> 16) & 0xFF, (SLIDER_COLOR >> 8) & 0xFF, SLIDER_COLOR & 0xFF);
    }

    // Draw a rectangle for the offset slider at the bottom of the window
    SDL_Rect offset_slider = {SLIDER_PADDING, height - SLIDER_WIDTH - SLIDER_PADDING, width - SLIDER_PADDING * 2, SLIDER_WIDTH};
    SDL_RenderFillRect(renderer, &offset_slider);

     // Set the draw color to the slider color or slider active color depending on which slider is active
     if (active_slider == 2) {
        SDL_SetRenderDrawColor(renderer, (SLIDER_ACTIVE_COLOR >> 24) & 0xFF, (SLIDER_ACTIVE_COLOR >> 16) & 0xFF, (SLIDER_ACTIVE_COLOR >> 8) & 0xFF, SLIDER_ACTIVE_COLOR & 0xFF);
    }
    else {
        SDL_SetRenderDrawColor(renderer, (SLIDER_COLOR >> 24) & 0xFF, (SLIDER_COLOR >> 16) & 0xFF, (SLIDER_COLOR >> 8) & 0xFF, SLIDER_COLOR & 0xFF);
    }

    // Draw a rectangle for the zoom slider at the right of the window
    SDL_Rect zoom_slider = {width - SLIDER_WIDTH - SLIDER_PADDING, SLIDER_PADDING, SLIDER_WIDTH, height - SLIDER_WIDTH - SLIDER_PADDING * 3};
    SDL_RenderFillRect(renderer, &zoom_slider);

    // Present the renderer on the window
    SDL_RenderPresent(renderer);
}

// A function to resize the points array and update their coordinates according to the new window size
void resize_points(int width, int height) {
    // Calculate the new number of points based on the window width
    int new_num_points = width;

    // Allocate a new array for the points
    Point *new_points = malloc(new_num_points * sizeof(Point));

    // Initialize the new array with NAN values
    for (int i = 0; i < new_num_points; i++) {
        new_points[i].x = i;
        new_points[i].y = NAN; // Use NAN macro to assign NAN value
    }

    // Copy the existing points to the new array and scale their coordinates
    for (int i = 0; i < num_points; i++) {
        new_points[i].x = points[i].x * width / WINDOW_WIDTH;
        new_points[i].y = points[i].y * height / WINDOW_HEIGHT;
    }

    // Free the old array and assign the new array to the global pointer
    free(points);
    points = new_points;

    // Update the global variable for the number of points
    num_points = new_num_points;
}

// The main function of the program
int main(int argc, char *argv[]) {
    // Declare a window and a renderer
    SDL_Window *window = NULL;
    SDL_Renderer *renderer = NULL;

    // Initialize the SDL library and create the window and the renderer
    if (!init(&window, &renderer)) {
        fprintf(stderr, "Failed to initialize!\n");
        return 1;
    }

    // Declare variables to store the current window width and height
    int width = WINDOW_WIDTH;
    int height = WINDOW_HEIGHT;

    // Get the initial window size and resize the points array accordingly
    SDL_GetWindowSize(window, &width, &height);
    resize_points(width, height);

    // Declare a variable to store the user input
    float value = 0.0f; // Changed from int to float

    // Declare a variable to store the quit flag
    int quit = 0;

    // Declare an event structure to handle user events
    SDL_Event e;

    // Enter the main loop
    while (!quit) {
        // Handle user events
        while (SDL_PollEvent(&e) != 0) {
            // If the user closes the window, set the quit flag to true
            if (e.type == SDL_QUIT) {
                quit = 1;
            }
            // If the user resizes the window, get the new window size and resize the points array and redraw the graph accordingly
            else if (e.type == SDL_WINDOWEVENT && (e.window.event == SDL_WINDOWEVENT_RESIZED || e.window.event == SDL_WINDOWEVENT_SIZE_CHANGED)) {
                SDL_GetWindowSize(window, &width, &height);
                resize_points(width, height);
                draw_graph(renderer, width, height);
            }
            // If the user moves the mouse wheel, check if it is over one of the sliders and adjust the offset or scale factor accordingly
            else if (e.type == SDL_MOUSEWHEEL) {
                // Get the mouse position
                int x, y;
                SDL_GetMouseState(&x, &y);

                // Check if the mouse is over the offset slider
                if (x >= SLIDER_PADDING && x <= width - SLIDER_PADDING && y >= height - SLIDER_WIDTH - SLIDER_PADDING && y <= height - SLIDER_PADDING) {
                    // Set the active slider to 1
                    active_slider = 1;

                    // Adjust the offset by adding or subtracting 10 pixels depending on the mouse wheel direction
                    if (e.wheel.y > 0) {
                        offset += 10.0f;
                    }
                    else if (e.wheel.y < 0) {
                        offset -= 10.0f;
                    }

                    // Redraw the graph with the new offset
                    draw_graph(renderer, width, height);
                }
                // Check if the mouse is over the zoom slider
                else if (x >= width - SLIDER_WIDTH - SLIDER_PADDING && x <= width - SLIDER_PADDING && y >= SLIDER_PADDING && y <= height - SLIDER_WIDTH - SLIDER_PADDING * 3) {
                    // Set the active slider to 2
                    active_slider = 2;

                    // Adjust the scale factor by multiplying or dividing by 1.1 depending on the mouse wheel direction
                    if (e.wheel.y > 0) {
                        scale *= 1.1f;
                    }
                    else if (e.wheel.y < 0) {
                        scale /= 1.1f;
                    }

                    // Redraw the graph with the new scale factor
                    draw_graph(renderer, width, height);
                }
                // Otherwise, set the active slider to 0
                else {
                    active_slider = 0;
                }
            }
        }

        // Read a value from the standard input
        value = read_value();

        // If the value is valid, update the points array and draw the graph
        if (value >= 0.0f) { // Changed from int to float comparison
            update_points(value, width, height);
            draw_graph(renderer, width, height);
        }
    }

    // Close the SDL library and destroy the window and the renderer
    close(window, renderer);

    // Return success
    return 0;
}
