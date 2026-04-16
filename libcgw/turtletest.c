
#include "CGWTurtle.h"


void
__CGWTurtleGeneratePath(
    CGWTurtleRef        Yertle,
    CGWPoint            destination
);


void
drawPixel(
    CGWTurtleRef    Yertle,
    CGWPoint        pixel,
    const void      *context
)
{
    printf("DRAW (%d, %d)\n", pixel.x, pixel.y);
}

int
turtleEventObserver(
    CGWTurtleEventPtr   the_event,
    const void          *context
)
{
    fprintf((FILE*)context, "[EVENT][%d]", the_event->event_id);
    switch ( the_event->event_id ) {
    
        case kCGWTurtleEventIdSetPosition:
            fprintf((FILE*)context, " SET POSITION (%d, %d) = (%u, %g)\n",
                the_event->event_data.set_position.cartesian.x,
                the_event->event_data.set_position.cartesian.y,
                the_event->event_data.set_position.polar.r,
                CGWRadiansToDegrees(the_event->event_data.set_position.polar.theta));
            the_event->event_data.set_position.cartesian.x = 12,
                the_event->event_data.set_position.cartesian.y = 8;
            return kCGWTurtleEventSetAlteredValue;
        
        default:
            fprintf((FILE*)context, "\n");
            break;
    }
    return kCGWTurtleEventSetReturnCodeOk;
}

int
turtlePathFinderObserver(
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
    printf("%s:  ", step_strs[the_event->event_data.path_finder.step_type]);
    switch ( the_event->event_data.path_finder.step_type ) {
    
        case kCGWTurtlePathFinderStepBegin:
            printf("(%d, %d) [dest = (%d, %d)]\n",
                    the_event->event_data.path_finder.step_data.begin.start.x,
                    the_event->event_data.path_finder.step_data.begin.start.y,
                    the_event->event_data.path_finder.step_data.begin.destination.x,
                    the_event->event_data.path_finder.step_data.begin.destination.y);
            break;
        case kCGWTurtlePathFinderStepEnd:
            printf("(%d, %d).\n",
                    the_event->event_data.path_finder.step_data.end.end.x,
                    the_event->event_data.path_finder.step_data.end.end.y);
            break;
        case kCGWTurtlePathFinderStepLine:
            printf("(%d, %d) -> (%d, %d)\n",
                    the_event->event_data.path_finder.step_data.line.start.x,
                    the_event->event_data.path_finder.step_data.line.start.y,
                    the_event->event_data.path_finder.step_data.line.end.x,
                    the_event->event_data.path_finder.step_data.line.end.y);
            break;
        case kCGWTurtlePathFinderStepHitLeftWall:
        case kCGWTurtlePathFinderStepHitRightWall:
        case kCGWTurtlePathFinderStepHitTopWall:
        case kCGWTurtlePathFinderStepHitBottomWall:
            printf("(%d, %d)\n",
                    the_event->event_data.path_finder.step_data.hit_wall.hit_point.x,
                    the_event->event_data.path_finder.step_data.hit_wall.hit_point.y);
            break;
        case kCGWTurtlePathFinderStepTeleport:
            printf("(%d, %d) ~~> (%d, %d) [dest ~~> (%d, %d)]\n",
                    the_event->event_data.path_finder.step_data.teleport.hit_point.x,
                    the_event->event_data.path_finder.step_data.teleport.hit_point.y,
                    the_event->event_data.path_finder.step_data.teleport.teleport_to.x,
                    the_event->event_data.path_finder.step_data.teleport.teleport_to.y,
                    the_event->event_data.path_finder.step_data.teleport.new_destination.x,
                    the_event->event_data.path_finder.step_data.teleport.new_destination.y);
            break;
        case kCGWTurtlePathFinderStepReflect:
            printf("(%d, %d) ∠ %.1f ~~> ∠ %.1f [dest ~~> (%d, %d)]\n",
                    the_event->event_data.path_finder.step_data.reflect.hit_point.x,
                    the_event->event_data.path_finder.step_data.reflect.hit_point.y,
                    CGWRadiansToDegrees(the_event->event_data.path_finder.step_data.reflect.incident_angle),
                    CGWRadiansToDegrees(the_event->event_data.path_finder.step_data.reflect.reflected_angle),
                    the_event->event_data.path_finder.step_data.reflect.new_destination.x,
                    the_event->event_data.path_finder.step_data.reflect.new_destination.y);
            break;
        case kCGWTurtlePathFinderSlideOnLeftWall:
        case kCGWTurtlePathFinderSlideOnRightWall:
        case kCGWTurtlePathFinderSlideOnTopWall:
        case kCGWTurtlePathFinderSlideOnBottomWall:
            printf("(%d, %d) ∠ %.1g [dest ~~> (%d, %d)]\n",
                    the_event->event_data.path_finder.step_data.slide_on_wall.hit_point.x,
                    the_event->event_data.path_finder.step_data.slide_on_wall.hit_point.y,
                    CGWRadiansToDegrees(the_event->event_data.path_finder.step_data.slide_on_wall.angle),
                    the_event->event_data.path_finder.step_data.slide_on_wall.new_destination.x,
                    the_event->event_data.path_finder.step_data.slide_on_wall.new_destination.y);
            break;
    
        default:
            printf("\n");
            break;
    }
    return 0;
}


int
main()
{
    CGWTurtleRef    Yertle = CGWTurtleCreate(
                                NULL,
                                CGWSizeMake(512, 240),
                                kCGWTurtleOptionsXBoundsPeriodic
                                  | kCGWTurtleOptionsYBoundsElastic
                                  | kGGWTurtleOptionsEnableLoggingMask,
                                CGWTurtleDefaultCallbacks);
    
    CGWTurtleSetDrawPixelCallback(Yertle, drawPixel);
    CGWTurtleSummarize(Yertle);
    
    CGWTurtleActionMove(Yertle, 5);
    CGWTurtleStateTurn(Yertle, 0.5 * M_PI_4);
    CGWTurtleActionMove(Yertle, 1);

    return 0;
}
