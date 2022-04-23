/*
 * trans.c - Matrix transpose B = A^T
 *
 * Each transpose function must have a prototype of the form:
 * void trans(int M, int N, int A[N][M], int B[M][N]);
 *
 * Rules:
 * - You are allowed to define at most 12 local variables of type int per
 * transpose function.
 * - Your transpose function may not use recursion.
 * - If you choose to use helper functions, you may not have more than 12 local
 * variables on the stack at a time between your helper functions and your top
 * level transpose function. For example, if your transpose declares 8
 * variables, and then you call a function which uses 4 variables, which calls
 * another function which uses 2, you will have 14 variables on the stack, and
 * you will be in violation of the rule.
 * - Your transpose function may not modify array A. You may, however, do
 * whatever you want with the contents of array B.
 * - You are NOT allowed to define any arrays in your code or to use any variant
 * of malloc.
 *
 * A transpose function is evaluated by counting the number of misses
 * on a 1KB direct mapped cache with a block size of 32 bytes.
 *
 * @Author justapig9020 <justapig9020@gmail.com>
 */
#include "cachelab.h"
#include <stdbool.h>
#include <stdio.h>

#define BLOCK_BITS 5
#define BLOCK_SIZE (1 << BLOCK_BITS)
#define INDEX_BITS 5
#define ENTRIES (1 << INDEX_BITS)
// #define BLOCK_COUNT(n) ((n * sizeof(n)) / BLOCK_SIZE)
#define INT_PER_BLOCK (BLOCK_SIZE / sizeof(int))
//#define INT_PER_BLOCK 4
#define CACHE_BLOCKS_PER_LINE(n) (n / INT_PER_BLOCK)

typedef unsigned long addr_t;

int is_transpose(int M, int N, int A[N][M], int B[M][N]);

addr_t cache_index_of(addr_t addr) {
    return (addr >> BLOCK_BITS) & (ENTRIES - 1);
}

bool same_cache_index(void *a, void *b) {
    return cache_index_of((addr_t)a) == cache_index_of((addr_t)b);
}

/*
 * transpose_submit - This is the solution transpose function that you
 *     will be graded on for Part B of the assignment. Do not change
 *     the description string "Transpose submission", as the driver
 *     searches for that string to identify the transpose function to
 *     be graded.
 */
char transpose_submit_desc[] = "Transpose submission";
void transpose_submit(int M, int N, int A[N][M], int B[M][N]) {
    int block_height;
    for (block_height = 1; block_height < INT_PER_BLOCK; block_height++) {
        if (same_cache_index(&B[0], &B[block_height]))
            break;
    }

    int tmp[INT_PER_BLOCK];
    for (int row_base = 0; row_base < N; row_base += INT_PER_BLOCK) {
        for (int col_base = 0; col_base < M; col_base += INT_PER_BLOCK) {
            int col_offset = 0;
            while (col_offset < INT_PER_BLOCK) {
                for (int row = 0; row < INT_PER_BLOCK && (row_base + row) < N; row++) {
                    for (int col=0; col<block_height && (col_base + col_offset + col) < M; col++) {
                        tmp[col] = A[row_base + row][col_base + col_offset + col];
                    }
                    for (int col=0; col<block_height && (col_base + col_offset + col) < M; col++) {
                        B[col_base + col_offset + col][row_base + row] = tmp[col];
                    }
                }
                col_offset += block_height;
            }
        }
    }
}

char trans_block_slice_desc[] = "Trans block slice";
void trans_block_slice(int M, int N, int A[N][M], int B[M][N]) {
    int tmp[INT_PER_BLOCK];
    int row_block, col_block;
    for (row_block = 0; row_block < CACHE_BLOCKS_PER_LINE(N) - 1; row_block++) {
        for (col_block = 0; col_block < CACHE_BLOCKS_PER_LINE(M); col_block++) {
            int row_base = row_block * INT_PER_BLOCK;
            int col_base = col_block * INT_PER_BLOCK;
            int col_next_base =
                (col_block + (row_block + 1) / CACHE_BLOCKS_PER_LINE(N)) *
                INT_PER_BLOCK;
            int row_next_base =
                ((row_block + 1) % CACHE_BLOCKS_PER_LINE(N)) * INT_PER_BLOCK;
            for (int row = 0; row < INT_PER_BLOCK; row++) {
                for (int col = 0; col < INT_PER_BLOCK; col++) {
                    tmp[col] = A[row_base + row][col_base + col];
                }
                for (int col = 0; col < INT_PER_BLOCK / 2; col++)
                    B[col_base + col][row_base + row] = tmp[col];
                for (int col = 0; col < INT_PER_BLOCK / 2; col++)
                    B[col_next_base + col][row_next_base + row] =
                        tmp[col + INT_PER_BLOCK / 2];
            }
            col_base += INT_PER_BLOCK / 2;
            for (int col = 0; col < INT_PER_BLOCK / 2; col++) {
                for (int row = 0; row < INT_PER_BLOCK; row++) {
                    B[col_base + col][row_base + row] =
                        B[col_next_base + col][row_next_base + row];
                }
            }
        }
    }
    for (col_block = 0; col_block < CACHE_BLOCKS_PER_LINE(M) - 1; col_block++) {
        int row_base = row_block * INT_PER_BLOCK;
        int col_base = col_block * INT_PER_BLOCK;
        int col_next_base =
            ((col_block + 1) % CACHE_BLOCKS_PER_LINE(M)) * INT_PER_BLOCK;
        int row_next_base =
            (row_block + (col_block + 1) / CACHE_BLOCKS_PER_LINE(M)) *
            INT_PER_BLOCK;
        for (int row = 0; row < INT_PER_BLOCK; row++) {
            for (int col = 0; col < INT_PER_BLOCK; col++) {
                tmp[col] = A[row_base + row][col_base + col];
            }
            for (int col = 0; col < INT_PER_BLOCK / 2; col++)
                B[col_base + col][row_base + row] = tmp[col];
            for (int col = 0; col < INT_PER_BLOCK / 2; col++)
                B[col_next_base + col][row_next_base + row] =
                    tmp[col + INT_PER_BLOCK / 2];
        }
        col_base += INT_PER_BLOCK / 2;
        for (int col = 0; col < INT_PER_BLOCK / 2; col++) {
            for (int row = 0; row < INT_PER_BLOCK; row++) {
                B[col_base + col][row_base + row] =
                    B[col_next_base + col][row_next_base + row];
            }
        }
    }
    int row_base = row_block * INT_PER_BLOCK;
    int col_base = col_block * INT_PER_BLOCK;
    for (int row = 0; row < INT_PER_BLOCK; row++) {
        for (int col = 0; col < INT_PER_BLOCK; col++) {
            tmp[col] = A[row_base + row][col_base + col];
        }
        for (int col = 0; col < INT_PER_BLOCK; col++) {
            B[col_base + col][row_base + row] = tmp[col];
        }
    }
}

char trans_block_and_buf_desc[] = "Trans block and buffer";
void trans_block_and_buf(int M, int N, int A[N][M], int B[M][N]) {
    for (int row_block = 0; row_block < CACHE_BLOCKS_PER_LINE(N); row_block++) {
        for (int col_block = 0; col_block < CACHE_BLOCKS_PER_LINE(M);
             col_block++) {
            int row_base = row_block * INT_PER_BLOCK;
            int col_base = col_block * INT_PER_BLOCK;
            int tmp0;
            int tmp1;
            int tmp2;
            int tmp3;
            int tmp4;
            int tmp5;
            int tmp6;
            int tmp7;
            for (int row = 0; row < INT_PER_BLOCK; row++) {
                tmp0 = A[row_base + row][col_base];
                tmp1 = A[row_base + row][col_base + 1];
                tmp2 = A[row_base + row][col_base + 2];
                tmp3 = A[row_base + row][col_base + 3];
                tmp4 = A[row_base + row][col_base + 4];
                tmp5 = A[row_base + row][col_base + 5];
                tmp6 = A[row_base + row][col_base + 6];
                tmp7 = A[row_base + row][col_base + 7];
                B[col_base][row_base + row] = tmp0;
                B[col_base + 1][row_base + row] = tmp1;
                B[col_base + 2][row_base + row] = tmp2;
                B[col_base + 3][row_base + row] = tmp3;
                B[col_base + 4][row_base + row] = tmp4;
                B[col_base + 5][row_base + row] = tmp5;
                B[col_base + 6][row_base + row] = tmp6;
                B[col_base + 7][row_base + row] = tmp7;
            }
        }
    }
}
/*
 * You can define additional transpose functions below. We've defined
 * a simple one below to help you get started.
 */

/*
 * trans - A simple baseline transpose function, not optimized for the cache.
 */
char trans_desc[] = "Simple row-wise scan transpose";
void trans(int M, int N, int A[N][M], int B[M][N]) {
    int i, j, tmp;

    for (i = 0; i < N; i++) {
        for (j = 0; j < M; j++) {
            tmp = A[i][j];
            B[j][i] = tmp;
        }
    }
}

/*
 * registerFunctions - This function registers your transpose
 *     functions with the driver.  At runtime, the driver will
 *     evaluate each of the registered functions and summarize their
 *     performance. This is a handy way to experiment with different
 *     transpose strategies.
 */
void registerFunctions() {
    /* Register your solution function */
    registerTransFunction(transpose_submit, transpose_submit_desc);

    /* Register any additional transpose functions */
    registerTransFunction(trans_block_slice, trans_block_slice_desc);
    registerTransFunction(trans_block_and_buf, trans_block_and_buf_desc);
    registerTransFunction(trans, trans_desc);
}

/*
 * is_transpose - This helper function checks if B is the transpose of
 *     A. You can check the correctness of your transpose by calling
 *     it before returning from the transpose function.
 */
int is_transpose(int M, int N, int A[N][M], int B[M][N]) {
    int i, j;

    for (i = 0; i < N; i++) {
        for (j = 0; j < M; ++j) {
            if (A[i][j] != B[j][i]) {
                return 0;
            }
        }
    }
    return 1;
}
