# Get #

## About ##

Just a simple program to retrieve a file over a URL. Basically it does
the same thing as BSD's fetch.

## Usage ##

    $ get <url>

Supported right now:

- HTTP, HTTPS and FTP
- IPv4 and IPv6 (v6 is preferred in DNS lookups)
- HTTP Basic Auth

Example:

    $ ./get http://www.gnu.org/licenses/gpl-3.0.txt

## Build ##

    $ mkdir build
    $ cd build
    $ cmake ..
    $ make -j8

## Dependencies ##

- OpenSSL
- termios

## Author ##

Copyright (C) 2015 Kurt Kanzenbach <kurt@kmk-computers.de>

## License ##

GPL Version 3
