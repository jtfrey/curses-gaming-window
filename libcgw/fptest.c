
#include <math.h>
#include <float.h>

#include "CGWFixedPrec.h"
#include "CGWGeom.h"

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
    
    CGWPointFP      p1 = CGWPointFPMake(-2.25, 0.75);
    
    printf("p               = <(%d) + (" CGW_FIXEDPREC_FMT "), (%d) + (" CGW_FIXEDPREC_FMT ") = <%f, %f>\n",
            p1.whole.x, p1.fx, p1.whole.y, p1.fy, 
            (double)p1.whole.x + CGWFixedPrecToDouble(p1.fx),
            (double)p1.whole.y + CGWFixedPrecToDouble(p1.fy));
    
    CGWPointFPTranslateWhole(&p1, 5, -2);
    printf("p + <5, -2>     = <(%d) + (" CGW_FIXEDPREC_FMT "), (%d) + (" CGW_FIXEDPREC_FMT ") = <%f, %f>\n\n",
            p1.whole.x, p1.fx, p1.whole.y, p1.fy, 
            (double)p1.whole.x + CGWFixedPrecToDouble(p1.fx),
            (double)p1.whole.y + CGWFixedPrecToDouble(p1.fy));
    
    int     i, r;
    
    for ( i = 0; i < 40; i++ ) printf("Sqrt(%d) = %d (frac %d)\n", i, CGWSqrtInt(i, &r), r);
    printf("\n");
    
    CGWCircle       c = CGWCircleMake(0, 0, 3);
    
    for ( i = 0; i < 5; i++ )
        printf("(%1$d, %1$d) in c = %2$d\n", i, CGWCircleContainsPoint(c, CGWPointMake(i, i)));
    printf("\n");
    
    CGWCircle       c2 = CGWCircleMake(0, 0, 1);
    
    for ( i = 5; i > 0; i-- ) {
        c2.origin.x = c2.origin.y = i;
        printf("{(%d,%d) r=%d} overlap {(%d,%d) r=%d} => %d\n", 
            c.origin.x, c.origin.y, c.radius,
            c2.origin.x, c2.origin.y, c2.radius,
            CGWCircleOverlapsCircle(c, c2));
    }
    printf("\n");
    c2.origin.y = 0;
    for ( i = 5; i > 0; i-- ) {
        c2.origin.x = i;
        printf("{(%d,%d) r=%d} overlap {(%d,%d) r=%d} => %d\n", 
            c.origin.x, c.origin.y, c.radius,
            c2.origin.x, c2.origin.y, c2.radius,
            CGWCircleOverlapsCircle(c, c2));
    }
    printf("\n");
    
    CGWRect     R = CGWRectMake(4, 4, 2, 4);   /* (4, 4) -> (6, 4) -> (6, 8) -> (4, 8) */
    c = CGWCircleMake(0, 0, 2);
    for ( i = 0; i < 13; i++ ) {
        c.origin.x = c.origin.y = i;
        printf("{(%d,%d) r=%d} overlap {(%d,%d) x (%u,%u)} => %d\n", 
            c.origin.x, c.origin.y, c.radius,
            R.origin.x, R.origin.y, R.size.w, R.size.h,
            CGWCircleOverlapsRect(c, R));
    }
    printf("\n");
    return 0;
}
