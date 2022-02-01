#include <stdio.h>
#include <stdlib.h>
#include "repl.h"
#include "vm.h"
#include "compiler.h"


int execute_command(InputBuffer *input) {
    if (strcmp(input->buffer, ".exit") == 0) {
        debug_input(input);
        close_input(input);
        printf("Goodbye\n");
        exit(EXIT_SUCCESS);
    } else if (strcmp(input->buffer, ".version") == 0) {
        printf("Touchstone v0.1\n");
        return COMMAND_SUCCESS;
    } else {
        debug_input(input);
        return COMMAND_ERROR_NOT_FOUND;
    }
}

void execute_statement(Statement *statement) {
   if (statement->type == STATEMENT_INSERT) {
       printf("EXECUTING INSERT\n");
       return;
   }
    if (statement->type == STATEMENT_SELECT) {
        printf("EXECUTING SELECT\n");
        return;
    }
}

int main() {
    InputBuffer *input = new_input_buffer();

    while (1) {
        print_prompt();
        read_input(input);

        if (input->buffer[0] == '.') {
            if (execute_command(input)) {
                debug_input(input);
                printf("Unrecognized command '%s'.\n", input->buffer);
            }

            continue;
        }

        Statement statement;
        if (prepare_statement(input, &statement)) {
            printf("Unrecognized keyword '%s'\n", input->buffer);
            continue;
        }

        execute_statement(&statement);
        printf("Done\n");
    }
}
