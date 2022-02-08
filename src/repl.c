//
// Created by Matthew Emerson on 1/31/22.
//

#include <stdlib.h>
#include <stdio.h>
#include "repl.h"

InputBuffer *new_input_buffer() {
    InputBuffer *input_buffer = malloc(sizeof(InputBuffer));
    input_buffer->buffer = NULL;
    input_buffer->buffer_len = 0;
    input_buffer->input_len = 0;
    return input_buffer;
}

void debug_input(InputBuffer *input) {
    printf("input_buffer buffer: '%s' buffer_len: %zu input_len: %zi\n", input->buffer, input->buffer_len, input->input_len);
}

void close_input(InputBuffer *input) {
    free(input->buffer);
    free(input);
}

void print_prompt() {
    printf("touchstone> ");
}

void read_input(InputBuffer *s) {
    ssize_t bytes_read = getline(&s->buffer, &s->buffer_len, stdin);
    if (bytes_read < 0) {
        printf("Error reading input_buffer\n");
        exit(EXIT_FAILURE);
    }
    s->input_len = bytes_read - 1;
    s->buffer[bytes_read - 1] = '\0';
}