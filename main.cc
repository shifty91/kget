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

#include "protocol_dispatcher.h"
#include "logger.h"

static inline
void print_usage_and_die()
{
    std::cerr << "usage: get <url>" << std::endl;
    exit(-1);
}

int main(int argc, char *argv[])
{
    // parse args
    if (argc <= 1)
        print_usage_and_die();

    // dispatch
    try {
        ProtocolDispatcher dispatcher(argv[1]);
        dispatcher.dispatch();
    } catch (const std::exception&) {
        log_info("Unfortunately an error has occured :(. For more information read "
                 "error messages above.");
        exit(-1);
    }

    return 0;
}
