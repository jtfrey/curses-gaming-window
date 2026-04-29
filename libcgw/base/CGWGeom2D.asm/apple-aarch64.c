
static inline
float
CGWPoint2DNormL1(
    CGWPoint2D  p
)
{
    float       out_norm;
    
    asm volatile (
    "ldr        %d0, [%x1]              // Vn = <p.x, p.y, …>\n"
    "fabs       %0.2S, %0.2S            // Vn = <|p.x|, |p.y|, …>\n"
    "faddp      %0.2S, %0.2S, %0.2S     // Vn = <|p.x|+|p.y|, …>\n"
    : "=w" (out_norm)
    : "r" (&p)
    : /* Nothing clobbered */
    );
    return out_norm;
}

//

static inline
float
CGWPoint2DNormL2(
    CGWPoint2D  p
)
{
    float       out_norm;
    
    asm volatile (
    "ldr        %d0, [%x1]              // Vn = <p.x, p.y, …>\n"
    "fmul       %0.2s, %0.2s, %0.2s     // Vn = <p.x * p.x, p.y * p.y, …>\n"
    "faddp      %0.2s, %0.2s, %0.2s     // Vn = <p.x * p.x + p.y * p.y, …>\n"
    "fsqrt      %s0, %s0                // Vn = Sqrt(p.x * p.x + p.y * p.y)\n"
    : "=w" (out_norm)
    : "r" (&p)
    : "v20"
    );
    return out_norm;
}

//

static inline
float
CGWPoint2DNormLInf(
    CGWPoint2D  p
)
{
    float       out_norm;
    
    asm volatile (
    "ldr        %d0, [%x1]              // Vn = <p.x, p.y, …>\n"
    "fabs       %0.2S, %0.2S            // Vn = <|p.x|, |p.y|, …>\n"
    "fmaxnmv    %s0, %0.4S              // Sn = max across the vector\n"
    : "=w" (out_norm)
    : "r" (&p)
    : /* Nothing clobbered */
    );
    return out_norm;
}

//

static inline
CGWXFormMatrix2D*
CGWXFormMatrix2DSetZero(
    CGWXFormMatrix2D    *M
)
{
    asm volatile (
    "                                   //        0...........31...........63\n"
    "                                   // XZR = [<0.0f> <0.0f> <0.0f> <0.0f>]\n"
    "stp        XZR, XZR, [%x0]         // M->M[0][0..3] = [<0.0f> <0.0f> <0.0f> <0.0f>\n"
    "                                   //                  <0.0f> <0.0f> <0.0f> <0.0f>]\n"
    "stp        XZR, XZR, [%x0, #16]    // M->M[0][4..7] = [<0.0f> <0.0f> <0.0f> <0.0f>\n"
    "                                   //                  <0.0f> <0.0f> <0.0f> <0.0f>]\n"
    : /* No output */
    : "r" (M)
    : /* No register usage */);
    return M;
}

//

static inline
CGWXFormMatrix2D*
CGWXFormMatrix2DSetIdentity(
    CGWXFormMatrix2D    *M
)
{
    asm volatile (
    "                                   //        0...........31...........63\n"
    "mov        X20, #0x3f800000        // X20 = [<1.0f> <0.0f> <0.0f> <0.0f>]\n"
    "                                   // XZR = [<0.0f> <0.0f> <0.0f> <0.0f>]\n"
    "stp        X20, XZR, [%x0]         // M->M[0][0..3] = [<1.0f> <0.0f> <0.0f> <0.0f>\n"
    "                                   //                  <0.0f> <0.0f> <0.0f> <0.0f>]\n"
    "lsl        X20, X20, #32           // X20 = [<0.0f> <1.0f> <0.0f> <0.0f>]\n"
    "stp        X20, XZR, [%x0, #16]    // M->M[1][0..3] = [<0.0f> <1.0f> <0.0f> <0.0f>\n"
    "                                   //                  <0.0f> <0.0f> <0.0f> <0.0f>]\n"
    : /* No output */
    : "r" (M)
    : "x20");
    return M;
}

//

static inline
CGWXFormMatrix2D*
CGWXFormMatrix2DSetTranslation(
    CGWXFormMatrix2D    *M,
    float               dx,
    float               dy
)
{
    asm volatile (
    "str         %s1, [%x0, #8]     // M->M[2] = dx\n"
    "str         %s2, [%x0, #24]    // M->M[6] = dy\n"
    : /* No output */
    : "r" (M), "w" (dx), "w" (dy)
    : /* No register usage */);
    return M;
}

//

static inline
CGWXFormMatrix2D*
CGWXFormMatrix2DTranslate(
    CGWXFormMatrix2D    *M,
    float               dx,
    float               dy
)
{
    asm volatile (
    "ldr         S20, [%x0, #8]     // S2 = M->M[2]\n"
    "fadd        S20, S20, %s1      // S2 += dx\n"
    "str         S20, [%x0, #8]     // M->M[2] = S2\n"
    "\n"
    "ldr         S20, [%x0, #24]    // S2 = M->M[6]\n"
    "fadd        S20, S20, %s2      // S2 += dy\n"
    "str         S20, [%x0, #24]    // M->M[6] = S2\n"
    : /* No output */
    : "r" (M), "w" (dx), "w" (dy)
    : "s20");
    return M;
}

//

static inline
CGWXFormMatrix2D*
CGWXFormMatrix2DScale(
    CGWXFormMatrix2D    *M,
    float               sx,
    float               sy
)
{
    asm volatile (
    "ldp    S20, S21, [%x0]        // S20 = M->M[0], S21 = M->M[1]\n"
    "fmul   S20, %s1, S20          // S20 *= sx\n"
    "fmul   S21, %s1, S21          // S21 *= sx\n"
    "stp    S20, S21, [%x0]        // M->M[0] = S20, M->M[1] = S21\n"
    "\n"
    "ldp    S20, S21, [%x0, #16]   // S20 = M->M[4], S21 = M->M[5]\n"
    "fmul   S20, %s2, S20          // S20 *= sy\n"
    "fmul   S21, %s2, S21          // S21 *= sy\n"
    "stp    S20, S21, [%x0, #16]\n"
    : /* No output */
    : "r" (M), "w" (sx), "w" (sy)
    : "s20", "s21");
    return M;
}

//

static inline
CGWXFormMatrix2D*
CGWXFormMatrix2DScaledSum(
    CGWXFormMatrix2D    *out_M,
    float               A,
    CGWXFormMatrix2D    *X,
    CGWXFormMatrix2D    *Y
)
{
    asm volatile (
    "    ldp     Q22, Q23, [%x3]            // load the two rows of Y into V22 and V23\n"
    "    fmov    W20, %s1                   // save a copy of A in W20 (we've loaded all of Y)\n"
    "    cbnz    W20, 1f                    // if A != 0, skip ahead...\n"
    "    stp     Q22, Q23, [%x0]            // ...this is just a copy of Y into out_M\n"
    "    ret\n"
    "    \n"
    "1:  ldp     Q20, Q21, [%x2]            // load the two rows of X into V20 and V21\n"
    "    mov     W4, #0x3f800000            // binary form of +1.0f\n"
    "    cmp     W20, W4                    // does A == 1?\n"
    "    b.ne    2f\n"
    "    fadd    V20.4s, V22.4s, V20.4s     // since A == 1, it's an unscaled sum\n"
    "    fadd    V21.4s, V23.4s, V21.4s\n"
    "    stp     Q20, Q21, [%x0]\n"
    "    ret\n"
    "  \n"
    "2:  orr     W4, W4, #0x80000000        // toggle negative on our 1.0f register\n"
    "    cmp     W20, W4                    // does A == -1?\n"
    "    b.ne    3f\n"
    "    fsub    V20.4s, V22.4s, V20.4s     // since A == -1, it's an unscaled difference\n"
    "    fsub    V21.4s, V23.4s, V21.4s\n"
    "    stp     Q20, Q21, [%x0]\n"
    "    ret\n"
    "\n"
    "3:  dup     V24.4S, W20                // fill V24 with 4 copies of W20 (= scalar A)\n"
    "    fmla    V22.4s, V20.4s, V24.4s     // V20 = A * X_1 + Y_1\n"
    "    fmla    V23.4s, V21.4s, V24.4s     // V20 = A * X_2 + Y_2\n"
    "    stp     Q22, Q23, [%x0]\n"
    : /* No output */
    : "r" (out_M), "w" (A), "r" (X), "r" (Y)
    : "x20", "v20", "v21", "v22", "v23", "v24");
    return out_M;
}

//

static inline
CGWPoint2D*
CGWXFormMatrix2DDotPoint2D(
    CGWPoint2D          *out_p,
    CGWXFormMatrix2D    *X,
    CGWPoint2D          *p
)
{
    asm volatile (
    "ldp     Q20, Q21, [%x1]            // load the two rows of X into V20 and V21\n"
    "ldr     D22, [%x2]                 // load p.x, p.y into the lowest two slots\n"
    "                                   // of V22.4S\n"
    "mov     X20, #0x000000003f800000   // load the binary form of <1.0f, 0.0f> into X20\n"
    "mov     V22.D[1], X20              // move <1.0f, 0.0f> into the upper half of V22.4S\n"
    "\n"
    "fmul    V23.4S, V20.4S, V22.4S     // V23 = elements of V20 . V22 (first row of X . p)\n"
    "faddp   V23.4S, V23.4S, V23.4S     // V23[0] = V23[0] + V23[1]; V23[1] = V23[2] + V23[3]\n"
    "faddp   V23.4S, V23.4S, V23.4S     // V23[0] = V23[0] + V23[1]\n"
    "                                   // p.x = V23[0]\n"
    "\n"
    "fmul    V24.4S, V21.4S, V22.4S     // V24 = elements of V21. V22 (second row of X . p)\n"
    "faddp   V24.4S, V24.4S, V24.4S     // V24[0] = V24[0] + V24[1]; V24[1] = V24[2] + V24[3]\n"
    "faddp   V24.4S, V24.4S, V24.4S     // V24[0] = V24[0] + V24[1]\n"
    "                                   // p.y = V24[0]\n"
    "\n"
    "stp     S23, S24, [%x0]            // out_p = <S3, S4>\n"
    : /* No output */
    : "r" (out_p), "r" (X), "r" (p)
    : "x20", "v20", "v21", "v22", "v23", "v24");
    return out_p;
}

//

static inline
CGWXFormMatrix2D*
CGWXFormMatrix2DMultiply(
    CGWXFormMatrix2D    *out_M,
    CGWXFormMatrix2D    *X,
    CGWXFormMatrix2D    *Y
)
{
    //
    //      X         Y
    //
    //      a b c 0   u v w 0     au + bx   av + by    aw + bz + c  0
    //      d e f 0   x y z 0  =  du + ex   dv + ey    dw + ez + f  0
    //      0 0 1 0   0 0 1 0     0         0          1            0
    //      0 0 0 0   0 0 0 0     0         0          0            0
    //
    // The CGWXFormMatrix2D only stores the first two rows; the other
    // two rows are implicit.  The trailing zero on the rows is present
    // for the sake of alignment.
    //
    // Calculating all matrix elements requires six (6) registers for
    // each element plus two for the elements of X and one additional
    // scratch register:  9 total.  This implementation uses S0 - S8.
    // Since all data are held until the end, no check of out_M's
    // being the same pointer as X or Y is necessary.
    //
    // Originally I'd thought to use SIMD, but ca. 50% of the factors
    // are zero so the extra work shifting Y matrix elements into V#
    // registers is likely more effort than just doing the work serially
    // through the FPU.
    //
    asm volatile (
    "ldp     S20, S28, [%x1]        // S20 = a, S28 = b\n"
    "\n"
    "ldr     S21, [%x2]             // S21 = u\n"
    "fmul    S21, S20, S21          // S21 = au\n"
    "\n"
    "ldr     S22, [%x2, #4]         // S22 = v\n"
    "fmul    S22, S20, S22          // S22 = av\n"
    "\n"
    "ldr     S23, [%x2, #8]         // S23 = w\n"
    "fmul    S23, S20, S23          // S23 = aw\n"
    "\n"
    "ldr     S24, [%x2, #16]        // S24 = x\n"
    "fmadd   S21, S24, S28, S21     // S21 += S24 * S28 = au + bx\n"
    "\n"
    "ldr     S24, [%x2, #20]        // S24 = y\n"
    "fmadd   S22, S24, S28, S22     // S22 += S24 * S28 = av + by\n"
    "\n"
    "ldr     S24, [%x2, #24]        // S24 = z\n"
    "fmadd   S23, S24, S28, S23     // S23 += S24 * S28 = aw + bz\n"
    "\n"
    "ldr     S24, [%x1, #8]         // S24 = c\n"
    "fadd    S23, S23, S24          // S23 += S24 = aw + bz + c\n"
    "\n"
    "// The first row of out_M matrix elements is in S21, S22, S23\n"
    "\n"
    "ldp     S20, S28, [%x1, #16]   // S20 = d, S28 = e\n"
    "\n"
    "ldr     S25, [%x2]             // S25 = u\n"
    "fmul    S25, S20, S25          // S25 = du\n"
    "\n"
    "ldr     S26, [%x2, #4]         // S26 = v\n"
    "fmul    S26, S20, S26          // S26 = dv\n"
    "\n"
    "ldr     S27, [%x2, #8]         // S27 = w\n"
    "fmul    S27, S20, S27          // S27 = dw\n"
    "\n"
    "ldr     S24, [%x2, #16]        // S24 = x\n"
    "fmadd   S25, S24, S28, S25     // S25 += S24 * S28 = du + ex\n"
    "\n"
    "ldr     S24, [%x2, #20]        // S24 = y\n"
    "fmadd   S26, S24, S28, S26     // S26 += S24 * S28 = dv + ey\n"
    "\n"
    "ldr     S24, [%x2, #24]        // S24 = z\n"
    "fmadd   S27, S24, S28, S27     // S27 += S24 * S28 = dw + ez\n"
    "\n"
    "ldr     S24, [%x1, #24]        // S24 = f\n"
    "fadd    S27, S27, S24          // S27 += S24 = dw + ez + f\n"
    "\n"
    "// The second row of out_M matrix elements is in S25, S26, S27\n"
    "\n"
    "stp     S21, S22, [%x0]        // out_M[0,0], out_M[0,1] <- S21, S22\n"
    "str     S23, [%x0, #8]         // out_M[0,2] <- S23\n"
    "stp     S25, S26, [%x0, #16]   // out_M[1,0], out_M[1,1] <- S25, S26\n"
    "str     S27, [%x0, #24]        // out_M[1,2] <- S27\n"
    : /* No output */
    : "r" (out_M), "r" (X), "r" (Y)
    : "s20", "s21", "s22", "s23", "s24", "s25", "s26", "s27", "s28");
    return out_M;
}

//

static inline
CGWXFormMatrix2D*
CGWXFormMatrix2DInvert(
    CGWXFormMatrix2D    *A_inv,
    CGWXFormMatrix2D    *A
)
{
    asm volatile (
    "// Load the six matrix elements:\n"
    "ldp     s20, s21, [%x1]\n"
    "ldr     s22, [%x1, #8]\n"
    "ldp     s23, s24, [%x1, #16]\n"
    "ldr     s25, [%x1, #24]\n"
    "\n"
    "fmul    s26, s21, s25              // compute…\n"
    "fmsub   s26, s22, s24, s26         // …a2 = s26\n"
    "fmul    s27, s22, s23              // compute…\n"
    "fmsub   s27, s20, s25, s27         // …a3 = s27\n"
    "// At this point we don't need s22 or s25 any longer\n"
    "fmul    s22, s20, s24              // compute…\n"
    "fmsub   s22, s21, s23, s22         // …a1 = s22\n"
    "// We don't need a1, though, we need 1/a1:\n"
    "fmov    s25, #1.0\n"
    "fdiv    s22, s25, s22\n"
    "\n"
    "// Calculate matrix elements and store them:\n"
    "fmul    s25, s22, s24              // A_00\n"
    "fnmul   s24, s22, s21              // A_01\n"
    "stp     s25, s24, [%x0]\n"
    "fmul    s25, s22, s26              // A_02\n"
    "str     s25, [%x0, #8]\n"
    "fnmul   s25, s22, s23              // A_10\n"
    "fmul    s24, s22, s20              // A_11\n"
    "stp     s25, s24, [%x0, #16]\n"
    "fmul    s25, s22, s27              // A_12\n"
    "str     s25, [%x0, #24]\n"
    : /* No output */
    : "r" (A_inv), "r" (A)
    : "s20", "s21", "s22", "s23", "s24", "s25", "s26", "s27");
    return A_inv;
}

//

static inline
CGWXFormMatrix2D*
CGWXFormMatrix2DInvertGE(
    CGWXFormMatrix2D    *A_inv,
    CGWXFormMatrix2D    *A
)
{
    asm volatile (
    "//\n"
    "// Input registers:  Ainv => %x0\n"
    "//                   A    => %x1\n"
    "//\n"
    "// [[s20 s21 s22 0]\n"
    "//  [s23 s24 s25 0]\n"
    "//  [0   0   1   0]]\n"
    "//\n"
    "// Scratch registers 26 - 31\n"
    "// \n"
    "ldp     s20, s21, [%x1]\n"
    "fabs    s22, s20\n"
    "fabs    s23, s21\n"
    "fcmp    s22, s23\n"
    "// The status registers are now setup to indicate\n"
    "// .lt if the two rows need to be swapped, .ge if\n"
    "// they're fine as-is.\n"
    "\n"
    "// Row 0 starts at offset 0; if the fcmp was .lt then\n"
    "// we want x20 => R1 and x21 => R0, otherwise x20 => R0\n"
    "// and x21 => R1 (if fcmp was .ge).\n"
    "// Row 1 starts at offset 16, put that in x20:\n"
    "mov     x20, #16\n"
    "//\n"
    "// Load the second-row matrix elements:\n"
    "csel    x21, x20, xzr, ge\n"
    "add     x21, %x1, x21\n"
    "ldp     s23, s24, [x21]\n"
    "ldr     s25, [x21, #8]\n"
    "\n"
    "// Load the first-row matrix elements:\n"
    "csel    x20, xzr, x20, ge\n"
    "add     x20, %x1, x20\n"
    "ldp     s20, s21, [x20]\n"
    "ldr     s22, [x20, #8]\n"
    "\n"
    "// We are now done with %x1, x20, and x21 and they can be\n"
    "// reused.  Compute the A_i0, A_i1 store offsets now:\n"
    "mov     x22, #4                     // get an addend of 4 ready\n"
    "csel    x20, xzr, x22, ge           // x20 = A_i0 = (swap?) ? 4 : 0\n"
    "csel    x21, x22, xzr, ge           // x21 = A_i1 = (swap?) ? 0 : 4\n"
    "\n"
    "// Compute a through f:\n"
    "fdiv    s26, s23, s20               // a = A_10 / A_00\n"
    "fmsub   s27, s26, s21, s24          // b = A_11 - a * A_01\n"
    "fmov    s28, #1.0\n"
    "fdiv    s27, s28, s27               // b = 1 / b\n"
    "fmsub   s28, s26, s22, s25          // c = A_12 - a * A_02\n"
    "fmul    s29, s24, s27               // d = A_11 * b\n"
    "fnmul   s30, s21, s27               // e = -A_01 * b\n"
    "fmul    s31, s21, s25               // f = A_01 * A_12\n"
    "fmsub   s31, s22, s24, s31          // f = f - A_02 * A_11\n"
    "fmul    s31, s27, s31               // f = f * b\n"
    "\n"
    "// The first row elements of the inverse\n"
    "// are not affected by the row swap, except\n"
    "// w.r.t. what A_ij values are in the registers:\n"
    "fdiv    s29, s29, s20               // Ainv_00 = d / A_00\n"
    "fdiv    s30, s30, s20               // Ainv_01 = e / A_00\n"
    "str     s29, [%x0, x20]\n"
    "str     s30, [%x0, x21]\n"
    "fdiv    s31, s31, s20               // Ainv_02 = f / A_00\n"
    "str     s31, [%x0, #8]\n"
    "\n"
    "fnmul   s29, s28, s27               // -c * b\n"
    "str     s29, [%x0, #24]             // Ainv_12 = -c * b\n"
    "fnmul   s26, s26, s27               // -a * b\n"
    "\n"
    "// Rather than doing a paired-store, store the A_10 and\n"
    "// A_11 elements separately using the offset registers\n"
    "// we calculated:\n"
    "add     x20, x20, #16\n"
    "add     x21, x21, #16\n"
    "str     s26, [%x0, x20]\n"
    "str     s27, [%x0, x21]\n"
    : /* No output */
    : "r" (A_inv), "r" (A)
    : "s20", "s21", "s22", "s23", "s24", "s25", "s26", "s27",
      "s28", "s29", "s30", "s31", "x20", "x21", "x22");
    return A_inv;
}