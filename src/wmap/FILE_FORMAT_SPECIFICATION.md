# File Format Specification for `.wmap`

`.wmap` files will act in place of modules for `waralang`. The core idea is
that two input files, namely an ASCII `.txt` file (which represents the map
visually and all its symbols) and a config `.json` file (which provides config
data including building to opcode mappings, symbol reservation and more)
written in accordance with `wmap/wmapConfig.schema.json`, and encode the data
into a single `.wmap` file which can be loaded by the `waralang` interpreter
for user code to execute on. Also for reader context the term `bid` and `bids`
are heavily used and is used to refer to a unique identifier for buildings on
the map.

## File Format Brief Overview

Here is a brief description of the contents of the binary file, in order:

- `heightBytes` - 1 byte - Specifies byte length of the height of the ASCII map
  in pixels.

- `widthBytes` - 1 byte - Specifies byte length of the width of the ASCII map
  in pixels.

- `bidBytes` - 1 byte - Specifies the byte length of the bid size of the ASCII
  map.

- `height` - `heightBytes` bytes - Specifies the height of the ASCII map.

- `width` - `widthBytes` bytes - Specifies the width of the ASCII map.

- `bidSize` - `bidBytes` bytes - Specifies the number of bytes used to
  represent a singular building on the ASCII map.

- `startX` - `widthBytes` bytes - Specifies the starting x-coordinate.

- `startY` - `heightBytes` bytes - Specifies the starting y-coordinate.

- `direction` - 1 byte - Specify the initial direction of travel. ^ > v <.
- `wordSizeBytes` - 1 byte - Specifies the amount of bytes for the wordSize.

- `wordSize` - `wordSizeBytes` bytes - Specifies the wordsize for the map.
  This will have an effect on how addresses work.

- `cells` - `height` * `width` * (1 + `bidSize`) bytes - Specifies info about
  each cell in the map with each cell being 1 + `bidSize` bytes long and
  containing:
  - 1 byte for the ASCII symbol on the map.
  - `bidSize` bytes will be used to specify the the bid the cell belongs to.

- `bidMap` - (Size depends on bid type please see below) - Specifies
  information on the opcode sequence associated with each bid, excluding the
  reserved bids. The map is an ordered one with the first map corresponding
  to the first bid, and so on. The functionality for a singular bid can be
  categorized into three types - a `FUNC` type used to perform some generic
  functionality, a `MEM` type marking that this building is meant to be used
  as primary memory, and a `REG` type for buildings meant to be used as registers.
  In general the format for a bid to opcode map is as follows:

  - `opcodeBytes` - 1 byte - Specifies the amount of bytes the current opcode
  sequence length is taking up.
  - `opcodeLength` - `opcodeBytes` bytes - Specifies the length of the current
  opcode sequence.
  - `opcodes` - `opcodeLength` bytes - A sequence of opcodes thats associated
  with the current bid, note that in the current implementation we will go with
  a limit of 256 opcodes (1 byte).

  Now an important distinction is that `MEM` and `REG` buildings will be limit
  `opcodeBytes` of length 1 and `opcodeLength` of length 1, and the singular
  opcode in `opcodes` will refer to the special `MEM` or `REG` opcode
  respectively.

  In the case of `REG` two additional fields are also present:
  - `regNameBytes` - 1 byte - Specifies the length of the the register's name.
  - `regName` - `regNameBytes` bytes - A sequence of ASCII characters
  representing the name of the register.

  And in the case of `MEM` buildings we have:
  - `baseAddressBytes` - 1 byte - Specifies the length of the the register's
  name.
  - `baseAddress` - `baseAddressBytes` bytes - A sequence of ASCII characters
  representing the name of the register.
  - `memSizeBytes` - 1 byte - Specifies how many of the next bytes are for
  specifying the size of the primary memory of the current building.
  - `memSize` - `memSizeBytes` bytes - Specifies the size of the primary
  memory for the current building.

  **Note**: I am also considering just have an extra byte before `opcodeBytes`
  to just specify which of the 3 types it is, so that might be a change to add
  as we add more types but now with the limited number of opcodes and types I
  feel like this is fine.

## Reserved `bids`

Currently there are 3 different classes of symbols in `waralang` that aren't
directly functional but are important for the interpreter to know for it to
work. These currently are: `traverables`, `colliders` and `junctions`. The
ASCII characters belonging to these classes should be defined in the config
`.json` file and the last 3 `bids` will be reserved for these based on the max
value for the `bidSize` parameter.

