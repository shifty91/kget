# Get #

## About ##

Just a simple program to retrieve a file over a URL. Basically it does
the same thing as BSD's fetch.

## Usage ##

    usage: get [options] <url> [more urls]
      options:
        --progress, -p   : show progressbar if available
        --output, -o     : specify output file name
        --continue, -c   : continue file download
        --follow, -f     : do not follow HTTP redirects
        --verify, -v     : verify server's SSL certificate
        --sslv2, -2      : use SSL version 2
        --sslv3, -3      : use SSL version 3
        --debug, -d      : enable debug output
        --help, -h       : print this help
        --version, -x    : print version information
    get version 1.8 (C) Kurt Kanzenbach <kurt@kmk-computers.de>

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

- OpenSSL (optional, used for HTTPS)
- LibSSH2 (optional, used for SFTP)
- termios

## Author ##

Copyright (C) 2015-2016 Kurt Kanzenbach <kurt@kmk-computers.de>

## License ##

GPL Version 3
