# `waralang` Language Rules

This document outlines the basic rules of a `waralang` source program.

## Parts of a `waralang` program

The source file would be divided into 2 parts namely:

- `wmap` specification section
- `agents` code section

For example:
```
use <path-to-wmap file>
tickrate 60

!main:
>^^countryoven##
/* Spawns a country oven agent with no `life cycle address` assuming the actor
ended up at the SPAWN_AGENT building*/

!countryoven:
>^<v8#10#18K001#
/* This is a comment (enclosed by /* */) ! F.Y.I. 18K001 is an address.
If the actor ended up at the ADD building then this would add 8 and 10
and store the result at address 18K001$ */

!otherdefinitions:

```

- `use` - The keyword used to specify which `.wmap` file to use. Essential for
every `waralang` program.

- `tickrate` - The keyword used to specify the ticks per minute the interpreter
will run the source program at.


- `>`, `<`, `^` and `v` - These symbols are used to change direction of an
`agent`. The symbols act as a sequence of steps which will be followed during
the course of the `agent`'s life. A 'step' is 'taken' when the agent reaches a
junction (or a building with parameters, but the symbols required must
correspond to the parameter type, and `>`, `<`, `^`, `v` are invalid for
parameters and are only for junctions).

- `!` - Used to prefix a new `agent` defintition.

- `:` - Used to end the `agent` id and start definition of it's steps.

- `#` - Used a terminator for a building parameter.

- `/* */` - Used to enclose comments (preprocessed and removed before
interpreting)

Execution happens in 'ticks' and we intend this to be a parameter you can set
before you run (or potentially in the initial section of the source), and also
could be changed at runtime for unique effects.

Note: `agents` are synchronized by age priority, i.e when they were spawned,
and the `main agent` has the highest priority.

Note: It is still not decided whether to stick exclusively to runtime
exceptions or to include exceptions like the one above during lexical
or syntax analysis.

## Conditional Movement and Building Parameters

Within an `agent's` definition the programmer can check for conditions
and have branching step sequences the following manner:

```
!main:
>^>v?ADDRESS(>^:<v)>

```

The sequence to the left of the `:` symbol is executed if the value at `ADDRESS`
is non-zero, otherwise the steps to the right of the `:` symbol will execute.
Once the steps in the chosen branch ae over, in both cases the steps after the
`)` symbol will execute. It is also possible to nest these conditionals over
and over and over again like so:

```
!main:
>^>v?ADDRESS1(>?ADDRESS2(>^:<v)^:<v)>

```

In this way movement as well parameter inputs to buildings can be controlled
by conditionals. There will be no way to check for relational operators
directly, and this is an intentional design to give more use to the relop
buildings scaterred around the map. If one needs to check if one number is greater
than the other, the agent would have to go and write use the relop building to
write the result to `ADDRESS` and then use a conditional step later referrencing
that same address.

Note: Parameters for building's can be marked as empty by simply adding typing
nothing followed by a `#` symbol.

## Types of Building Parameters

In `waralang` there are only truly three types of data
that are cast down to when parameters are parsed:
- `Whole Numbers`
- `Address names (aliases)`
- `Agent identifiers`

When you pass parameters when inside a building, the type
will be decided based on the context of the set parameters
of the buildings. If there is inconsistency with the data
entered and the parameter expected, for example, if you
are at the `ADD` building which expects a whole number for
the first argument and you provide `countryoven` then an
exception will be raised. Furthermore, if you are at the
`SPAWN_AGENT` building and you pass in `10`, the value
will be treated as an `Agent identifier` and if an agent
named `10` does exist, an exception will not be raised and
it is considered valid `waralang` code.

## Ideas for further language features code features:
- `agent` parameters - When spawning an agent you could
pass in parameters that can be used when writing an `agents`
steps. Agent parameters can be any symbol as long as it makes
syntactic sense when substituted in te `agent's` steps. When
referencing `agent` parameters in the steps {} is used to enclose
them, are used to indicate substituition.

- `GOTO` building

- `Loops` from `brainfuck` where the user can specify an `address`
to not be 0. ADDRESS[>^>^]

- Another way to do loops: screw loops and hello recursion!
!FUNC(n)
?n[<>FUNC(n-1)]