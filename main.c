/*
 * This is an implementation of a very simple linked list for being used as a memory allocator,
 * its purpose is to store every global assert in a game, saving all pointer references inside a
 * linked list to free the programmer of the task of dealing with memory leaks. As it uses function
 * pointers to alloc and freeing the resources, it can be used to store any kind of data. In the
 * main function you can see how to use it with a normal string and with a custom data type.
 */
#include <stdio.h>
#include <malloc.h>
#include <string.h>

/* List stuff */
typedef struct MEMALLOC_NODE {
    void* data;
    struct MEMALLOC_NODE* next;
    void* (*create)(void* data);
    void (*destroy)(void* data);
} MEMALLOC_NODE;

typedef struct MEMALLOC {
    unsigned long int size;
    struct MEMALLOC_NODE* node;
    void* (*add)(struct MEMALLOC*, void* data, void* (*create)(void*), void (*destroy)(void*));
} MEMALLOC;

MEMALLOC* memalloc_init(void);
void memalloc_quit(MEMALLOC*);
void* memalloc_node_add(MEMALLOC* , void* data, void* (*create)(void*), void (*destroy)(void*));

/* Constructors and destructors for different types */
void* create(void*);
void destroy(void*);
void* create_testdata(void*);
void destroy_testdata(void*);

/* Data types for testing */
typedef struct TEST_DATA {
    char* buffer;
    int id;
} TEST_DATA;

struct test_data {
    char* buffer;
    int id;
};

MEMALLOC* memalloc_init(void) {
    MEMALLOC* memalloc = malloc(sizeof(struct MEMALLOC));
    if (memalloc != NULL) {
        memset(memalloc, 0, sizeof(struct MEMALLOC));
        memalloc->node = NULL;
        memalloc->size = 0;
        memalloc->add = memalloc_node_add;
    }
    return memalloc;
}

void* memalloc_node_add(MEMALLOC* memalloc, void* data, void* (*create)(void* params), void (*destroy)(void* params)) {
    MEMALLOC_NODE* aux = NULL;
    aux = malloc(sizeof(struct MEMALLOC_NODE));
    if (aux != NULL) {
        aux->data = create(data);
        aux->destroy = destroy;
        aux->next = NULL;
        if (memalloc->node == NULL) {
            memalloc->node = aux;
            memalloc->size++;
        } else {
            MEMALLOC_NODE* last = memalloc->node;
            memalloc->node = aux;
            aux->next = last;
            memalloc->size++;
        }
        return aux->data;
    }
    return NULL;
}

void memalloc_quit(MEMALLOC* memalloc) {
    MEMALLOC_NODE* node = NULL;
    if (memalloc != NULL) {
        node = memalloc->node;
        while(node != NULL) {
            node->destroy(node->data);
            MEMALLOC_NODE* aux = node->next;
            free(node);
            node = aux;
        }
        free(memalloc);
        memalloc = NULL;
    }
}

void* create(void* param) {
    char* buffer = NULL;
    buffer = malloc(sizeof(char)*1024);
    if (buffer != NULL) {
        memset(buffer, 0, sizeof(char)*1024);
        strcpy(buffer, param);
    }
    return buffer;
}

void destroy(void* param) {
    if (param != NULL) {
        free(param);
        param = NULL;
    }
}

void* create_testdata(void* param) {
    struct test_data* d = param;
    TEST_DATA* test = NULL;
    test = malloc(sizeof(TEST_DATA));
    if (test != NULL) {
        test->buffer = malloc(sizeof(char)*1024);
        if (test->buffer != NULL) {
            memset(test->buffer, 0, sizeof(char)*1024);
            strcpy(test->buffer, d->buffer);
        }
        test->id = d->id;
    }
    return test;
}

void destroy_testdata(void* param) {
    TEST_DATA* test = param;
    if (test != NULL) {
        if (test->buffer != NULL) {
            free(test->buffer);
            test->buffer = NULL;
        }
        free(test);
        test = NULL;
    }
}

int main(int argc, char **argv) {
    struct test_data t;
    t.buffer = "test data";
    t.id = 7;
    MEMALLOC* memalloc = memalloc_init();
    memalloc->add(memalloc, "Testing string", &create, &destroy);
    memalloc->add(memalloc, &t, &create_testdata, &destroy_testdata);
    fprintf(stdout, "Memory Allocations: %ld\n", memalloc->size);
    memalloc_quit(memalloc);
    return 0;
}
