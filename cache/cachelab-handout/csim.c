#include <stddef.h>
#define _POSIX_C_SOURCE 200809L
#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include "cachelab.h"
#include <stdbool.h>

#define ADDR_LEN 64
typedef long addr_t;

struct Args {
    int set_index_bits;
    int associativity;
    int block_offset_bits;
    const char *file_name;
};

struct TraceParser {
    FILE *trace;
};

typedef enum {
    Load,
    Store,
    Inst,
    Modify,
} Operation;

struct Trace {
    addr_t addr;
    Operation op;
};

static void parse_arg(int argc, char *argv[], struct Args *parsed) {
    int cmd;
    while ((cmd = getopt(argc, argv, "s:E:b:t:")) != -1) {
        switch(cmd) {
        case 's':
            parsed->set_index_bits = atoi(optarg);
            break;
        case 'E':
            parsed->associativity = atoi(optarg);
            break;
        case 'b':
            parsed->block_offset_bits = atoi(optarg);
            break;
        case 't':
            parsed->file_name = optarg;
            break;
        }
    }
}

struct CacheEntry {
    bool valid; 
    addr_t tag;
    long lru;
};

struct CacheLine {
    struct CacheEntry *sets;
};

struct Cache {
    size_t set_num;
    size_t idx_size;
    size_t tag_size;
    struct CacheLine *lines;
};

static bool init_cache_line(struct CacheLine *line, addr_t associativity) {
    struct CacheEntry *entries = malloc(sizeof(*entries) * associativity);
    if (!entries)
        return false;
    for (int i=0; i<associativity; i++) {
        entries[i].valid = false;
    }
    line->sets = entries;
    return true;
}

static struct Cache *new_cache(int associativity, int index_size_bit, int block_size_bit) {
    if (block_size_bit >= ADDR_LEN)
        return NULL;

    if (index_size_bit >= ADDR_LEN)
        return NULL;
    addr_t index_size = 1 << index_size_bit;

    addr_t tag_size = ADDR_LEN - index_size_bit - block_size_bit;

    struct Cache *cache = malloc(sizeof(*cache));
    if (!cache)
        return NULL;
    
    struct CacheLine * lines= malloc(sizeof(struct CacheLine) * index_size);
    if (!lines)
        goto free_cache;

    int inited = 0;
    for (int i=0; i<index_size; i++) {
        bool ret = init_cache_line(&lines[i], associativity);
        if (!ret)
            goto free_lines;
        inited += 1;
    }
    cache->set_num = associativity;
    cache->idx_size = index_size;
    cache->tag_size = tag_size;
    cache->lines = lines;
    return cache;

free_lines:
    for (int i=0; i<inited ; i++)
        free(lines[i].sets);
    free(lines);
free_cache:
    free(cache);
    return NULL;
}

static void cache_access(struct Cache *cache, struct Trace *trace) {

}

static void free_cache(struct Cache *cache) {
    if (!cache)
        return;

    struct CacheLine *lines = cache->lines;
    if (lines) {
        for (int i=0; i<cache->set_num; i++)
            free(lines[i].sets);
    }
    free(lines);

    free(cache);
}

static struct TraceParser *new_trace_parser(const char *file_name) {
    struct TraceParser *parser = malloc(sizeof(*parser));
    if (!parser)
        return NULL;

    FILE *file = fopen(file_name, "r");
    if (!file)
        return NULL;

    parser->trace = file;
    return parser;
}

static const char *next_word(const char *buf) {
    const char *ptr = buf;
    while (*ptr == ' ')
        ptr += 1;
    return ptr;
}

static bool scan_operation(const char **buf, Operation *op) {
    bool ret = true;
    const char *ptr = *buf;
    ptr = next_word(ptr);
    switch(ptr[0]) {
    case 'I':
        *op = Inst;
        break;
    case 'L':
        *op = Load;
        break;
    case 'S':
        *op = Store;
        break;
    case 'M':
        *op = Modify;
        break;
    default:
        ret = false;
        break;
    }
    *buf = ptr + 1;
    return ret;
}

static bool scan_address(const char **buf, addr_t *addr) {
    const char *ptr = *buf;
    ptr = next_word(ptr);
    int matched = sscanf(ptr, "%lx", addr);
    return matched == 1;
}

static bool parse_trace_line(const char *line, ssize_t len, struct Trace *trace) {
    Operation op;
    addr_t addr;
    if (!scan_operation(&line, &op))
        return false;

    if (!scan_address(&line, &addr))
        return false;
    
    trace->addr = addr;
    trace->op = op;
    return true;
}

static bool next_trace(struct TraceParser *parser, struct Trace *trace) {
    bool ret = false;
    char *line_buf = NULL;
    size_t n = 0;
    ssize_t get = getline(&line_buf, &n, parser->trace);
    if (get != -1) {
        line_buf[get] = '\0';
        printf("%s", line_buf);
        ret = parse_trace_line(line_buf, get, trace);
    }
    free(line_buf);
    return ret;
}

static bool next_data_trace(struct TraceParser *parser, struct Trace *trace) {
    if (!parser || !trace)
        return false;
    do {
        if (!next_trace(parser, trace))
            return false;
    } while (trace->op == Inst);
    return true;
}

static void free_trace_parser(struct TraceParser *parser) {
    if (!parser)
        return;
    fclose(parser->trace);
    free(parser);
}

int main(int argc, char *argv[])
{
    struct Args parsed;
    parse_arg(argc, argv, &parsed);
    struct Cache *cache = new_cache(parsed.associativity, parsed.set_index_bits, parsed.block_offset_bits);
    if (!cache)
        goto fail;

    struct TraceParser *parser = new_trace_parser(parsed.file_name);
    if (!parser)
        goto free_cache;

    struct Trace trace;
    while (next_data_trace(parser, &trace)) {
        cache_access(cache, &trace);
    }

    free_trace_parser(parser);
    free_cache(cache);
    printSummary(0, 0, 0);
    return 0;

free_cache:
    printf("Free cache\n");
    free_cache(cache);
fail:
    return 1;
}
