//
// Created by Matthew Emerson on 1/31/22.
//

#ifndef TOUCHSTONE_COMPILER_H
#define TOUCHSTONE_COMPILER_H

#include "db.h"
#include "repl.h"

typedef enum {
    COMMAND_SUCCESS,
    COMMAND_ERROR_NOT_FOUND
} CommandResult;

typedef enum {
    PREPARE_SUCCESS,
    PREPARE_ERROR_NOT_FOUND,
    PREPARE_ERROR_SYNTAX,
    PREPARE_ERROR_OUT_OF_BOUNDS
} PrepareResult;

PrepareResult prepare_statement(InputBuffer *input, Statement *statement);

#endif //TOUCHSTONE_COMPILER_H
