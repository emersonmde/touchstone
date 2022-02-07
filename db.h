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
    EXECUTE_ERROR_TABLE_FULL,
    EXECUTE_ERROR_DUPLICATE_KEY
} ExecuteResult;

typedef struct {
    int fd;
    uint32_t file_len;
    uint32_t num_pages;
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
    uint32_t root_page_num;
} Table;

typedef struct {
    StatementType type;
    Row row_to_insert;
} Statement;

typedef struct {
    Table *table;
    uint32_t page_num;
    uint32_t cell_num;
    bool end_of_table;
} Cursor;

typedef enum {
    INTERNAL_NODE,
    LEAF_NODE
} NodeType;

void print_constants();
void print_leaf_node(void *node);
void print_row(Row *row);

void serialize_row(Row *source, void *destination);
void deserialize_row(void *source, Row *destination);
void *get_page(Pager *pager, uint32_t page_num);

ExecuteResult execute_statement(Statement *statement, Table *table);
ExecuteResult execute_insert(Statement *statement, Table *table);
ExecuteResult execute_select(Statement *statement, Table *table);

Table *open_db(const char *filename);
void close_db(Table *table);

void pager_flush(Pager *pager, uint32_t page_num);

Cursor *table_start(Table *table);
Cursor *table_find(Table *table, uint32_t key);
void *cursor_ptr(Cursor *cursor);
void cursor_advance(Cursor *cursor);

// btree
NodeType get_node_type(void *node);
void set_node_type(void *node, NodeType type);
uint32_t *leaf_node_num_cells(void *node);
void *leaf_node_cell(void *node, uint32_t cell_num);
uint32_t *leaf_node_key(void *node, uint32_t cell_num);
void *leaf_node_value(void *node, uint32_t cell_num);
void initialize_leaf_node(void *node);
void leaf_node_insert(Cursor *cursor, uint32_t key, Row *value);
Cursor *leaf_node_find(Table *table, uint32_t page_num, uint32_t key);


#endif //TOUCHSTONE_DB_H
