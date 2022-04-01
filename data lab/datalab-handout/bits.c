/*
 * CS:APP Data Lab
 *
 * @Author justapig9020 <justapig9020@gmail.com>
 *
 * bits.c - Source file with your solutions to the Lab.
 *          This is the file you will hand in to your instructor.
 */

/* Instructions to Students:

You will provide your solution to the Data Lab by
editing the collection of functions in this source file.

INTEGER CODING RULES:

  Replace the "return" statement in each function with one
  or more lines of C code that implements the function. Your code
  must conform to the following style:

  long Funct(long arg1, long arg2, ...) {
      // brief description of how your implementation works
      long var1 = Expr1;
      ...
      long varM = ExprM;

      varJ = ExprJ;
      ...
      varN = ExprN;
      return ExprR;
  }

  Each "Expr" is an expression using ONLY the following:
  1. (Long) integer constants 0 through 255 (0xFFL), inclusive. You are
      not allowed to use big constants such as 0xffffffffL.
  3. Function arguments and local variables (no global variables).
  4. Local variables of type int and long
  5. Unary integer operations ! ~
     - Their arguments can have types int or long
     - Note that ! always returns int, even if the argument is long
  6. Binary integer operations & ^ | + << >>
     - Their arguments can have types int or long
  7. Casting from int to long and from long to int

  Some of the problems restrict the set of allowed operators even further.
  Each "Expr" may consist of multiple operators. You are not restricted to
  one operator per line.

  You are expressly forbidden to:
  1. Use any control constructs such as if, do, while, for, switch, etc.
  2. Define or use any macros.
  3. Define any additional functions in this file.
  4. Call any functions.
  5. Use any other operations, such as &&, ||, -, or ?:
  6. Use any form of casting other than between int and long.
  7. Use any data type other than int or long.  This implies that you
     cannot use arrays, structs, or unions.

  You may assume that your machine:
  1. Uses 2s complement representations for int and long.
  2. Data type int is 32 bits, long is 64.
  3. Performs right shifts arithmetically.
  4. Has unpredictable behavior when shifting if the shift amount
     is less than 0 or greater than 31 (int) or 63 (long)

EXAMPLES OF ACCEPTABLE CODING STYLE:
  //
  // pow2plus1 - returns 2^x + 1, where 0 <= x <= 63
  //
  long pow2plus1(long x) {
     // exploit ability of shifts to compute powers of 2
     // Note that the 'L' indicates a long constant
     return (1L << x) + 1L;
  }

  //
  // pow2plus4 - returns 2^x + 4, where 0 <= x <= 63
  //
  long pow2plus4(long x) {
     // exploit ability of shifts to compute powers of 2
     long result = (1L << x);
     result += 4L;
     return result;
  }

NOTES:
  1. Use the dlc (data lab checker) compiler (described in the handout) to
     check the legality of your solutions.
  2. Each function has a maximum number of operations (integer, logical,
     or comparison) that you are allowed to use for your implementation
     of the function.  The max operator count is checked by dlc.
     Note that assignment ('=') is not counted; you may use as many of
     these as you want without penalty.
  3. Use the btest test harness to check your functions for correctness.
  4. Use the BDD checker to formally verify your functions
  5. The maximum number of ops for each function is given in the
     header comment for each function. If there are any inconsistencies
     between the maximum ops in the writeup and in this file, consider
     this file the authoritative source.

CAUTION:
  Do not add an #include of <stdio.h> (or any other C library header)
  to this file.  C library headers almost always contain constructs
  that dlc does not understand.  For debugging, you can use printf,
  which is declared for you just below.  It is normally bad practice
  to declare C library functions by hand, but in this case it's less
  trouble than any alternative.

  dlc will consider each call to printf to be a violation of the
  coding style (function calls, after all, are not allowed) so you
  must remove all your debugging printf's again before submitting your
  code or testing it with dlc or the BDD checker.  */

extern int printf(const char *, ...);

/* Edit the functions below.  Good luck!  */
// 2
/*
 * copyLSB - set all bits of result to least significant bit of x
 *   Examples:
 *     copyLSB(5L) = 0xFFFFFFFFFFFFFFFFL,
 *     copyLSB(6L) = 0x0000000000000000L
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 5
 *   Rating: 2
 */
long copyLSB(long x) {
    x <<= 63;
    x >>= 63;
    return x;
}
/*
 * dividePower2 - Compute x/(2^n), for 0 <= n <= 62
 *  Round toward zero
 *   Examples: dividePower2(15L,1L) = 7L, dividePower2(-33L,4L) = -2L
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 15
 *   Rating: 2
 */
long dividePower2(long x, long n) {
    long is_negative = !!(x >> 63);
    long mask = x >> 63;
    long round = (is_negative << n) + (is_negative ^ mask) + is_negative;
    x += round;
    return x >> n;
}
/*
 * distinctNegation - returns 1 if x != -x.
 *     and 0 otherwise
 *   Legal ops: ! ~ & ^ | +
 *   Max ops: 5
 *   Rating: 2
 */
long distinctNegation(long x) {
    long neg = ~x + 1;
    return !!(x ^ neg);
}
/*
 * anyEvenBit - return 1 if any even-numbered bit in word set to 1
 *   where bits are numbered from 0 (least significant) to 63 (most significant)
 *   Examples anyEvenBit(0xAL) = 0L, anyEvenBit(0xEL) = 1L
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 14
 *   Rating: 2
 */
long anyEvenBit(long x) {
    long mask = 0x55;
    mask |= mask << 8;
    mask |= mask << 16;
    mask |= mask << 32;
    return (long)(!!(x & mask));
}
// 3
/*
 * isLessOrEqual - if x <= y  then return 1, else return 0
 *   Example: isLessOrEqual(4L,5L) = 1L.
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 24
 *   Rating: 3
 */
long isLessOrEqual(long x, long y) {
    // If x y have same sign, x ^ y's MSB is 0. 1 otherwise
    long sign = !((x ^ y) >> 63);
    // If x y have different sign and x is negative. Than x <= y always true
    long diff = (!!(x >> 63)) & (!sign);
    /*
     * If x y have same sign, -x and y have different sign. Therefore, y - x
     * never overflow. 0 <= y - x, if x is TMIN, -x is also TMIN. Since x, y
     * have same sign. y is negative in this case, therefore y + x = y + TMIN
     * always lead to a overflow and caused a positive number and always holds 0
     * <= y - x
     */
    long neg_x = (~x) + 1;
    long same = (!((y + neg_x) >> 63)) & sign;
    return same | diff;
}
/*
 * replaceByte(x,n,c) - Replace byte n in x with c
 *   Bytes numbered from 0 (LSB) to 3 (MSB)
 *   Examples: replaceByte(0x12345678,1,0xab) = 0x1234ab78
 *   You can assume 0 <= n <= 7 and 0 <= c <= 255
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 10
 *   Rating: 3
 */
long replaceByte(long x, long n, long c) {
    long shift = n << 3; // n * 8
    x &= ~(0xFFL << shift);
    x |= c << shift;
    return x;
}
/*
 * conditional - same as x ? y : z
 *   Example: conditional(2,4L,5L) = 4L
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 16
 *   Rating: 3
 */
long conditional(long x, long y, long z) {
    long condition = !!x;
    long mask = (condition << 63) >> 63;
    return (y & mask) | (z & ~mask);
}
/*
 * bitMask - Generate a mask consisting of all 1's
 *   between lowbit and highbit
 *   Examples: bitMask(5L,3L) = 0x38L
 *   Assume 0 <= lowbit < 64, and 0 <= highbit < 64
 *   If lowbit > highbit, then mask should be all 0's
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 16
 *   Rating: 3
 */
long bitMask(long highbit, long lowbit) {
    long neg1 = ~1L + 1;
    long hi_mask = (((1L << highbit) + neg1) << 1) + 1;
    long lo_mask = (1L << lowbit) + neg1;
    return hi_mask & ~lo_mask;
}
// 4
/*
 * isPalindrome - Return 1 if bit pattern in x is equal to its mirror image
 *   Example: isPalindrome(0x6F0F0123c480F0F6L) = 1L
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 70
 *   Rating: 4
 */
long isPalindrome(long x) {
    // 0x00000000ffffffff
    long mask = ~((1L << 63) >> 31);
    long prefix = (x >> 32) & mask;
    long postfix = x & mask;
    long rev_mask = (0xff << 8) | 0xff;
    // 0x0000ffff 0xffff0000
    prefix = ((prefix & rev_mask) << 16) | ((prefix & (~rev_mask)) >> 16);
    // 0x00ff00ff 0xff00ff00
    rev_mask = (0xff << 16) | 0xff;
    prefix = ((prefix & rev_mask) << 8) | ((prefix & (~rev_mask)) >> 8);
    // 0x0f0f0f0f 0xf0f0f0f0
    rev_mask = (0xf << 8) | 0xf;
    rev_mask = (rev_mask << 16) | rev_mask;
    prefix = ((prefix & rev_mask) << 4) | ((prefix & (~rev_mask)) >> 4);
    // 0x33333333
    rev_mask = (0x33 << 8) | 0x33;
    rev_mask = (rev_mask << 16) | rev_mask;
    prefix = ((prefix & rev_mask) << 2) | ((prefix & (~rev_mask)) >> 2);
    // 0x55555555
    rev_mask = (0x55 << 8) | 0x55;
    rev_mask = (rev_mask << 16) | rev_mask;
    prefix = ((prefix & rev_mask) << 1) | ((prefix & (~rev_mask)) >> 1);
    return !(prefix ^ postfix);
}
/*
 * trueFiveEighths - multiplies by 5/8 rounding toward 0,
 *  avoiding errors due to overflow
 *  Examples:
 *    trueFiveEighths(11L) = 6L
 *    trueFiveEighths(-9L) = -5L
 *    trueFiveEighths(0x3000000000000000L) = 0x1E00000000000000L (no overflow)
 *  Legal ops: ! ~ & ^ | + << >>
 *  Max ops: 20
 *  Rating: 4
 */
long trueFiveEighths(long x) {
    /*
     * For positive
     * x * 5 / 8 = x * (4 + 1) / 8 = (x * 4 + x) / 8
     * = x / 2 + x / 8
     * (x / 2 - e1) + (x / 8 - e2), e1 + e2 may greater than 1
     * e1 = 0.(x & 1), e2 = 0.(x & 0x7)
     */
    long neg = !!(x >> 63);
    long e1 = (x & 1L) << 2;
    long e2 = (x & 7L);
    long frac = e1 + e2;
    long carry = frac >> 3;
    long round = (!!(frac & 7)) & neg;
    return (x >> 1) + (x >> 3) + carry + round;
}
/*
 * logicalNeg - implement the ! operator, using all of
 *              the legal operators except !
 *   Examples: logicalNeg(3L) = 0L, logicalNeg(0L) = 1L
 *   Legal ops: ~ & ^ | + << >>
 *   Max ops: 12
 *   Rating: 4
 */
long logicalNeg(long x) {
    long neg = 1L << 63;
    x += neg;
    // abs
    long mask = x >> 63;
    x = (x ^ mask) + (mask & 1);
    // Now x is negative only if x is originally 0
    x >>= 63;
    x = ~x;
    x += 1;
    return x;
}
