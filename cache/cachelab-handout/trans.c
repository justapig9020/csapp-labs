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
#include <stdio.h>

#define BLOCK_SIZE 32
#define ENTRIES 32
#define BLOCK_COUNT(n) ((n * sizeof(n)) / BLOCK_SIZE)
#define INT_PER_BLOCK (BLOCK_SIZE / sizeof(int))

int is_transpose(int M, int N, int A[N][M], int B[M][N]);

/*
 * transpose_submit - This is the solution transpose function that you
 *     will be graded on for Part B of the assignment. Do not change
 *     the description string "Transpose submission", as the driver
 *     searches for that string to identify the transpose function to
 *     be graded.
 */
char transpose_submit_desc[] = "Transpose submission";
void transpose_submit(int M, int N, int A[N][M], int B[M][N]) {
    for (int row_block = 0; row_block < BLOCK_COUNT(N); row_block++) {
        for (int col_block = 0; col_block < BLOCK_COUNT(N); col_block++) {
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
