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

char trans_case_by_case_desc[] = "Trans case by case";
char transpose_submit_desc[] = "Transpose submission";
void trans_case_by_case(int col_size, int row_size, int A[row_size][col_size],
                        int B[col_size][row_size]) {
    int col_base;
    int row_base;
    int r;
    int c;
    int tmp0;
    int tmp1;
    int tmp2;
    int tmp3;
    int tmp4;
    int tmp5;
    int tmp6;
    int tmp7; // 12
    if (col_size == 32) {
        for (row_base = 0; row_base < row_size; row_base += INT_PER_BLOCK) {
            for (col_base = 0; col_base < col_size; col_base += INT_PER_BLOCK) {
                for (r = 0; r < INT_PER_BLOCK; r++) {
                    tmp0 = A[row_base + r][col_base + 0];
                    tmp1 = A[row_base + r][col_base + 1];
                    tmp2 = A[row_base + r][col_base + 2];
                    tmp3 = A[row_base + r][col_base + 3];
                    tmp4 = A[row_base + r][col_base + 4];
                    tmp5 = A[row_base + r][col_base + 5];
                    tmp6 = A[row_base + r][col_base + 6];
                    tmp7 = A[row_base + r][col_base + 7];

                    B[col_base + 0][row_base + r] = tmp0;
                    B[col_base + 1][row_base + r] = tmp1;
                    B[col_base + 2][row_base + r] = tmp2;
                    B[col_base + 3][row_base + r] = tmp3;
                    B[col_base + 4][row_base + r] = tmp4;
                    B[col_base + 5][row_base + r] = tmp5;
                    B[col_base + 6][row_base + r] = tmp6;
                    B[col_base + 7][row_base + r] = tmp7;
                }
            }
        }
    } else if (col_size == 64) {
        for (row_base = 0; row_base < row_size; row_base += INT_PER_BLOCK) {
            for (col_base = 0; col_base < col_size; col_base += INT_PER_BLOCK) {
                for (r = 0; r < INT_PER_BLOCK / 2; r++) {
                    tmp0 = A[row_base + r][col_base + 0];
                    tmp1 = A[row_base + r][col_base + 1];
                    tmp2 = A[row_base + r][col_base + 2];
                    tmp3 = A[row_base + r][col_base + 3];
                    tmp4 = A[row_base + r][col_base + 4];
                    tmp5 = A[row_base + r][col_base + 5];
                    tmp6 = A[row_base + r][col_base + 6];
                    tmp7 = A[row_base + r][col_base + 7];

                    // Top left
                    B[col_base + 0][row_base + r] = tmp0;
                    B[col_base + 1][row_base + r] = tmp1;
                    B[col_base + 2][row_base + r] = tmp2;
                    B[col_base + 3][row_base + r] = tmp3;

                    // Top right
                    B[col_base + 0][row_base + r + INT_PER_BLOCK / 2] = tmp4;
                    B[col_base + 1][row_base + r + INT_PER_BLOCK / 2] = tmp5;
                    B[col_base + 2][row_base + r + INT_PER_BLOCK / 2] = tmp6;
                    B[col_base + 3][row_base + r + INT_PER_BLOCK / 2] = tmp7;
                }

                // Move Top right to buttom left
                // Here r is works like c
                for (r = 0; r < INT_PER_BLOCK / 2; r++) {
                    tmp4 = A[row_base + 4][col_base + r];
                    tmp5 = A[row_base + 5][col_base + r];
                    tmp6 = A[row_base + 6][col_base + r];
                    tmp7 = A[row_base + 7][col_base + r];

                    tmp0 = B[col_base + r][row_base + 0 + INT_PER_BLOCK / 2];
                    tmp1 = B[col_base + r][row_base + 1 + INT_PER_BLOCK / 2];
                    tmp2 = B[col_base + r][row_base + 2 + INT_PER_BLOCK / 2];
                    tmp3 = B[col_base + r][row_base + 3 + INT_PER_BLOCK / 2];

                    B[col_base + r][row_base + 0 + INT_PER_BLOCK / 2] = tmp4;
                    B[col_base + r][row_base + 1 + INT_PER_BLOCK / 2] = tmp5;
                    B[col_base + r][row_base + 2 + INT_PER_BLOCK / 2] = tmp6;
                    B[col_base + r][row_base + 3 + INT_PER_BLOCK / 2] = tmp7;

                    B[col_base + INT_PER_BLOCK / 2 + r][row_base + 0] = tmp0;
                    B[col_base + INT_PER_BLOCK / 2 + r][row_base + 1] = tmp1;
                    B[col_base + INT_PER_BLOCK / 2 + r][row_base + 2] = tmp2;
                    B[col_base + INT_PER_BLOCK / 2 + r][row_base + 3] = tmp3;
                }
                for (r = 0; r < INT_PER_BLOCK / 2; r++) {
                    tmp0 = A[row_base + r + INT_PER_BLOCK / 2][col_base + 4];
                    tmp1 = A[row_base + r + INT_PER_BLOCK / 2][col_base + 5];
                    tmp2 = A[row_base + r + INT_PER_BLOCK / 2][col_base + 6];
                    tmp3 = A[row_base + r + INT_PER_BLOCK / 2][col_base + 7];

                    // Top left
                    B[col_base + 4][row_base + r + INT_PER_BLOCK / 2] = tmp0;
                    B[col_base + 5][row_base + r + INT_PER_BLOCK / 2] = tmp1;
                    B[col_base + 6][row_base + r + INT_PER_BLOCK / 2] = tmp2;
                    B[col_base + 7][row_base + r + INT_PER_BLOCK / 2] = tmp3;
                }
            }
        }
    } else if (col_size == 61) {
        for (row_base = 0; row_base < row_size - INT_PER_BLOCK;
             row_base += INT_PER_BLOCK) {
            for (col_base = 0; col_base < col_size - INT_PER_BLOCK;
                 col_base += INT_PER_BLOCK) {
                for (r = 0; r < INT_PER_BLOCK; r++) {
                    tmp0 = A[row_base + r][col_base + 0];
                    tmp1 = A[row_base + r][col_base + 1];
                    tmp2 = A[row_base + r][col_base + 2];
                    tmp3 = A[row_base + r][col_base + 3];
                    tmp4 = A[row_base + r][col_base + 4];
                    tmp5 = A[row_base + r][col_base + 5];
                    tmp6 = A[row_base + r][col_base + 6];
                    tmp7 = A[row_base + r][col_base + 7];

                    B[col_base + 0][row_base + r] = tmp0;
                    B[col_base + 1][row_base + r] = tmp1;
                    B[col_base + 2][row_base + r] = tmp2;
                    B[col_base + 3][row_base + r] = tmp3;
                    B[col_base + 4][row_base + r] = tmp4;
                    B[col_base + 5][row_base + r] = tmp5;
                    B[col_base + 6][row_base + r] = tmp6;
                    B[col_base + 7][row_base + r] = tmp7;
                }
            }
            for (c = 0; c < col_size - col_base; c++) {
                tmp0 = A[row_base + 0][col_base + c];
                tmp1 = A[row_base + 1][col_base + c];
                tmp2 = A[row_base + 2][col_base + c];
                tmp3 = A[row_base + 3][col_base + c];
                tmp4 = A[row_base + 4][col_base + c];
                tmp5 = A[row_base + 5][col_base + c];
                tmp6 = A[row_base + 6][col_base + c];
                tmp7 = A[row_base + 7][col_base + c];
                B[col_base + c][row_base + 0] = tmp0;
                B[col_base + c][row_base + 1] = tmp1;
                B[col_base + c][row_base + 2] = tmp2;
                B[col_base + c][row_base + 3] = tmp3;
                B[col_base + c][row_base + 4] = tmp4;
                B[col_base + c][row_base + 5] = tmp5;
                B[col_base + c][row_base + 6] = tmp6;
                B[col_base + c][row_base + 7] = tmp7;
            }
        }
        for (col_base = 0; col_base < col_size - INT_PER_BLOCK;
             col_base += INT_PER_BLOCK) {
            for (r = 0; r < row_size - row_base; r++) {
                tmp0 = A[row_base + r][col_base + 0];
                tmp1 = A[row_base + r][col_base + 1];
                tmp2 = A[row_base + r][col_base + 2];
                tmp3 = A[row_base + r][col_base + 3];
                tmp4 = A[row_base + r][col_base + 4];
                tmp5 = A[row_base + r][col_base + 5];
                tmp6 = A[row_base + r][col_base + 6];
                tmp7 = A[row_base + r][col_base + 7];

                B[col_base + 0][row_base + r] = tmp0;
                B[col_base + 1][row_base + r] = tmp1;
                B[col_base + 2][row_base + r] = tmp2;
                B[col_base + 3][row_base + r] = tmp3;
                B[col_base + 4][row_base + r] = tmp4;
                B[col_base + 5][row_base + r] = tmp5;
                B[col_base + 6][row_base + r] = tmp6;
                B[col_base + 7][row_base + r] = tmp7;
            }
        }
        for (c = col_base; c < col_size; c++) {
            for (r = row_base; r < row_size; r++) {
                B[c][r] = A[r][c];
            }
        }
    }
}

char dynamic_working_set_desc[] = "Dynamic working set";
void dynamic_working_set(int M, int N, int A[N][M], int B[M][N]) {
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
                for (int row = 0; row < INT_PER_BLOCK && (row_base + row) < N;
                     row++) {
                    for (int col = 0; col < block_height &&
                                      (col_base + col_offset + col) < M;
                         col++) {
                        tmp[col] =
                            A[row_base + row][col_base + col_offset + col];
                    }
                    for (int col = 0; col < block_height &&
                                      (col_base + col_offset + col) < M;
                         col++) {
                        B[col_base + col_offset + col][row_base + row] =
                            tmp[col];
                    }
                }
                col_offset += block_height;
            }
        }
    }
}

char trans_block_slice_desc[] = "Trans block slice";
void trans_block_slice(int c_size, int r_size, int A[r_size][c_size],
                       int B[c_size][r_size]) {
    // No more than 12 local variables
    int c_base;
    int r_base;
    int r;
    int c;
    int tmp0;
    int tmp1;
    int tmp2;
    int tmp3;
    int next_c_base;
    int next_r_base; // 10
    for (c_base = 0; c_base < (c_size - INT_PER_BLOCK);
         c_base += INT_PER_BLOCK) {
        for (r_base = 0; r_base <= (r_size - INT_PER_BLOCK);
             r_base += INT_PER_BLOCK) {
            // In this loop remaining columns and rows always more than a block
            // Iterate through matrix block by block
            next_r_base = r_base + INT_PER_BLOCK;
            next_c_base = c_base;
            if (next_r_base > (r_size - INT_PER_BLOCK)) {
                next_c_base += INT_PER_BLOCK;
                next_r_base = 0;
            }
            for (r = 0; r < INT_PER_BLOCK; r++) {
                // Iterate thtough entries in the block
                // First half block
                tmp0 = A[r_base + r][c_base + 0];
                tmp1 = A[r_base + r][c_base + 1];
                tmp2 = A[r_base + r][c_base + 2];
                tmp3 = A[r_base + r][c_base + 3];
                B[c_base + 0][r_base + r] = tmp0;
                B[c_base + 1][r_base + r] = tmp1;
                B[c_base + 2][r_base + r] = tmp2;
                B[c_base + 3][r_base + r] = tmp3;

                // Second half block
                // Buffer to next B block
                tmp0 = A[r_base + r][c_base + 4];
                tmp1 = A[r_base + r][c_base + 5];
                tmp2 = A[r_base + r][c_base + 6];
                tmp3 = A[r_base + r][c_base + 7];

                B[next_c_base + 0][next_r_base + r] = tmp0;
                B[next_c_base + 1][next_r_base + r] = tmp1;
                B[next_c_base + 2][next_r_base + r] = tmp2;
                B[next_c_base + 3][next_r_base + r] = tmp3;
            }
            // Migrate next B block back to Second half block
            for (c = 0; c < INT_PER_BLOCK; c++) {
                tmp0 = B[next_c_base + 0][next_r_base + c];
                tmp1 = B[next_c_base + 1][next_r_base + c];
                tmp2 = B[next_c_base + 2][next_r_base + c];
                tmp3 = B[next_c_base + 3][next_r_base + c];
                B[c_base + (INT_PER_BLOCK / 2) + 0][r_base + c] = tmp0;
                B[c_base + (INT_PER_BLOCK / 2) + 1][r_base + c] = tmp1;
                B[c_base + (INT_PER_BLOCK / 2) + 2][r_base + c] = tmp2;
                B[c_base + (INT_PER_BLOCK / 2) + 3][r_base + c] = tmp3;
            }
        }
        // Remaining rows
        for (r = 0; r < (r_size - r_base); r++) {
            tmp0 = A[r_base + r][c_base + 0];
            tmp1 = A[r_base + r][c_base + 1];
            tmp2 = A[r_base + r][c_base + 2];
            tmp3 = A[r_base + r][c_base + 3];
            B[c_base + 0][r_base + r] = tmp0;
            B[c_base + 1][r_base + r] = tmp1;
            B[c_base + 2][r_base + r] = tmp2;
            B[c_base + 3][r_base + r] = tmp3;

            tmp0 = A[r_base + r][c_base + 4];
            tmp1 = A[r_base + r][c_base + 5];
            tmp2 = A[r_base + r][c_base + 6];
            tmp3 = A[r_base + r][c_base + 7];
            B[c_base + 4][r_base + r] = tmp0;
            B[c_base + 5][r_base + r] = tmp1;
            B[c_base + 6][r_base + r] = tmp2;
            B[c_base + 7][r_base + r] = tmp3;
        }
    }
    for (r_base = 0; r_base < (r_size - 2 * INT_PER_BLOCK);
         r_base += INT_PER_BLOCK) {
        // Last column block expect last block
        next_r_base = r_base + INT_PER_BLOCK;
        next_c_base = c_base;
        for (r = 0; r < INT_PER_BLOCK; r++) {
            for (c = 0; c < INT_PER_BLOCK / 2; c++) {
                if (c >= (c_size - c_base))
                    break;
                B[c_base + c][r_base + r] = A[r_base + r][c_base + c];
            }

            for (c = 0; c < ((c_size - c_base) - (INT_PER_BLOCK / 2)); c++) {
                B[next_c_base + c][next_r_base + r] =
                    A[r_base + r][c_base + c + INT_PER_BLOCK / 2];
            }
        }
        // Migrate next B block back to Second half block
        for (c = 0; c < ((c_size - c_base) - (INT_PER_BLOCK / 2)); c++) {
            for (r = 0; r < INT_PER_BLOCK; r++) {
                B[c_base + (INT_PER_BLOCK / 2) + c][r_base + r] =
                    B[next_c_base + c][next_r_base + r];
            }
        }
    }
    // Last block
    for (r = r_base; r < r_size; r++) {
        for (c = c_base; c < c_size; c++) {
            B[c][r] = A[r][c];
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
    registerTransFunction(trans_case_by_case, transpose_submit_desc);

    /* Register any additional transpose functions */
    registerTransFunction(dynamic_working_set, dynamic_working_set_desc);
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
