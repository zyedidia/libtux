#include <stdio.h>
#include <errno.h>
#include <argp.h>
#include <string.h>
#include <unistd.h>

#include "tux.h"
#include "sud.h"

typedef struct {
    uint8_t* data;
    size_t size;
} buf_t;

buf_t bufreadfile(const char* filename);

enum {
    INPUTMAX = 256,
};

struct Args {
    char* inputs[INPUTMAX];
    size_t ninputs;
};

static char doc[] = "tux-run: libtux runner";

static char args_doc[] = "INPUT...";

static struct argp_option options[] = {
    { "help",           'h',               0,      0, "show this message", -1 },
    { 0 },
};

static error_t
parse_opt(int key, char* arg, struct argp_state* state)
{
    struct Args* args = state->input;

    switch (key) {
    case 'h':
        argp_state_help(state, state->out_stream, ARGP_HELP_STD_HELP);
        break;
    case ARGP_KEY_ARG:
        if (args->ninputs < INPUTMAX)
            args->inputs[args->ninputs++] = arg;
        break;
    default:
        return ARGP_ERR_UNKNOWN;
    }

    return 0;
}

static struct argp argp = { .options = options, .parser = parse_opt, .args_doc = args_doc, .doc = doc };

struct Args args;

static size_t
mb(size_t x)
{
    return x * 1024 * 1024;
}

int
main(int argc, char** argv)
{
    argp_parse(&argp, argc, argv, ARGP_NO_HELP | ARGP_IN_ORDER, 0, &args);

    if (args.ninputs <= 0) {
        fprintf(stderr, "no input file provided\n");
        return 1;
    }

    struct Platform* plat = sud_new_plat();

    struct Tux* tux = tux_new(plat, (struct TuxOptions) {
        .pagesize = getpagesize(),
        .stacksize = mb(2),
    });

    buf_t f = bufreadfile(args.inputs[0]);
    if (!f.data) {
        fprintf(stderr, "error opening: %s: %s\n", args.inputs[0], strerror(errno));
        return 1;
    }

    struct TuxProc* p = tux_proc_newfile(tux, f.data, f.size, args.ninputs, &args.inputs[0]);
    if (!p) {
        fprintf(stderr, "error creating process\n");
        return 1;
    }

    uint64_t code = tux_proc_start(tux, p);
    printf("exited with code: %ld\n", code);

    return 0;
}
