# Get #

## About ##

Just a simple program to retrieve a file over a URL. Basically it does
the same thing as BSD's fetch.

## Usage ##

    usage: get [options] <url>
      options:
        -p       : show progressbar if available
        -u <user>: username
        -k <pw>  : password
        -f       : do not follow HTTP redirects

Supported right now:

- HTTP, HTTPS, FTP and SFTP
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
- LibSSH2
- termios

## Author ##

Copyright (C) 2015 Kurt Kanzenbach <kurt@kmk-computers.de>

## License ##

GPL Version 3
