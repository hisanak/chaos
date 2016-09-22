/*
 * chaos
 *
 * Authors: Filipe Manco <filipe.manco@neclab.eu>
 *
 *
 * Copyright (c) 2016, NEC Europe Ltd., NEC Corporation All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. Neither the name of the copyright holder nor the names of its
 *    contributors may be used to endorse or promote products derived from
 *    this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 *
 * THIS HEADER MAY NOT BE EXTRACTED OR MODIFIED IN ANY WAY.
 */

#include "cmdline.h"

#include <errno.h>
#include <getopt.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


static void __init(cmdline* cmd)
{
    memset(cmd, 0, sizeof(cmdline));
}

static void __parse_create(int argc, char** argv, cmdline* cmd)
{
    if (argc != 2) {
        fprintf(stderr, "Invalid number of arguments for 'create'.\n");
        cmd->error = true;
        return;
    }

    cmd->kernel = argv[1];
}

static void __parse_destroy(int argc, char** argv, cmdline* cmd)
{
    if (argc != 2) {
        fprintf(stderr, "Invalid number of arguments for 'destroy'.\n");
        cmd->error = true;
        return;
    }

    errno = 0;
    cmd->gid = (h2_guest_id) strtol(argv[1], NULL, 10);
    if (errno) {
        fprintf(stderr, "Invalid value for 'guest_id' argument'.\n");
        cmd->error = true;
    }
}


int cmdline_parse(int argc, char** argv, cmdline* cmd)
{
    __init(cmd);


    const char *short_opts = "h";
    const struct option long_opts[] = {
        { "help"               , no_argument       , NULL , 'h' },
        { NULL , 0 , NULL , 0 }
    };

    int opt;
    int opt_index;

    while (1) {
        opt = getopt_long(argc, argv, short_opts, long_opts, &opt_index);

        if (opt == -1) {
            break;
        }

        switch (opt) {
            case 'h':
                cmd->help = true;
                break;

            default:
                cmd->error = true;
                break;
        }
    }

    /* Now parse command */
    if (optind < argc) {
        argc -= optind;
        argv += optind;

        optind = 0;
        optopt = 0;

        if (strcmp(argv[optind], "create") == 0) {
            cmd->op = op_create;
            __parse_create(argc, argv, cmd);
        } else if (strcmp(argv[optind], "destroy") == 0) {
            cmd->op = op_destroy;
            __parse_destroy(argc, argv, cmd);
        } else {
            cmd->error = true;

            fprintf(stderr, "Invalid command '%s'.\n", argv[optind]);
        }
    } else if (!cmd->help) {
        cmd->error = true;
    }

    return 0;
}

void cmdline_usage(char* argv0)
{
    printf("Usage: %s [option]... <command> [args...]\n", argv0);
    printf("\n");
    printf("  -h, --help             Display this help and exit.\n");
    printf("\n");
    printf("commands:\n");
    printf("    create <config_file>\n");
    printf("        Create a new guest based on <config_file>.\n");
    printf("\n");
    printf("    destroy <guest_id>\n");
    printf("        Create a new guest based on <config_file>.\n");
    printf("\n");
}
