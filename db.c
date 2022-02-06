//
// Created by Matthew Emerson on 2/1/22.
//

#include <memory.h>
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include "db.h"

const uint32_t ID_SIZE = size_of_attribute(Row, id);
const uint32_t USERNAME_SIZE = size_of_attribute(Row, username);
const uint32_t EMAIL_SIZE = size_of_attribute(Row, email);
const uint32_t ROW_SIZE = ID_SIZE + USERNAME_SIZE + EMAIL_SIZE;
const uint32_t ID_OFFSET = 0;
const uint32_t USERNAME_OFFSET = ID_OFFSET + ID_SIZE;
const uint32_t EMAIL_OFFSET = USERNAME_OFFSET + USERNAME_SIZE;

const uint32_t PAGE_SIZE = 4096;
const uint32_t ROWS_PER_PAGE = PAGE_SIZE / ROW_SIZE;
const uint32_t TABLE_MAX_ROWS = TABLE_MAX_PAGES * ROWS_PER_PAGE;


void serialize_row(Row *source, void *destination) {
    memcpy(destination + ID_OFFSET, &source->id, ID_SIZE);
    strncpy(destination + USERNAME_OFFSET, source->username, USERNAME_SIZE);
    strncpy(destination + EMAIL_OFFSET, source->email, EMAIL_SIZE);
}

void deserialize_row(void *source, Row *destination) {
    memcpy(&destination->id, source + ID_OFFSET, ID_SIZE);
    memcpy(&destination->username, source + USERNAME_OFFSET, USERNAME_SIZE);
    memcpy(&destination->email, source + EMAIL_OFFSET, EMAIL_SIZE);
}

void *get_page(Pager *pager, uint32_t page_num) {
    if (page_num > TABLE_MAX_PAGES) {
        printf("Error: Page out of bounds: %d of %d\n", page_num, TABLE_MAX_PAGES);
        exit(EXIT_FAILURE);
    }

    if (pager->pages[page_num] == NULL) {
        // Cache miss, read page from disk
        void *page = malloc(PAGE_SIZE);
        uint32_t num_pages = pager->file_len / PAGE_SIZE;

        // Account for a possible partial page at the end of the file
        if (pager->file_len % PAGE_SIZE) {
            num_pages += 1;
        }

        if (page_num <= num_pages) {
            lseek(pager->fd, page_num * PAGE_SIZE, SEEK_SET);
            ssize_t bytes_read = read(pager->fd, page, PAGE_SIZE);
            if (bytes_read == -1) {
                printf("Error: Error reading DB file: %d", errno);
                exit(EXIT_FAILURE);
            }
        }

        pager->pages[page_num] = page;
    }

    return pager->pages[page_num];
}

void print_row(Row *row) {
    printf("Row id: %d, username: %s, email: %s\n", row->id, row->username, row->email);
}

ExecuteResult execute_insert(Statement *statement, Table *table) {
    if (table->num_rows > TABLE_MAX_ROWS) {
        return EXECUTE_ERROR_TABLE_FULL;
    }

    Cursor *cursor = table_end(table);
    serialize_row(&statement->row_to_insert, cursor_ptr(cursor));
    free(cursor);
    table->num_rows += 1;
    return EXECUTE_SUCCESS;
}

ExecuteResult execute_select(Statement *statement, Table *table) {
    Cursor *cursor = table_start(table);
    Row row;
    while (!cursor->end_of_table) {
        deserialize_row(cursor_ptr(cursor), &row);
        print_row(&row);
        cursor_advance(cursor);
    }
    free(cursor);
    return EXECUTE_SUCCESS;
}

ExecuteResult execute_statement(Statement *statement, Table *table) {
    switch (statement->type) {
        case STATEMENT_INSERT:
            return execute_insert(statement, table);
        case STATEMENT_SELECT:
            return execute_select(statement, table);
    }
}

Pager *open_pager(const char *filename) {
    int fd = open(filename, O_RDWR | O_CREAT, S_IRUSR | S_IWUSR);

    if (fd == -1) {
        printf("Unable to open DB file\n");
        exit(EXIT_FAILURE);
    }

    off_t file_len = lseek(fd, 0, SEEK_END);

    Pager *pager = malloc(sizeof(Pager));
    pager->fd = fd;
    pager->file_len = file_len;

    for (uint32_t i = 0; i < TABLE_MAX_PAGES; i++) {
        pager->pages[i] = NULL;
    }

    return pager;
}

Table *open_db(const char *filename) {
    Pager *pager = open_pager(filename);
    uint32_t num_rows = pager->file_len / ROW_SIZE;

    Table *table = malloc(sizeof(Table));
    table->pager = pager;
    table->num_rows = num_rows;

    return table;
}

void close_db(Table *table) {
    Pager *pager = table->pager;
    uint32_t num_full_pages = table->num_rows / ROWS_PER_PAGE;

    for (uint32_t i = 0; i < num_full_pages; i++) {
        if (pager->pages[i] == NULL) {
            continue;
        }
        pager_flush(pager, i, PAGE_SIZE);
        free(pager->pages[i]);
        pager->pages[i] = NULL;
    }

    uint32_t num_additional_rows = table->num_rows % ROWS_PER_PAGE;
    if (num_additional_rows > 0) {
        uint32_t page_num = num_full_pages;
        if (pager->pages[page_num] != NULL) {
            pager_flush(pager, page_num, num_additional_rows * ROW_SIZE);
            free(pager->pages[page_num]);
            pager->pages[page_num] = NULL;
        }
    }

    int result = close(pager->fd);
    if (result == -1) {
        printf("Error closing DB file\n");
        exit(EXIT_FAILURE);
    }

    for (uint32_t i = 0; i < TABLE_MAX_PAGES; i++) {
        void *page = pager->pages[i];
        if (page) {
            free(page);
            pager->pages[i] = NULL;
        }
    }
    free(pager);
    free(table);
}

void pager_flush(Pager *pager, uint32_t page_num, const uint32_t size) {
    if (pager->pages[page_num] == NULL) {
        printf("Error: Tried to flush null page\n");
        exit(EXIT_FAILURE);
    }

    off_t offset = lseek(pager->fd, page_num * PAGE_SIZE, SEEK_SET);
    if (offset == -1) {
        printf("Error: Unable to seek to page %d: %d\n", page_num, errno);
        exit(EXIT_FAILURE);
    }

    ssize_t bytes_written = write(pager->fd, pager->pages[page_num], size);
    if (bytes_written == -1) {
        printf("Error: Unable to write page %d: %d", page_num, errno);
        exit(EXIT_FAILURE);
    }
}

Cursor *table_start(Table *table) {
    Cursor *cursor = malloc(sizeof(Cursor));
    cursor->table = table;
    cursor->row_num = 0;
    cursor->end_of_table = table->num_rows == 0;

    return cursor;
}

Cursor *table_end(Table *table) {
    Cursor *cursor = malloc(sizeof(Cursor));
    cursor->table = table;
    cursor->row_num = table->num_rows;
    cursor->end_of_table = true;

    return cursor;
}

void *cursor_ptr(Cursor *cursor) {
    uint32_t row_num = cursor->row_num;
    uint32_t page_num = row_num / ROWS_PER_PAGE;
    void *page = get_page(cursor->table->pager, page_num);
    uint32_t row_offset = (row_num % ROWS_PER_PAGE) * ROW_SIZE;
    return page + row_offset;
}

void cursor_advance(Cursor *cursor) {
    cursor->row_num += 1;
    cursor->end_of_table = cursor->row_num >= cursor->table->num_rows;
}
