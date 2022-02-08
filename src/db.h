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
void print_tree(Pager *pager, uint32_t page_num, uint32_t indent_level);
void print_row(Row *row);

void serialize_row(Row *source, void *destination);
void deserialize_row(void *source, Row *destination);
void *get_page(Pager *pager, uint32_t page_num);
void pager_flush(Pager *pager, uint32_t page_num);
uint32_t get_unused_page_num(Pager *pager);

ExecuteResult execute_statement(Statement *statement, Table *table);
ExecuteResult execute_insert(Statement *statement, Table *table);
ExecuteResult execute_select(Statement *statement, Table *table);

Table *open_db(const char *filename);
void close_db(Table *table);

Cursor *table_start(Table *table);
Cursor *table_find(Table *table, uint32_t key);
void *cursor_ptr(Cursor *cursor);
void cursor_advance(Cursor *cursor);

// Shared btree
NodeType get_node_type(void *node);
void set_node_type(void *node, NodeType type);
void create_new_root(Table *table, uint32_t right_child_page_num);
uint32_t get_node_max_key(void *node);
bool is_node_root(void *node);
void set_node_root(void *node, bool is_root);
uint32_t *node_parent(void *node);

// Leaf node
uint32_t *leaf_node_num_cells(void *node);
void *leaf_node_cell(void *node, uint32_t cell_num);
uint32_t *leaf_node_key(void *node, uint32_t cell_num);
void *leaf_node_value(void *node, uint32_t cell_num);
void initialize_leaf_node(void *node);
void leaf_node_insert(Cursor *cursor, uint32_t key, Row *value);
Cursor *leaf_node_find(Table *table, uint32_t page_num, uint32_t key);
void leaf_node_split_and_insert(Cursor *cursor, uint32_t key, Row *value);
uint32_t *leaf_node_next_leaf(void *node);

// Internal node
void initialize_internal_node(void *node);
uint32_t *internal_node_num_keys(void *node);
uint32_t *internal_node_right_child(void *node);
uint32_t *internal_node_cell(void *node, uint32_t cell_num);
uint32_t *internal_node_child(void *node, uint32_t child_num);
uint32_t *internal_node_key(void *node, uint32_t key_num);
uint32_t internal_node_find_child(void *node, uint32_t key);
Cursor *internal_node_find(Table *table, uint32_t page_num, uint32_t key);
void update_internal_node_key(void *node, uint32_t old_key, uint32_t new_key);
void internal_node_insert(Table *table, uint32_t parent_page_num, uint32_t child_page_num);

#endif //TOUCHSTONE_DB_H
