# Instruction Set for the `waralang` interpreter

This file specifies what opcodes would be understood by the `waralang`
interpreter. As of now, we are limiting ourselves to a opcode size of
1 byte, which will allow for 256 possible opcodes. Below is the current
list of opcodes that are/will be supported by the interpreter:

- NOP - 00000000 - Do nothing for 1 tick.
- MEM - 00000001 - Specifies that this BID acts as a memory device.
- REG - 00000010 - Specifies that this BID acts as a register device.
- ADD - 00000011 - Add 2 numbers and store in a memory location.
- MULT - 00000100 - Multiply 2 numbers and store in a memory location.
- LOAD - 00000101 - Load a value from a primary memory address to a register.
- STORE - 00000111 - Store a value from a register to primary memory.
- COMP - 00001000 - Compare two words with a comparison type and store result
  in a memory location.
- STDIN - 00001001 - Takes user input a decimal integer or character (or
  string) during runtime and stores in a register.
- STDOUT_ASCII - 00001010 - Outputs an ASCII character based on a value in a
  word (need a byte offset logic though).
- STDOUT_INT - 00001011 - Outputs a word as a decimal integer.
- STDOUT_BIN - 00001111 - Outputs a word in binary.
- SPAWN_AGENT - 0001000 - Spawn an agent with an `agent-id`, with an optional
`life cycle address`

**Note** - As the number of opcodes increase, it would probably be more
beneficial to merge STDOUTs into one building and have an outmode parameter
instead.

## Agent Lifecycle

By default, an `agent` will simply execute its step sequence and then terminate.
This is the `default lifecycle` of the `agent` and gives no control over when
this agent dies.

Optionally for more control the programmer can add a `life cycle address` which
just contains a number, initially set to 2 when the `agent` is spawned.

When this `agent` spawns into the map, the `ADDRESS` is overwritten with the number 2.
The `ADDRESS` can be modified to change the `life cycle state` of the `agent` with
the following three codes:

- `ADDRESS` = 2 - The `agent` is running. If the sequence of steps is over, the
`agent` DOES NOT die. Instead, the sequence is repeated from the beginning, while
keeping the current physical position of the `agent` on the map.

- `ADDRESS` = 1 - The `agent` is waiting. It's execution is paused and will remain
stationary wherever it is on the map. Other `agents` can still collide with the
paused `agent` however and have it direction changed as per the `collision rules`.

- `ADDRESS` = 0 - Kills the agent the same `tick`.

Thus using `life cycle addresses` one can implement rudimentary loops in movement,
and terminate or pause `agents` arbitrary by simply writing to the `life`
`cycle address`.

Note: If the `life cycle address` has any value other than 0, 1 or 2 when it's
`agent` is about to be calculated, the program will crash with an exception.
