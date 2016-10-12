/*
 * Copyright (C) 2015-2016 Kurt Kanzenbach <kurt@kmk-computers.de>
 *
 * This file is part of Get.
 *
 * Get is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Get is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Get.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef _UTILS_H_
#define _UTILS_H_

#include <termios.h>
#include <cstdlib>
#include <cstdio>
#include <fstream>
#include <sys/ioctl.h>
#include <unistd.h>
#include <cstring>

#include "logger.h"

class Utils
{
public:
    // taken from: http://stackoverflow.com/questions/13694170/how-do-i-hide-user-input-with-cin-in-c
    static inline void hide_stdin_keystrokes()
    {
        termios tty;

        tcgetattr(STDIN_FILENO, &tty);

        tty.c_lflag &= ~ECHO;

        tcsetattr(STDIN_FILENO, TCSANOW, &tty);
    }

    // taken from: http://stackoverflow.com/questions/13694170/how-do-i-hide-user-input-with-cin-in-c
    static inline void show_stdin_keystrokes()
    {
        termios tty;

        tcgetattr(STDIN_FILENO, &tty);

        tty.c_lflag |= ECHO;

        tcsetattr(STDIN_FILENO, TCSANOW, &tty);
    }

    static inline bool file_exists(const std::string& file)
    {
        // in C++ 17 we can simply use the following
#if 0
        using fs = std::filesystem;
        return fs::exists(file);
#endif
        // but now, we have to stick with fstream...
        std::ifstream ifs(file);
        return ifs.good();
    }

    static inline std::string get_home()
    {
        char *home = getenv("HOME");
        if (home == NULL)
            return "";
        return home;
    }

    static inline std::string user_input(const std::string& prefix = "")
    {
        std::cout << prefix << ": ";
        std::string input;
        if (!(std::cin >> input))
            EXCEPTION("Failed to get user input.");
        return input;
    }

    static inline std::string user_input_pw(const std::string& prefix = "")
    {
        std::cout << prefix << ": ";
        std::string input;
        hide_stdin_keystrokes();
        if (!(std::cin >> input))
            EXCEPTION("Failed to get user input.");
        show_stdin_keystrokes();
        return input;
    }

    static inline unsigned terminal_width()
    {
        int rc;
        struct winsize w;
        rc = ioctl(STDOUT_FILENO, TIOCGWINSZ, &w);
        if (rc) {
            log_err("ioctl() for getting terminal width failed: " << strerror(rc) << ". Using 80 columns.");
            return 80;
        }
        return w.ws_col;
    }
};

#endif /* _UTILS_H_ */
