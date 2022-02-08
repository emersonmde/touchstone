#include <stdio.h>
#include <stdlib.h>
#include "compiler.h"
#include "repl.h"


CommandResult execute_command(InputBuffer *input, Table *table) {
    if (strcmp(input->buffer, ".exit") == 0) {
        close_input(input);
        close_db(table);
        printf("Goodbye\n");
        exit(EXIT_SUCCESS);
    } else if (strcmp(input->buffer, ".version") == 0) {
        printf("Touchstone v0.1\n");
        return COMMAND_SUCCESS;
    } else if (strcmp(input->buffer, ".constants") == 0) {
        printf("Constants:\n");
        print_constants();
        return COMMAND_SUCCESS;
    } else if (strcmp(input->buffer, ".print_tree") == 0) {
        printf("Tree:\n");
        print_tree(table->pager, 0, 0);
        return COMMAND_SUCCESS;
    } else {
        debug_input(input);
        return COMMAND_ERROR_NOT_FOUND;
    }
}

int main(int argc, char *argv[]) {
    if (argc < 2) {
        printf("Error: DB filename required\n");
        exit(EXIT_FAILURE);
    }

    char *filename = argv[1];
    Table *table = open_db(filename);
    InputBuffer *input = new_input_buffer();

    while (1) {
        print_prompt();
        read_input(input);

        if (input->buffer[0] == '.') {
            switch (execute_command(input, table)) {
                case COMMAND_SUCCESS:
                    break;
                case COMMAND_ERROR_NOT_FOUND:
                    debug_input(input);
                    printf("Unrecognized command '%s'.\n", input->buffer);
                    break;
            }
            continue;
        }

        Statement statement;
        switch (prepare_statement(input, &statement)) {
            case PREPARE_SUCCESS:
                break;
            case PREPARE_ERROR_NOT_FOUND:
                printf("Error: Unrecognized keyword '%s'\n", input->buffer);
                continue;
            case PREPARE_ERROR_SYNTAX:
                printf("Error: Syntax error: '%s'\n", input->buffer);
                continue;
            case PREPARE_ERROR_OUT_OF_BOUNDS:
                printf("Error: Argument out of bounds: '%s'\n", input->buffer);
                break;
        }

        switch (execute_statement(&statement, table)) {
            case EXECUTE_SUCCESS:
                printf("Done\n");
                break;
            case EXECUTE_ERROR_TABLE_FULL:
                printf("Error: Table full\n");
                break;
            case EXECUTE_ERROR_DUPLICATE_KEY:
                printf("Error: Duplicate key\n");
                break;
        }
    }
}
