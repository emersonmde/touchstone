//
// Created by Matthew Emerson on 1/31/22.
//

#include <string.h>
#include "repl.h"
#include "compiler.h"

PrepareResult prepare_insert(InputBuffer *input, Statement *statement) {
    statement->type =STATEMENT_INSERT;

    char *keyword = strtok(input->buffer, " ");
    char *id_str = strtok(NULL, " ");
    char *username = strtok(NULL, " ");
    char *email = strtok(NULL, " ");

    if (id_str == NULL || username == NULL || email == NULL) {
        return PREPARE_ERROR_SYNTAX;
    }

    if (strlen(username) > COLUMN_USERNAME_SIZE) {
        return PREPARE_ERROR_OUT_OF_BOUNDS;
    }

    if (strlen(email) > COLUMN_USERNAME_SIZE) {
        return PREPARE_ERROR_OUT_OF_BOUNDS;
    }

    int id = atoi(id_str);
    if (id < 0) {
        return PREPARE_ERROR_OUT_OF_BOUNDS;
    }
    statement->row_to_insert.id = id;
    strcpy(statement->row_to_insert.username, username);
    strcpy(statement->row_to_insert.email, email);

    return PREPARE_SUCCESS;
}

PrepareResult prepare_statement(InputBuffer *input, Statement *statement) {
    if (strncasecmp(input->buffer, "insert", 6) == 0) {
        return prepare_insert(input, statement);
    }
    if (strncasecmp(input->buffer, "select", 6) == 0) {
        statement->type = STATEMENT_SELECT;
        return PREPARE_SUCCESS;
    }

    return PREPARE_ERROR_NOT_FOUND;
}