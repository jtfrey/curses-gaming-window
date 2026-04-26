//
// CGWGeom.s
//
// Apple Silicon (ARM64) assembly language subroutines
// and functions.
//

.macro	PUSH1 register
		STR	\register, [SP, #-16]!
.endmacro

.macro	POP1 	register
		LDR	\register, [SP], #16
.endmacro

.macro 	PUSH2 register1, register2
		STP	\register1, \register2, [SP, #-16]!
.endmacro

.macro 	POP2	register1, register2
		LDP	\register1, \register2, [SP], #16
.endmacro



.global _CGWPolarPointStandardizeAsm
.align 4

// CGWPolarPoint* <- X0
_CGWPolarPointStandardizeAsm:
// (CGWPolarPoint   *p_out -> X0,
//  CGWPolarPoint   *p_in -> X1,
//  float           epsilon -> S0)
            // Float registers 8 - 15 (lowest 64-bits) must be saved
            PUSH2   D8, D9
            adrp    X2, CGWGeom_2PI@PAGE
            add     X2, X2, CGWGeom_2PI@PAGEOFF
            ldp     S6, S7, [X2]        // S6 = 2π, S7 = π
            fmov    S8, #0.0            // S8 = 0.0
            fmov    S9, #1.0            // S9 = 1.0
            ldp     S1, S2, [X1]        // S1 = p_in->r, S2 = p_in->theta
            fcmp    S1, #0.0            // S1 == 0.0f ?
            b.ge    2f
            fneg    S1, S1              // S1 = -S1
            fcmp    S2, #0.0            // S2 == 0.0f?
            b.lt    1f
            fsub    S2, S2, S7          // S2 -= π
            b       2f
1:          fadd    S2, S2, S7          // S2 += π
2:          fabs    S4, S1              // S4 = |S1|
            fcmp    S4, S0              // S4 == S0 (epsilon)
            b.lt    8f                  // Return <0,0>

            // S1 now contains a standardized and clamped value of r
            // S2 contains the (possibly rotated) value of theta
            fabs    S4, S2              // S4 = fabs(S2)
            fdiv    S4, S4, S6          // S4 = n_2xpi = fabs(theta) / 2π
            frintz  S4, S4              // S4 = n_2xpi = floor(fabs(theta) / 2π)
            fcmp    S2, #0.0            // S2 == 0.0f?
            fcsel   S5, S9, S8, lt      // S5 = (theta < 0.0f) ? 1.0 : 0.0
            fadd    S4, S4, S5          // S4 = n_2xpi = floor(fabs(theta) / 2π) + ((theta < 0.0f) ? 1.0 : 0.0)
            b.lt    3f                  // React to previous fcmp...
            fneg    S4, S4              // S4 = -S4
3:          fcmp    S4, #0.0            // S4 == 0.0f?
            fcsel   S4, S4, S8, ne      // S4 = (S4 == 0.0f) ? 0.0f : n_2xpi
            fmadd   S2, S4, S6, S2      // S2 += n_2xpi * 2π
            fcmp    S2, S7              // S2 == π?
            fcsel   S5, S6, S8, ge      // S5 = (S2 >= π) ? 2π : 0
            fsub    S2, S2, S5          // S2 -= (S2 >= π) ? 2π : 0
            fsub    S5, S2, S7          // S5 = theta - π
            fabs    S5, S5              // S5 = |theta - π|
            fcmp    S5, S0              // S5 == S0 (epsilon?)
            fcsel   S5, S7, S8, lt      // S5 = (|theta - π| < epsilon) ? π : 0.0f
            fsub    S2, S2, S5          // S2 -= (|theta - π| < epsilon) ? π : 0.0f
            fabs    S5, S2              // S5 = |theta|
            fcmp    S5, S0              // |theta| == epsilon?
            b.ge    4f
            fmov    S2, #0.0            // return theta = 0.0f
            b       7f
4:          fsub    S4, S2, S7          // S4 = theta - π
            fabs    S4, S4              // S4 = |theta - π|
            fcmp    S4, S0              // S4 == epsilon?
            b.ge    5f
            fsub    S2, S8, S7          // theta = 0.0f - π
            b       7f
5:          adrp    X2, CGWGeom_PI_2@PAGE
            add     X2, X2, CGWGeom_PI_2@PAGEOFF
5:          ldr     S6, [X2]            // S6 = π/2
            fsub    S4, S2, S6          // S4 = theta - π/2
            fabs    S4, S4              // S4 = |theta - π/2|
            fcmp    S4, S0              // S4 == epsilon?
            b.ge    6f
            fmov    S2, S6              // S2 = π/2
            b       7f
6:          fadd    S4, S2, S6          // S4 = theta + π/2
            fabs    S4, S4              // S4 = |theta + π/2|
            fcmp    S4, S0              // S4 == epsilon?
            b.ge    7f
            fsub    S2, S8, S6          // S2 = 0 - π/2
7:          stp     S1, S2, [X0]        // out_p->r = S1, out_p->theta = S2
            b       9f
8:          stp     WZR, WZR, [X0]      // out_p->r = out_p->theta = 0.0
            // Pop the stack and return:
9:          POP2    D8, D9
            ret



.global _CGWPolarPointThetaClampWithThresholdsAsm
.align 4

// float <- S0
_CGWPolarPointThetaClampWithThresholdsAsm:
// (float theta -> S0,
//  float theta_min_x -> S1
//  float theta_min_y -> S2)
                    //
                    // S3:  2π
                    // S4:  scratch, π, π/2
                    // S5:  scratch
                    //      
                    adrp    X0, CGWGeom_2PI@PAGE
                    add     X0, X0, CGWGeom_2PI@PAGEOFF
                    ldr     S3, [X0]                    // S3 = 2π
                    fabs    S4, S0                      // S4 = fabs(A0 = theta)
                    fdiv    S5, S4, S3                  // S5 = n_2xpi = fabs(theta) / 2π
                    frintz  S5, S5                      // S5 = n_2xpi = floor(n_2xpi)
                    fcmp    S0, #0.0                    // theta < 0?
                    b.ge    F0001_no_inc_n_2xpi
                    fmov    S6, #1.0                    // S6 = 1.0
                    fadd    S5, S5, S6                  // S5 = n_2xpi = n_2xpi + 1
                    b       F0001_calc_theta_prime      // no need to check for n_2xpi > 0
F0001_no_inc_n_2xpi:      
                    fcmp    S5, #0.0
                    b.eq    F0001_n_2xpi_is_zero
                    fneg    S5, S5                      // we will be adding 2π(-n_2xpi)
F0001_calc_theta_prime:   
                    fmadd   S0, S5, S3, S0              // S0 = theta = theta_prime = n_2xpi * ±2π + theta

F0001_n_2xpi_is_zero:     
                    adrp    X0, CGWGeom_PI@PAGE
                    add     X0, X0, CGWGeom_PI@PAGEOFF
                    ldr     S4, [X0]
                    fcmp    S0, S4                      //S0 == π?
                    b.lt    F0001_skip_ge_pi
                    fsub    S0, S0, S3                  // S0 -= 2π

F0001_skip_ge_pi:
                    fsub    S5, S0, S4                  // S4 still has π, so S5 = theta - π
                    fabs    S5, S5
                    adrp    X0, CGWGeom_ZEROISH@PAGE
                    add     X0, X0, CGWGeom_ZEROISH@PAGEOFF
                    ldr     S6, [X0]                    // S6 = 1e-6
                    fcmp    S5, S6                      // |theta - π| < 1e-6?
                    b.ge    F0001_skip_force_neg_pi
                    fmov    S0, S4
                    fneg    S0, S0                      // theta = 0 - π
                    
F0001_skip_force_neg_pi:
                    fabs    S5, S0                      // |theta|
                    fcmp    S5, S1                      // |theta| < theta_min_x?
                    b.ge    F0001_skip_zero
                    fmov    S0, #0.0                    // return 0.0
                    ret
                    
F0001_skip_zero:
                    fsub    S6, S4, S1                  // π - theta_min_x
                    fcmp    S5, S6                      // |theta| > π - theta_min_x?
                    b.le    F0001_skip_neg_pi
                    fmov    S0, S4
                    fneg    S0, S0                      // return 0 - π
                    ret

F0001_skip_neg_pi:
                    adrp    X0, CGWGeom_PI_2@PAGE
                    add     X0, X0, CGWGeom_PI_2@PAGEOFF
                    ldr     S4, [X0]                    // S4 = π/2
                    fsub    S5, S0, S4                  // S5 = theta - π/2
                    fabs    S5, S5                      // |theta - π/2|
                    fcmp    S5, S2                      // |theta - π/2| == theta_min_y?
                    b.ge    F0001_skip_pi_2
                    fmov    S0, S4                      // return π/2
                    ret
                    
F0001_skip_pi_2:
                    fadd    S5, S0, S4
                    fabs    S5, S5                      // |theta + π/2|
                    fcmp    S5, S2                      // |theta + π/2| < theta_min_y
                    b.ge    F0001_skip_neg_pi_2
                    fmov    S0, S4
                    fneg    S0, S0
                    
F0001_skip_neg_pi_2:
                    ret


.data

CGWGeom_2PI:        .single  6.28318530717958647692528676655900577
CGWGeom_PI:         .single  3.14159265358979323846264338327950288
CGWGeom_NEG_PI:     .single -3.14159265358979323846264338327950288
CGWGeom_PI_2:       .single  1.57079632679489661923132169163975144
CGWGeom_ZEROISH:    .single  1e-6

