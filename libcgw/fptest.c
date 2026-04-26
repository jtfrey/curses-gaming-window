
#include <math.h>
#include <float.h>

#if 0

#include "CGWFixedPrec.h"
#include "CGWGeom2D.h"
#include "CGWCSGeom2D.h"
#include "CGWTiming.h"

CGWCSGeomShapeI     hitbox_list[] = {
                            CGWCSGeomIStructAddCircle(-1, 0, 2)
                            CGWCSGeomIStructAddRect(-1, -1, 2, 2)
                            CGWCSGeomIStructAddCircle( 1, 0, 3)
                            CGWCSGeomIStructAddPoint(0, 0)
                            CGWCSGeomIStructAddPoint(0, 1)
                            CGWCSGeomIStructAddPoint(1, 0)
                            CGWCSGeomIStructAddPoint(0,-1)
                            CGWCSGeomIStructAddPoint(-1, 0)
                            CGWCSGeomIStructAddCircle(0, 0, 1)
                            CGWCSGeomIStructAddRect(0, 1, 1, 1)
                            CGWCSGeomIStructAddEndOfList()
                        };

CGWCSGeomI          hitbox =  CGWCSGeomIStructStart(10)
                                CGWCSGeomIStructAddCircle(-1, 0, 1)
                                CGWCSGeomIStructAddRect(-1, -1, 2, 2)
                                CGWCSGeomIStructAddCircle( 1, 0, 1)
                                CGWCSGeomIStructAddPoint(0, 0)
                                CGWCSGeomIStructAddPoint(0, 1)
                                CGWCSGeomIStructAddPoint(1, 0)
                                CGWCSGeomIStructAddPoint(0,-1)
                                CGWCSGeomIStructAddPoint(-1, 0)
                                CGWCSGeomIStructAddCircle(0, 0, 1)
                                CGWCSGeomIStructAddRect(0, 1, 1, 1)
                            CGWCSGeomIStructEnd();

CGWPolarPoint
CGWPolarPointMakeLocal(
    float           r,
    float           theta
)
{
    CGWPolarPoint   the_point = { .r = r, .theta = theta };
    printf("<%g, %g>\n", r, theta);
    return the_point;
}

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
    
    CGWPointFP2D    p1 = CGWPointFP2DMake(CGWFixedPrecCreateWithDouble(-2.25), CGWFixedPrecCreateWithDouble(0.75));
    
    printf("p               = <" CGW_FIXEDPREC_FMT ", " CGW_FIXEDPREC_FMT "> = <%f, %f>\n",
            p1.x, p1.y, 
            CGWFixedPrecToDouble(p1.x),
            CGWFixedPrecToDouble(p1.y));
    
    p1 = CGWPointFP2DSum(p1, CGWPointFP2DMake(CGWFixedPrecCreateWithDouble(5.0), CGWFixedPrecCreateWithDouble(-2.0)));
    printf("p               = <" CGW_FIXEDPREC_FMT ", " CGW_FIXEDPREC_FMT "> = <%f, %f>\n",
            p1.x, p1.y, 
            CGWFixedPrecToDouble(p1.x),
            CGWFixedPrecToDouble(p1.y));
    
    int     i, r;
    
    for ( i = 0; i < 40; i++ ) printf("Sqrt(%d) = %d (frac %d)\n", i, CGWSqrtInt(i, &r), r);
    printf("\n");
    
    CGWCircleI      c = CGWCircleIMake(0, 0, 3);
    
    for ( i = 0; i < 5; i++ )
        printf("(%1$d, %1$d) in c = %2$d\n", i, CGWCircleIContainsPoint(c, CGWPointI2DMake(i, i)));
    printf("\n");
    
    CGWCircleI      c2 = CGWCircleIMake(0, 0, 1);
    
    for ( i = 5; i > 0; i-- ) {
        c2.origin.x = c2.origin.y = i;
        printf("{(%d,%d) r=%d} overlap {(%d,%d) r=%d} => %d\n", 
            c.origin.x, c.origin.y, c.radius,
            c2.origin.x, c2.origin.y, c2.radius,
            CGWCircleIOverlapsCircle(c, c2));
    }
    printf("\n");
    c2.origin.y = 0;
    for ( i = 5; i > 0; i-- ) {
        c2.origin.x = i;
        printf("{(%d,%d) r=%d} overlap {(%d,%d) r=%d} => %d\n", 
            c.origin.x, c.origin.y, c.radius,
            c2.origin.x, c2.origin.y, c2.radius,
            CGWCircleIOverlapsCircle(c, c2));
    }
    printf("\n");
    
    CGWRectI2D  R = CGWRectI2DMake(4, 4, 2, 4);   /* (4, 4) -> (6, 4) -> (6, 8) -> (4, 8) */
    c = CGWCircleIMake(0, 0, 2);
    for ( i = 0; i < 13; i++ ) {
        c.origin.x = c.origin.y = i;
        printf("{(%d,%d) r=%d} overlap {(%d,%d) x (%u,%u)} => %d\n", 
            c.origin.x, c.origin.y, c.radius,
            R.origin.x, R.origin.y, R.size.w, R.size.h,
            CGWCircleIOverlapsRect(c, R));
    }
    printf("\n");
    
    printf("initial CGS geom list has %u shapes\n", hitbox.n_shapes);
    
    CGWCSGeomIRef       cgs_obj = CGWCSGeomICreateWithShapesList(hitbox_list);
    CGWPointI2D         p;
    int                 x, y;
    
    printf("hitbox CGS geom list has %u shapes\n", cgs_obj->n_shapes);
    printf("     ");
    for ( x = -5; x <= 5; x++ ) printf("%3d", x);
    printf("\n");
    for ( y = -5; y <= 5; y++ ) {
        printf("%3d :", y);
        for ( x = -5; x <= 5; x++ ) printf(" %c ", CGWCSGeomIContainsPoint(cgs_obj, CGWPointI2DMake(x, y)) ? '*' : '.');
        printf("\n");
    }
    printf("\n");
    
    CGWTimingInterval   timer = CGWTimingIntervalMake(1.0f);
    float               f_tot, f_in, f_out;
    
    f_in = 1.0f, f_tot = 0.0;
    CGWTimingIntervalStart(timer);
    for ( i=0; i < 10000000; i++ ) {
        f_out = CGWQSqrtF(f_in, 3);
        f_tot += f_out;
        f_in += 1.0f;
    }
    CGWTimingIntervalStopNow(timer);
    printf("10000000 calls to CGWQSqrtF(x, 3) = %g took %g µs\n", f_tot, timer.accumulated_time);
    
    f_in = 1.0f, f_tot = 0.0;
    timer = CGWTimingIntervalMake(1.0f);
    CGWTimingIntervalStart(timer);
    for ( i=0; i < 10000000; i++ ) {
        f_out = sqrtf(f_in);
        f_tot += f_out;
        f_in += 1.0f;
    }
    CGWTimingIntervalStopNow(timer);
    printf("10000000 calls to sqrtf(x) = %g took %g µs\n", f_tot, timer.accumulated_time);

    f_in = 1.0f, f_tot = 0.0;
    timer = CGWTimingIntervalMake(1.0f);
    CGWTimingIntervalStart(timer);
    for ( i=0; i < 10000000; i++ ) {
        f_out = CGWQSqrtFAsm(f_in, 3);
        f_tot += f_out;
        f_in += 1.0f;
    }
    CGWTimingIntervalStopNow(timer);
    printf("10000000 calls to CGWQSqrtFAsm(x, 3) = %g took %g µs\n", f_tot, timer.accumulated_time);    

    CGWPolarPointSetGlobalThetaMin(512, 240);

    CGWPolarPoint   rp;
    float           theta = 0.5f * M_PI_4;
    
    for ( i = -18; i <= 18; i++ ) {
        rp = CGWPolarPointMake(5.0f, (float)i * theta);
        CGWPolarPointStandardize(&rp, &rp, F_ANGLE_TOL);
        printf("CGWPolarPointMake(5, %d(π/8)) = { .r=%g, .theta=%g }\n", i, rp.r, rp.theta);
    }

    //
    //  100000000 calls to CGWPointPThetaClampWithThresholds() = -0.893969 took 474283 µs
    //  100000000 calls to CGWPointPThetaClampWithThresholdsAsm() = -0.893969 took 343862 µs
    //
    double          Theta = -5.25 * M_PI;
    double          Theta_avg = 0.0;
    
    timer = CGWTimingIntervalMake(1.0f);
    CGWTimingIntervalStart(timer);
    for ( i=0; i < 100000000; i++ ) {
        double      Theta_prime = CGWPolarPointThetaClampWithThresholds(theta, 0.0001, 0.0001);
        
        Theta_avg = (i == 0) ? Theta_prime : 0.5 * (Theta_avg + Theta_prime);
        theta += M_PI_4;
    }
    CGWTimingIntervalStopNow(timer);
    printf("100000000 calls to CGWPolarPointThetaClampWithThresholds() = %g took %g µs\n", Theta_avg, timer.accumulated_time);

    return 0;
}

#else

#include "CGWGeom2D.h"

int
main()
{
    CGWXFormMatrix2D        M1, M2, M3;
    CGWPoint2D              p = CGWPoint2DMake(0.5f, -0.5f);
    CGWPointI2D             c = CGWPointI2DMake(-5, 10);
    float                   s;
    
    CGWXFormMatrix2DSetZero(&M1);
    CGWXFormMatrix2D_IJ(M1.M, 0, 0) = 1.0f;
    CGWXFormMatrix2D_IJ(M1.M, 0, 1) = 2.0f;
    CGWXFormMatrix2D_IJ(M1.M, 0, 2) = 3.0f;
    CGWXFormMatrix2D_IJ(M1.M, 1, 0) = 4.0f;
    CGWXFormMatrix2D_IJ(M1.M, 1, 1) = 5.0f;
    CGWXFormMatrix2D_IJ(M1.M, 1, 2) = 6.0f;
    
    CGWXFormMatrix2DSetIdentity(&M2);
    printf("M2 = [[%f, %f, %f, %f]\n         [%f, %f, %f, %f]]\n", M2.M[0], M2.M[1], M2.M[2], M2.M[3], M2.M[4], M2.M[5], M2.M[6], M2.M[7]);
    
    CGWXFormMatrix2DSetZero(&M3);
    
    CGWXFormMatrix2DDotPoint2D(&p, &M1, &p);
    printf("<%f, %f>\n", p.x, p.y);
    CGWPointI2DSum(c, CGWPointI2DOrigin);
    
    CGWXFormMatrix2DScaledSum(&M3, -2.0f, &M2, &M1);
    printf("M3 = [[%f, %f, %f, %f]\n     [%f, %f, %f, %f]]\n", M3.M[0], M3.M[1], M3.M[2], M3.M[3], M3.M[4], M3.M[5], M3.M[6], M3.M[7]);
    
    CGWXFormMatrix2DMultiply(&M3, &M3, &M1);
    printf("M3 * I = [[%f, %f, %f, %f]\n         [%f, %f, %f, %f]]\n", M3.M[0], M3.M[1], M3.M[2], M3.M[3], M3.M[4], M3.M[5], M3.M[6], M3.M[7]);
    
    CGWXFormMatrix2DSetIdentity(&M2);
    
    CGWXFormMatrix2DScale(&M2, -2.5f, +2.5f);
    printf("M2 = [[%f, %f, %f, %f]\n         [%f, %f, %f, %f]]\n", M2.M[0], M2.M[1], M2.M[2], M2.M[3], M2.M[4], M2.M[5], M2.M[6], M2.M[7]);
    
    CGWXFormMatrix2DSetZero(&M2);
    CGWXFormMatrix2D_IJ(M2.M, 0, 1) = 5.0f;
    CGWXFormMatrix2D_IJ(M2.M, 1, 0) = 0.125f;
    //CGWXFormMatrix2D_IJ(M2.M, 0, 2) = 1.0f;
    printf("M2 = [[%f, %f, %f, %f]\n         [%f, %f, %f, %f]]\n", M2.M[0], M2.M[1], M2.M[2], M2.M[3], M2.M[4], M2.M[5], M2.M[6], M2.M[7]);
    CGWXFormMatrix2DInvertGE(&M3, &M2);
    printf("M2^-1 = [[%f, %f, %f, %f]\n         [%f, %f, %f, %f]]\n", M3.M[0], M3.M[1], M3.M[2], M3.M[3], M3.M[4], M3.M[5], M3.M[6], M3.M[7]);
    
    CGWXFormMatrix2DMultiply(&M3, &M2, &M3);
    printf("M2 . M2^-1 = [[%f, %f, %f, %f]\n              [%f, %f, %f, %f]]\n", M3.M[0], M3.M[1], M3.M[2], M3.M[3], M3.M[4], M3.M[5], M3.M[6], M3.M[7]);
    
    CGWPolarPoint       pp = CGWPolarPointMake(1e-12, M_2xPI + 0.01);
    
    printf("pp = < %g, %g >\n", pp.r, pp.theta);
    CGWPolarPointStandardize(&pp, &pp, 1e-32);
    printf("pp = < %g, %g >\n", pp.r, pp.theta);
    
    s = CGWPoint2DNormL1(p);
    printf("|< %g, %g >|_1 = %g\n", p.x, p.y, s);
    
    s = CGWPoint2DNormL2(p);
    printf("|< %g, %g >|_2 = %g\n", p.x, p.y, s);
    
    s = CGWPoint2DNormLInf(p);
    printf("|< %g, %g >|_∞ = %g\n", p.x, p.y, s);
    
    return 0;
}

#endif
