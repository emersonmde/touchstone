//
// Created by Matthew Emerson on 1/31/22.
//

#ifndef TOUCHSTONE_COMPILER_H
#define TOUCHSTONE_COMPILER_H

enum {
    COMMAND_SUCCESS,
    COMMAND_ERROR_NOT_FOUND
};

enum {
    PREPARE_SUCCESS,
    PREPARE_ERROR_NOT_FOUND
};

int prepare_statement(InputBuffer *input, Statement *statement);

#endif //TOUCHSTONE_COMPILER_H
