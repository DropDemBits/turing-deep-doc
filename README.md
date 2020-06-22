# Turing Low-level Documentation
This repository contains files describing some low-level aspects of the Turing
interpreter (also know as TProlog) used by Turing and OpenTuring.

All information was gathered from the [OpenTuring](https://github.com/Open-Turing-Project/OpenTuring) repository.

## File overview
The following files are included in this repository:

- `turing_abort_codes.txt`: Internal abort code numbers and their translations
- `bytecode_header.c`:      Format of the bytecode files packed inside compiled
                            & exported Turing programs
- `UninitPattern.md`:       Document describing uninitialization patterns used
                            by the UNINIT and NEW\* family of opcodes
- `TuringOpcodeRef.md`:     Reference for all of the valid opcodes used by the
                            TProlog bytecode interpreter.
- `RefTodo.txt`:            Todo reference for `TuringOpcodeReference.md`
                            (incomplete).

## License
All files are licensed under the MIT License.
