# Get #

## About ##

Just a simple program to retrieve a file over a URL. Basically it does
the same thing as BSD's fetch.

## Usage ##

    usage: get [options] <url> [more urls]
      --continue, -c: continue file download
      --debug, -d:    enable debug output
      --follow, -f:   do not follow HTTP redirects
      --help, -h:     print this help
      --output, -o:   specify output file name
      --progress, -p: show progressbar if available
      --sslv2, -2:    use SSL version 2
      --sslv3, -3:    use SSL version 3
      --verify, -v:   verify server's SSL certificate
      --version, -x:  print version information
    get version 1.13 (C) Kurt Kanzenbach <kurt@kmk-computers.de>

Supported right now:

- HTTP, HTTPS, FTP and SFTP
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
- OpenSSL (optional, used for HTTPS)
- LibSSH2 (optional, used for SFTP)
- Libunwind (optional, used for generating backtraces)
- termios

## Author ##

Copyright (C) 2015-2019 Kurt Kanzenbach <kurt@kmk-computers.de>

## License ##

GPL Version 3
