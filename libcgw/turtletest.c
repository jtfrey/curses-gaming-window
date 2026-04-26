
#include "CGWTurtle2D.h"


int
turtlePathFinderStep(
    CGWTurtleEventPtr   the_event,
    const void          *context
)
{
    static const char*  step_strs[] = {
                            "begin",
                            "line",
                            "hit-left-wall",
                            "hit-right-wall",
                            "hit-top-wall",
                            "hit-bottom-wall",
                            "teleport",
                            "reflect",
                            "slide-on-left-wall",
                            "slide-on-right-wall",
                            "slide-on-top-wall",
                            "slide-on-bottom-wall",
                            "end"
                        };
    CGWTurtleEventPathFinderStep *EVENT = (CGWTurtleEventPathFinderStep*)the_event;
    
    printf("%s:  ", step_strs[EVENT->step_type]);
    switch ( EVENT->step_type ) {
    
        case kCGWTurtlePathFinderStepBegin:
            printf("(%g, %g) [dest = (%g, %g)]\n",
                    EVENT->step_data.begin.start.x,
                    EVENT->step_data.begin.start.y,
                    EVENT->step_data.begin.destination.x,
                    EVENT->step_data.begin.destination.y);
            break;
        case kCGWTurtlePathFinderStepEnd:
            printf("(%g, %g).\n",
                    EVENT->step_data.end.end.x,
                    EVENT->step_data.end.end.y);
            break;
        case kCGWTurtlePathFinderStepLine:
            printf("(%g, %g) -> (%g, %g)\n",
                    EVENT->step_data.line.start.x,
                    EVENT->step_data.line.start.y,
                    EVENT->step_data.line.end.x,
                    EVENT->step_data.line.end.y);
            break;
        case kCGWTurtlePathFinderStepHitLeftWall:
        case kCGWTurtlePathFinderStepHitRightWall:
        case kCGWTurtlePathFinderStepHitTopWall:
        case kCGWTurtlePathFinderStepHitBottomWall:
            printf("(%g, %g)\n",
                    EVENT->step_data.hit_wall.hit_point.x,
                    EVENT->step_data.hit_wall.hit_point.y);
            break;
        case kCGWTurtlePathFinderStepTeleport:
            printf("(%g, %g) ~~> (%g, %g) [dest ~~> (%g, %g)]\n",
                    EVENT->step_data.teleport.hit_point.x,
                    EVENT->step_data.teleport.hit_point.y,
                    EVENT->step_data.teleport.teleport_to.x,
                    EVENT->step_data.teleport.teleport_to.y,
                    EVENT->step_data.teleport.new_destination.x,
                    EVENT->step_data.teleport.new_destination.y);
            break;
        case kCGWTurtlePathFinderStepReflect:
            printf("(%g, %g) ∠ %.1f ~~> ∠ %.1f [dest ~~> (%g, %g)]\n",
                    EVENT->step_data.reflect.hit_point.x,
                    EVENT->step_data.reflect.hit_point.y,
                    CGWRadiansToDegrees(EVENT->step_data.reflect.incident_angle),
                    CGWRadiansToDegrees(EVENT->step_data.reflect.reflected_angle),
                    EVENT->step_data.reflect.new_destination.x,
                    EVENT->step_data.reflect.new_destination.y);
            break;
        case kCGWTurtlePathFinderSlideOnLeftWall:
        case kCGWTurtlePathFinderSlideOnRightWall:
        case kCGWTurtlePathFinderSlideOnTopWall:
        case kCGWTurtlePathFinderSlideOnBottomWall:
            printf("(%g, %g) ∠ %.1g [dest ~~> (%g, %g)]\n",
                    EVENT->step_data.slide_on_wall.hit_point.x,
                    EVENT->step_data.slide_on_wall.hit_point.y,
                    CGWRadiansToDegrees(EVENT->step_data.slide_on_wall.angle),
                    EVENT->step_data.slide_on_wall.new_destination.x,
                    EVENT->step_data.slide_on_wall.new_destination.y);
            break;
    
        default:
            printf("\n");
            break;
    }
    return kCGWTurtleEventSetReturnCodeOk;
}

int
turtleDraw(
    CGWTurtleEventPtr   the_event,
    const void          *context
)
{
    CGWTurtleEventDrawPixel *EVENT = (CGWTurtleEventDrawPixel*)the_event;
    printf("DRAW(%d, %d)\n", EVENT->position.x, EVENT->position.y);
    return kCGWTurtleEventSetReturnCodeOk;
}


int
main()
{
    CGWTurtleRef    Yertle = CGWTurtleCreate(
                                NULL,
                                CGWRectI2DMake(0, 0, 255, 239),
                                kCGWTurtleOptionsXBoundsPeriodic
                                  | kCGWTurtleOptionsYBoundsElastic
                                  | kGGWTurtleOptionsEnableLoggingMask);
    CGWTurtleSummarize(Yertle);
    CGWTurtleSetEventObserver(Yertle, kCGWTurtleEventDrawPixel, turtleDraw, NULL);
    CGWTurtleSetEventObserver(Yertle, kCGWTurtleEventPathFinderStep, turtlePathFinderStep, NULL);
    
    CGWTurtleActionMove(Yertle, 15.0f/512.0f);
    CGWTurtleSummarize(Yertle);
    
    CGWTurtleStateTurn(Yertle, 0.5f * M_PI_4);
    CGWTurtleSummarize(Yertle);
    
    CGWTurtleActionMove(Yertle, 15.0f/512.0f);
    CGWTurtleSummarize(Yertle);
    
    CGWTurtleActionMove(Yertle, 300.0f/512.0f);
    CGWTurtleSummarize(Yertle);
    
    CGWTurtleDestroy(Yertle);
    return 0;
}

