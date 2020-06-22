# Uninitialization Patterns

## Sources
The main reference for all of the information presented is from TLBUNR.c in the
OpenTuring project.
The file used can be found [here](https://github.com/Open-Turing-Project/OpenTuring/blob/master/turing/tlib/TLBUNR.c).


## Introduction
Uninitialization patterns are sequences of uninitialization operations
(uninit-ops) that set a region of memory into a defined pattern.

Uninitialization patters work with 2 pointers to memory:

- The uninitialization area (the memory area which will be uninitialized)
- The pattern area (the source of the uninit-ops)

Each uninit-ops is composed of 2 bytes with the following format:
| Byte 1   | Byte 0   |
|----------|----------|
| CCCCCCCC | OOOOOOcc |

Or

| Bits    | Name          | Purpose                              |
|---------|---------------|--------------------------------------|
| 0 .. 1  | CountType     | Determines the size of count data    |
| 2 .. 7  | Opcode        | Selects the operation to perform     |
| 8 .. 15 | CountByteData | Count for the `CountByte` count type |

All uninit-ops (and 2-byte pairs) are stored in little endian ordering
(least significant bytes first).


## Count type
All uninit-ops have a count type, regardless of if it is used for the operation.
The count type is used in uninit-class operations as the size of the
uninitialized type, and in control-class operations as a parameter for the command.

With the exception of `CountByte`, all of the count types have their count value
specified in the bytes following the command. `CountByte` has the count value
specfied in the same uninit-op in `CountByteData`.

The possible count types are:
| `CountType` | Name      | # of 2-byte pairs following uninit-op |
|-------------|-----------|---------------------------------------|
| 0           | CountNull | None (Resets the count to 0)          |
| 1           | CountLong | 2                                     |
| 2           | CountWord | 1                                     |
| 3           | CountByte | 0 (In `CountByteData`)                |

`CountLong` specifies a 32-bit count value, with the 2-byte pairs
arranged in big endian order (most significant bytes first).

`CountWord` specifies a 16-bit count value.

`CountByte` specifies an 8-bit count value.

`CountNull` resets the count value to 0.


## Uninitialization Opcodes
Uninitialization opcodes (opcodes) perform certain operations that affect either the uninit-op
execution flow, or write to the uninitialization area.

Opcodes that affect the uninit-op execution flow are **control-class opcodes**, while opcodes that
affect the uninitialization area are **uninit-class opcodes**

### Control-Class Opcodes
Control-class opcodes affect the uninit-op execution flow.
The count value specified in the uninit-op is used as a parameter for the commands

The following valid control-class opcodes in TProlog are the following:
| `Opcode` | Name   | Parameter Name |
|----------|--------|----------------|
| 0        | Skip   | SkipOver       |
| 1        | Begin  | (None)         |
| 2        | End    | (None)         |
| 3        | Repeat | RepeatCount    |
| 4        | Call   | GroupOffset    |

`Skip` advances the uninitialization area pointer by `SkipOver` bytes.

`Repeat` repeats the following uninit-op or group of uninit-ops `RepeatCount` times.
If `RepeatCount` is zero, a predefined `RepeatCount` value (always zero in TProlog)
is used as the repeat count.

`Call` executes an uninit-op or a group of uninit-ops, then returning execution flow to the
uninit-op following `Call`. The location the uninit-op(s) to execute is determined relative to
the beginning of the current uninitialization pattern, `GroupOffset` before that.
While in the original TLIB `Call` would cause an assertion fail if `CountType` was `CountNull`,
TProlog does not have this restriction, and this will cause an infinite loop if caution is not
exercised.

`Begin` begins a group of uninit-ops. Grouping uninit-ops is used to allow `Repeat` and `Call`
execute multiple uninit-ops without knowing the number of uninit-ops in a group.

`End` ends a group of uninit-ops.

### Uninitialization-Class Opcodes
Uninitialization-class (or uninit-class) opcodes write to the uninitialization area.
The count value specified by the uninit-op (`Size`) is used as the size of the
uninitialization type, as well as how far to advance the uninitialization area pointer.

`Size` can be larger than the size uninitialization pattern, allowing for the uninitialization
of string types.

The following valid uninit-class in TProlog are the following:
| `Opcode` | Name     | Pattern                   |
|----------|----------|---------------------------|
| 10       | UInt     | `80 00 00 00`             |
| 11       | UNat     | `FF FF FF FF`             |
| 12       | UReal    | `80 00 00 00 80 00 00 00` |
| 13       | UBoolean | `FF`                      |
| 14       | USet     | See Below                 |
| 15       | UString  | `80` `00`                 |
| 16       | UPointer | See Below                 |

All uninit-class operations write a specific pattern to the uninitialization area.
In the table above, bytes are specified in hexadecimal, arranged in big endian ordering,
and separeated by spaces.

#### Variable Length Patterns
The pattern written by `USet` and `UPointer` depends on the value of `Size`.

`USet` writes the following patterns for the given `Size` values:
| `Size` | Pattern       |
|--------|---------------|
| 1      | `80`          |
| 2      | `80 00`       |
| 4      | `80 00 00 00` |

`UPointer` writes the following patterns for the given `Size` values:
| `Size` | Pattern                   |
|--------|---------------------------|
| 4      | `FF FF FF FF`             |
| 8      | `FF FF FF FF 00 00 00 00` |

All other values for `Size` will write nothing for the given uninit-class opcodes.


## Example
The following usage of uninitialization patterns are written in Turing.

Fill a memory region with the pattern for `UInt`, given the size of the region, and
the region address.

Note: UReal is used as the size of the region is assumed to be a multiple of 8.

```
proc Bset(dest : addressint, size : nat4)
    type __procedure: proc x()
    
    % Utilize UNINIT opcode for execution
    %% UNINIT Pattern %%
    % Count Bits
    const CountByte : nat2 := 16#03
    const CountLong : nat2 := 16#01
    const CountNull : nat2 := 16#00

    % Control Opcs
    const Begin  : nat2 := 1 shl 2
    const End    : nat2 := 2 shl 2
    const Repeat : nat2 := 3 shl 2
    
    % Uninit Opcs
    const UInt   : nat2 := 10 shl 2
    const UNat   : nat2 := 11 shl 2
    const UReal  : nat2 := 12 shl 2

    %% TProlog Opcodes %%
    const PROC     : nat4 := 16#BA
    const PUSHADDR : nat4 := 16#BB
    const RETURN   : nat4 := 16#CD
    const UNINIT   : nat4 := 16#EF
    
    % Shift adjustment for 8 byte alignment
    const SHF_ADJ := 3

    % Setup blocks
    var pattern : array 1 .. 4 of nat2
        := init (
            Repeat | CountLong,
            0, /* Lo Count */
            0, /* Hi Count */
            UReal  | CountByte | (8 shl 8),
        )
    
    var execUninit : array 1 .. 8 of nat4
        := init (
            PROC, 0,
            PUSHADDR, 0, /* Variable */
            PUSHADDR, 0, /* Pattern  */
            UNINIT,
            RETURN,
        )
    
    % Setup params (Extra shift to account for UReal placing 8 bytes at a time)
    pattern(2) := (size shr (16+SHF_ADJ)) and 16#FFFF
    pattern(3) := (size shr ( 0+SHF_ADJ)) and 16#FFFF
    
    % Write the params for UNINIT
    execUninit(4) := dest
    execUninit(6) := addr(pattern)
    
    cheat(__procedure, addr(execUninit))()
end Bset
```