/**
 * Copyright (C) 2015 Kurt Kanzenbach <kurt@kmk-computers.de>
 *
 * Just a simple tool for fetching a file by URL.
 *
 */

#include <iostream>
#include <stdexcept>
#include <cstdlib>
#include <libgen.h>

#include <unistd.h>

#include "config.h"
#include "protocol_dispatcher.h"
#include "logger.h"

static inline
void print_usage_and_die()
{
    std::cerr << "usage: get [options] <url>" << std::endl;
    std::cerr << "  options:" << std::endl;
    std::cerr << "    -p       : show progressbar if available" << std::endl;
    std::cerr << "    -u <user>: username" << std::endl;
    std::cerr << "    -k <pw>  : password" << std::endl;
    std::cerr << "    -f       : do not follow HTTP redirects" << std::endl;
    std::cerr << "    -v       : verify server's SSL certificate" << std::endl;
    std::cerr << "    -2       : use SSL version 2" << std::endl;
    std::cerr << "    -3       : use SSL version 3" << std::endl;
    std::cerr << "get version 1.2 (C) Kurt Kanzenbach <kurt@kmk-computers.de>" << std::endl;
    exit(-1);
}

int main(int argc, char *argv[])
{
    // parse args
    auto *config = Config::instance();
    std::string url;
    std::string user;
    std::string pw;
    int c;

    if (argc <= 1)
        print_usage_and_die();

    while ((c = getopt(argc, argv, "23vpfu:k:")) != -1) {
        switch (c) {
        case 'p':
            config->show_pg() = true;
            break;
        case 'f':
            config->follow_redirects() = false;
            break;
        case 'u':
            user = optarg;
            break;
        case 'k':
            pw = optarg;
            break;
        case 'v':
            config->verify_peer() = true;
            break;
        case '2':
            config->use_sslv2() = true;
            break;
        case '3':
            config->use_sslv3() = true;
            break;
        case '?':
        case ':':
            print_usage_and_die();
        }
    }
    if (optind >= argc)
        print_usage_and_die();
    url = argv[optind];

    // dispatch
    try {
        ProtocolDispatcher dispatcher(url, user, pw);
        dispatcher.dispatch();
    } catch (const std::exception&) {
        log_info("Unfortunately an error has occured :(. For more information read "
                 "error messages above.");
        std::exit(-1);
    }

    return 0;
}
