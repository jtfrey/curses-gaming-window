
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
