
#include <math.h>
#include <float.h>

#include "CGWFixedPrec.h"

int
main()
{
    CGWFixedPrec    pi = CGWFixedPrecCreateWithFloat(M_PI);
    CGWFixedPrec    three = CGWFixedPrecCreateWithFloat(3.0f);
    CGWFixedPrec    tmp;
    
    printf("Fractional bits   = %d\n", CGW_FIXEDPREC_FRAC_BITS);
    printf("Whole bits        = %d\n", CGW_FIXEDPREC_WHOLE_BITS);
    printf("Minimum bits      = %d\n", CGW_FIXEDPREC_MIN_BITS);
    printf("Total bits        = %d\n", CGW_FIXEDPREC_BITS);
    printf("Significand bits  = %d\n", CGW_FIXEDPREC_SIGBITS);
    
    printf(CGW_FIXEDPREC_FMT " (%d)\n", pi, cgw_fperrno);
    printf("%f\n", CGWFixedPrecToFloat(pi));
    
    pi = CGWFixedPrecAdd(pi, pi);
    printf(CGW_FIXEDPREC_FMT " (%d)\n", pi, cgw_fperrno);
    printf("%f\n", CGWFixedPrecToFloat(pi));
    
    pi = CGWFixedPrecAdd(pi, pi);
    printf(CGW_FIXEDPREC_FMT " (%d)\n", pi, cgw_fperrno);
    printf("%f\n", CGWFixedPrecToFloat(pi));
    
    pi = CGWFixedPrecCreateWithFloat(0.0000000000000001);
    printf(CGW_FIXEDPREC_FMT " (%d)\n", pi, cgw_fperrno);
    printf("%f\n\n", CGWFixedPrecToFloat(pi));
    
    pi = CGWFixedPrecCreateWithFloat(M_PI);
    printf(CGW_FIXEDPREC_FMT " (%d)\n", pi, cgw_fperrno);
    printf("%f\n", CGWFixedPrecToFloat(pi));
    printf(CGW_FIXEDPREC_FMT " (%d)\n", three, cgw_fperrno);
    printf("%f\n", CGWFixedPrecToFloat(three));
    
    pi = CGWFixedPrecMul(pi, three);
    printf("CGWFixedPrecMul = " CGW_FIXEDPREC_FMT " (%d)\n", pi, cgw_fperrno);
    printf("%f\n", CGWFixedPrecToFloat(pi));
    pi = CGWFixedPrecMod(pi, &tmp);
    printf("CGWFixedPrecMul = " CGW_FIXEDPREC_FMT " + " CGW_FIXEDPREC_FMT " (%d)\n", tmp, pi, cgw_fperrno);
    printf("%f + %f\n", CGWFixedPrecToFloat(tmp), CGWFixedPrecToFloat(pi));
    pi += tmp;
    pi = -pi;
    printf(CGW_FIXEDPREC_FMT " (%d)\n", pi, cgw_fperrno);
    printf("%f\n\n", CGWFixedPrecToFloat(pi));
    
    pi = CGWFixedPrecOneQuarter;
    CGWFixedPrecErrorReset();
    printf(CGW_FIXEDPREC_FMT " = %f (%d)\n", pi, CGWFixedPrecToFloat(pi), cgw_fperrno);
    do {
        pi = CGWFixedPrecMulPow2(pi, -1);
        printf(CGW_FIXEDPREC_FMT " = %f (%d)\n", pi, CGWFixedPrecToFloat(pi), cgw_fperrno);
    } while ( pi && cgw_fperrno == kCGWFixedPrecErrorNone);
    printf("\n");
    
    pi = CGWFixedPrecOne;
    CGWFixedPrecErrorReset();
    printf(CGW_FIXEDPREC_FMT " = %f (%d)\n", pi, CGWFixedPrecToFloat(pi), cgw_fperrno);
    do {
        pi = CGWFixedPrecMulPow2(pi, 1);
        printf(CGW_FIXEDPREC_FMT " = %f (%d)\n", pi, CGWFixedPrecToFloat(pi), cgw_fperrno);
    } while ( pi && cgw_fperrno == kCGWFixedPrecErrorNone);
    printf("\n");
    
    pi = CGWFixedPrecNegOne;
    CGWFixedPrecErrorReset();
    printf(CGW_FIXEDPREC_FMT " = %f (%d)\n", pi, CGWFixedPrecToFloat(pi), cgw_fperrno);
    do {
        pi = CGWFixedPrecMulPow2(pi, 1);
        printf(CGW_FIXEDPREC_FMT " = %f (%d)\n", pi, CGWFixedPrecToFloat(pi), cgw_fperrno);
    } while ( pi && cgw_fperrno == kCGWFixedPrecErrorNone);
    printf("\n");
    
    pi = -CGWFixedPrecOneQuarter;
    CGWFixedPrecErrorReset();
    printf(CGW_FIXEDPREC_FMT " = %f (%d)\n", pi, CGWFixedPrecToFloat(pi), cgw_fperrno);
    do {
        pi = CGWFixedPrecMulPow2(pi, -1);
        printf(CGW_FIXEDPREC_FMT " = %f (%d)\n", pi, CGWFixedPrecToFloat(pi), cgw_fperrno);
    } while ( pi && cgw_fperrno == kCGWFixedPrecErrorNone);
    printf("\n");
    
    pi = -CGWFixedPrecOne;
    CGWFixedPrecErrorReset();
    printf(CGW_FIXEDPREC_FMT " = %f (%d)\n", pi, CGWFixedPrecToFloat(pi), cgw_fperrno);
    do {
        pi = CGWFixedPrecMulPow2(pi, 1);
        printf(CGW_FIXEDPREC_FMT " = %f (%d)\n", pi, CGWFixedPrecToFloat(pi), cgw_fperrno);
    } while ( pi && cgw_fperrno == kCGWFixedPrecErrorNone);
    printf("\n");
    
    pi = CGWFixedPrecCreateWithFloat(M_PI);
    printf(CGW_FIXEDPREC_FMT " = %f (%d)\n", pi, CGWFixedPrecToFloat(pi), cgw_fperrno);
    pi = CGWFixedPrecMul(pi, CGWFixedPrecTwo);
    printf(CGW_FIXEDPREC_FMT " = %f (%d)\n", pi, CGWFixedPrecToFloat(pi), cgw_fperrno);
    printf("\n");
    
    pi = CGWFixedPrecOneHalf;
    CGWFixedPrecErrorReset();
    printf(CGW_FIXEDPREC_FMT " = %f (%d)\n", pi, CGWFixedPrecToFloat(pi), cgw_fperrno);
    pi = CGWFixedPrecMul(pi, CGWFixedPrecOneEighth);
    printf(CGW_FIXEDPREC_FMT " = %f (%d)\n", pi, CGWFixedPrecToFloat(pi), cgw_fperrno);
    printf("\n");
    
    int             si = -512;
    
    printf("%d = 0x%08X = " CGW_FIXEDPREC_FMT " (%d)\n", si, si, (pi = CGWFixedPrecCreateWithInt(si)), cgw_fperrno);
    printf("   = %f\n\n", CGWFixedPrecToFloat(pi));
    
    unsigned int    ui = 512;
    
    printf("%u = 0x%08X = " CGW_FIXEDPREC_FMT " (%d)\n", ui, ui, (pi = CGWFixedPrecCreateWithUnsignedInt(ui)), cgw_fperrno);


    CGWFixedPrec    DX = CGWFixedPrecCreateWithDouble(-4.5);
    
    printf("   = %f + (%f)\n", CGWFixedPrecToFloat(pi), CGWFixedPrecToFloat(DX));
    
    if ( CGWFixedPrecAbs(DX) > CGWFixedPrecOne ) {
        pi = CGWFixedPrecAdd(pi, CGWFixedPrecExtractWhole(DX));
        DX = CGWFixedPrecExtractFrac(DX);
    }
    printf("   = %f + (%f)\n\n", CGWFixedPrecToFloat(pi), CGWFixedPrecToFloat(DX));
    
    long int        sli = -384;
    
    printf("%ld = 0x%016lX = " CGW_FIXEDPREC_FMT " (%d)\n", sli, sli, (pi = CGWFixedPrecCreateWithLongInt(sli)), cgw_fperrno);
    printf("   = %f\n\n", CGWFixedPrecToFloat(pi));
    
    return 0;
}
