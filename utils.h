#ifndef _UTILS_H_
#define _UTILS_H_

#include <termios.h>

class Utils
{
public:
    // taken from: http://stackoverflow.com/questions/13694170/how-do-i-hide-user-input-with-cin-in-c
    static inline void hideStdinKeystrokes()
    {
        termios tty;

        tcgetattr(STDIN_FILENO, &tty);

        tty.c_lflag &= ~ECHO;

        tcsetattr(STDIN_FILENO, TCSANOW, &tty);
    }

    // taken from: http://stackoverflow.com/questions/13694170/how-do-i-hide-user-input-with-cin-in-c
    static inline void showStdinKeystrokes()
    {
        termios tty;

        tcgetattr(STDIN_FILENO, &tty);

        tty.c_lflag |= ECHO;

        tcsetattr(STDIN_FILENO, TCSANOW, &tty);
    }
};

#endif /* _UTILS_H_ */
