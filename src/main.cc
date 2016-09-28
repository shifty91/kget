/**
 * Copyright (C) 2015 Kurt Kanzenbach <kurt@kmk-computers.de>
 *
 * Just a simple tool for fetching a file by URL.
 *
 */

#include <iostream>
#include <stdexcept>
#include <vector>
#include <cstdlib>
#include <libgen.h>
#include <unistd.h>
#include <getopt.h>

#include "config.h"
#include "protocol_dispatcher.h"
#include "logger.h"

static struct option long_opts[] = {
    { "progress", no_argument,       NULL, 'p' },
    { "user",     required_argument, NULL, 'u' },
    { "pass",     required_argument, NULL, 'k' },
    { "follow",   no_argument,       NULL, 'f' },
    { "verify",   no_argument,       NULL, 'v' },
    { "sslv2",    no_argument,       NULL, '2' },
    { "sslv3",    no_argument,       NULL, '3' },
    { "output",   required_argument, NULL, 'o' },
    { NULL,       0,                 NULL,  0  }
};

__attribute__((noreturn)) static inline
void print_usage_and_die()
{
    std::cerr << "usage: get [options] <url> [more urls]" << std::endl;
    std::cerr << "  options:" << std::endl;
    std::cerr << "    --progress, -p   : show progressbar if available" << std::endl;
    std::cerr << "    --output, -o     : specify output file name" << std::endl;
    std::cerr << "    --user, -u <user>: username" << std::endl;
    std::cerr << "    --pass, -k <pw>  : password" << std::endl;
    std::cerr << "    --follow, -f     : do not follow HTTP redirects" << std::endl;
    std::cerr << "    --verify, -v     : verify server's SSL certificate" << std::endl;
    std::cerr << "    --sslv2, -2      : use SSL version 2" << std::endl;
    std::cerr << "    --sslv3, -3      : use SSL version 3" << std::endl;
    std::cerr << "get version 1.3 (C) Kurt Kanzenbach <kurt@kmk-computers.de>" << std::endl;
    std::exit(-1);
}

int main(int argc, char *argv[])
{
    // parse args
    auto *config = Config::instance();
    std::vector<std::string> urls;
    std::string user, pw, output;
    int c;

    if (argc <= 1)
        print_usage_and_die();

    while ((c = getopt_long(argc, argv, "23vpfu:k:o:", long_opts, NULL)) != -1) {
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
        case 'o':
            output = optarg;
            break;
        default:
            print_usage_and_die();
        }
    }
    if (optind >= argc)
        print_usage_and_die();
    for (auto i = optind; i < argc; ++i)
        urls.emplace_back(argv[i]);
    if (urls.size() > 1 && output.size() > 0)
        print_usage_and_die();

    // dispatch
    for (auto&& url: urls) {
        try {
            ProtocolDispatcher dispatcher(url, user, pw, output);
            dispatcher.dispatch();
        } catch (const std::exception&) {
            log_info("Unfortunately an error has occured :(. For more information read "
                     "error messages above.");
            std::exit(-1);
        }
    }

    return 0;
}
