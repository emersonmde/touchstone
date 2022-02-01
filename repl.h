//
// Created by Matthew Emerson on 1/31/22.
//

#ifndef TOUCHSTONE_REPL_H
#define TOUCHSTONE_REPL_H

#include <string.h>

typedef struct {
    char *buffer;
    size_t buffer_len;
    ssize_t input_len;
} InputBuffer;

InputBuffer *new_input_buffer();

void debug_input(InputBuffer *input);

void close_input(InputBuffer *input);

void print_prompt();

void read_input(InputBuffer *s);

#endif //TOUCHSTONE_REPL_H
