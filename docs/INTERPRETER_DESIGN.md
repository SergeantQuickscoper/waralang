# `waralang` Interpreter Design

This document outlines the requirements and architecture of the `waralang`
interpreter.

## Basic modules
The interpreter can be thought of as a combination of the following modules:

- `interpreter-core` - reads the source code and sends the corresponding
commands to modify the `runtime-state`.

- `runtime-state` - a module that keeps track of the current state of the runtime.

- `decoder` - a module that reads a `.wmap` file and sets up the initial
`runtime-state`.

- `view-out` - a module that reads the current runtime state and whose sole
job is to show it to the user in a user friendly way. Maybe we could also use
it to collect user input.

Eveything else can be thought of a sub-module of either of these modules.

## Basic flow of execution

When a source program is executed by the interpreter a rough outline of
execution would be:

1. The source program is read into memory.

2. The first section tells us what the `wmap` file is and thus the `decoder`
has the job of reading through it and setting up the initial `runtime-state`.

3. The initial `runtime-state` contains a table of accessible memory blocks,
building functionalities, source code grid, current tick rate value, active
agents table, etc.

4. All elements of the `runtime-state` are accessible by the `interpreter-core`
and the job of the `interpreter-core` is to calculate everything that happens
during the course of a tick, and apply the updates back to the `runtime-state`.
Basic OPCODE definiitions and so on are all contained within `interpreter-core`.

5. Finally once a tick is written and the `runtime-state` updated, the
`interpreter-core` calls the `view-out` function to refresh the frame
presented to the user. `view-out` will handle both the `main-agent` view
and the `map-view`.

6. A `tick` is done and the `interpreter-core` proceeds to calculate the next
`tick`. Any illegal operations would be recognized when the `interpreter-core`
tries to perform it on the `runtime-state`, the core would recieve the error
and then raise an exception to the user.

This design is inspired by and intentionally mirrors a CPU (`intepreter-core`)
and Primary Memory (`runtime-state`) with the former reading and writing to
the latter every CPU cycle (`tick`).

The rest of this document will go into more detail of each module.
