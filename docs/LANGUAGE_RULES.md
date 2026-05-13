# `waralang` Language Rules

This document outlines the basic rules of a `waralang` source program stored in a `.wl` file.

## Parts of a `waralang` program

The source file would be divided into 2 parts namely:

- Specification section
- `agents` code section

## Specification section

### `.wmap` file location

Specified with the keyword `use`.

- Must appear at the top of the file.

- Is required in every `.wl` file.

```
use <path to .wmap file>
```

### `tickrate` specification

Specified using keyword `tickrate` in ticks per second
that the source program will run at.

- Is required in every `.wl` file.

```
tickrate 20
```

## `agents` code section

A main agent is required in evry `.wl` file.
When the program is run, an instance of the main agent will spawn automatically.

### Agents

`agent` names and storage building names can only contain alphanumeric characters and `_`.

Declared with:
```
!< agent name >([Parameter 1], [Parameter 2], ...):
```

### Agent code substitution

Call an agent using this format:

```
{ <agent name> ([Parameter 1], [Parameter 2], ...) }
```

This code will substitute itself with the code in agent `<agent name>`.

### Using agent parameters

Agent parameters can be used by enclosing them in `{}`

```
{<Parameter name>}
```

This code will behave the same as if the value of the parameter was in the source code.

### Storage buildings

Both MEM and REG addresses must be enclosed in `[]`

`->` is used as the seperator between baseAddress and subAddress of MEM.

REG:
```
[ <REG name> ]
```

MEM:
```
[ <MEM name (baseAddress)> -> <subAddress> ]
```

### Comments

`/* */` are used to enclose comments (preprocessed and removed before
interpreting)

```
/* This is
a comment. */
```

### Directions

`>`, `<`, `^` and `v` - These symbols are used to change direction of an
`agent`. The symbols act as a sequence of steps which will be followed during
the course of the `agent`'s life. A 'step' is 'taken' when the agent reaches a
junction (or a building with parameters, but the symbols required must
correspond to the parameter type, and `>`, `<`, `^`, `v` are invalid for
parameters and are only for junctions).

Note: `agents` are synchronized by age priority, i.e when they were spawned,
and the `main agent` has the highest priority.

### Conditional Statements

Within an `agent's` definition the programmer can check for conditions
and have branching step sequences the following manner:

```
?<Address>(<Code 1>:<Code 2>)
```

The sequence to the left of the `:` symbol(Code 1) is executed if the value at `Address`
is non-zero, otherwise the steps to the right of the `:` symbol(Code 2) will execute.
Once the steps in the chosen branch are over, in both cases the steps after the
`)` symbol will execute. It is also possible to nest these conditionals.
- Note: `Address` must be a REG

In this way movement as well parameter inputs to buildings can be controlled
by conditionals. There will be no way to check for relational operators
directly, and this is an intentional design to give more use to the relop
buildings scaterred around the map. If one needs to check if one number is greater
than the other, the agent would have to go and write use the relop building to
write the result to `Address` and then use a conditional step later referrencing
that same address.

### Using opcodes

When an agent enters a building, the opcodes associated with that building will execute. Opcodes generally require parameters.
The parameters can be given directly in the code, with each parameter ending with a `#`.

```
[Parameter 1]#[Parameter 2]#....#
```

## Types of opcode Parameters

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

## Example code snippet:
```
use waraCode.wl
tickrate 60

!main:
>^^countryoven##
/* Spawns a country oven agent with no `life cycle address` assuming the actor
ended up at the SPAWN_AGENT building*/

!countryoven:
>^<v8#10#[COUNTRYOVEN]#
/* This is a comment (enclosed by /* */) ! F.Y.I. [COUNTRYOVEN] is a REG.
If the actor ended up at the ADD building then this would add 8 and 10
and store the result at address [COUNTRYOVEN]$ */
```

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