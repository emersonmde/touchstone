//
// Created by Matthew Emerson on 2/1/22.
//

#ifndef TOUCHSTONE_DB_H
#define TOUCHSTONE_DB_H

#include <stdint.h>
#include <stddef.h>
#include <stdlib.h>
#include <stdbool.h>

#define COLUMN_USERNAME_SIZE    32
#define COLUMN_EMAIL_SIZE       255
#define TABLE_MAX_PAGES         100
#define size_of_attribute(Struct, Attribute)    sizeof(((Struct *)0)->Attribute)

typedef enum {
    STATEMENT_INSERT,
    STATEMENT_SELECT
} StatementType;

typedef enum {
    EXECUTE_SUCCESS,
    EXECUTE_ERROR_TABLE_FULL
} ExecuteResult;

typedef struct {
    int fd;
    uint32_t file_len;
    void *pages[TABLE_MAX_PAGES];
} Pager;

typedef struct {
    uint32_t id;
    char username[COLUMN_USERNAME_SIZE + 1];
    char email[COLUMN_EMAIL_SIZE + 1];
} Row;

typedef struct {
//    void *pages[TABLE_MAX_PAGES];
    Pager *pager;
    uint32_t num_rows;
} Table;

typedef struct {
    StatementType type;
    Row row_to_insert;
} Statement;

typedef struct {
    Table *table;
    uint32_t row_num;
    bool end_of_table;
} Cursor;

void serialize_row(Row *source, void *destination);
void deserialize_row(void *source, Row *destination);

void print_row(Row *row);

ExecuteResult execute_statement(Statement *statement, Table *table);
ExecuteResult execute_insert(Statement *statement, Table *table);
ExecuteResult execute_select(Statement *statement, Table *table);

Table *open_db(const char *filename);
void close_db(Table *table);

void pager_flush(Pager *pager, uint32_t page_num, const uint32_t size);

Cursor *table_start(Table *table);
Cursor *table_end(Table *table);
void *cursor_ptr(Cursor *cursor);
void cursor_advance(Cursor *cursor);

#endif //TOUCHSTONE_DB_H
