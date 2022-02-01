//
// Created by Matthew Emerson on 1/31/22.
//

#include <string.h>
#include "vm.h"
#include "repl.h"
#include "compiler.h"

int prepare_statement(InputBuffer *input, Statement *statement) {
    if (strncasecmp(input->buffer, "insert", 6) == 0) {
        statement->type = STATEMENT_INSERT;
        return PREPARE_SUCCESS;
    }
    if (strncasecmp(input->buffer, "select", 6) == 0) {
        statement->type = STATEMENT_SELECT;
        return PREPARE_SUCCESS;
    }

    return PREPARE_ERROR_NOT_FOUND;
}