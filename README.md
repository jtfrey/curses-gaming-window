# The natural follow-up project to tetrominotris

When I wrote [Tetrominotris](https://github.com/jtfrey/tetrominotris) I used the ncurses library to draw pseudo-graphical colored items to the screen and simplify checking for special keys like the arrows.  Background color was used to produce solid colored blocks.

At a terminal size of 77 columns by 44 rows, things were...chunky.  But the game ran at an extremely high frame rate and Tetri...I mean Tetrominotris...responds to single-key inputs.

Eventually I had a thought:  if the font size in the terminal window were set to the lowest possible at a dimension of say 512 columns by 240 rows, would each character essentially become a pixel?

So I gave it a try on Mac OS X with the Terminal.app.  And at a font size of 1 the terminal window did, in fact, become a pixel display.  With some optimized pixel buffer code and some tweaks to the ncurses workflow I could realize hundreds to thousands of frames per second, depending on the complexity of the content.  With all that extra time between frames, I layered multiple pixel buffers to create a compositing buffer which could produce parallax scrolling effects, for example.

Along the way, I added some ARM64 assembly variants of functions (why not?) and quite a bit of supporting code.

As of April 2026, this is still a work in progress; in the evenings and on the weekends when the mood strikes me, I add to it.  Maybe someday it will include a fully-fledged game!


## Turtle graphics

In the midst of the project I recalled some of the first programming I encountered back in 7th grade:  Apple II turtle graphics.  A cursor — known as the turtle — could be prompted to move straight ahead or turn, leaving a line of pixels behind it.  Disabling the virtual stylus, the turtle would move without depositing pixels.

With the `libcgw_base` and `libcgw_curses` libraries in this project, a turtle was realizable.  The `libcgw_turtle` library was the result, with the `yertle` program making use of it to implement a turtle-drawing program.

To engage `yertle`, a turtle script must be written.  A turtle script is a text file containing commands, empty lines, and comments delimited by `#` or `//`:

```
#
# Clear the screen, wait for a keypress, then exit
#
clear
pause
```

Commands occur as a single-word verb followed by zero or more mandatory arguments, and are summarized below:

| Command form | Explanantion |
| :----------- | :----------- |
| `reset` | the turtle is moved back to the origin, the color is reset, and the stylus is enabled |
| `pause` | wait for a single keypress before proceeding; this is often used at the end of a program because otherwise the simulator exists immediately |
| `clear` | clear the screen |
| `color++` | select the next color in the palette |
| `color--` | select the previous color in the palette |
| `color <idx>` | select the color at the given index `<idx>`; when `<idx>` exceeds the number of colors it wraps around |
| `drawing-delay <dt>` | wait the given number of seconds `<dt>` between drawing each pixel; defaults to 0.05 |
| `stylus <1\|0>` | enable (1) or disable (0) the stylus |
| `moveto <x> <y>` | alter the turtle's position (without drawing) to be (`<x>`, `<y>`) where the coordinate system runs from [0,1] in both axes |
| `move <d>` | move the turtle the distance `<d>`; if the stylus is enabled pixels will be drawn |
| `turnto-<deg\|rad> <a>` | rotate the turtle to angle `<a>`, where the choice of `<deg\|rad>` in the command dictates which unit is associated with `<a>` |
| `turn-<deg\|rad> <a>` | rotate the turtle by `<a>` relative to it's current orientation, where the choice or `<deg\|rad>` in the command dictates which unit is associated with `<a>` |
| `curve-<dev\|rad> <r> <phi>` | given the turtle's current position and direction, rotate the direction by 90° and move `<r>` away from the position to establish the center of a circle; then move the turtle through an arc of `<a>` on that circle |
| `contracting-curve-<dev\|rad> <r> <phi> <contrct>` | given the turtle's current position and direction, rotate the direction by 90° and move `<r>` away from the position to establish the center of a circle; then move the turtle through an arc of `<phi>` on that circle, contracting the circle's radius by a constant `<contrct>` for every radian of rotation |
| `prop-contracting-curve-<dev\|rad> <r> <phi> <contrct>` | given the turtle's current position and direction, rotate the direction by 90° and move `<r>` away from the position to establish the center of a circle; then move the turtle through an arc of `<a>` on that circle, contracting the circle's radius by a constant `<contrct>` for every radian of rotation |

There are several example `.turtle` scripts in the [yertle](./yertle) subdirectory.
