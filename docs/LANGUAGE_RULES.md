# `waralang` Language Rules

This document outlines the basic rules of a `waralang` source program.

## Parts of a `waralang` program

The source file would be divided into 3 parts namely:

- `wmap` specification section
- Master `agent's` code section
- Map `agents` code section

For example:
```
use <path-to-wmap file>
at 60
#
>                 v

^    countryoven   <

#
!countryoven:
>^<v8

!otherdefinitions:

```

The above code would simply spawn countryoven `agents` in an infinite loop.
For further clarification see the following explanation for each symbol in
the above source code:

- `use` - The keyword used to specify which `.wmap` file to use. Essential for
every `waralang` program.

- `at` - The keyword used to specify the ticks per minute the interpreter will
run the source program at.

- `#` - This symbol is used to divide the sections in a `waralang` program. It
also acts as a 'roof' and 'floor' to the `main agent` code section, meaning
that if the `main agent` pointer reaches the horizontal division marked by this
symbol it wraps back around to the opposite side (similar to the snake game!).

- `>`, `<`, `^` and `v` - These symbols are used to change direction of an
`agent`. In the context of the `main agent` they are placed by the user and
will change the direction of the `main agent` when it's pointer reaches the
symbol's cell on the grid. In the context of `map agents` the symbols act
as a sequence of steps which will be followed during the course of the
`agent`'s life. A 'step' is 'taken' when the agent reaches a junction (or
a building with parameters, but the symbols required must correspond to
the parameter type, and `>`, `<`, `^`, `v` are invalid for parameters and
are only for junctions).

- `<agent-id> (countryoven)` - Once the `main agent` reaches an id, it spawns
the correponding map agent onto the spawn area in the map. It's steps are
then executed until it dies.
s
- `!` - Used to prefix a new `map agent` defintition.

- `:` - Used to end the `map agent` id and start definition of it's steps.

Execution happens in 'ticks' and we intend this to be a parameter you can set
before you run (or potentially in the initial section of the source), and also
could be changed at runtime for unique effects.

Note: `agents` are synchronized by age priority, i.e when they were spawned,
and the `main agent` has the highest priority.

## `Map Agent` spawn block rules
When the `main agent` pointer reaches an ` map agent` id, a scan (from left or
top) is done to find the end of the name and compare with the `agent` table to
check if it is a valid `map agent` or not, and if no runtime exception is
generated the pointer simply 'pops out' at the other end of the `map agent` id
block

For example (where the arrow symbol indicates incoming `main agent` pointer):

### Example 1:

Tick 1:
```
> countryoven
```

Tick 2:
```
 >countryoven
```

Tick 3:
countryoven agent spawned!

Tick 3:
```
  countryoven>
```

### Example 2:

Tick 1:
```
    v
  countryoven
```

Tick 2:
countryoven agent spawned!

Tick 3:
```

  countryoven
    v
```

### Example 3:

Tick 1:
```

 c
 o
>u
 n
 t
 r
 y
 o
 v
 e
 n

```

Tick 2:

countryoven agent spawned!


Tick 3:
```

 c
 o
 u>
 n
 t
 r
 y
 o
 v
 e
 n

```

### Example 4:

Tick 1:
```
 nevoyrtnouc<
```
Tick 2:
Exception occured! No valid agent identifier found.

As illustrated only valid `map agent` identifier blocks are written left to
right and top to bottom.

Note: It is still not decided whether to stick exclusively to runtime
exceptions or to include exceptions like the one above during lexical
or syntax analysis.
