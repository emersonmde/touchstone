//
// Created by Matthew Emerson on 1/31/22.
//

#ifndef TOUCHSTONE_VM_H
#define TOUCHSTONE_VM_H

typedef enum {
    STATEMENT_INSERT,
    STATEMENT_SELECT
} StatementType;

typedef struct {
    StatementType type;
} Statement;

#endif //TOUCHSTONE_VM_H
