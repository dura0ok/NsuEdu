#include "terminal_library.h"

void tc_get_cols_rows(int *cols, int *rows) {

    struct winsize size;
    ioctl(1, TIOCGWINSZ, &size);
    *cols = size.ws_col;
    *rows = size.ws_row;

}

void set_keypress() {
    struct termios new_settings;

    tcgetattr(0, &stored_settings);

    new_settings = stored_settings;

    new_settings.c_lflag &= (~ICANON);
    new_settings.c_lflag &= (~ECHO);
    new_settings.c_cc[VTIME] = 0;
    new_settings.c_cc[VMIN] = 1;

    tcsetattr(0, TCSANOW, &new_settings);
}

void reset_keypress() {
    tcsetattr(0, TCSANOW, &stored_settings);
}



