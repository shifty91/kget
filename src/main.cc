/*
 * Copyright (C) 2015-2018 Kurt Kanzenbach <kurt@kmk-computers.de>
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

#include <iostream>
#include <stdexcept>
#include <vector>
#include <cstdlib>
#include <libgen.h>

#include <kopt/kopt.h>

#include "get_config.h"
#include "config.h"
#include "protocol_dispatcher.h"
#include "logger.h"

[[noreturn]] static inline
void print_usage_and_die(const Kopt::OptionParser& parser, int die)
{
    std::cerr << parser.get_usage("<url> [more urls]");
    std::cerr << "get version " << VERSION << " (C) Kurt Kanzenbach <kurt@kmk-computers.de>"
              << std::endl;
    std::exit(die ? EXIT_FAILURE : EXIT_SUCCESS);
}

[[noreturn]] static inline
void print_version_and_die()
{
    std::cerr << "get version " << VERSION << " (C) Kurt Kanzenbach <kurt@kmk-computers.de>"
              << std::endl;
    std::exit(EXIT_SUCCESS);
}

int main(int argc, char *argv[])
{
    // parse args
    auto *config = Config::instance();
    Kopt::OptionParser parser{argc, argv};

    parser.add_flag_option("progress", "show progressbar if available", 'p');
    parser.add_flag_option("follow", "do not follow HTTP redirects", 'f');
    parser.add_flag_option("verify", "verify server's SSL certificate", 'v');
    parser.add_flag_option("sslv2", "use SSL version 2", '2');
    parser.add_flag_option("sslv3", "use SSL version 3", '3');
    parser.add_argument_option("output", "specify output file name", 'o');
    parser.add_flag_option("debug", "enable debug output", 'd');
    parser.add_flag_option("version", "print version information", 'x');
    parser.add_flag_option("help", "print this help", 'h');
    parser.add_flag_option("continue", "continue file download", 'c');

    if (argc <= 1)
        print_usage_and_die(parser, 1);

    try {
        parser.parse();
    } catch (const std::exception& ex) {
        log_info("Error while parsing command line arguments: ", ex.what());
        print_usage_and_die(parser, 1);
    }

    // help/version?
    if (*parser["version"])
        print_version_and_die();
    if (*parser["help"])
        print_usage_and_die(parser, 0);

    // configure get
    if (*parser["progress"])
        config->show_pg() = true;
    if (*parser["follow"])
        config->follow_redirects() = false;
    if (*parser["verify"])
        config->verify_peer() = true;
    if (*parser["sslv2"])
        config->use_sslv2() = true;
    if (*parser["sslv3"])
        config->use_sslv3() = true;
    if (*parser["continue"])
        config->continue_download() = true;
    if (*parser["debug"])
        config->debug() = true;

    // urls given?
    if (parser.unparsed_options().empty())
        print_usage_and_die(parser, 1);
    if (parser.unparsed_options().size() > 1 && !parser["output"]->value().empty())
        print_usage_and_die(parser, 1);

    // dispatch
    for (auto&& url: parser.unparsed_options()) {
        try {
            ProtocolDispatcher dispatcher(url, parser["output"]->value());
            dispatcher.dispatch();
        } catch (const std::exception&) {
            log_info("Unfortunately an error has occured :(. For more information read "
                     "error messages above.");
            std::exit(-1);
        }
    }

    return EXIT_SUCCESS;
}
