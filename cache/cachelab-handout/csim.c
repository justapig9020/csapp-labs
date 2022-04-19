#include <stddef.h>
#define _POSIX_C_SOURCE 200809L
#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include "cachelab.h"
#include <stdbool.h>

#define VERBOSE(...) if (verbose_mode) printf(__VA_ARGS__)
#define ADDR_LEN 64
#define IS_HIT(n) ((n) != -1)

bool verbose_mode = false;
typedef unsigned long addr_t;

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

typedef enum {
    Miss = 0,
    Hit,
} AccResult;

struct Trace {
    addr_t addr;
    Operation op;
    size_t size;
};

static addr_t pow_2(addr_t exp) {
    return 1L << exp;
}

static void parse_arg(int argc, char *argv[], struct Args *parsed) {
    int cmd;
    while ((cmd = getopt(argc, argv, "vs:E:b:t:")) != -1) {
        switch (cmd) {
        case 'v':
            verbose_mode = true;
            break;
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

struct Record {
    int hit;
    int miss;
    int eviction;
};

struct Cache {
    size_t set_num;
    addr_t tag_size;
    size_t idx_size_bit;
    size_t block_size_bit;
    struct Record record;
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

static void init_record(struct Record *record) {
    record->hit = 0;
    record->miss = 0;
    record->eviction = 0;
}

static struct Cache *new_cache(int associativity, int index_size_bit, int block_size_bit) {
    if (block_size_bit >= ADDR_LEN)
        return NULL;

    if (index_size_bit >= ADDR_LEN)
        return NULL;
    addr_t index_size = pow_2(index_size_bit);

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
    cache->idx_size_bit = index_size_bit;
    cache->tag_size = tag_size;
    cache->lines = lines;
    cache->block_size_bit = block_size_bit;
    init_record(&cache->record);
    return cache;

free_lines:
    for (int i=0; i<inited ; i++)
        free(lines[i].sets);
    free(lines);
free_cache:
    free(cache);
    return NULL;
}

static size_t addr_to_line_idx(addr_t addr, addr_t block_size_bit, addr_t idx_size_bit) {
    addr_t avoid_block = addr >> block_size_bit;
    addr_t idx_size = pow_2(idx_size_bit);
    addr_t index_mask = idx_size - 1;
    return avoid_block & index_mask;
}

static size_t addr_to_tag(addr_t addr, addr_t block_size_bit, addr_t idx_size_bit) {
    return addr >> (block_size_bit + idx_size_bit);
}

static struct CacheLine *accessing_line(struct Cache *cache, addr_t addr) {
    addr_t idx = addr_to_line_idx(addr, cache->block_size_bit, cache->idx_size_bit);
    return &cache->lines[idx];
}

static bool is_entry_hit(struct CacheEntry *entry, addr_t tag) {
    bool tag_match = tag == entry->tag;
    return entry->valid && tag_match;
}

static int try_access_line(struct CacheLine *line, addr_t tag, size_t set_num) {
    struct CacheEntry *sets = line->sets;
    for (int i=0; i<set_num; i++) {
        if (is_entry_hit(&sets[i], tag))
            return i;
    }
    return -1;
}

static int find_victim(struct CacheLine *line, size_t set_num) {
    struct CacheEntry *sets = line->sets;
    const size_t target = set_num - 1;
    for (int i=0; i<set_num; i++) {
        if (!sets[i].valid)
            return -1;
        if (sets[i].lru == target)
            return i;
    }
    return -1;
}

static int find_slot(struct CacheLine *line, size_t set_num) {
    struct CacheEntry *sets = line->sets;
    for (int i=0; i<set_num; i++)
        if (!sets[i].valid)
            return i;
    return -1;
}

/*
 * Increase all valid lru values which less than "target".
 */
static void update_lru(struct CacheEntry *sets, long target, size_t set_num) {
    for (int i=0; i<set_num; i++)
        if (sets[i].valid && sets[i].lru < target)
            sets[i].lru += 1;
}

// Return true if eviction some entry, false returned otherwise.
static bool cache_update_miss(struct CacheLine *line, addr_t tag, size_t set_num) {
    int victim = find_victim(line, set_num);
    struct CacheEntry *sets = line->sets;

    if (-1 != victim) {
        long vic_lru = sets[victim].lru;
        update_lru(sets, vic_lru, set_num);
        sets[victim].tag = tag;
        sets[victim].lru = 0;
        return true;
    } else {
        int slot = find_slot(line, set_num);
        if (slot < 0)
            exit(2);
        update_lru(sets, set_num - 1, set_num);
        sets[slot].tag = tag;
        sets[slot].lru = 0;
        sets[slot].valid = true;
        return false;
    }
}

static void cache_update_hit(struct CacheLine *line, int idx, size_t set_num) {
    struct CacheEntry *sets = line->sets;
    long hit_lru = sets[idx].lru;
    update_lru(sets, hit_lru, set_num);
    sets[idx].lru = 0;
}

static AccResult cache_access(struct Cache *cache, addr_t addr) {
    struct CacheLine *line = accessing_line(cache, addr);
    addr_t tag = addr_to_tag(addr, cache->block_size_bit, cache->idx_size_bit);
    size_t set_num = cache->set_num;

    int result = try_access_line(line, tag, set_num);
    if (IS_HIT(result)) {
        int index = result;
        cache_update_hit(line, index, set_num);
        cache->record.hit += 1;
        return Hit;
    } else {
        bool is_eviction = cache_update_miss(line, tag, set_num);
        cache->record.miss += 1;
        cache->record.eviction += is_eviction;
        return Miss;
    }
}

static int cache_access_times(struct Cache *cache, addr_t addr, size_t times) {
    int hit_times = 0;
    for (int i=0; i<times; i++) {
        AccResult is_hit = cache_access(cache, addr);
        hit_times += is_hit;
    }
    return hit_times;
}

// Return hit times
static int sim_cache_access(struct Cache *cache, struct Trace *trace) {
    size_t times;
    switch (trace->op) {
    case Load:
    case Store: // Both load and store makes 1 access;
        times = 1;
        break;
    case Modify:
        times = 2;
        break;
    default:
        times = 0;
    }
    int hit_times = cache_access_times(cache, trace->addr, times);
    return hit_times;
}

static struct Record *get_record(struct Cache *cache) {
    if (cache)
        return &cache->record;
    return NULL;
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
    switch (ptr[0]) {
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

static bool is_hex(char c) {
    switch (c) {
    case '0'...'9':
    case 'a'...'f':
        return true;
    default:
        return false;
    }
}

static const char *pass_hex(const char *buf) {
    const char *ptr = buf;
    while (is_hex(*ptr))
        ptr += 1;
    return ptr;
}

static bool scan_address(const char **buf, addr_t *addr) {
    const char *ptr = *buf;
    ptr = next_word(ptr);
    int matched = sscanf(ptr, "%lx", addr);
    ptr = pass_hex(ptr);
    *buf = ptr;
    return matched == 1;
}

static bool scan_access_size(const char **buf, size_t *size) {
    const char *ptr = *buf;

    ptr = next_word(ptr);
    if (ptr[0] != ',')
        return false;

    ptr = next_word(ptr + 1);
    int matched = sscanf(ptr, "%lx", size);
    return matched == 1;
}

static bool parse_trace_line(const char *line, ssize_t len, struct Trace *trace) {
    Operation op;
    addr_t addr;
    size_t size;
    if (!scan_operation(&line, &op))
        return false;

    if (!scan_address(&line, &addr))
        return false;
    
    if (!scan_access_size(&line, &size))
        return false;

    trace->addr = addr;
    trace->op = op;
    trace->size = size;
    return true;
}

static bool next_trace(struct TraceParser *parser, struct Trace *trace) {
    bool ret = false;
    char *line_buf = NULL;
    size_t n = 0;
    ssize_t get = getline(&line_buf, &n, parser->trace);
    if (get != -1) {
        line_buf[get] = '\0';
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

static char operation_to_char(Operation op) {
    switch (op) {
    case Load:
        return 'L';
    case Store:
        return 'S';
    case Modify:
        return 'M';
    default:
        return ' ';
    }
}

static void verbose(struct Trace *trace, int hits) {
    const char op = operation_to_char(trace->op);
    VERBOSE("%c %lx,%ld", op, trace->addr, trace->size);
    if (hits == 0) {
        VERBOSE(" miss");
    } else {
        for (int i=0; i<hits; i++)
            VERBOSE(" hit");
    }
    VERBOSE(" \n");
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
        int hits = sim_cache_access(cache, &trace);
        verbose(&trace, hits);
    }

    struct Record *record = get_record(cache);
    printSummary(record->hit, record->miss, record->eviction);
    free_trace_parser(parser);
    free_cache(cache);
    return 0;

free_cache:
    free_cache(cache);
fail:
    return 1;
}
