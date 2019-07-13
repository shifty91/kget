# Get #

## About ##

Just a simple program to retrieve a file over a URL. Basically it does
the same thing as BSD's fetch.

## Usage ##

    usage: get [options] <url> [more urls]
      --continue, -c: Continue file download
      --debug, -d:    Enable debug output
      --follow, -f:   Do not follow HTTP redirects
      --help, -h:     Print this help
      --ipv4, -4:     Use IPv4 only
      --ipv6, -6:     Use IPv6 only
      --output, -o:   Specify output file name
      --progress, -p: Show progressbar if available
      --sslv2, -2:    Use SSL version 2
      --sslv3, -3:    Use SSL version 3
      --verify, -v:   Verify server's SSL certificate
      --version, -x:  Print version information
    get version 1.14 (C) Kurt Kanzenbach <kurt@kmk-computers.de>

Supported right now:

- HTTP, HTTPS, FTP, FTPS and SFTP
- IPv4 and IPv6 (v6 is preferred in DNS lookups)
- HTTP Basic Auth

Example:

    $ ./get http://www.gnu.org/licenses/gpl-3.0.txt

## Build ##

### Linux ###

    $ git submodule init
    $ git submodule update
    $ mkdir build
    $ cd build
    $ cmake ..
    $ make -j8
    $ sudo make install

### FreeBSD ###

    $ git submodule init
    $ git submodule update
    $ mkdir build
    $ cd build
    $ CC=gcc7 CXX=g++7 cmake -DCUSTOM_RPATH="/usr/local/lib/gcc7" ..
    $ make -j8
    $ sudo make install

## Dependencies ##

- Modern Compiler with CPP 17 Support (e.g. gcc >= 7 or clang >= 5)
- OpenSSL (optional, used for HTTPS and FTPS)
- LibSSH2 (optional, used for SFTP)
- Libunwind (optional, used for generating backtraces)
- termios

## Author ##

Copyright (C) 2015-2019 Kurt Kanzenbach <kurt@kmk-computers.de>

## License ##

GPL Version 3
