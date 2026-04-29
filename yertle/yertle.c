/**
 *
 * YPL — Yertle Programming Language
 *
 * Commands are added to a text file -- by convention one per line -- and have
 * a form that has a mandatory word followed by a fixed number of arguments.
 */

#include "CGWTurtle2D.h"
#include "CGWCurses.h"

int
turtleDraw(
    CGWTurtleEventPtr   the_event,
    const void          *context
)
{
    CGWTurtleEventDrawPixel *EVENT = (CGWTurtleEventDrawPixel*)the_event;
    int                     *color = (int*)context;
    mvaddch(EVENT->position.y, EVENT->position.x, ' ' | COLOR_PAIR(*color));
    refresh();
    return kCGWTurtleEventSetReturnCodeOk;
}

//

int
main(
    int         argc,
    const char* argv[]
)
{
    CGWSizeI2D          display_size;
    CGWPoint2D          pt;
    CGWTurtleRef        Yertle = NULL;
    short               colors_orig[30][3];
    short               color_max = 1, n_color = 0, pair_max = 1, n_pair = 0;
    int                 stylus_color = 3;
    int                 argi = 1;
    int                 rc = 0;
    char                err_msg[1024];
    
    
#define SAVE_COLOR(I)  color_content(I, &colors_orig[I][0], &colors_orig[I][1], &colors_orig[I][2])
    
    
    if ( argc < 2 ) exit(0);
    
    CGWCursesInit(CGWSizeI2DMake(0, 0), &display_size);
    
    // Correction, we do want sychronous input available to us:
    nodelay(stdscr, FALSE);
    
    // Baseline colors:
    //
    //   1 = black
    //   2 = white
    //   3 = light gray
    //   4 = med gray
    //   5 = dark gray
    //   6 = bright red
    //   7 = bright green
    //   8 = bright blue
    //   9 = bright yellow
    //  10 = bright purple
    //  11 = bright teal
    //  12 = med red
    //  13 = med green
    //  14 = med blue
    //  15 = med yellow
    //  16 = med purple
    //  17 = med teal
    SAVE_COLOR(color_max), init_color(color_max++, 0, 0, 0);
    SAVE_COLOR(color_max), init_color(color_max++, 1000, 1000, 1000);
    SAVE_COLOR(color_max), init_color(color_max++, 250, 250, 250), n_color++;
    SAVE_COLOR(color_max), init_color(color_max++, 500, 500, 500), n_color++;
    SAVE_COLOR(color_max), init_color(color_max++, 750, 750, 750), n_color++;
    SAVE_COLOR(color_max), init_color(color_max++, 1000, 0, 0), n_color++;
    SAVE_COLOR(color_max), init_color(color_max++, 0, 1000, 0), n_color++;
    SAVE_COLOR(color_max), init_color(color_max++, 0, 0, 1000), n_color++;
    SAVE_COLOR(color_max), init_color(color_max++, 1000, 1000, 0), n_color++;
    SAVE_COLOR(color_max), init_color(color_max++, 1000, 0, 1000), n_color++;
    SAVE_COLOR(color_max), init_color(color_max++, 0, 1000, 1000), n_color++;
    SAVE_COLOR(color_max), init_color(color_max++, 500, 0, 0), n_color++;
    SAVE_COLOR(color_max), init_color(color_max++, 0, 500, 0), n_color++;
    SAVE_COLOR(color_max), init_color(color_max++, 0, 0, 500), n_color++;
    SAVE_COLOR(color_max), init_color(color_max++, 500, 500, 0), n_color++;
    SAVE_COLOR(color_max), init_color(color_max++, 500, 0, 500), n_color++;
    SAVE_COLOR(color_max), init_color(color_max++, 0, 500, 500), n_color++;
    
    // Additional pairs with the colors defined above
    // as the color in both fore- and background
    // for a solid block:
    //
    //   1 = black on black (our background color)
    //   2 = white on black (for text)
    //   3 = white
    //   4 = light gray
    //   5 = med gray
    //   6 = dark gray
    //   7 = bright red
    //   8 = bright green
    //   9 = bright blue
    //  10 = bright yellow
    //  11 = bright purple
    //  12 = bright teal
    //  13 = med red
    //  14 = med green
    //  15 = med blue
    //  16 = med yellow
    //  17 = med purple
    //  18 = med teal
    init_pair(pair_max++, 1, 1);
    init_pair(pair_max++, 2, 1);
    init_pair(pair_max++, 2, 2), n_pair++;
    init_pair(pair_max++, 3, 3), n_pair++;
    init_pair(pair_max++, 4, 4), n_pair++;
    init_pair(pair_max++, 5, 5), n_pair++;
    init_pair(pair_max++, 6, 6), n_pair++;
    init_pair(pair_max++, 7, 7), n_pair++;
    init_pair(pair_max++, 8, 8), n_pair++;
    init_pair(pair_max++, 9, 9), n_pair++;
    init_pair(pair_max++, 10, 10), n_pair++;
    init_pair(pair_max++, 11, 11), n_pair++;
    init_pair(pair_max++, 12, 12), n_pair++;
    init_pair(pair_max++, 13, 13), n_pair++;
    init_pair(pair_max++, 14, 14), n_pair++;
    init_pair(pair_max++, 15, 15), n_pair++;
    init_pair(pair_max++, 16, 16), n_pair++;
    init_pair(pair_max++, 17, 17), n_pair++;
    
    //
    // Create our turtle display to fill the curses screen.
    // Periodic boundary conditions are applied to all edges of the
    // screen.
    //
    Yertle = CGWTurtleCreate(NULL,
                             CGWRectI2DMake(0, 0, display_size.w - 1, display_size.h - 1),
                             kCGWTurtleOptionsBoundsPeriodic);

    // Turn the stylus on to start:
    CGWTurtleStateSetStylusIsOn(Yertle, true);
    
    // To effect drawing to the curses screen we need to observe pixel events generated
    // by the turtle; as context we provide a pointer to the selected color pair index:
    CGWTurtleSetEventObserver(Yertle, kCGWTurtleEventDrawPixel, turtleDraw, &stylus_color);
    
    while ( (rc == 0) && (argi < argc) ) {
        FILE        *program_fptr = fopen(argv[argi++], "r");
        
        if ( program_fptr ) {
            while ( (rc == 0) && ! feof(program_fptr) ) {
                char        command[32];
                
                if ( fscanf(program_fptr, "%30s", command) == 1 ) {
                    if ( (strcmp(command, "#") == 0) || (strcmp(command, "//") == 0) ) {
                        // Comment line, read until a newline is encountered:
                        while ( ! feof(program_fptr) ) {
                            char        c = fgetc(program_fptr);
                            if ( (c == '\n') || (c == '\r') || (c == '\0') ) break;
                        }
                    }
                    else if ( strcasecmp(command, "reset") == 0 ) {
                        CGWTurtleStateReset(Yertle);
                        CGWTurtleStateSetStylusIsOn(Yertle, true);
                        stylus_color = 3;
                    }
                    else if ( strcasecmp(command, "pause") == 0 ) {
                        chtype      saved_chars[30], *char_ptr = saved_chars;
                        int         x = 1;
                        
                        while ( char_ptr < (saved_chars + sizeof(saved_chars)) ) *char_ptr++ = mvinch(1, x++);
                        attron(COLOR_PAIR(2));
                        mvprintw(1, 1, "Press any key to continue...");
                        attroff(COLOR_PAIR(2));
                        getch();
                        
                        // Restore the screen:
                        x = 1, char_ptr = saved_chars;
                        while ( char_ptr < (saved_chars + sizeof(saved_chars)) ) mvaddch(1, x++, *char_ptr++);
                    }
                    else if ( strcasecmp(command, "clear") == 0 ) {
                        clear();
                    }
                    else if ( strcasecmp(command, "add-color-rgb") == 0 ) {
                        short           r, g, b;
                        
                        if ( fscanf(program_fptr, "%hd %hd %hd", &r, &g, &b) == 3 ) {
                            if ( (r>=0 && r<=1000) && (g>=0 && g<=1000) && (b>=0 && b<=1000) ) {
                                int         color_idx = color_max;
                                
                                SAVE_COLOR(color_max), init_color(color_max++, r, g, b), n_color++;
                                init_pair(pair_max++, color_idx, color_idx), n_pair++;
                            }
                        }
                    }
                    else if ( strcasecmp(command, "add-color-hex") == 0 ) {
                        unsigned int    packed_hex_color;
                        
                        if ( fscanf(program_fptr, "%X", &packed_hex_color) == 1 ) {
                            int         r = (1000 * ((packed_hex_color & 0xFF0000) >> 16)) / 255,
                                        g = (1000 * ((packed_hex_color & 0x00FF00) >> 8)) / 255,
                                        b = (1000 * (packed_hex_color & 0x0000FF)) / 255;
                            int         color_idx = color_max;
                            
                            SAVE_COLOR(color_max), init_color(color_max++, r, g, b), n_color++;
                            init_pair(pair_max++, color_idx, color_idx), n_pair++;
                        }
                    }
                    else if ( strcasecmp(command, "color++") == 0 ) {
                        stylus_color = 3 + ((stylus_color - 3 + 1) % n_pair);
                    }
                    else if ( strcasecmp(command, "color--") == 0 ) {
                        stylus_color = 3 + ((stylus_color - 3 - 1) % n_pair);
                    }
                    else if ( strcasecmp(command, "color") == 0 ) {
                        int         c;
                        
                        if ( fscanf(program_fptr, "%d", &c) == 1 ) {
                            if ( c >= 0 ) stylus_color = 3 + (c % n_pair);
                        }
                    }
                    else if ( strcasecmp(command, "drawing-delay") == 0 ) {
                        double      dt;
                        
                        if ( fscanf(program_fptr, "%lg", &dt) == 1 ) {
                            if ( dt < 0.0 ) dt = 0.0;
                            CGWTurtleStateSetDrawingDelay(Yertle, dt);
                        }
                    }
                    else if ( strcasecmp(command, "stylus") == 0 ) {
                        int         b;
                        
                        if ( fscanf(program_fptr, "%d", &b) == 1 ) {
                            CGWTurtleStateSetStylusIsOn(Yertle, b ? true : false);
                        }
                    }
                    else if ( strcasecmp(command, "moveto") == 0 ) {
                        float       x, y;
                        
                        if ( fscanf(program_fptr, "%g %g", &x, &y) == 2 ) {
                            CGWTurtleStateSetPosition(Yertle, CGWPoint2DMake(x, y));
                        }
                    }
                    else if ( strcasecmp(command, "move") == 0 ) {
                        float       d;
                        
                        if ( fscanf(program_fptr, "%g", &d) == 1 ) {
                            CGWTurtleActionMove(Yertle, d);
                        }
                    }
                    else if ( strncasecmp(command, "turnto-" ,7) == 0 ) {
                        float       a;
                        
                        if ( fscanf(program_fptr, "%g", &a) == 1 ) {
                            if ( strcasecmp(command + 7, "deg") == 0 ) {
                                a = CGWDegreesToRadians(a);
                            }
                            else if ( strcasecmp(command + 7, "rad") ) {
                                break;
                            }
                            CGWTurtleStateSetAngle(Yertle, a);
                        }
                    }
                    else if ( strncasecmp(command, "turn-" ,5) == 0 ) {
                        float       a;
                        
                        if ( fscanf(program_fptr, "%g", &a) == 1 ) {
                            if ( strcasecmp(command + 5, "deg") == 0 ) {
                                a = CGWDegreesToRadians(a);
                            }
                            else if ( strcasecmp(command + 5, "rad") ) {
                                break;
                            }
                            CGWTurtleStateTurn(Yertle, a);
                        }
                    }
                    else if ( strncasecmp(command, "curve-", 6) == 0 ) { 
                        float       r, phi;
                        
                        if ( fscanf(program_fptr, "%g %g", &r, &phi) == 2 ) {
                            CGWPoint2D          o, d_v;
                            CGWXFormMatrix2D    d_rot;
                            
                            if ( strcasecmp(command + 6, "deg") == 0 ) {
                                phi = CGWDegreesToRadians(phi);
                            }
                            else if ( strcasecmp(command + 6, "rad") ) {
                                break;
                            }
                            // Create a rotation matrix for the direction vector:
                            CGWXFormMatrix2DSetIdentity(&d_rot);
                            CGWXFormMatrix2DSetRotation(&d_rot, (phi < 0.0f) ? -M_PI_2 : M_PI_2);
                            
                            // Current position and orientation:
                            o = CGWTurtleStateGetPosition(Yertle);
                            d_v = CGWTurtleStateGetOrientationVector(Yertle);
                            
                            // Scale the orientation to the radius:
                            d_v.x *= r, d_v.y *= r;
                            
                            // Apply the rotation:
                            CGWXFormMatrix2DDotPoint2D(&d_v, &d_rot, &d_v);
                            
                            // Translate the turtle's current position:
                            o = CGWPoint2DSum(o, d_v);
                            
                            CGWTurtleActionCurve(Yertle, o, phi);
                        }
                    }
                    else if ( strncasecmp(command, "contracting-curve-", 18) == 0 ) { 
                        float       r, phi, per_radian;
                        
                        if ( fscanf(program_fptr, "%g %g %g", &r, &phi, &per_radian) == 3 ) {
                            CGWPoint2D          o, d_v;
                            CGWXFormMatrix2D    d_rot;
                            
                            if ( strcasecmp(command + 18, "deg") == 0 ) {
                                phi = CGWDegreesToRadians(phi);
                            }
                            else if ( strcasecmp(command + 18, "rad") ) {
                                break;
                            }
                            // Create a rotation matrix for the direction vector:
                            CGWXFormMatrix2DSetIdentity(&d_rot);
                            CGWXFormMatrix2DSetRotation(&d_rot, (phi < 0.0f) ? -M_PI_2 : M_PI_2);
                            
                            // Current position and orientation:
                            o = CGWTurtleStateGetPosition(Yertle);
                            d_v = CGWTurtleStateGetOrientationVector(Yertle);
                            
                            // Scale the orientation to the radius:
                            d_v.x *= r, d_v.y *= r;
                            
                            // Apply the rotation:
                            CGWXFormMatrix2DDotPoint2D(&d_v, &d_rot, &d_v);
                            
                            // Translate the turtle's current position:
                            o = CGWPoint2DSum(o, d_v);
                            
                            CGWTurtleActionCurveAndContract(Yertle, o, phi, per_radian, false);
                        }
                    }
                    else if ( strncasecmp(command, "prop-contracting-curve-", 23) == 0 ) { 
                        float       r, phi, per_radian;
                        
                        if ( fscanf(program_fptr, "%g %g %g", &r, &phi, &per_radian) == 3 ) {
                            CGWPoint2D          o, d_v;
                            CGWXFormMatrix2D    d_rot;
                            
                            if ( strcasecmp(command + 23, "deg") == 0 ) {
                                phi = CGWDegreesToRadians(phi);
                            }
                            else if ( strcasecmp(command + 23, "rad") ) {
                                break;
                            }
                            // Create a rotation matrix for the direction vector:
                            CGWXFormMatrix2DSetIdentity(&d_rot);
                            CGWXFormMatrix2DSetRotation(&d_rot, (phi < 0.0f) ? -M_PI_2 : M_PI_2);
                            
                            // Current position and scaled orientation vector:
                            o = CGWTurtleStateGetPosition(Yertle);
                            d_v = CGWPoint2DScale(CGWTurtleStateGetOrientationVector(Yertle), r);
                            
                            // Apply the rotation:
                            CGWXFormMatrix2DDotPoint2D(&d_v, &d_rot, &d_v);
                            
                            // Translate the turtle's current position:
                            o = CGWPoint2DSum(o, d_v);
                            
                            CGWTurtleActionCurveAndContract(Yertle, o, phi, per_radian, true);
                        }
                    }
                    else {
                        snprintf(err_msg, sizeof(err_msg), "ERROR:  unknown command: %s\n", command);
                        rc = 1;
                    }
                }
            }
            fclose(program_fptr);
        } else {
            snprintf(err_msg, sizeof(err_msg), "ERROR:  unable to open program file\n");
            rc = 1;
        }
    }
terminate:
    // Restore original color palette:
    clear();
    while ( color_max-- ) {
        init_color(color_max, colors_orig[color_max][0], colors_orig[color_max][1], colors_orig[color_max][2]);
    }
    CGWCursesShutdown();
    CGWTurtleSummarize(Yertle);
    CGWTurtleDestroy(Yertle);
    if ( rc ) {
        fprintf(stderr, "%s", err_msg);
    }
    return rc;
}

