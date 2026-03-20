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

**Note** - As the number of opcodes increase, it would probably be more
beneficial to merge STDOUTs into one building and have an outmode parameter
instead.