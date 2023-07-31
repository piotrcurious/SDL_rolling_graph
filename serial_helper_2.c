// Include the necessary headers
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <event.h>
#include <termios.h>
#include <fcntl.h>
#include <unistd.h>

// Define some constants
#define SERIAL_PORT "/dev/ttyS0" // The default serial port device file
#define SERIAL_BAUD B9600 // The default serial port baud rate
#define SERIAL_BUFFER 256 // The serial port buffer size

// Declare some global variables to store the serial port name and parameters
char *serial_device = SERIAL_PORT; // The serial port device file
speed_t serial_baud = SERIAL_BAUD; // The serial port baud rate

// Declare a global variable to store the event base
struct event_base *evbase;

// Declare a global variable to store the serial port file descriptor
int serial_fd;

// A function to initialize the serial port with the given device file and baud rate
int init_serial(const char *device, speed_t baud) {
    // Open the device file in read-write mode, without controlling terminal, and non-blocking
    int fd = open(device, O_RDWR | O_NOCTTY | O_NONBLOCK);
    if (fd == -1) {
        fprintf(stderr, "Failed to open %s\n", device);
        return -1;
    }

    // Get the current configuration of the serial interface
    struct termios config;
    if (tcgetattr(fd, &config) != 0) {
        fprintf(stderr, "Failed to get configuration of %s\n", device);
        close(fd);
        return -1;
    }

    // Set the input and output baud rates
    if (cfsetispeed(&config, baud) != 0 || cfsetospeed(&config, baud) != 0) {
        fprintf(stderr, "Failed to set baud rate of %s\n", device);
        close(fd);
        return -1;
    }

    // Set the other configuration flags
    config.c_cflag |= (CLOCAL | CREAD); // Enable the receiver and set local mode
    config.c_cflag &= ~PARENB; // No parity bit
    config.c_cflag &= ~CSTOPB; // 1 stop bit
    config.c_cflag &= ~CSIZE; // Mask the character size bits
    config.c_cflag |= CS8; // 8 data bits
    config.c_cflag &= ~CRTSCTS; // Disable hardware flow control

    config.c_lflag &= ~(ICANON | ECHO | ECHOE | ISIG); // Enable raw input

    config.c_iflag &= ~(IXON | IXOFF | IXANY); // Disable software flow control
    config.c_iflag &= ~(IGNBRK | BRKINT | PARMRK | ISTRIP | INLCR | IGNCR | ICRNL); // Disable special handling of received bytes

    config.c_oflag &= ~OPOST; // Enable raw output

    // Set the new configuration of the serial interface
    if (tcsetattr(fd, TCSANOW, &config) != 0) {
        fprintf(stderr, "Failed to set configuration of %s\n", device);
        close(fd);
        return -1;
    }

    // Return the file descriptor
    return fd;
}

// A function to read data from the serial port and write it to standard output
void read_serial(int fd, short events, void *arg) {
    // Declare a buffer to store the data
    char buffer[SERIAL_BUFFER];

    // Read data from the serial port
    int bytes = read(fd, buffer, sizeof(buffer));
    
    // Check for errors or no data
    if (bytes <= 0) {
        fprintf(stderr, "Failed to read from serial port or no data available\n");
        return;
    }

    // Write data to standard output
    bytes = write(STDOUT_FILENO, buffer, bytes);

     // Check for errors or no data
     if (bytes <= 0) {
         fprintf(stderr, "Failed to write to standard output or no data available\n");
         return;
     }
}

// A function to create and start a thread that runs the event loop for reading serial data
void *serial_thread(void *arg) {
    // Initialize the serial port with the given device file and baud rate
    serial_fd = init_serial(serial_device, serial_baud); // Use global variables for device file and baud rate
    
     // Check for errors
     if (serial_fd == -1) {
         fprintf(stderr, "Failed to initialize serial port\n");
         pthread_exit(NULL);
     }

     // Create a new event for reading serial data
     struct event *serial_event = event_new(evbase, serial_fd, EV_READ | EV_PERSIST, read_serial, NULL);

     // Add the event to the event base with no timeout
     event_add(serial_event, NULL);

     // Start the event loop for reading serial data
     event_base_dispatch(evbase);

     // Free the event
     event_free(serial_event);

     // Close the serial port
     close(serial_fd);

     // Exit the thread
     pthread_exit(NULL);
}

// The main function of the program
int main(int argc, char *argv[]) {
    // Declare a variable to store the thread id
    pthread_t tid;

    // Declare a variable to store the option character
    int opt;

    // Parse the command line arguments using getopt
    while ((opt = getopt(argc, argv, "d:b:")) != -1) {
        switch (opt) {
            case 'd': // Device file option
                serial_device = optarg; // Store the device file name in the global variable
                break;
            case 'b': // Baud rate option
                serial_baud = atoi(optarg); // Convert the baud rate string to an integer and store it in the global variable
                break;
            default: // Invalid option or missing argument
                fprintf(stderr, "Usage: %s [-d device] [-b baud]\n", argv[0]); // Print usage message and exit
                return 1;
        }
    }

    // Initialize libevent for pthreads
    evthread_use_pthreads();

    // Create a new event base
    evbase = event_base_new();

    // Create and start a thread that runs the event loop for reading serial data
    pthread_create(&tid, NULL, serial_thread, NULL);

    // Wait for the thread to finish
    pthread_join(tid, NULL);

    // Free the event base
    event_base_free(evbase);

    // Return success
    return 0;
}
