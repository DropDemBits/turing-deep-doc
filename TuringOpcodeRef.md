# Turing Opcode Reference

This file serves as a reference for all of the current valid opcodes for the following implementations of the Turing language:
 - Turing 4.1.1
 - Turing 4.1.2
 - OpenTuring 1.0.1 / OpenTuring QT 1.2.0 beta.

## Definitions
-----------
**Abort**(code)

Aborts the program with the given error code. The error message corresponds to the respective abort number in the abort code reference. Equivalent to
```TLEABT(code);```

**Instruction**

A memory structure which controls modifies the state of the program

**Opcode**

The first 4 bytes of an instruction which denotes what kind of instruction to execute

**sp**(#, type)

Indicates the n'th stack value with the given type.
A stack value is defined to be the value which is offset by *n* values
from the current stack pointer (i.e. sp - *n* \* sizeof(type)). The offset
is affected by previous "**sp**(#, type)"'s, as the size of a stack argument
depends on the given type
**sp**(0) refers to the value at the current stack pointer


## Conventions
--------------

```thing``` - Indicates a reference to a stack or opcode argument "thing". 
              Always camel case.

```-``` - Indicates that the stack or opcode argument is not used or accessed

## Instruction Structure
-------------------------
Instructions in the Turing Interpreter have varying lengths, and therefore 
depend on the original opcode. Instructions can have a maximum of four 
arguments:

> [ OPC ] [ arg0 ] [ arg1 ] [ arg2 ] [ arg3 ]

However, not all instructions require 4 memory arguments [from turing.reflect]
 - 224 instructions do not have memory arguments
 - 21 instructions have 1 memory argument
 - 7 instructions have 2 memory arguments
 - 2 instructions have 3 memory arguments
 - 1 instruction has 4 memory arguments

The size of an opcode in memory is always 4 bytes, but the size of each 
argument in an instructions depends on the argument type. In most cases, it 
will be 4 bytes. However, for instructions which accept a real8 (e.g. PUSHREAL),
the argument size will be 8 bytes.

Instruction in this document are documented as following:

| OP | Name  | Arg0       | Arg1 | Arg2 | Arg3 |
|----|-------|------------|------|------|------|
| xx | INSTR | arg (nat4) |  -   |  -   |  -   |

where:
  - OP denotes opcode number (in hexadecimal)
  - Name denotes opcode name
  - Arg0 .. Arg3 denotes the argument names, with the type inside the brackets

For opcodes that accept arguments from the stack, the format is as follows:

| Offset | Type    | Name    |
|--------|---------|---------|
|      0 | type    | argName |

where:
  - Offset is the offset, in bytes, from the current stack pointer
  - Type denotes the type of the argument
  - Name denotes the argument name

After the execution of the opcode, all of the stack arguments are popped off
the stack unless otherwise specified.

#### Instruction Template #####
| OP | Name  | Arg0 | Arg1 | Arg2 | Arg3 |
|----|-------|------|------|------|------|
| xx | INSTR |  -   |  -   |  -   |  -   |

#### Stack Arguments
| Offset | Type | Name |
|--------|------|------|
|      0 | type | name |

#### Stack Returns
| Offset | Type | Name |
|--------|------|------|
|      0 | type | name |

#### Type Definition
| Instruction | type | .. other types .. |
|-------------|------|-------------------|
| subinstruct | type | type              |

#### Description
#### Exceptions
#### Operation

## Implementation Constants ##
------------------------------
```
MAX_STRING_LEN := 256
```

## Data Structures ##
---------------------

All of the data structures described below are written in C/C++.


### PointerDescriptor
```
struct PointerDescriptor
{
    TLaddressint address;
    TLnat4 cookie;
};
```

-----
### CaseDescriptor
```
struct CaseDescriptor
{
    TLint4 lowerBound;
    TLint4 upperBound;
    TLoffset defaultBranch
    TLoffset cases[]; // Is a variable length array

    // Compute length by:
    // n = (upperBound - lowerBound) + 1
};
```

-----
### ForDescriptor
```
struct ForDescriptor
{
    TLint4 counter;
    TLint4 end;
    TLint4 step;
    TLaddressint lastSP;
};
```

-----
### DimensionDescriptor
```
struct DimensionDescriptor
{
    TLint4 lowerBound;  // Lowest bound
    TLint4 range;       // Length of the dimension
};
```

-----
### ArrayDescriptor
```
struct ArrayDescriptor
{
    TLnat4 arraySize;  // Array size in bytes
    TLnat4 elemSize;   // Element size in bytes
    TLint4 elemCount;  // Number of elements
    TLnat4 dimCount;   // Number of dimensions
    DimensionDescriptor dimLengths[]; // Length = dimCount
};
```

-----
### UnionDescriptor
```
struct UnionDescriptor
{
    TLint4 lowest;        // Lowest tag value
    TLint4 highest;       // Highest tag value
    TLnat2 mappings[301]; // Valid tag mappings
};
```

-----
### ClassDescriptor
```
struct ClassDescriptor
{
    TLaddressint baseClass; // For interfaces
    TLaddressint inherits;  // Inherited class
    TLnat4 attributes;      // Class attributes
    TLnat4 objectSize;      // Size for an instance
    TLaddressint classId;
    TLaddressint initProc;  // Address to jump to for init
    TLnat4 numOperations;   // Number of procedures contained in the class
};
```

-----
### MonitorDescriptor
```
struct MonitorDescriptor
{
    // TODO: ???
};
```

-------------------------
## Instruction Listing ##
-------------------------
### ABORT - Abort Program
| OP | Name  | Arg0             | Arg1 | Arg2 | Arg3 |
|----|-------|------------------|------|------|------|
| 00 | ABORT | abortType (nat4) |  -   |  -   |  -   |

#### Stack Arguments
None

#### Stack Returns
None

#### Description
Aborts the program with the given ```abortType```

Abort Type list:

| ```abortType``` | ErrNo | Message                                       |
|-----------------|-------|-----------------------------------------------|
| 1               |    21 | Assert condition is false                     |
| 2               |    16 | Pre condition is false                        |
| 3               |    17 | Post condition is false                       |
| 4               |    19 | For loop invariant is false                   |
| 5               |    18 | Loop invariant is false                       |
| 6               |    20 | Module invariant is false                     |
| 7               | 10000 | Cannot convert pointer to more specific class |
| 8               |    25 | Case selector is out of range                 |
| 9               |    26 | Function failed to give a result              |
| \*\*            |    15 | Case selector is out of range                 |

\*\* Only applies to Turing compiled with CHECKED defined

#### Default Type
The default type varies depending on the source keyword and construct:
 - type = 1 for "assert"
 - type = 2 for "pre"
 - type = 3 for "post"
 - type = 4 for "invariant" inside of a for loop construct
 - type = 5 for "invariant" inside of a loop construct
 - type = 6 for "invariant" inside of a module
 - type = 8 for a "case" without a default label
 - type = 9 at the end of every function

Any other type will abort with the "Case selector is out of range" error

#### Exceptions
See "Abort Type List"

#### Operation
```
Abort (abortType)
```

-------------------------------------------------------------------------------
### ABORTCOND - Conditionally Abort Program
| OP | Name      | Arg0             | Arg1 | Arg2 | Arg3 |
|----|-----------|------------------|------|------|------|
| 01 | ABORTCOND | abortType (nat4) |  -   |  -   |  -   |

#### Stack Arguments
| Offset | Type | Name      |
|--------|------|-----------|
|      0 | int4 | willAbort |

#### Stack Returns
None

#### Description
Conditionally aborts a program depending on the value of ```willAbort```
If ```willNotAbort``` is 0, the program is aborted with the default
code, otherwise the program continues. The abort type maps to the same values
as ABORT.

See ABORT for default types and conditions.

#### Exceptions
See ABORT - "Abort Type List"

#### Operation
```
if willNotAbort == 0 then
  Abort (abortType)
fi
```

-------------------------------------------------------------------------------
### ABS* - Absolute Value of Number
| OP | Name    | Arg0 | Arg1 | Arg2 | Arg3 |
|----|---------|------|------|------|------|
| 02 | ABSINT  |  -   |  -   |  -   |  -   |
| 03 | ABSREAL |  -   |  -   |  -   |  -   |

#### Stack Arguments
| Offset | Type  | Name     |
|--------|-------|----------|
|      0 | value | number   |

#### Stack Returns
| Offset | Type  | Name     |
|--------|-------|----------|
|      0 | value | absolute |

#### Type Definition
| Instruction | type  |
|-------------|-------|
| ABSINT      | int4  |
| ABSREAL     | real8 |

#### Description
Gets the absolute value of ```number```.
The result is given back in ```absolute```.

#### Exceptions
None

#### Operation
```
absolute := Abs (number)
```

-------------------------------------------------------------------------------
### ADD* - Add Numbers
| OP | Name      | Arg0 | Arg1 | Arg2 | Arg3 |
|----|-----------|------|------|------|------|
| 04 | ADDINT    |  -   |  -   |  -   |  -   |
| 05 | ADDINTNAT |  -   |  -   |  -   |  -   |
| 06 | ADDNAT    |  -   |  -   |  -   |  -   |
| 07 | ADDNATINT |  -   |  -   |  -   |  -   |
| 08 | ADDREAL   |  -   |  -   |  -   |  -   |

#### Stack Arguments
| Offset      | Type | Name   |
|-------------|------|--------|
|           0 | rval | rvalue |
| sizeof rval | lval | lvalue |

#### Stack Returns
| Offset | Type | Name   |
|--------|------|--------|
|      0 | lval | result |

#### Type Definition
| Instruction | lval  | rval  |
|-------------|-------|-------|
| ADDINT      | int4  | int4  |
| ADDINTNAT   | int4  | nat4  |
| ADDNAT      | nat4  | nat4  |
| ADDNATINT   | nat4  | int4  |
| ADDREAL     | real8 | real8 |

Adds ```lvalue``` and ```rvalue``` together, storing the result in ```result```.

#### Exceptions
##### ADDINT, ADDINTNAT, ADDNAT, ADDNATINT
- Abort (50) "Overflow in Integer expression":
  If ```result``` is too large to fit into the target type

#### ADDREAL
- Abort (51) "Overflow in Real expression":
  If ```result``` is too large to fit into the target type

#### Operation
```
result := lvalue + rvalue

// Check for overflow
```

-------------------------------------------------------------------------------
### ADDSET - Add Sets / Union of Sets
| OP | Name   | Arg0           | Arg1 | Arg2 | Arg3 |
|----|--------|----------------|------|------|------|
| 09 | ADDSET | setSize (nat4) |  -   |  -   |  -   |

#### Stack Arguments
##### Form 1
| Offset | Type    | Name   |
|--------|---------|--------|
|      0 | set32   | rvalue |
|      4 | set32   | lvalue |

##### Form 2
| Offset | Type    | Name   |
|--------|---------|--------|
|      0 | addrint | tvalue |
|      4 | addrint | rvalue |
|      8 | addrint | lvalue |


#### Stack Returns
##### Form 1
| Offset | Type    | Name   |
|--------|---------|--------|
|      0 | set32   | result |

##### Form 2
| Offset | Type    | Name   |
|--------|---------|--------|
|      0 | addrint | tvalue |

#### Description
Computes the union of the two sets ```lvalue``` and ```rvalue```.

Sets in Turing are represented in two ways
 - As a single set32 / nat4
 - As an array of set16 / nat2's

Due to these two representations, two forms of parameter input are required to
merge each set appropriately.

```setSize``` discriminates between these two types, as it determines the
length of the set in bytes.

If ```setSize``` less than or equal to 4 bytes, parameter
'Form 1' is used and a simple bitwise OR ```lvalue``` and ```rvalue``` is
performed. The resulting set is stored in ```result```

Otherwise, 'Form 2' is used, and the bitwise OR is performed between all set
values of ```lvalue``` and ```rvalue```, stored in the same location inside
```tvalue```.

```setSize``` applies to the length of both sets, as merging two sets of
different length is not supported in Turing.

#### Exceptions
None

#### Operation
```
if setSize <= 4 then
  // Perform union of single word sets
  result := lvalue | rvalue
else
  // Perform word-by-word union of sets
  Tset := PointerTo (set16, tvalue)
  Rset := PointerTo (set16, rvalue)
  Lset := PointerTo (set16, lvalue)

  Words := setSize >> 1 // Get set size in words
  WordIdx := 1

  // TProlog performs a check if 'WordIdx' <= 'Words', but because 'Words' can
  // only be greater than 0, the check is not needed

  loop
    *Tset := (*Lset | *Rset)

    Tset += 2
    Rset += 2
    Lset += 2

    if Words == WordIdx then
      break
    fi

    Words += 1
  pool
fi
```

-------------------------------------------------------------------------------
### ALLOCFLEXARRAY - Allocate Flexible Array
| OP | Name           | Arg0 | Arg1 | Arg2 | Arg3 |
|----|----------------|------|------|------|------|
| 0A | ALLOCFLEXARRAY |  -   |  -   |  -   |  -   |

#### Stack Arguments
| Offset | Type    | Name          |
|--------|---------|---------------|
|      0 | addrint | uninitPattern |
|      4 | addrint | descriptor    |
|      8 | addrint | reference     |

#### Stack Returns
None

#### Description
Allocates a flexible array from the heap.

First, a block of memory with the size given in array descriptor 
```descriptor```, plus four bytes, is allocated from the heap. If the size is
zero, the four bytes of memory are still allocated.

Next, the variable reference at ```reference``` is updated to point to the data 
area (```block of memory + 4```), and the memory at ```block of memory + 0``` is
pointed to the variable reference.

Finally, if ```uninitPattern``` is not equal to NULL / zero, the data at
```uninitPattern``` will be used to fill the block of memory with the 
appropriate memory pattern.

#### Exceptions
If there is not enough memory to allocate the array, an ExecutionError with the 
message "Not enough memory for dynamic array allocation" is thrown.

#### Operation
```
// TODO: ALLOCFLEXARRAY
```

-------------------------------------------------------------------------------
### ALLOCGLOB - Allocate Global Data
| OP | Name      | Arg0 | Arg1 | Arg2 | Arg3 |
|----|-----------|------|------|------|------|
| 0B | ALLOCGLOB |  -   |  -   |  -   |  -   |

#### Stack Arguments
| Offset | Type    | Name |
|--------|---------|------|
|      0 |    int4 | size |
|      4 | addrint | addr |

#### Stack Returns
None

#### Description
Allocates ```size``` bytes of memory from the heap and stores the address in
```addr```. ```size``` is rounded up to the next 4-byte boundary.

#### Exceptions
- ExecutionError ("Not enough memory for dynamic allocation"):
  If there is not enough memory to allocate the space required.

#### Operation
```
size := (size + 0x3) & (~0x3)
*addr := AllocHeap (size)
```

-------------------------------------------------------------------------------
### ALLOCGLOBARRAY - Allocate Global Array
| OP | Name           | Arg0 | Arg1 | Arg2 | Arg3 |
|----|----------------|------|------|------|------|
| 0C | ALLOCGLOBARRAY |  -   |  -   |  -   |  -   |

#### Stack Arguments
| Offset | Type    | Name          |
|--------|---------|---------------|
|      0 | addrint | uninitPattern |
|      4 | addrint | descriptor    |
|      8 | addrint | reference     |

#### Stack Returns
None

#### Description
Allocates a fixed array from the heap.

First, a block of memory with the size given in the ArrayDescriptor 
```descriptor```, is allocated from the heap.

Next, the variable reference at ```reference``` is updated to point to the data 
area (```block of memory + 0```).

Finally, if ```uninitPattern``` is not equal to NULL / zero, the data at
```uninitPattern``` will be used to fill the block of memory with the 
appropriate memory pattern.

#### Exceptions
- ExecutionError ("Not enough memory for dynamic array allocation"):
  If there is not enough memory to allocate the array.
- Abort (12) "Dynamic array upper bound is less than lower bound":
  If the arraySize in ```descriptor``` is zero.

#### Operation
```
array := PointerTo(ArrayDescriptor, descriptor)

if array.arraySize < 0 then
  Abort (12) // Dynamic array upper bound is less than lower bound
fi

*reference := AllocHeap (array.arraySize)

UninitBlock (*reference, uninitPatter)
```

-----
### ALLOCLOC - Allocate Local Data
| OP | Name     | Arg0 | Arg1 | Arg2 | Arg3 |
|----|----------|------|------|------|------|
| 0D | ALLOCLOC |  -   |  -   |  -   |  -   |

#### Stack Arguments
| Offset | Type    | Name |
|--------|---------|------|
|      0 |    int4 | size |
|      4 | addrint | addr |

#### Stack Returns
None

#### Description
Allocates ```size``` bytes of memory from stack space and stores the address in
```addr```. ```size``` is rounded up to the next 4-byte boundary.

#### Exceptions
- ExecutionError ("Stack overflow during dynamic allocation"):
  If there is not enough space on the stack to allocate the memory required.

#### Operation
```
size := (size + 0x3) & (~0x3)

GlobalSP -= size
*addr := GlobalSP
```

----
### ALLOCLOCARRAY - Allocate Local Array
| OP | Name          | Arg0 | Arg1 | Arg2 | Arg3 |
|----|---------------|------|------|------|------|
| 0E | ALLOCLOCARRAY |  -   |  -   |  -   |  -   |

#### Stack Arguments
| Offset | Type    | Name          |
|--------|---------|---------------|
|      0 | addrint | uninitPattern |
|      4 | addrint | descriptor    |
|      8 | addrint | reference     |

#### Stack Returns
None

#### Description
Allocates a fixed array from stack space.

First, a block of memory with the size given in array descriptor 
```descriptor```, is allocated from stack space.

Next, the variable reference at ```reference``` is updated to point to the data 
area (```block of memory + 0```).

Finally, if ```uninitPattern``` is not equal to NULL / zero, the data at
```uninitPattern``` will be used to fill the block of memory with the 
appropriate memory pattern.

#### Exceptions
- ExecutionError ("Stack overflow during dynamic allocation")
  If there is not enough space on the stack to allocate the array.
- Abort (12):
  If the size in ```desc``` is zero

#### Operation
```
array := PointerTo (ArrayDescriptor, descriptor)

if array.arraySize < 0 then
  Abort (12) // Dynamic array upper bound is less than lower bound
fi

GlobalSP -= array.arraySize
*reference := GlobalSP

UninitBlock (*reference, uninitPattern)
```

-----
### AND - Bitwise AND
| OP | Name  | Arg0 | Arg1 | Arg2 | Arg3 |
|----|-------|------|------|------|------|
| 0F | AND   |  -   |  -   |  -   |  -   |

#### Stack Arguments
| Offset | Type | Name |
|--------|------|------|
|      0 | nat4 | rval |
|      4 | nat4 | lval |

#### Stack Returns
| Offset | Type | Name   |
|--------|------|--------|
|      0 | nat4 | result |

#### Description
Performs a bitwise logical-AND operation between ```lval``` and ```rval```, and 
stores the result in ```result```

#### Exceptions
None

#### Operation
```
result := lval & rval
```
-----
### ARRAYUPPER - Array Upper Bound
| OP | Name       | Arg0        | Arg1 | Arg2 | Arg3 |
|----|------------|-------------|------|------|------|
| 10 | ARRAYUPPER | dim (int2)  |  -   |  -   |  -   |

#### Stack Arguments
| Offset | Type    | Name |
|--------|---------|------|
|      0 | addrint | desc |

#### Stack Returns
| Offset | Type | Name  |
|--------|------|-------|
|      0 | int4 | bound |

#### Description
Calculates the array's upper bound for the given dimension ```dim```, and using
the given array descriptor ```desc```. The upper bound is stored in ```bound```.

#### Exceptions
- ExecutionError ("Array dimension passed to 'upper' is out of range")
  If the given ```dim``` is less than 1 or greater than the maximum number of 
  dimensions for the given array

#### Operation
```
array := PointerTo (ArrayDescriptor, desc)

if dim < 1 || dim > array.dimCount then
  ExecutionError ("Array dimension passed to 'upper' is out of range")
fi

*bound := array.dimLengths[dim].range + array.dimLengths[dim].lowerBound
```

-----
### ASN* - Assign Value of * to Destination
| OP | Name         |  Arg0 | Arg1 | Arg2 | Arg3 |
|----|--------------|-------|------|------|------|
| 11 | ASNADDR      |   -   |   -  |   -  |   -  |
| 13 | ASNINT       |   -   |   -  |   -  |   -  |
| 15 | ASNINT1      |   -   |   -  |   -  |   -  |
| 17 | ASNINT2      |   -   |   -  |   -  |   -  |
| 19 | ASNINT4      |   -   |   -  |   -  |   -  |
| 1B | ASNNAT       |   -   |   -  |   -  |   -  |
| 1D | ASNNAT1      |   -   |   -  |   -  |   -  |
| 1F | ASNNAT2      |   -   |   -  |   -  |   -  |
| 21 | ASNNAT4      |   -   |   -  |   -  |   -  |
| 25 | ASNPTR       |   -   |   -  |   -  |   -  |
| 27 | ASNREAL      |   -   |   -  |   -  |   -  |
| 29 | ASNREAL4     |   -   |   -  |   -  |   -  |
| 2B | ASNREAL8     |   -   |   -  |   -  |   -  |

#### Stack Arguments
| Offset        | Type    | Name          |
|---------------|---------|---------------|
|             0 | *type*  | value         |
| sizeof *type* | addrint | dest          |


#### Stack Returns
None

#### Type Definition
| Instruction  | Type    |
|--------------|---------|
| ASNADDR      | addrint |
| ASNINT       | int4    |
| ASNINT1      | int1    |
| ASNINT2      | int2    |
| ASNINT4      | int4    |
| ASNNAT       | nat4    |
| ASNNAT1      | nat1    |
| ASNNAT2      | nat2    |
| ASNNAT4      | nat4    |
| ASNPTR \*    | addrint |
| ASNREAL      | real8   |
| ASNREAL4     | real8   |
| ASNREAL8     | real8   |

\* The destination pointer for ASNPTR points to a pointer descriptor

#### Description
Assigns the value of ```value``` to the memory pointed to by ```dest```.

The type for ```value``` is dependent on what the specific opcode is. See the
table above for more details

Depending on the opcode, the placement of the destination pointer may either
be at offset 8 (for ASNREAL and ASNREAL8), or offset 4 (for the rest).

For ASNPTR, the pointer cookie of the destination pointer descriptor is updated
to that of the the cookie at the memory location if ```value``` is not NULL.

ASNREAL4 picks up a real8 value from the stack, but stores the value at the
address as a real4.

#### Value Checking / Exceptions
Some instructions may perform checks to the ```value``` to ensure that it is
a legal value:
 - For ASNINT, the value is tested for being equal to 0x800000000. If
  it is, then an Abort(50) is thrown. 
 - For ASNNAT, the value is tested for being equal to 0xFFFFFFFF. If it is, 
 then an Abort(50) is thrown.
 - For ASNINT1, ASNINT2, ASNNAT1, and ASNNAT2, the value is tested to see if it within the
  range of possible values for the given type. If it isn't, then an Abort(15)
  is thrown.

All other instructions do not perform value checking.

#### Operation (Not ASNPTR)
```
*dest := value
```

#### Operation (ASNPTR)
```
Pointer := PointerTo (PointerDescriptor, dest)
Pointer.address := value

if value != NULL then
  // Update cookie
  Pointer.cookie := *(value - 4)
fi
```

-----
### ASN*INV - Assign Value of * to Destination (swapped)
| OP | Name            |  Arg0 | Arg1 | Arg2 | Arg3 |
|----|-----------------|-------|------|------|------|
| 12 | ASNADDRINV      |   -   |   -  |   -  |   -  |
| 14 | ASNINTINV       |   -   |   -  |   -  |   -  |
| 16 | ASNINT1INV      |   -   |   -  |   -  |   -  |
| 18 | ASNINT2INV      |   -   |   -  |   -  |   -  |
| 1A | ASNINT4INV      |   -   |   -  |   -  |   -  |
| 1C | ASNNATINV       |   -   |   -  |   -  |   -  |
| 1E | ASNNAT1INV      |   -   |   -  |   -  |   -  |
| 20 | ASNNAT2INV      |   -   |   -  |   -  |   -  |
| 22 | ASNNAT4INV      |   -   |   -  |   -  |   -  |
| 26 | ASNPTRINV       |   -   |   -  |   -  |   -  |
| 28 | ASNREALINV      |   -   |   -  |   -  |   -  |
| 2A | ASNREAL4INV     |   -   |   -  |   -  |   -  |
| 2C | ASNREAL8INV     |   -   |   -  |   -  |   -  |

#### Stack Arguments
| Offset | Type    | Name          |
|--------|---------|---------------|
|      0 | addrint | dest          |
|      4 | *type*  | value         |

#### Stack Returns
None

#### Description
Assigns the value of ```value``` to the memory pointed to by ```dest```.
As the operation is similar to the respective ASN* instructions, the details
will still apply to these instructions. However, the locations of the
argument in the stack are fixed in the same place.

#### Exceptions
See ASN* - Assign Value of * to Destination

#### Operation
```
*dest := value
```

-----
### ASNNONSCALAR / ASNNONSCALARINV - Copy Bytes to Destination
| OP | Name            |  Arg0        | Arg1 | Arg2 | Arg3 |
|----|-----------------|--------------|------|------|------|
| 23 | ASNNONSCALAR    | bytes (nat4) |   -  |   -  |   -  |
| 24 | ASNNONSCALARINV | bytes (nat4) |   -  |   -  |   -  |

#### Stack Arguments

##### ASNNONSCALAR
| Offset | Type    | Name          |
|--------|---------|---------------|
|      0 | addrint | src           |
|      4 | addrint | dest          |

##### ASNNONSCALARINV
| Offset | Type    | Name          |
|--------|---------|---------------|
|      0 | addrint | dest          |
|      4 | addrint | src           |

#### Stack Returns
None

#### Description
Copies ```bytes``` number of bytes from the memory of ```src``` to the 
memory of ```dest```. This is essentially a wrapper for the C ```memcpy()``` function.

#### Exceptions
None

#### Operation
```
MemCopy(dest, src, bytes)
```

-----
### ASNSTR / ASNSTRINV - Copy String to Destination
| OP | Name         |  Arg0 | Arg1 | Arg2 | Arg3 |
|----|--------------|-------|------|------|------|
| 2D | ASNSTR       |   -   |   -  |   -  |   -  |
| 2E | ASNSTRINV    |   -   |   -  |   -  |   -  |

#### Stack Arguments

##### ASNSTR
| Offset | Type    | Name          |
|--------|---------|---------------|
|      0 | nat4    | maxLen        |
|      4 | addrint | src           |
|      8 | addrint | dest          |

##### ASNSTRINV
| Offset | Type    | Name          |
|--------|---------|---------------|
|      0 | nat4    | maxLen        |
|      4 | addrint | dest          |
|      8 | addrint | src           |

#### Stack Returns
None

#### Description
Copies the string bytes from ```src``` to the memory location at ```dest```.
Verification is performed to ensure that the length of the ```src``` string 
is less than, or equal to, the length of the ```dest``` string.

#### Exceptions
- Abort(35):
  If the ```src``` string is greater than the length of ```maxLen```

#### Operation
```
if StrLen (src) > maxLen then
  Abort (35) // Length of string value exceeds max length of variable
fi

StrCpy (dest, src)
```

-----
### BEGINHANDLER - Begin Exception Handler
| OP | Name         | Arg0          | Arg1                 | Arg2 | Arg3 |
|----|--------------|---------------|----------------------|------|------|
| 2F | BEGINHANDLER | skip (offset) | handlerArea (offset) |  -   |  -   |

#### Stack Arguments
None

#### Stack Returns
None

#### Description
Sets up the exception handler area in order to handle exceptions and errors.
Skips over the exception handler code to prevent accidental execution outside 
of the exception context.

#### Exceptions
None

#### Operation
```
HandlerArea := PointerTo (GlobalFP - handlerArea)
HandlerArea -> nextHandler = ActiveHandler
SetupHandlerArea (HandlerArea)
ActiveHandler := HandlerArea

// Subtract to account for already skipping over operands
GlobalPC += skip - 2 * (sizeof offset)
```

-----
### BITSASSIGN - Set Bits
| OP | Name       | Arg0          | Arg1        | Arg2         | Arg3 |
|----|------------|---------------|-------------|--------------|------|
| 30 | BITSASSIGN | size (offset) | mask (nat4) | shift (nat4) |  -   |

#### Stack Arguments
| Offset | Type    | Name   |
|--------|---------|--------|
|      0 |    nat4 | value  |
|      4 | addrint | target |

#### Stack Returns
None

#### Description
Sets the bits in ```target``` to the specified ```value```. ```size```
determines the width of the bits to affect (nat1, nat2, nat4), while
```mask``` and ```shift``` determine the location of the bits to modify.

#### Exceptions
If ```size``` is not 1, 2, or 4, Abort (50) will be thrown.

#### Operation
```
value := value << shift
target &= ~mask
target |= value
```
-----
### BITSEXTRACT - Extract Bits
| OP | Name        | Arg0        | Arg1         | Arg2 | Arg3 |
|----|-------------|-------------|--------------|------|------|
| 31 | BITSEXTRACT | mask (nat4) | shift (nat4) |  -   |  -   |

#### Stack Arguments
| Offset | Type    | Name  |
|--------|---------|-------|
|      0 |    nat4 | value |

#### Stack Returns
| Offset | Type    | Name  |
|--------|---------|-------|
|      0 |    nat4 | bits  |

#### Description
Extracts bits from ```value```, with the result being stored in ```bits```.
```mask``` selects the span of bits to select, and ```shift``` brings them down
to the lower-most position.

#### Exceptions
None

#### Operation
```
bits := (value & mask) >> shift
```

-----
### CALL - Call Method
| OP | Name | Arg0            | Arg1 | Arg2 | Arg3 |
|----|------|-----------------|------|------|------|
| 32 | CALL | stkOff (offset) |  -   |  -   |  -   |

#### Stack Arguments
| Offset       | Type    | Name     |
|--------------|---------|----------|
| ```stkOff``` | addrint | procAddr |

#### Stack Returns
| Offset | Type    | Name    |
|--------|---------|---------|
|      0 | addrint | retAddr |

Calls the procedure at ```procAddr```. Leaves the return address in ```retAddr```.

#### Exceptions
Aborts with abort code 45 if ```procAddr``` is an invalid address
Throws an ExecutionError if ```procAddr``` is zero or unresolved

#### Operation
```
procAddr := Peek (addrint, stkOff)

if (IsUninitialized(procAddr)) then
  Abort (45) // Variable has no value
fi

Push (addrint, GlobalPC, retAddr)

if SavedInitPC == 0 then
  // Save PC & FP
  SavedInitPC := GlobalPC
  SavedInitFP := GlobalFP
fi

GlobalPC := procAddr

// Perform implementation specific debug handling //
```
-----
### CALLEXTERNAL - Call External Method
| OP | Name | Arg0               | Arg1 | Arg2 | Arg3 |
|----|------|--------------------|------|------|------|
| 33 | CALL | externIdx (offset) |  -   |  -   |  -   |

#### Stack Arguments
None

#### Stack Returns
None

#### Description
Calls a method not implemented inside of the Turing bytecode environment.
Clears Errno before performing the call.

#### Exceptions
None

#### Operation
```
Errno := 0

if !InRange (externIdx, 0, MaxExternalFunc) then
  Abort (1) // Array subscript is out of range
fi

LookupAndExecute(externIdx, GlobalSP) // Execute the external function

if WasInterrupted(CurrentProcess)
  GlobalPC -= sizeof opcode // Try the external call again
fi

```
-----
### CALLIMPLEMENTBY - Call Implemented Method
| OP | Name            | Arg0          | Arg1               | Arg2 | Arg3 |
|----|-----------------|---------------|--------------------|------|------|
| 34 | CALLIMPLEMENTBY | link (offset) | procAddr (addrint) |  -   |  -   |

#### Stack Arguments
None

#### Stack Returns
| Offset | Type    | Name    |
|--------|---------|---------|
|      0 | addrint | retAddr |

#### Description
Executes a method that follows down an interface. Jumps to ```procAddr```,
preserving the return address in ```retAddr```.
```link``` is used when patching the code area to the correct address. It is
not used during runtime operation.

#### Exceptions
None

#### Operation
```
Push(addrint, GlobalPC + sizeof offset, retAddr)
GlobalPC := *procAddr
```

-----
### CASE - Case of
| OP | Name | Arg0                | Arg1 | Arg2 | Arg3 |
|----|------|---------------------|------|------|------|
| 35 | CASE | descriptor (offset) |   -  |   -  |   -  |

#### Stack Arguments
| Offset | Type    | Name     |
|--------|---------|----------|
|      0 |    int4 | selector |

#### Stack Returns
None

#### Description
Jumps to a specifc branch depending on the value of ```selector```.
```descriptor``` points to a case descriptor, relative to the current
GlobalPC.

Each of the case branch jump offsets and the ```defaultBranch``` jump offset
are relative to GlobalPC, before skipping over ```descriptor```.

#### Case Descriptor
The case descriptor provides information on how to process the selector value.
In a case descriptor, there exists a variable amount of entries, but the entry
count is able to be computed using the following:

```
n = (upperBound - lowerBound) + 1
```

The maximum number of entries is 1000.

| Offset   | Type    | Name          |
|----------|---------|---------------|
|        0 |    int4 | lowerBound    |
|        4 |    int4 | upperBound    |
|        8 |  offset | defaultBranch |
|       12 |  offset | case 0        |
|       16 |  offset | case 1        |
|     ...  |  offset |      ...      |
| 12 + n*4 |  offset | case n-1      |

#### Exceptions
- Quit (CaseSelectorOutOfRange)*: If ```selector``` does not match any of the
  selector values

\* This quit only occurs if the case statement does not have a user-specified
   default branch.

#### Operation
```
CaseDescriptor descriptor := *(GlobalPC + descriptor)
selector := Pop(0, int4)

if selector < descriptor.lowerBound || selector > descriptor.upperBound then
  // Jump to the default branch
  GlobalPC += descriptor.defaultBranch
else
  // Jump to the specific case branch
  GlobalPC += descriptor.cases[selector - descriptor.lowerBound]
fi
```

-----
### CAT - Concatenate
| OP | Name | Arg0 | Arg1 | Arg2 | Arg3 |
|----|------|------|------|------|------|
| 36 | CAT  |  -   |  -   |  -   |  -   |

#### Stack Arguments
| Offset | Type    | Name   |
|--------|---------|--------|
|      0 | addrint | target |
|      4 | addrint | lstr   |
|      8 | addrint | rstr   |

#### Stack Returns
| Offset | Type    | Name   |
|--------|---------|--------|
|      0 | addrint | target |

#### Description
Joins ```lstr``` with ```rstr```, storing the result in ```target```.

#### Exceptions
- ExecutionError ("String generated by string catenation too long"):
  If the resulting length of ```target``` is too long to fit into a string type
  (i.e. length of ```target``` > MAX_STRING_LEN )

#### Operation
```
if (StrLen (lstr) + StrLen (rstr)) > MAX_STRING_LEN then
  ExecutionError ("String generated by string catenation too long")
fi

target := StrCat(lstr, rstr)
```

-----
### CHARSUBSTR1 - Substring of Char(n) from Bound to End
| OP | Name        | Arg0        | Arg1 | Arg2 | Arg3 |
|----|-------------|-------------|------|------|------|
| 37 | CHARSUBSTR1 | kind (nat1) |  -   |  -   |  -   |

#### Stack Arguments
| Offset | Type    | Name   |
|--------|---------|--------|
|      0 | addrint | target |
|      4 | int4    | length |
|      8 | int4    | index  |

#### Stack Returns
| Offset | Type    | Name   |
|--------|---------|--------|
|      0 | addrint | target |

#### Description
Takes a substring from ```target``` using ```index```.
The starting point of the substring depends on the value of ```kind```:

| ```kind```     | Description                                                          |
|----------------|----------------------------------------------------------------------|
| FromStart (0)  | Starts the substring index from offset 0, going forwards             |
| FromEnd (!= 0) | Starts the substring index from offset ```length```, going backwards |

The resulting string subsection is stored in ```target```.

#### Exceptions
- ExecutionError ("Char substring index is less than 1"):
  If the computed substring index goes beyond the start of ```target```
- ExecutionError ("Char substring index is greater than length of char(n)"):
  If the computed substring index goes beyond the end of ```target```

#### Operation
```
StrIdx := -1

if kind == 0 then
  // FromStart
  StrIdx := index
else
  // FromEnd
  StrIdx := index - length
fi

if StrIdx < 0 then
  ExecutionError ("Char substring index is less than 1")
elseif StrIdx >= length then
  ExecutionError ("Char substring index is greater than length of char(n)")
fi

// Adjust target start
target += (StrIdx - 1)
```

-----
### CHARSUBSTR2 - Substring of Char(n) from Left Bound To Right Bound
| OP | Name        | Arg0        | Arg1 | Arg2 | Arg3 |
|----|-------------|-------------|------|------|------|
| 38 | CHARSUBSTR2 | kind (nat1) |  -   |  -   |  -   |

#### Stack Arguments
| Offset | Type    | Name      |
|--------|---------|-----------|
|      0 | addrint | target    |
|      4 | int4    | length    |
|      8 | int4    | toIndex   |
|     12 | int4    | fromIndex |
|     16 | addrint | source    |

#### Stack Returns
| Offset | Type    | Name   |
|--------|---------|--------|
|      0 | addrint | target |

#### Description
Takes a substring from ```source``` and copies it to ```target```, using
```fromIndex``` and ```toIndex```

The computed substring indices depends on the value of ```kind```

| ```kind```        | Description                                          |
|-------------------|------------------------------------------------------|
| BothFromStart (2) | Compute both from the start                          |
| EndFromEnd    (3) | Compute the end index from the end of ```source```   |
| StartFromEnd  (4) | Compute the start index from the end of ```source``` |
| BothFromEnd   (5) | Compute both from the end of ```source```            |

#### Exceptions
- ExecutionError ("Left bound of char substring is less than 1"):
  If the computed start index goes beyond the start of ```source```.

- ExecutionError ("Left bound of char substring exceeds right bound by more than 1"):
  If the computed start index goes beyond the end of ```source```.

- ExecutionError ("Right bound of char substring is greater than size of char(n)"):
  If the computed end index goes beyond the end of ```source```.

- ExecutionError ("Char substring too large to fit into 'string'"):
  If the substring selected by the indices is longer than MAX_STRING_LEN.

- ExecutionError ("char(n) converted to string contains EOS or uninitchar")
  If ```source``` contains the EOS (0x00) or uninitchar (0x80) characters.

- Quit (25):
  If ```kind``` is not one of the accepted values: 2, 3, 4, 5.

#### Operation
```
StartIdx := fromIndex
EndIdx := toIndex

case kind of
  label 2: // BothFromStart
    StartIdx += 0
    EndIdx += 0
  label 3: // EndFromEnd
    StartIdx += 0
    EndIdx += length
  label 4: // StartFromEnd
    StartIdx += length
    EndIdx += 0
  label 5: // BothFromEnd
    StartIdx += length
    EndIdx += length
esac

if StartIdx < 0 then
  ExecutionError ("Left bound of char substring is less than 1")
elseif StartIdx >= length then
  ExecutionError ("Left bound of char substring exceeds right bound by more than 1")
elseif EndIdx >= length then
  ExecutionError ("Right bound of char substring is greater than length of string")
fi

// Compute substring length
SubstrLen := EndIdx - StartIdx - 1

SrcPtr := PointerTo (char, source)
DstPtr := PointerTo (char, target)

while SubstrLen > 0
  // Check for bad characters
  if *SrcPtr == 0x80 || *SrcPtr == 0x00 then
    ExecutionError ("char(n) converted to string contains EOS or uninitchar")
  fi

  *DstPtr := *SrcPtr

  SrcPtr += 1
  DstPtr += 1
  SubstrLen -= 1
elihw

// Null terminate
*DstPtr := 0
```

-----
### CHARTOCSTR - Convert Char to Char(n)
| OP | Name       | Arg0 | Arg1 | Arg2 | Arg3 |
|----|------------|------|------|------|------|
| 39 | CHARTOCSTR |  -   |  -   |  -   |  -   |

#### Stack Arguments
| Offset | Type    | Name   |
|--------|---------|--------|
|      0 | addrint | source |
|      4 | addrint | target |

#### Stack Returns
| Offset | Type    | Name   |
|--------|---------|--------|
|      0 | addrint | target |

#### Description
Copies a single character from the ```source``` variable into the ```target```
location.

#### Exceptions
None

#### Operation
```
target[0] := *source
target[1] := 0x00 // Not in TProlog
```

-----
### CHARTOSTR* - Convert Char to String
| OP | Name          | Arg0 | Arg1 | Arg2 | Arg3 |
|----|---------------|------|------|------|------|
| 3A | CHARTOSTR     |  -   |  -   |  -   |  -   |
| 3B | CHARTOSTRLEFT |  -   |  -   |  -   |  -   |

#### Stack Arguments (CHARTOSTR)
| Offset | Type    | Name   |
|--------|---------|--------|
|      0 | addrint | target |
|      4 |    int4 | chr    |

#### Stack Returns (CHARTOSTR)
| Offset | Type    | Name   |
|--------|---------|--------|
|      0 | addrint | target |

#### Stack Arguments (CHARTOSTRLEFT)
| Offset | Type    | Name     |
|--------|---------|----------|
|      0 | addrint | target   |
|      4 |    nat4 | preserve |
|      8 |    int4 | chr      |

#### Stack Returns (CHARTOSTRLEFT)
| Offset | Type    | Name     |
|--------|---------|----------|
|      0 | addrint | target   |
|      4 |    nat4 | preserve |

#### Description
Copies ```chr``` into the string location at ```target```.

#### Exceptions
- ExecutionError ("char(n) converted to string contains EOS of uninitchar"):
  If ```chr``` is the EOS character (0x80 or 0x00) for an uninitialized
  string

#### Operation
```
if (value == 0x80) || (value == 0x00) then
  ExecutionError ("char(n) converted to string contains EOS of uninitchar")
fi

target[0] := chr
target[1] := 0x00
```

-----
### CHKCHRSTRSIZE - Check Char(n) Size
| OP | Name          | Arg0            | Arg1 | Arg2 | Arg3 |
|----|---------------|-----------------|------|------|------|
| 3C | CHKCHRSTRSIZE | reqLen (offset) |  -   |  -   |  -   |

#### Stack Arguments
| Offset | Type    | Name   |
|--------|---------|--------|
|      0 |    int4 | length |

#### Stack Returns
None

#### Operation
Checks if ```length``` matches the required length of ```reqLen```.

#### Exceptions
- Quit (42) "right side of assignment to char(n) is not length 'n'":
  If the char(n) ```length``` does not match ```reqLen```

#### Operation
```
if length != reqLen then
  Quit (42) // right side of assignment to char(n) is not length 'n'
fi
```

-----
### CHKCSTRRANGE - Check Char(n) Length
| OP | Name         | Arg0            | Arg1 | Arg2 | Arg3 |
|----|--------------|-----------------|------|------|------|
| 3D | CHKCSTRRANGE | reqLen (offset) |  -   |  -   |  -   |

#### Stack Arguments
| Offset | Type    | Name  |
|--------|---------|-------|
|      0 | addrint | charn |

#### Stack Returns
| Offset | Type    | Name  |
|--------|---------|-------|
|      0 | addrint | charn |

#### Operation
Checks if the length of ```charn``` matches the required length of
```reqLen```.

#### Exceptions
- ExecutionError ("Length of char(n) parameter is not ```reqLen```"):
  If the length of charn is large than ```reqLen```.

#### Operation
```
if StrLen (charn) > reqLen then
  ExecutionError ("Length of char(n) parameter is not ```reqLen```")
fi
```

-----
### CHKRANGE - Check Range of Value
| OP | Name     | Arg0             | Arg1      | Arg2      | Arg3             |
|----|----------|------------------|-----------|-----------|------------------|
| 3E | CHKRANGE | stkOff (addrint) | lo (int4) | hi (int4) | checkType (int1) |

#### Stack Arguments
| Offset       | Type    | Name  |
|--------------|---------|-------|
| ```stkOff``` | int4    | value |

#### Stack Returns
None

The value at ```stkOff``` is preserved.

#### Description
Checks if ```value``` is greater than or equal to ```lo``` and less than or
equal to ```hi```. If the value isn't within the given bounds, an abort is
performed.

The specific abort that will be performed is dependent on the ```checkType```:

| ```checkType``` | ErrNo | Message                                            |
|-----------------|-------|----------------------------------------------------|
| 0               |    15 | Assignment value is out of range                   |
| 1               | 10000 | Size of dynamic char(n) is less than 1             |
| 2               | 10000 | Value passed to 'chr' is < 0 or > 255              |
| 3               |    50 | Overflow in Integer expression                     |
| 4               |    24 | Result value is out of range                       |
| 5               | 10000 | Zero or negative for loop step                     |
| 6               |    38 | Pred applied to the first value of the enumeration |
| 7               |    39 | Succ applied to the first value of the enumeration |
| 8               |    14 | Tag value is out of range                          |
| 9               |    22 | Value parameter is out of range                    |
| \*\*            |    25 | Case selector is out of range                      |

\*\* Only applies to Turing compiled with CHECKED defined

#### Exceptions
See above.

#### Operation
```
value := Peek (int4, stkOff)

if (value < lo || value > hi) then
  Abort (checkType)
fi
```

-----
### CHKSTRRANGE - Check String Range
| OP | Name        | Arg0          | Arg1 | Arg2 | Arg3 |
|----|-------------|---------------|------|------|------|
| 3F | CHKSTRRANGE | maxLen (int2) |  -   |  -   |  -   |

#### Stack Arguments
| Offset | Type    | Name   |
|--------|---------|--------|
|      0 | addrint | string |

#### Stack Returns
| Offset | Type    | Name   |
|--------|---------|--------|
|      0 | addrint | string |

#### Description
Checks if the length of ```string``` is less or equal to ```maxLen```.

#### Exceptions
- Abort(23) "Length of string parameter exceeds max length of string formal":
  If the  length of ```string``` is larger than ```maxLen```.

#### Operation
```
if StrLen (string) > maxLen then
  Abort (23) // "Length of string parameter exceeds max length of string formal"
fi
```

-----
### CHKSTRSIZE - Check String Size
| OP | Name       | Arg0            | Arg1 | Arg2 | Arg3 |
|----|------------|-----------------|------|------|------|
| 40 | CHKSTRSIZE | reqLen (offset) |  -   |  -   |  -   |

#### Stack Arguments
| Offset | Type    | Name   |
|--------|---------|--------|
|      0 | addrint | string |

#### Stack Returns
| Offset | Type    | Name   |
|--------|---------|--------|
|      0 | addrint | string |

#### Operation
Checks if the length of ```string``` matches the required length of
```reqLen```.

#### Exceptions
- Quit (42) "right side of assignment to char(n) is not length 'n'":
  If the length of ```string``` does not match ```reqLen```

#### Operation
```
if StrLen (string) != reqLen then
  Quit (42) // right side of assignment to char(n) is not length 'n'
fi
```

-----
### CLOSE - Close File Handle
| OP | Name  | Arg0 | Arg1 | Arg2 | Arg3 |
|----|-------|------|------|------|------|
| 41 | CLOSE |  -   |  -   |  -   |  -   |

#### Stack Arguments
| Offset | Type | Name |
|--------|------|------|
|      0 | int4 |  fd  |

#### Stack Returns
None

#### Description
Closes the given file handle ```fd``` and frees up any resources associated with
the file handle.

#### Exceptions
- BadStreamAbort (): If ```fd``` is less than -2
- ExecutionError ("Close of illegal stream number ```fd```"): If ```fd``` is
  greater than MAX_FD (20)
- ExecutionError ("Close of closed stream number ```fd```"): If ```fd``` points
  to a closed stream

#### Operation
```
Close (fd)
```

-----
### COPYARRAYDESC - Copy Array Descriptor
| OP | Name          | Arg0 | Arg1 | Arg2 | Arg3 |
|----|---------------|------|------|------|------|
| 42 | COPYARRAYDESC |  -   |  -   |  -   |  -   |

#### Stack Arguments
| Offset | Type    | Name     |
|--------|---------|----------|
|      0 | addrint | srcDesc  |
|      4 | addrint | destDesc |

#### Stack Returns
None

#### Description
Copys the array descriptor info from ```srcDesc``` into ```destDesc```.
The information copied includes:
- The total size of the array
- The size of each element
- The number of elements
- The number of dimensions
- The length of each dimension

#### Exceptions
None

#### Operation
```
// Copy basic array descriptor info
MemCpy (destDesc, srcDesc, sizeof ArrayDescriptor)

// Copy dimension info
MemCpy (
  destDesc->dimLengths,
  srcDesc->dimLengths,
  sizeof DimensionDescriptor * srcDesc->numDimensions)
```

-----
### CSTRTOCHAR - Convert Char(n) to Char
| OP | Name       | Arg0 | Arg1 | Arg2 | Arg3 |
|----|------------|------|------|------|------|
| 43 | CSTRTOCHAR |  -   |  -   |  -   |  -   |

#### Stack Arguments
| Offset | Type    | Name  |
|--------|---------|-------|
|      0 |    int4 | len   |
|      4 | addrint | charn |

#### Stack Returns
| Offset | Type    | Name  |
|--------|---------|-------|
|      0 |    int4 | len   |
|      4 |    char | chr   |

#### Description
Extracts a single character from the C string at ```charn``` and puts it into
```chr```. ```len``` is the length of the C string

#### Exceptions
- ExecutionError ("char(n) coerced to char is not length 1"): If ```len``` is
  not of length 1

#### Operation
```
if len != 1 then
  ExecutionError ("char(n) coerced to char is not length 1")
fi

chr := charn[0] // Get the first character
```

-----
### CSTRTOSTR* - Convert Char(n) to String
| OP | Name          | Arg0 | Arg1 | Arg2 | Arg3 |
|----|---------------|------|------|------|------|
| 44 | CSTRTOSTR     |  -   |  -   |  -   |  -   |
| 45 | CSTRTOSTRLEFT |  -   |  -   |  -   |  -   |

#### Stack Arguments (CSTRTOSTR)
| Offset | Type    | Name   |
|--------|---------|--------|
|      0 | addrint | target |
|      4 |    int4 | len    |
|      8 | addrint | source |

#### Stack Arguments (CSTRTOSTRLEFT)
| Offset | Type    | Name   |
|--------|---------|--------|
|      0 | addrint | target |
|      4 |    int4 | len    |
|      8 |    nat4 | save   |
|     12 | addrint | source |

#### Stack Returns (CSTRTOSTR)
| Offset | Type    | Name   |
|--------|---------|--------|
|      0 | addrint | target |

#### Stack Returns (CSTRTOSTRLEFT)
| Offset | Type    | Name   |
|--------|---------|--------|
|      0 |    nat4 | save   |
|      4 | addrint | target |

#### Description
Converts the Char(n) source string into a type string operand by copying the
string data from ```source``` to ```target```.

```len``` specifies how many characters to copy from the ```source``` into the
```target```.

#### Exceptions
- ExecutionError ("char(n) converted to string contains EOS of uninitchar"):
  If ```source``` contains the EOS character (0x80 or 0x00) for an uninitialized
  string
- ExecutionError ("char(n) size of ```len``` is too large to convert to string"):
  If the requested number of characters to copy is too large to fit inside of a
  string

#### Operation
```
if len >= MAX_STRING_LENGTH then
  ExecutionError ("char(n) size of ```len``` is too large to convert to string")
fi

targetChar := PointerTo (char, target)
sourceChar := PointerTo (char, source)
copied := 0

do
  if (*sourceChar == 0) || (*sourceChar == 0x80) then
    ExecutionError ("char(n) converted to string contains EOS of uninitchar")
  fi

  *targetChar := *sourceChar
  targetChar += 1
  sourceChar += 1
  copied += 1
until copied == len

*targetChar := 0x00 // Null terminate string
```

-----
### DEALLOCFLEXARRAY - Deallocate Flexible Array
| OP | Name             | Arg0 | Arg1 | Arg2 | Arg3 |
|----|------------------|------|------|------|------|
| 46 | DEALLOCFLEXARRAY |  -   |  -   |  -   |  -   |

#### Stack Arguments
| Offset | Type    | Name |
|--------|---------|------|
|      0 | addrint | area |

#### Stack Returns
None

#### Description
Frees the memory used up by the flexible array at ```area``` and NULLs / zeros
the variable reference pointing to this ```area```

First, the variable reference at ```area - 4``` is zero'd out, then the 
```area``` is freed from the heap.

#### Exceptions
None

#### Operation
```
varLink := PointerTo (addrint, area - 4)
*varLink := 0   // Zero variable link
FreeHeap (area)
```


-----
### DECSP - Decrement Stack Pointer
| OP | Name  | Arg0            | Arg1 | Arg2 | Arg3 |
|----|-------|-----------------|------|------|------|
| 47 | DECSP | amount (offset) |  -   |  -   |  -   |

#### Stack Arguments
None

#### Stack Returns
None

#### Description
Decrements the stack pointer by ```amount``` bytes and zeros out the memory 
area. Allocates space for local variables.

If ```amount``` is zero, no action is performed.

#### Exceptions
None

#### Operation
```
GlobalSP -= amount
```

-----
### DIV* - Integer Divide Values
| OP | Name    | Arg0 | Arg1 | Arg2 | Arg3 |
|----|---------|------|------|------|------|
| 48 | DIVINT  |  -   |  -   |  -   |  -   |
| 49 | DIVNAT  |  -   |  -   |  -   |  -   |
| 4A | DIVREAL |  -   |  -   |  -   |  -   |

#### Stack Arguments
| Offset      | Type  | Name     |
|-------------|-------|----------|
|           0 | type  | divisor  |
| sizeof type | type  | dividend |

#### Stack Returns
| Offset | Type  | Name     |
|--------|-------|----------|
|      0 | divis | quotient |

#### Type Definition
| Instruction | type  | divis |
|-------------|-------|-------|
| DIVINT      | int4  | int4  |
| DIVNAT      | nat4  | nat4  |
| DIVREAL     | real8 | int4  |

Divides ```dividend``` by ```divisor``` and stores the result in
```quotient```. The fractional digits of ```quotient``` are truncated away.

#### Exceptions
DIVINT, DIVNAT
- Aborts with Abort (33) "Division (or modulus) by zero in Integer expression"

DIVREAL
- Aborts with Abort (113) "Division (or modulus) by zero in Real expression"
- Throws ExecutionError with message "Overflow in real \'div\' operation" if the
  operation overflowed

#### Operation

DIVINT, DIVNAT
````
if divisor == 0 then
  Abort (33) // Division (or modulus) by zero in Integer expression
fi

quotient := dividend / divisor
````

DIVREAL
````
if divisor == 0 then
  Abort (113) // Division (or modulus) by zero in Real expression
fi

quotient := floor(dividend / divisor)

if !IsFinite(quotient) then
  Abort (51) // Overflow in Real expression
fi
````

-----
### EMPTY - Is Condition Empty
| OP | Name  | Arg0 | Arg1 | Arg2 | Arg3 |
|----|-------|------|------|------|------|
| 4B | EMPTY |  -   |  -   |  -   |  -   |

#### Stack Arguments
| Offset | Type    | Name      |
|--------|---------|-----------|
|      0 | addrint | condition |

#### Stack Returns
| Offset | Type | Name    |
|--------|------|---------|
|      0 | int4 | isEmpty |

#### Description
Checks if the given ```condition``` has no pending processes waiting on it. If
true, ```isEmpty``` is set to 1. Otherwise, it is set to 0.

##### Note
Subsequent executions of EMPTY may or may not set ```isEmpty``` to the same
value if the given condition is a timeout condition, as a process can timeout
a wait between the the two executions.

This means that the following expression
```
empty(condition) and empty(condition)
```
may evaluate to false.

#### Exceptions
None

-----
### ENDFOR - End For Loop Construct
| OP | Name   | Arg0              | Arg1 | Arg2 | Arg3 |
|----|--------|-------------------|------|------|------|
| 4C | ENDFOR | jumpBack (offset) |  -   |  -   |  -   |

#### Stack Arguments
| Offset | Type    | Name    |
|--------|---------|---------|
|      0 | addrint | forInfo |

#### Stack Returns
None

#### Description
Completes one iteration of the for-loop.
The counter in ```forInfo``` is checked to see if it has reached the end value,
or if the next step will take the counter to the limits of int4. If either of
these conditions are satisfied, the for-loop is complete. Otherwise, GlobalPC
is moved back by ```jumpBack``` bytes.

The stack pointer is also restored to the previous location.

#### Exceptions
None

#### Operation
```
descriptor := PointerTo (ForDescriptor, forInfo)

GlobalSP := descriptor.lastSP

if descriptor.step > 0 then
  // Check positive step
  if descriptor.counter < (maxint - descriptor.step) && descriptor.counter < descriptor.end then
    // Step & go back
    descriptor.counter += descriptor.step

    // Add to account for the operand skip
    GlobalPC -= (jumpBack + sizeof offset)
  fi
else
  // Check negative step
  if descriptor.counter > (minint - descriptor.step) && descriptor.counter > descriptor.end then
    // Step & go back
    descriptor.counter += descriptor.step

    // Add to account for the operand skip
    GlobalPC -= (jumpBack + sizeof offset)
  fi
fi

// Fell through to here, for-loop is done
```

-----
### EOF - Test for EOF
| OP | Name  | Arg0 | Arg1 | Arg2 | Arg3 |
|----|-------|------|------|------|------|
| 4D | EOF   |  -   |  -   |  -   |  -   |

#### Stack Arguments
| Offset | Type | Name |
|--------|------|------|
|      0 | type | name |

#### Stack Returns
| Offset | Type | Name |
|--------|------|------|
|      0 | type | name |

#### Description

-----
### EQ* - Are Values Equal
| OP | Name     | Arg0       | Arg1 | Arg2 | Arg3 |
|----|----------|------------|------|------|------|
| 4E | EQADDR   |  -         |  -   |  -   |  -   |
| 4F | EQCHARN  | len (int4) |  -   |  -   |  -   |
| 50 | EQINT    |  -         |  -   |  -   |  -   |
| 51 | EQINTNAT |  -         |  -   |  -   |  -   |
| 52 | EQNAT    |  -         |  -   |  -   |  -   |
| 53 | EQREAL   |  -         |  -   |  -   |  -   |
| 55 | EQSTR    |  -         |  -   |  -   |  -   |

#### EQCHARN
##### Stack Arguments
| Offset      | Type | Name   |
|-------------|------|--------|
|           0 | rval | rvalue |
| sizeof rval | lval | lvalue |

##### Stack Returns
| Offset      | Type | Name   |
|-------------|------|--------|
|           0 | rval | rvalue |
| sizeof rval | lval | lvalue |
| sizeof lval | int4 | isEQ   |

#### Generic
##### Stack Arguments
| Offset      | Type | Name   |
|-------------|------|--------|
|           0 | rval | lvalue |
| sizeof rval | lval | rvalue |

##### Stack Returns
| Offset | Type | Name |
|--------|------|------|
|      0 | int4 | isEQ |

#### Type Definition
| Instruction | lval    | rval    |
|-------------|---------|---------|
| EQADDR      | addrint | addrint |
| EQCHARN     | addrint | addrint |
| EQINT       | int4    | int4    |
| EQINTNAT    | nat4    | int4    |
| EQNAT       | nat4    | nat4    |
| EQREAL      | real8   | real8   |
| EQSTR       | addrint | addrint |

#### Description
Performs the "equal-to" comparison between ```lvalue``` and
```rvalue```.

If the condition is satisfied, ```isEQ``` is set to 'true' (1).
Otherwise, ```isEQ``` is set to 'false' (0).

For EQINTNAT, if ```rvalue``` is negative, ```isEQ``` is always 'false'.

#### Exceptions
None

#### Operation (Not EQSTR, EQCHRN, or EQSET)
```
if lvalue == rvalue then
  isEQ := 1
else
  isEQ := 0
fi
```

#### Operation (EQSTR)
```
if StrCmp (lvalue, rvalue) == 0 then
  isEQ := 1
else
  isEQ := 0
fi
```

#### Operation (EQCHARN)
```
// Ignore zeros in the Char(n)'s
if MemCmp (lvalue, rvalue, len) == 0 then
  isEQ := 1
else
  isEQ := 0
fi
```

-----
### EQSET - Are Sets Equal
| OP | Name  | Arg0           | Arg1 | Arg2 | Arg3 |
|----|-------|----------------|------|------|------|
| 92 | EQSET | setSize (nat4) |  -   |  -   |  -   |

#### Stack Arguments
##### Form 1
| Offset | Type    | Name   |
|--------|---------|--------|
|      0 | set32   | rvalue |
|      4 | set32   | lvalue |

##### Form 2
| Offset | Type    | Name   |
|--------|---------|--------|
|      0 | addrint | rvalue |
|      4 | addrint | lvalue |


#### Stack Returns
| Offset | Type    | Name   |
|--------|---------|--------|
|      0 | int4    | isEQS  |

#### Description
Tests if ```lvalue``` is equal to ```rvalue```.
A set (<i>A</i>) is defined to be equal to another set (<i>B</i>) if <i>A</i>
contains the same elements as <i>B</i>, and if <i>B</i> does for <i>A</i>.

Sets in Turing are represented in two ways
 - As a single set32 / nat4
 - As an array of set16 / nat2's

Due to these two representations, two forms of parameter input are required to
test each set appropriately.

```setSize``` discriminates between these two types, as it determines the
length of the set in bytes.

If ```setSize``` less than or equal to 4 bytes, parameter 'Form 1' is used and
the test between ```lvalue``` and ```rvalue``` is performed for the first set32.

Otherwise, 'Form 2' is used, and the equality test is performed between all set
words of ```lvalue``` and ```rvalue```.

In both cases, the test result is stored in ```isEQS```

```setSize``` applies to the length of both sets, as merging two sets of
different length is not supported in Turing.

#### Exceptions
None

#### Operation
```
if setSize <= 4 then
  // Perform equality test between single word sets
  // A & B
  isSubS := (lvalue & rvalue) == 0
else
  // Perform word-by-word equality test of sets
  Rset := PointerTo (set16, rvalue)
  Lset := PointerTo (set16, lvalue)

  Words := setSize >> 1 // Get set size in words
  WordIdx := 1

  // TProlog performs a check if 'WordIdx' <= 'Words', but because 'Words' can
  // only be greater than 0, the check is not needed

  isEQS := 1

  loop
    if (*Lset & *Rset) != 0 then
      // Either set contains elements not found in the other
      isEQS := 0
      break
    fi

    Rset += 2
    Lset += 2

    if Words == WordIdx then
      break
    fi

    Words += 1
  pool
fi
```

-----
### EXP* - Exponentiation
| OP | Name        | Arg0 | Arg1 | Arg2 | Arg3 |
|----|-------------|------|------|------|------|
| 56 | EXPINTINT   |  -   |  -   |  -   |  -   |
| 57 | EXPREALINT  |  -   |  -   |  -   |  -   |
| 58 | EXPREALREAL |  -   |  -   |  -   |  -   |

#### Stack Arguments
| Offset      | Type  | Name     |
|-------------|-------|----------|
|           0 | exp   | exponent |
| sizeof exp  | bas   | base     |

#### Stack Returns
| Offset | Type | Name  |
|--------|------|-------|
|      0 | bas  | power |

#### Type Definition
| Instruction | bas   | exp   |
|-------------|-------|-------|
| EXPINTINT   | int4  | int4  |
| EXPREALINT  | real8 | int4  |
| EXPREALREAL | real8 | real8 |

#### Description
Performs exponentiation between ```base``` and ```exponent```, storing the
result inside ```power```.

#### Exceptions
EXPREALREAL, EXPREALINT, EXPINTINT
- Quit (excpZeroTakenToZeroPower): If both ```exponent``` and ```base``` are zero.

EXPINTINT
- Quit (excpZeroTakenToZeroPower): If both ```exponent``` and ```base``` are zero.
- Quit (excpIntegerTakenToNegativePower): If both ```exponent``` and
  ```base``` are negative.
- Quit (excpIntegerOverflow): If ```power``` is too large.

EXPREALINT
- Quit (excpZeroTakenToZeroPower): If both ```exponent``` and ```base``` are zero.
- Quit (excpRealOverflow): If ```power``` is too large.

EXPREALREAL
- Quit (excpZeroTakenToZeroPower): If both ```exponent``` and ```base``` are zero.
- Quit (excpRealOverflow): If ```power``` is too large.
- Quit (excpZeroTakenToNegativePower): If ```exponent``` is negative and
  ```base``` is zero.
- Quit (excpNegativeValueTakenToRealPower): If ```base``` is negative.

#### Operation
```
if !CheckBaseAndExponent(base, exponent) then
  Quit ( /* Approprate error code */ )
fi

power := base ** exponent

if !CheckPower(power) then
  Quit ( /* Approprate error code */ )
fi
```

----
### FETCH* - Fetch from Address
| OP | Name       | Arg0             | Arg1 | Arg2 | Arg3 |
|----|------------|------------------|------|------|------|
| 59 | FETCHADDR  |  -               |  -   |  -   |  -   |
| 5A | FETCHBOOL  |  -               |  -   |  -   |  -   |
| 5B | FETCHINT   |  -               |  -   |  -   |  -   |
| 5C | FETCHINT1  |  -               |  -   |  -   |  -   |
| 5D | FETCHINT2  |  -               |  -   |  -   |  -   |
| 5E | FETCHINT4  |  -               |  -   |  -   |  -   |
| 5F | FETCHNAT   |  -               |  -   |  -   |  -   |
| 60 | FETCHNAT1  |  -               |  -   |  -   |  -   |
| 61 | FETCHNAT2  |  -               |  -   |  -   |  -   |
| 62 | FETCHNAT4  |  -               |  -   |  -   |  -   |
| 63 | FETCHPTR   |  -               |  -   |  -   |  -   |
| 64 | FETCHREAL  |  -               |  -   |  -   |  -   |
| 65 | FETCHREAL4 |  -               |  -   |  -   |  -   |
| 66 | FETCHREAL8 |  -               |  -   |  -   |  -   |
| 67 | FETCHSET   | setSize (offset) |  -   |  -   |  -   |
| 68 | FETCHSTR   |  -               |  -   |  -   |  -   |

#### Stack Arguments
| Offset | Type    | Name |
|--------|---------|------|
|      0 | addrint | addr |

#### Stack Returns
| Offset | Type    | Name  |
|--------|---------|-------|
|      0 | addrint | value |

#### Type Definition
| Instruction | type    |
|-------------|---------|
| FETCHADDR   | addrint |
| FETCHBOOL   | int4    |
| FETCHINT    | int4    |
| FETCHINT1   | int1    |
| FETCHINT2   | int2    |
| FETCHINT4   | int4    |
| FETCHNAT    | nat4    |
| FETCHNAT1   | nat1    |
| FETCHNAT2   | nat2    |
| FETCHNAT4   | nat4    |
| FETCHPTR    | addrint |
| FETCHREAL   | real8   |
| FETCHREAL4  | real4   |
| FETCHREAL8  | real8   |
| FETCHSET    | nat4    |
| FETCHSTR    | addrint |

#### Description
Fetches the specified type from ```addr```, storing it in ```value```.

FETCHSET only grabs a portion of a set.

As FETCHSTR already points to the string, only validation is performed.

FETCHINT1, and FETCHINT2 promotes the value at ```addr``` to an int4.

FETCHNAT1, and FETCHNAT2 promotes the value at ```addr``` to n nat4.

FETCHREAL4 promotes the value at ```addr``` to a real8.

FETCHPTR dereferences the pointer descriptor.

#### Exceptions
FETCHADDR
- Abort (45) "Variable has no value":
  If the value at ```addr``` is equal to 0xFFFFFFFF

FETCHBOOL
- Abort (45) "Variable has no value":
  If the value at ```addr``` is equal to 0xFF

FETCHINT
- Abort (45) "Variable has no value":
  If the value at ```addr``` is equal to <b>undefint</b> (0x80000000)

FETCHNAT
- Abort (45) "Variable has no value":
  If the value at ```addr``` is equal to <b>undefnat</b> (0xFFFFFFFF)

FETCHPTR
- Abort (45) "Variable has no value":
  If the pointer descriptor at ```addr``` points to 0xFFFFFFFF
- ExecutionError ("Reference to previously freed pointer"):
  If the pointer descriptor was previously freed by FREE and the memory has
  been reallocated

FETCHREAL:
- Abort (45) "Variable has no value":
  If the value at ```addr``` is equal to 0x80000000_80000000

FETCHSET:
- Abort (45) "Variable has no value":
  If the value at ```addr``` is equal to the set's uninitialized
  state (See "Operation" for specific values)
- Abort (25): If ```setSize``` is not equal to 1, 2, or 4

FETCHSTR:
- Abort (45) "Variable has no value":
  If the first character at ```addr``` is equal to 0x80 (UninitStr)

#### Operation (Not FETCHSET)
```
if !CheckValues (addr) then
  /* ... Appropriate exceptions ... */
fi

value := *addr
```

#### Operation (FETCHSET)
```
if (setSize != 1 || setSize != 2 || setSize != 4) then
  Abort (25) // Case selector is out of range
fi

case (setSize) of
  label 1:
    if (nat1)*addr == 0x80 then
      Abort (45) // Variable has no value
    fi

    value := (nat1)*addr
  label 2:
    if (nat2)*addr == 0x8000 then
      Abort (45) // Variable has no value
    fi

    value := (nat2)*addr
  label 4:
    if (nat4)*addr == 0x80000000 then
      Abort (45) // Variable has no value
    fi

    value := (nat4)*addr
esac
```

-----
### FIELD - Get Field
| OP | Name  | Arg0              | Arg1 | Arg2 | Arg3 |
|----|-------|-------------------|------|------|------|
| 69 | FIELD | fieldOff (offset) |  -   |  -   |  -   |

#### Stack Arguments
| Offset | Type    | Name      |
|--------|---------|-----------|
|      0 | addrint | baseAddr  |

#### Stack Returns
| Offset | Type    | Name      |
|--------|---------|-----------|
|      0 | addrint | fieldAddr |

#### Description
Computes the address to access a specific field in a memory 
structure by adding ```fieldOff``` to the base ```baseAddr```, storing the 
address in ```fieldAddr```.

#### Exceptions
None

#### Operation
```
fieldAddr := fieldOff + baseAddr
```

-----
### FOR - Begin For Loop
| OP | Name | Arg0               | Arg1 | Arg2 | Arg3 |
|----|------|--------------------|------|------|------|
| 6A | FOR  | skipBlock (offset) |  -   |  -   |  -   |

#### Stack Arguments
| Offset | Type    | Name    |
|--------|---------|---------|
|      0 | addrint | forInfo |
|      4 | int4    | step    |
|      8 | int4    | end     |
|     12 | int4    | start   |

#### Stack Returns
None

#### Description
Begins the for-loop by initializing the for-loop descriptor at ```forInfo```.
The counter is initialized to ```start```, and is adjusted in steps of
```step```, until the counter is equal to ```end```.

The stack pointer is preserved in order to restore the stack state after the
for-loop is done.

If the for-loop bounds indicate that the for-loop has already completed, then
the for-loop block is skipped over

#### Exceptions
None

#### Operation
```
if (step > 0 && start <= end) || (step < 0 && start >= end)
  descriptor := PointerTo (ForDescriptor, forInfo)
  descriptor.counter := start
  descriptor.end     := end
  descriptor.step    := step
  descriptor.lastSP  := GlobalSP
else
  // Sub to include operand skip
  GlobalPC += skipBlock - sizeof offset
fi
```

-----
### FORK - Fork Process
| OP | Name  | Arg0       | Arg1         | Arg2            | Arg3 |
|----|-------|------------|--------------|-----------------|------|
| 6B | FORK  | oPatchLink | aProcessName | oParamBlockSize |  -   |

#### Stack Arguments
| Offset | Type | Name |
|--------|------|------|
|      0 | type | name |

#### Stack Returns
| Offset | Type | Name |
|--------|------|------|
|      0 | type | name |

#### Description

-----
### FREE - Free Heap Object
| OP | Name  | Arg0     | Arg1 | Arg2 | Arg3 |
|----|-------|----------|------|------|------|
| 6C | FREE  | oMonitor |  -   |  -   |  -   |

#### Stack Arguments
| Offset | Type    | Name    |
|--------|---------|---------|
|      0 | addrint | pointer |

#### Stack Returns
None

#### Description
Frees the given heap object pointed to by ```pointer```.

If the heap object has an associated MonitorDescriptor, ```oMonitor```
is non-zero and is an offset to the MonitorDescriptor field. Otherwise,
```oMonitor``` is zero.

#### Exceptions
- ExecutionError ("Nil pointer passed to 'free'"):
  If the address pointed to by ```pointer``` is <b>nil</b> (zero or NULL).

-----
### FREECLASS - Free Class Instance
| OP | Name      | Arg0     | Arg1 | Arg2 | Arg3 |
|----|-----------|----------|------|------|------|
| 6D | FREECLASS | oMonitor |  -   |  -   |  -   |

#### Stack Arguments
| Offset | Type    | Name    |
|--------|---------|---------|
|      0 | addrint | pointer |

#### Stack Returns
None

#### Description
Frees the given class instance pointed to by ```pointer```.

If the class instance has an associated MonitorDescriptor, ```oMonitor```
is non-zero and is an offset to the MonitorDescriptor field. Otherwise,
```oMonitor``` is zero.

#### Exceptions
- ExecutionError ("Nil pointer passed to 'free'"):
  If the address pointed to by ```pointer``` is <b>nil</b> (zero or NULL).

-----
### FREEU - Free Raw Memory
| OP | Name  | Arg0 | Arg1 | Arg2 | Arg3 |
|----|-------|------|------|------|------|
| 6E | FREEU |  -   |  -   |  -   |  -   |

#### Stack Arguments
| Offset | Type    | Name    |
|--------|---------|---------|
|      0 | addrint | pointer |

#### Stack Returns
None

#### Description
Frees the given raw memory area pointed to by ```pointer```.

#### Exceptions
- ExecutionError ("Nil pointer passed to 'free'"):
  If the address pointed to by ```pointer``` is <b>nil</b> (zero or NULL).

-----
### GECLASS - Is Same or Descendent Class
| OP | Name    | Arg0 | Arg1 | Arg2 | Arg3 |
|----|---------|------|------|------|------|
| 70 | GECLASS |  -   |  -   |  -   |  -   |

#### Stack Arguments
| Offset | Type    | Name      |
|--------|---------|-----------|
|      0 | addrint | reference |
|      4 | addrint | clazz     |

#### Stack Returns
| Offset | Type    | Name         |
|--------|---------|--------------|
|      0 | int4    | isDescendent |

#### Description
Checks if ```clazz``` is a descendant of or the same class as ```reference```.
If true, ```isDescendent``` is set to 1.
A class is considered to be a descendant of another class only when the class
inherits the other class through a number of other classes.

<b>nil</b> is considered to be a descendant of all classes, as it has no class
that describes it.

#### Exceptions
None

#### Operation
```
if clazz != 0 then
  Inherits := clazz

  // Walk up the inherit chain
  while (Inherits != reference)
    Inherits := PointerTo (ClassDescriptor, Inherits).inherits

    if Inherits == 0 then
      // End of chain reached, does not inherit
      break
    fi
  elihw

  if Inherits == reference then
    isDescendent := 1
  else
    isDescendent := 0
  fi
else
  // Is nil, checks out
  isDescendent := 1
fi
```

-----
### GE* - Are Values Greater Than Or Equal
| OP | Name     | Arg0       | Arg1 | Arg2 | Arg3 |
|----|----------|------------|------|------|------|
| 6F | GECHARN  | len (int4) |  -   |  -   |  -   |
| 71 | GEINT    |  -         |  -   |  -   |  -   |
| 72 | GEINTNAT |  -         |  -   |  -   |  -   |
| 73 | GENAT    |  -         |  -   |  -   |  -   |
| 74 | GENATINT |  -         |  -   |  -   |  -   |
| 75 | GEREAL   |  -         |  -   |  -   |  -   |
| 77 | GESTR    |  -         |  -   |  -   |  -   |

#### GECHARN
##### Stack Arguments
| Offset      | Type | Name   |
|-------------|------|--------|
|           0 | rval | rvalue |
| sizeof rval | lval | lvalue |

##### Stack Returns
| Offset      | Type | Name   |
|-------------|------|--------|
|           0 | rval | rvalue |
| sizeof rval | lval | lvalue |
| sizeof lval | int4 | isGE   |

#### Generic
##### Stack Arguments
| Offset      | Type | Name   |
|-------------|------|--------|
|           0 | rval | rvalue |
| sizeof rval | lval | lvalue |

##### Stack Returns
| Offset | Type | Name |
|--------|------|------|
|      0 | int4 | isGE |

#### Type Definition
| Instruction | lval    | rval    |
|-------------|---------|---------|
| GECHARN     | addrint | addrint |
| GEINT       | int4    | int4    |
| GEINTNAT    | nat4    | int4    |
| GENAT       | nat4    | nat4    |
| GENATINT    | int4    | nat4    |
| GEREAL      | real8   | real8   |
| GESTR       | addrint | addrint |

#### Description
Performs the "greater-than or equal-to" comparison between ```lvalue``` and
```rvalue```.

If the condition is satisfied, ```isGE``` is set to 'true' (1).
Otherwise, ```isGE``` is set to 'false' (0).

For GEINTNAT, if ```rvalue``` is negative, ```isGE``` is always 'false'.

For GENATINT, if ```lvalue``` is negative, ```isGE``` is always 'true'.

#### Exceptions
None

#### Operation (Not GESTR, GECHRN, or GESET)
```
if lvalue >= rvalue then
  isGE := 1
else
  isGE := 0
fi
```

#### Operation (GESTR)
```
if StrCmp (lvalue, rvalue) == 1 then
  isGE := 1
else
  isGE := 0
fi
```

#### Operation (GECHARN)
```
// Ignore zeros in the Char(n)'s
if StrnCmp (lvalue, rvalue, len) == 1 then
  isGE := 1
else
  isGE := 0
fi
```

-----
### GESET - Test if Superset
| OP | Name  | Arg0           | Arg1 | Arg2 | Arg3 |
|----|-------|----------------|------|------|------|
| 76 | GESET | setSize (nat4) |  -   |  -   |  -   |

#### Stack Arguments
##### Form 1
| Offset | Type    | Name   |
|--------|---------|--------|
|      0 | set32   | rvalue |
|      4 | set32   | lvalue |

##### Form 2
| Offset | Type    | Name   |
|--------|---------|--------|
|      0 | addrint | rvalue |
|      4 | addrint | lvalue |


#### Stack Returns
| Offset | Type    | Name   |
|--------|---------|--------|
|      0 | int4    | isSupS |

#### Description
Tests if ```lvalue``` is a superset of ```rvalue```.
A set (<i>A</i>) is defined to be a superset of another set (<i>B</i>) if
<i>B</i> only contains elements found in <i>A</i>.

Sets in Turing are represented in two ways
 - As a single set32 / nat4
 - As an array of set16 / nat2's

Due to these two representations, two forms of parameter input are required to
test each set appropriately.

```setSize``` discriminates between these two types, as it determines the
length of the set in bytes.

If ```setSize``` less than or equal to 4 bytes, parameter 'Form 1' is used and
the test between ```lvalue``` and ```rvalue``` is performed for the first set32.

Otherwise, 'Form 2' is used, and the superset test is performed between all set
words of ```lvalue``` and ```rvalue```.

In both cases, the test result is stored in ```isSupS```

```setSize``` applies to the length of both sets, as merging two sets of
different length is not supported in Turing.

#### Exceptions
None

#### Operation
```
if setSize <= 4 then
  // Perform superset test between single word sets
  // ~A & B
  isSubS := ((~lvalue) & rvalue) == 0
else
  // Perform word-by-word superset of sets
  Rset := PointerTo (set16, rvalue)
  Lset := PointerTo (set16, lvalue)

  Words := setSize >> 1 // Get set size in words
  WordIdx := 1

  // TProlog performs a check if 'WordIdx' <= 'Words', but because 'Words' can
  // only be greater than 0, the check is not needed

  isSupS := 1

  loop
    if ((~*Lset) & (*Rset)) != 0 then
      // Lset contains elements not in Rset
      isSupS := 0
      break
    fi

    Rset += 2
    Lset += 2

    if Words == WordIdx then
      break
    fi

    Words += 1
  pool
fi
```

-----
### GET - Get
| OP | Name  | Arg0 | Arg1 | Arg2 | Arg3 |
|----|-------|------|------|------|------|
| 78 | GET   |  -   |  -   |  -   |  -   |

#### Stack Arguments
| Offset | Type | Name |
|--------|------|------|
|      0 | type | name |

#### Stack Returns
| Offset | Type | Name |
|--------|------|------|
|      0 | type | name |

#### Description

-----
### GETPRIORITY - Get Process Priority
| OP | Name        | Arg0 | Arg1 | Arg2 | Arg3 |
|----|-------------|------|------|------|------|
| 79 | GETPRIORITY |  -   |  -   |  -   |  -   |

#### Stack Arguments
None

#### Stack Returns
| Offset | Type    | Name     |
|--------|---------|----------|
|      0 |    nat2 | priority |

#### Description
Returns the current process's priority in ```priority```

#### Exceptions
None

#### Operation
```
priority := GetPriority(CurrentProcess, priority)
```

-----
### GTCLASS - Is Descendant Class Only
| OP | Name    | Arg0 | Arg1 | Arg2 | Arg3 |
|----|---------|------|------|------|------|
| 7A | GTCLASS |  -   |  -   |  -   |  -   |

#### Stack Arguments
| Offset | Type    | Name      |
|--------|---------|-----------|
|      0 | addrint | reference |
|      4 | addrint | clazz     |

#### Stack Returns
| Offset | Type    | Name         |
|--------|---------|--------------|
|      0 | int4    | isDescendent |

#### Description
Checks if ```clazz``` is a descendant of ```reference```.
If true, ```isDescendent``` is set to 1
A class is considered to be a descendant of another class only when the class
inherits the other class through a number of other classes.

<b>nil</b> is considered to be a descendat of all classes, as it has no class
that describes it.

#### Exceptions
None

#### Operation
```
if clazz != 0 then
  Inherits := clazz

  // Walk up the inherit chain
  do
    Inherits := PointerTo (ClassDescriptor, Inherits).inherits

    if Inherits == 0 then
      // End of chain reached, does not inherit
      break
    fi
  while (Inherits != reference)

  if Inherits == reference then
    isDescendent := 1
  else
    isDescendent := 0
  fi
else
  // Is nil, checks out
  isDescendent := 1
fi
```

-----
### IF - Begin If
| OP | Name  | Arg0            | Arg1 | Arg2 | Arg3 |
|----|-------|-----------------|------|------|------|
| 7B | IF    | skipTo (offset) |  -   |  -   |  -   |

#### Stack Arguments
| Offset | Type | Name |
|--------|------|------|
|      0 | int4 | test |

#### Stack Returns
None

#### Description
Checks the value of ```test``` and determines whether to jump to ```skipTo``` or
not.
If ```test``` is zero, then the jump to ```skipTo``` is taken.
Otherwise, the instruction is skipped, and advances onto the next instruction.

In all cases, ```test``` is popped off of the stack.

#### Exceptions
None

#### Operation
```
if test == 0 then
  // Skip positive branch
  // Subtract by "sizeof offset" to account for skipping over the operands
  GlobalPC += skipTo - sizeof offset
fi

// Take current branch
```

-----
### IN - Check If Element In Set
| OP | Name  | Arg0           | Arg1      | Arg2      | Arg3 |
|----|-------|----------------|-----------|-----------|------|
| 7C | IN    | setSize (nat4) | lo (int4) | hi (int4) |  -   |

#### Stack Arguments
##### Form 1
| Offset | Type    | Name    |
|--------|---------|---------|
|      0 | set32   | set     |
|      4 | set32   | element |

##### Form 2
| Offset | Type    | Name    |
|--------|---------|---------|
|      0 | addrint | set     |
|      4 | addrint | element |

#### Stack Returns
| Offset | Type    | Name    |
|--------|---------|---------|
|      0 | int4    | isInSet |

#### Description
Tests if ```element``` is contained within ```set```.

Sets in Turing are represented in two ways
 - As a single set32 / nat4
 - As an array of set16 / nat2's

Due to these two representations, two forms of parameter input are required to
test the set appropriately.

```setSize``` discriminates between these two types, as it determines the
length of the set in bytes.

If ```setSize``` less than or equal to 4 bytes, parameter 'Form 1' is used and
the containment test between ```element``` and ```set``` is performed for the
first set32.

Otherwise, 'Form 2' is used, and the containment test is performed at the
```set```'s corresponding word index for ```element```.

In both cases, the test result is stored in ```isInSet```

#### Exceptions
- Abort (29) "Set element is out of range":
  If ```element``` is outside of the bounds of ```lo``` .. ```hi```.

#### Operation
```
if element < lo || element > hi then
  Abort (29) // Set element is out of range
fi 

// Bit offset
BitSel := (element - lo) & 0x1F
// Byte offset
WordSel := (element - lo) >> 4

if setSize <= 4 then
  // Perform containment in the single word set
  BitTest := (1 << BitSel)
  isInSet := (set & BitTest) != 0
else
  // Access the specific word and test the set there
  Lset := PointerTo (set16, set + WordSel)
  isInSet := (set & BitTest) != 0
fi
```

-----
### INCLINENO - Increment Line Number
| OP | Name      | Arg0 | Arg1 | Arg2 | Arg3 |
|----|-----------|------|------|------|------|
| 7D | INCLINENO |  -   |  -   |  -   |  -   |

#### Stack Arguments
None

#### Stack Returns
None

#### Description
Increments the current line number by one.

**TProlog Specific**
If the current process is in stepping mode (```stepSpec``` != 0) and the 
stepping frame pointer is NULL (```stepFP``` == 0), then the current state is 
changed to "NextLine" and the current cycle is finished.

#### Exceptions
None

#### Operation
```
LineNo += 1
```

-----
### INCSP - Increment Stack Pointer
| OP | Name  | Arg0         | Arg1 | Arg2 | Arg3 |
|----|-------|--------------|------|------|------|
| 7E | INCSP | amount (off) |  -   |  -   |  -   |

#### Stack Arguments
None

#### Stack Returns
None

#### Description
Increments the stack pointer by ```amount``` bytes. Frees up the stack space 
used by local variables.

#### Exceptions
None

#### Operation
```
GlobalSP += amount
```

-----
### INFIXAND - Test AND condition
| OP | Name     | Arg0            | Arg1 | Arg2 | Arg3 |
|----|----------|-----------------|------|------|------|
| 7F | INFIXAND | skipTo (offset) |  -   |  -   |  -   |

#### Stack Arguments
| Offset | Type | Name |
|--------|------|------|
|      0 | int4 | test |

#### Stack Returns
None

#### Description
Tests the previous <b>and</b> condition result given in ```test```, and chooses
whether to jump or not.

If ```test``` is equal to zero, the jump to ```skipTo``` is taken, and ```test``` is kept on 
the stack.
Otherwise, the instruction is skipped, and ```test``` is popped off.

This instruction operates in a similar manner to the IF instruction

#### Exceptions
None

#### Operation
```
if test == 0 then
  // Subtract to account for already skipping over operand
  GlobalPC += skipTo - sizeof offset
fi
```

-----
### INFIXOR - Test OR condition
| OP | Name    | Arg0         | Arg1 | Arg2 | Arg3 |
|----|---------|--------------|------|------|------|
| 80 | INFIXOR | skipTo (off) |  -   |  -   |  -   |

#### Stack Arguments
| Offset | Type | Name |
|--------|------|------|
|      0 | int4 | test |

#### Stack Returns
None

#### Description
Tests the previous <b>or</b> condition result given in ```test```, and chooses
whether to jump or not.

If ```test``` is not equal to zero, the jump to ```skipTo``` is taken, and 
```test``` is still kept on the stack.
Otherwise, the instruction is skipped, and ```test``` is popped off the stack.

#### Exceptions
None

#### Operation
```
if test != 0 then
  // Subtract to account for already skipping over operand
  GlobalPC += skipTo - sizeof offset
fi
```

-----
### INITARRAYDESC - Initialize Array Descriptor
| OP | Name          | Arg0 | Arg1 | Arg2 | Arg3 |
|----|---------------|------|------|------|------|
| 81 | INITARRAYDESC |  -   |  -   |  -   |  -   |

#### Stack Arguments
| Offset | Type    | Name       |
|--------|---------|------------|
|      0 | addrint | descriptor |
|      4 | int4    | elemSize   |
|      8 | int4    | dimCount   |
|      8 | addrint | loBoundEnd |
| 16 ... | addrint | hiBounds   |

#### Stack Returns
None

#### Description
Initializes the given array descriptor at ```descriptor``` using the values on 
the stack.

```elemSize``` is the size of an individual element, in bytes.

```dimCount``` is the number of dimensions inside of the array.

```loBoundEnd``` points to a sequential list of integers that correspond to the
lower bounds for each dimension. Iterated from the deepest dimension to the
shallowest dimension's bounds.

```hiBounds``` is a list of integers on the stack that correspond to the upper
bounds for each dimension. The ordering of the bounds in ```hiBounds``` begins
from the deepest dimension to the shallowest dimension.

The shallowest dimension corresponds to the first dimension range (or only
dimension range for a single-dimension array) for the array, while the deepest
dimension corresponds to the inner-most dimension.

```elemCount``` is computed using each dimensions upper 
and lower bounds, and ```arraySize``` is computed using the bounds for each 
dimension and ```elemSize```.

#### Exceptions
- ExecutionError ("Index range for dynamic array is too large"):
  If the corresponding upper and lower bounds for a dimension define a range
  too large to fit inside of a positive integer

- ExecutionError ("Index range for dynamic array is negative"):
  If the upper bound is smaller than the corresponding lower bound.

-----
### INITCONDITION - Initialize Condition
| OP | Name          | Arg0       | Arg1  | Arg2 | Arg3 |
|----|---------------|------------|-------|------|------|
| 82 | INITCONDITION | oPatchlink | aName |  -   |  -   |

#### Stack Arguments
| Offset | Type    | Name              |
|--------|---------|-------------------|
|      0 | addrint | conditionVar      |
|      4 | addrint | monitorDescriptor |
|      8 | nat4    | numConditions     |
|     12 | addrint | conditionQueues   |

#### Stack Returns
None

#### Description
Initializes the given ```conditionVar```.

The given ```conditionVar``` is linked to the given ```monitorDescriptor``` and
added to the ```monitorDescriptor```'s list of condition variables.

If the condition variable is an array of conditions, ```numConditions``` is
the number of elements the condition array has, and ```conditionQueues```
points to the location of each condition's queues. All of the conditions
share the same condition type.
Otherwise, ```numConditions``` is always one, and ```conditionQueues``` points
to a single queue.

#### Exceptions
None

-----
### INITMONITOR - Initialize Monitor
| OP | Name        | Arg0       | Arg1  | Arg2 | Arg3 |
|----|-------------|------------|-------|------|------|
| 83 | INITMONITOR | oPatchlink | aName |  -   |  -   |

#### Stack Arguments
| Offset | Type    | Name              |
|--------|---------|-------------------|
|      0 | addrint | monitorDescriptor |

#### Stack Returns
None

#### Description
Initializes the given ```monitorDescriptor``` to the default values.

#### Exceptions
None

-----
### INITUNIT - Initialize Unit
| OP | Name     | Arg0          | Arg1                 | Arg2              | Arg3             |
|----|----------|---------------|----------------------|-------------------|------------------|
| 84 | INITUNIT | link (offset) | markerAddr (addrint) | initSkip (offset) | markerVal (int1) |

#### Stack Arguments
None

#### Stack Returns
None

#### Description
Begins the initialization of a unit.

First, the value at ```markerAddr``` is checked. If the value is nonzero,
initialization is skipped.

Otherwise, GlobalPC is advanced by ```initSkip``` bytes, and ```markerVal```
is written to ```markerAddr```.

#### Exceptions
None

#### Operation

````
if *markerAddr == 0 then
  // -8 is added to accomidate already skipping over the operands
  GlobalPC += initSkip - 8
  *markerAddr := markerVal
else
  // Unit is alread initialized
fi
````

-----
### INTREAL* - Convert Integer into Real
| OP | Name        | Arg0 | Arg1 | Arg2 | Arg3 |
|----|-------------|------|------|------|------|
| 85 | INTREAL     |  -   |  -   |  -   |  -   |
| 86 | INTREALLEFT |  -   |  -   |  -   |  -   |

#### Stack Arguments (INTREAL)
| Offset | Type  | Name     |
|--------|-------|----------|
|      0 | int4  | value    |

#### Stack Returns (INTREAL)
| Offset | Type  | Name     |
|--------|-------|----------|
|      0 | real8 | result   |

#### Stack Arguments (INTREALLEFT)
| Offset | Type  | Name     |
|--------|-------|----------|
|      0 | real8 | preserve |
|      8 | int4  | value    |

#### Stack Returns (INTREALLEFT)
| Offset | Type  | Name     |
|--------|-------|----------|
|      0 | real8 | preserve |
|      8 | real8 | result   |

#### Description
Converts ```value``` into an real8, storing the result in  ```result```. Also 
allocates enough space to accomodate the real8.

#### Exceptions
None

#### Operation
```
result := (real8)value
```

-----
### INTSTR - Convert Integer into String
| OP | Name   | Arg0 | Arg1 | Arg2 | Arg3 |
|----|--------|------|------|------|------|
| 87 | INTSTR |  -   |  -   |  -   |  -   |

#### Stack Arguments
| Offset | Type    | Name    |
|--------|---------|---------|
|      0 | addrint | destStr |
|      4 | int4    |  base   |
|      8 | int4    |  width  |
|     12 | int4    |  value  |

#### Stack Returns
| Offset | Type    | Name    |
|--------|---------|---------|
|      0 | addrint | destStr |

#### Description
Converts ```value``` into a string using the specified conversion arguments 
(```width``` and ```base```), and stores the string at ```destStr```.

```width``` specifies the minimum number of characters in the string. If there 
are less digits than what is specified in ```width```, it is padded with spaces.

```base``` specifies the integer base to use during conversion.

#### Exceptions
- Quit (excpNegativeFieldWidthSpecified): If ```width``` is negative.
- Quit (excpResultStringTooBig): If the string generated in the conversion
  exceeds the length of MAX_STRING_LEN.
- Quit (excpIllegalStringConversionBase): If ```base``` is outside of the range
  2 .. 36 (inclusive).

#### Operation
```
// TODO: INTSTR
```

-----
### JSR - Jump Subroutine
| OP | Name | Arg0          | Arg1 | Arg2 | Arg3 |
|----|------|---------------|------|------|------|
| 88 | JSR  | off (addrint) |  -   |  -   |  -   |

#### Stack Arguments
None

#### Stack Returns
| Offset | Type    | Name  |
|--------|---------|-------|
|      0 | addrint | retPC |

#### Description
Stores the current PC plus 4 bytes in ```retPC```, and moves the PC back by ```off``` bytes. 
The addition of 4 bytes to the saved PC is done so that ```off``` isn't executed as an 
instruction.

#### Exceptions
None

#### Operation
```
Push (addrint, GlobalPC + 4, retPC)
GlobalPC -= off
```

-----
### JUMP - Relative Jump Forward
| OP | Name  | Arg0          | Arg1 | Arg2 | Arg3 |
|----|-------|---------------|------|------|------|
| 89 | JUMP  | off (addrint) |  -   |  -   |  -   |

#### Stack Arguments
None

#### Stack Returns
None

#### Description
Advances the PC by ```off``` bytes. Includes skipping over the ```off``` operand.

#### Exceptions
None

#### Operation
```
GlobalPC += off
```

-----
### JUMPB - Relative Jump Back
| OP | Name  | Arg0          | Arg1 | Arg2 | Arg3 |
|----|-------|---------------|------|------|------|
| 8A | JUMPB | off (addrint) |  -   |  -   |  -   |

#### Stack Arguments
None

#### Stack Returns
None

#### Description
Moves the PC back by ```off``` bytes. PC is relative to the address after the
opcode.

#### Exceptions
None

#### Operation

```
GlobalPC -= off
```

-----
### LECLASS - Is Same or Ancestor Class
| OP | Name    | Arg0 | Arg1 | Arg2 | Arg3 |
|----|---------|------|------|------|------|
| 8C | LECLASS |  -   |  -   |  -   |  -   |

#### Stack Arguments
| Offset | Type    | Name      |
|--------|---------|-----------|
|      0 | addrint | reference |
|      4 | addrint | clazz     |

#### Stack Returns
| Offset | Type    | Name       |
|--------|---------|------------|
|      0 | int4    | isAncestor |

#### Description
Checks if ```clazz``` is an ancestor of or the same class as ```reference```.
If true, ```isAncestor``` is set to 1.
A class is considered to be an ancestor of another class only when the other class
inherits the ancestor class through a number of other classes.

<b>nil</b> is considered to be an ancestor of all classes, as it has no class
that describes it.

#### Exceptions
None

#### Operation
```
if reference != 0 then
  Inherits := reference

  // Walk up the inherit chain
  while (Inherits != clazz)
    Inherits := PointerTo (ClassDescriptor, Inherits).inherits

    if Inherits == 0 then
      // End of chain reached, does not inherit
      break
    fi
  elihw

  if Inherits == clazz then
    isAncestor := 1
  else
    isAncestor := 0
  fi
else
  // Is nil, checks out
  isAncestor := 1
fi
```

-----
### LE* - Are Values Less Than or Equal To
| OP | Name     | Arg0       | Arg1 | Arg2 | Arg3 |
|----|----------|------------|------|------|------|
| 8B | LECHARN  | len (int4) |  -   |  -   |  -   |
| 8D | LEINT    |  -         |  -   |  -   |  -   |
| 8E | LEINTNAT |  -         |  -   |  -   |  -   |
| 8F | LENAT    |  -         |  -   |  -   |  -   |
| 90 | LENATINT |  -         |  -   |  -   |  -   |
| 91 | LEREAL   |  -         |  -   |  -   |  -   |
| 93 | LESTR    |  -         |  -   |  -   |  -   |

#### LECHARN
##### Stack Arguments
| Offset      | Type | Name   |
|-------------|------|--------|
|           0 | rval | rvalue |
| sizeof rval | lval | lvalue |

##### Stack Returns
| Offset      | Type | Name   |
|-------------|------|--------|
|           0 | rval | rvalue |
| sizeof rval | lval | lvalue |
| sizeof lval | int4 | isLE   |

#### Generic
##### Stack Arguments
| Offset      | Type | Name   |
|-------------|------|--------|
|           0 | rval | rvalue |
| sizeof rval | lval | lvalue |

##### Stack Returns
| Offset | Type | Name |
|--------|------|------|
|      0 | int4 | isLE |

#### Type Definition
| Instruction | lval    | rval    |
|-------------|---------|---------|
| LECHARN     | addrint | addrint |
| LEINT       | int4    | int4    |
| LEINTNAT    | int4    | nat4    |
| LENAT       | nat4    | nat4    |
| LENATINT    | nat4    | int4    |
| LEREAL      | real8   | real8   |
| LESTR       | addrint | addrint |

#### Description
Performs the "less-than or equal-to" comparison between ```lvalue``` and
```rvalue```.

If the condition is satisfied, ```isLE``` is set to 'true' (1).
Otherwise, ```isLE``` is set to 'false' (0).

For LEINTNAT, if ```lvalue``` is negative, ```isLE``` is always 'true'.

For LENATINT, if ```rvalue``` is negative, ```isLE``` is always 'false'.

#### Exceptions
None

#### Operation (Not LESTR, LECHARN)
```
if lvalue <= rvalue then
  isLE := 1
else
  isLE := 0
fi
```

#### Operation (LESTR)
```
if StrCmp (lvalue, rvalue) == -1 then
  isLE := 1
else
  isLE := 0
fi
```

#### Operation (LECHARN)
```
// Ignore zeros in the Char(n)'s
if StrnCmp (lvalue, rvalue, len) == -1 then
  isLE := 1
else
  isLE := 0
fi
```

-----
### LESET - Test if Subset
| OP | Name  | Arg0           | Arg1 | Arg2 | Arg3 |
|----|-------|----------------|------|------|------|
| 92 | LESET | setSize (nat4) |  -   |  -   |  -   |

#### Stack Arguments
##### Form 1
| Offset | Type    | Name   |
|--------|---------|--------|
|      0 | set32   | rvalue |
|      4 | set32   | lvalue |

##### Form 2
| Offset | Type    | Name   |
|--------|---------|--------|
|      0 | addrint | rvalue |
|      4 | addrint | lvalue |


#### Stack Returns
| Offset | Type    | Name   |
|--------|---------|--------|
|      0 | int4    | isSubS |

#### Description
Tests if ```lvalue``` is a subset of ```rvalue```.
A set (<i>A</i>) is defined to be a subset of another set (<i>B</i>) if
<i>A</i> only contains elements found in <i>B</i>.

Sets in Turing are represented in two ways
 - As a single set32 / nat4
 - As an array of set16 / nat2's

Due to these two representations, two forms of parameter input are required to
test each set appropriately.

```setSize``` discriminates between these two types, as it determines the
length of the set in bytes.

If ```setSize``` less than or equal to 4 bytes, parameter 'Form 1' is used and
the test between ```lvalue``` and ```rvalue``` is performed for the first set32.

Otherwise, 'Form 2' is used, and the subset test is performed between all set
words of ```lvalue``` and ```rvalue```.

In both cases, the test result is stored in ```isSubS```

```setSize``` applies to the length of both sets, as merging two sets of
different length is not supported in Turing.

#### Exceptions
None

#### Operation
```
if setSize <= 4 then
  // Perform subset test between single word sets
  // A & ~B
  isSubS := (lvalue & (~rvalue)) == 0
else
  // Perform word-by-word subset of sets
  Rset := PointerTo (set16, rvalue)
  Lset := PointerTo (set16, lvalue)

  Words := setSize >> 1 // Get set size in words
  WordIdx := 1

  // TProlog performs a check if 'WordIdx' <= 'Words', but because 'Words' can
  // only be greater than 0, the check is not needed

  isSubS := 1

  loop
    if (*Lset & (~*Rset)) != 0 then
      // Lset contains elements not in Rset
      isSubS := 0
      break
    fi

    Rset += 2
    Lset += 2

    if Words == WordIdx then
      break
    fi

    Words += 1
  pool
fi
```

-----
### LOCATE* - Locate On Stack

| OP | Name        | Arg0                | Arg1             | Arg2 | Arg3 |
|----|-------------|---------------------|------------------|------|------|
| 94 | LOCATEARG   | argOff (offset)     |  -               |  -   |  -   |
| 95 | LOCATECLASS | classOff (offset)   |  -               |  -   |  -   |
| 96 | LOCATELOCAL | localOff (offset)   |  -               |  -   |  -   |
| 97 | LOCATEPARM  | paramOff (offset)   |  -               |  -   |  -   |
| 98 | LOCATETEMP  | localsArea (offset) | tempOff (offset) |  -   |  -   |

#### Stack Arguments
None

#### Stack Returns
| Offset | Type    | Name   |
|--------|---------|--------|
|      0 | addrint | addrTo |

#### Description
Locates the specified type on the stack, storing the stack address in
```addrTo```.

#### Exceptions
None

#### Operation (LOCATEARG)
```
Temp := GlobalSP + argOff
Push (Temp)
```

#### Operation (LOCATECLASS)
```
// Class Descriptor pointer was passed in as a part of the procedure call
Temp := PeekAt (addrint, GlobalFP + 12) + classOff
Push (Temp)
```

#### Operation (LOCATELOCAL)
```
Temp := (CurrentFP - localOff)
Push (Temp)
```

#### Operation (LOCATEPARM)
```
Temp := (CurrentFP + 12) + paramOff
Push (Temp)
```

#### Operation (LOCATETEMP)
```
Temp := (CurrentFP - localsArea) + tempOff
Push (Temp)
```

-----
### LTCLASS - Is Ancestor Class Only
| OP | Name    | Arg0 | Arg1 | Arg2 | Arg3 |
|----|---------|------|------|------|------|
| 99 | LTCLASS |  -   |  -   |  -   |  -   |

#### Stack Arguments
| Offset | Type    | Name      |
|--------|---------|-----------|
|      0 | addrint | reference |
|      4 | addrint | clazz     |

#### Stack Returns
| Offset | Type    | Name       |
|--------|---------|------------|
|      0 | int4    | isAncestor |

#### Description
Checks if ```clazz``` is an ancestor of ```reference```.
If true, ```isAncestor``` is set to 1
A class is considered to be an ancestor of another class only when the other class
inherits the ancestor class through a number of other classes.

<b>nil</b> is considered to be an ancestor of all classes, as it has no class
that describes it.

#### Exceptions
None

#### Operation
```
if reference != 0 then
  Inherits := reference

  // Walk up the inherit chain
  do
    Inherits := PointerTo (ClassDescriptor, Inherits).inherits

    if Inherits == 0 then
      // End of chain reached, does not inherit
      break
    fi
  while (Inherits != clazz)

  if Inherits == clazz then
    isAncestor := 1
  else
    isAncestor := 0
  fi
else
  // Is nil, checks out
  isAncestor := 1
fi
```

-----
### MAX* - Maximum of Values
| OP | Name    | Arg0 | Arg1 | Arg2 | Arg3 |
|----|---------|------|------|------|------|
| 9A | MAXINT  |  -   |  -   |  -   |  -   |
| 9B | MAXNAT  |  -   |  -   |  -   |  -   |
| 9C | MAXREAL |  -   |  -   |  -   |  -   |

#### Stack Arguments
| Offset             | Type        | Name   |
|--------------------|-------------|--------|
|                  0 | <i>type</i> | lvalue |
| sizeof <i>type</i> | <i>type</i> | rvalue |

#### Stack Returns
| Offset | Type        | Name |
|--------|-------------|------|
|      0 | <i>type</i> | max  |

#### Type Definition
| Instruction | <i>type</i> |
|-------------|-------------|
| MAXINT      | int4        |
| MAXNAT      | nat4        |
| MAXREAL     | real8       |

#### Description
Stores the largest value between the two input values into ```max```.

#### Exceptions
None

#### Operation
```
if lvalue >= rvalue then
  max := lvalue
else
  max := rvalue
fi
```

-----
### MIN* - Minimum of Values
| OP | Name    | Arg0 | Arg1 | Arg2 | Arg3 |
|----|---------|------|------|------|------|
| 9D | MININT  |  -   |  -   |  -   |  -   |
| 9E | MINNAT  |  -   |  -   |  -   |  -   |
| 9F | MINREAL |  -   |  -   |  -   |  -   |

#### Stack Arguments
| Offset             | Type        | Name   |
|--------------------|-------------|--------|
|                  0 | <i>type</i> | lvalue |
| sizeof <i>type</i> | <i>type</i> | rvalue |

#### Stack Returns
| Offset | Type        | Name |
|--------|-------------|------|
|      0 | <i>type</i> | min  |

#### Type Definition
| Instruction | <i>type</i> |
|-------------|-------------|
| MININT      | int4        |
| MINNAT      | nat4        |
| MINREAL     | real8       |

#### Description
Stores the smallest value between the two input values into ```min```.

#### Exceptions
None

#### Operation
```
if lvalue <= rvalue then
  max := lvalue
else
  max := rvalue
fi
```

-----
### MODINT - Modulus of Integers
| OP | Name   | Arg0 | Arg1 | Arg2 | Arg3 |
|----|--------|------|------|------|------|
| A0 | MODINT |  -   |  -   |  -   |  -   |

#### Stack Arguments
| Offset | Type  | Name     |
|--------|-------|----------|
|      0 |  int4 | divisor  |
|      4 |  int4 | dividend |

#### Stack Returns
| Offset | Type  | Name      |
|--------|-------|-----------|
|      0 |  int4 | remainder |

#### Description
Stores the result of ```dividend``` <b>mod</b> ```divisor``` into
```remainder```. The <b>mod</b> operator uses truncating division.

In order to properly handle signs, ```intParts``` is reduced by one when
the signs between the divisor and dividend are different in order to compute
the correct value.

#### Exceptions
- Abort(33) "Division (or Modulus) by zero in Integer Expression":
  If ```divisor``` is zero.

#### Operation
```
if divisor == 0 then
  Abort (33) // "Division (or Modulus) by zero in Integer Expression"
fi

intParts := (dividend / divisor)

if (intParts * divisor) != dividend then
  if sign (dividend) != sign (divisor) then
    // Adjust intParts
    intParts -= 1
  fi

  remainder := divisor - intParts
else
  remainder := 0
fi
```

-----
### MODNAT - Modulus of Naturals
| OP | Name   | Arg0 | Arg1 | Arg2 | Arg3 |
|----|--------|------|------|------|------|
| A1 | MODNAT |  -   |  -   |  -   |  -   |

#### Stack Arguments
| Offset | Type  | Name     |
|--------|-------|----------|
|      0 |  nat4 | divisor  |
|      4 |  nat4 | dividend |

#### Stack Returns
| Offset | Type  | Name      |
|--------|-------|-----------|
|      0 |  nat4 | remainder |

#### Description
Stores the result of ```dividend``` <b>mod</b> ```divisor``` into
```remainder```. The <b>mod</b> operator uses truncating division.

#### Exceptions
- Abort(33) "Division (or Modulus) by zero in Integer Expression":
  If ```divisor``` is zero.

#### Operation
```
if divisor == 0 then
  Abort (33) // "Division (or Modulus) by zero in Integer Expression"
fi

remainder := dividend - (dividend / divisor)
```

-----
### MODREAL - Modulus of Reals
| OP | Name    | Arg0 | Arg1 | Arg2 | Arg3 |
|----|---------|------|------|------|------|
| A2 | MODREAL |  -   |  -   |  -   |  -   |

#### Stack Arguments
| Offset | Type  | Name     |
|--------|-------|----------|
|      0 | real8 | divisor  |
|      8 | real8 | dividend |

#### Stack Returns
| Offset | Type  | Name      |
|--------|-------|-----------|
|      0 | real8 | remainder |

#### Description
Stores the result of ```dividend``` <b>mod</b> ```divisor``` into
```remainder```. The <b>mod</b> uses truncating division.

#### Exceptions
- Abort(113) "Division (or Modulus) by zero in Real Expression":
  If ```divisor``` is zero.

- ExecutionError ("Overflow in real 'mod' operation" is thrown):
  If the result of the expression overflows.

```
if divisor == 0 then
  Abort (113) // "Division (or Modulus) by zero in Real Expression"
fi

remainder := dividend - divisor * Floor (dividend / divisor)

if !IsFinite (remainder) then
  ExecutionError ("Overflow in real 'mod' operation" is thrown)
fi
```

-----
### MONITORENTER - Enter Monitor Area
| OP | Name         | Arg0 | Arg1 | Arg2 | Arg3 |
|----|--------------|------|------|------|------|
| A3 | MONITORENTER |  -   |  -   |  -   |  -   |

#### Stack Arguments
| Offset | Type | Name              |
|--------|------|-------------------|
|      0 | addr | monitorDescriptor |

#### Stack Returns
None

#### Description
Enters into a monitor area, associating the current process with ```monitorDescriptor```.
If another process is already associated with the given monitor descriptor,
the current process becoms blocked and waits to enter the monitor area.

#### Exceptions
None

#### Operation
```
EnterMonitorArea(monitorDescriptor)
```
-----
### MONITOREXIT - Exit Monitor Area
| OP | Name        | Arg0 | Arg1 | Arg2 | Arg3 |
|----|-------------|------|------|------|------|
| A4 | MONITOREXIT |  -   |  -   |  -   |  -   |

#### Stack Arguments
| Offset | Type | Name              |
|--------|------|-------------------|
|      0 | addr | monitorDescriptor |

#### Stack Returns
None

#### Description
Exits from a monitor area and allows another process to enter into the monitor
area. The next process to wake up is derived from ```monitorDescriptor```

#### Exceptions
None

#### Operation
```
ExitMonitorArea(monitorDescriptor)
```
-----
### MULINT - Multiply Integers
| OP | Name   | Arg0 | Arg1 | Arg2 | Arg3 |
|----|--------|------|------|------|------|
| A5 | MULINT |  -   |  -   |  -   |  -   |

#### Stack Arguments
| Offset | Type  | Name   |
|--------|-------|--------|
|      0 |  int4 | lvalue |
|      8 |  int4 | rvalue |

#### Stack Returns
| Offset | Type  | Name   |
|--------|-------|--------|
|      0 |  int4 | result |

#### Description
Multiplies ```lvalue``` by ```rvalue``` and stores the result into
```result```.

#### Exceptions
- Abort(50) "Overflow in Integer expression":
  If the result is too big to store inside of an int4.

#### Operation
```
result := lvalue * rvalue

if lvalue != 0 && (result / lvalue) != rvalue then
  Abort (50) // Overflow in Integer expression
fi
```

-----
### MULNAT - Multiply Naturals
| OP | Name   | Arg0 | Arg1 | Arg2 | Arg3 |
|----|--------|------|------|------|------|
| A6 | MULNAT |  -   |  -   |  -   |  -   |

#### Stack Arguments
| Offset | Type  | Name   |
|--------|-------|--------|
|      0 |  nat4 | lvalue |
|      8 |  nat4 | rvalue |

#### Stack Returns
| Offset | Type  | Name   |
|--------|-------|--------|
|      0 |  nat4 | result |

#### Description
Multiplies ```lvalue``` by ```rvalue``` and stores the result into
```result```.

#### Exceptions
- Abort(50) "Overflow in Integer expression":
  If the result is too big to store inside of a nat4.

#### Operation
```
result := lvalue * rvalue

if lvalue != 0 && (result / lvalue) != rvalue then
  Abort (50) // Overflow in Integer expression
fi
```
-----
### MULREAL - Multiply Reals
| OP | Name    | Arg0 | Arg1 | Arg2 | Arg3 |
|----|---------|------|------|------|------|
| A7 | MULREAL |  -   |  -   |  -   |  -   |

#### Stack Arguments
| Offset | Type  | Name   |
|--------|-------|--------|
|      0 | real8 | lvalue |
|      8 | real8 | rvalue |

#### Stack Returns
| Offset | Type  | Name   |
|--------|-------|--------|
|      0 | real8 | result |

#### Description
Multiplies ```lvalue``` by ```rvalue``` and stores the result into
```result```.

#### Exceptions
- Abort(51) "Overflow in Real expression":
  If the result is too big to store inside of a real8.

#### Operation
```
result := lvalue * rvalue

if !IsFinite(result) then
  Abort (51) // Overflow in Real expression
fi
```

-----
### MULSET - Multiply Sets / Intersection of Sets
| OP | Name   | Arg0           | Arg1 | Arg2 | Arg3 |
|----|--------|----------------|------|------|------|
| A8 | MULSET | setSize (nat4) |  -   |  -   |  -   |

#### Stack Arguments
##### Form 1
| Offset | Type    | Name   |
|--------|---------|--------|
|      0 | set32   | rvalue |
|      4 | set32   | lvalue |

##### Form 2
| Offset | Type    | Name   |
|--------|---------|--------|
|      0 | addrint | tvalue |
|      4 | addrint | rvalue |
|      8 | addrint | lvalue |


#### Stack Returns
##### Form 1
| Offset | Type    | Name   |
|--------|---------|--------|
|      0 | set32   | result |

##### Form 2
| Offset | Type    | Name   |
|--------|---------|--------|
|      0 | addrint | tvalue |

#### Description
Computes the intersection of the two sets ```lvalue``` and ```rvalue```.

Sets in Turing are represented in two ways
 - As a single set32 / nat4
 - As an array of set16 / nat2's

Due to these two representations, two forms of parameter input are required to
merge each set appropriately.

```setSize``` discriminates between these two types, as it determines the
length of the set in bytes.

If ```setSize``` less than or equal to 4 bytes, parameter
'Form 1' is used and a simple bitwise AND ```lvalue``` and ```rvalue``` is
performed. The resulting set is stored in ```result```.

Otherwise, 'Form 2' is used, and the bitwise AND is performed between all set
values of ```lvalue``` and ```rvalue```, stored in the same location inside
```tvalue```.

```setSize``` applies to the length of both sets, as merging two sets of
different length is not supported in Turing.

#### Exceptions
None

#### Operation
```
if setSize <= 4 then
  // Perform intersection of single word sets
  result := lvalue & rvalue
else
  // Perform word-by-word union of sets
  Tset := PointerTo (set16, tvalue)
  Rset := PointerTo (set16, rvalue)
  Lset := PointerTo (set16, lvalue)

  Words := setSize >> 1 // Get set size in words
  WordIdx := 1

  // TProlog performs a check if 'WordIdx' <= 'Words', but because 'Words' can
  // only be greater than 0, the check is not needed

  loop
    *Tset := (*Lset & *Rset)

    Tset += 2
    Rset += 2
    Lset += 2

    if Words == WordIdx then
      break
    fi

    Words += 1
  pool
fi
```


-----
### NATREAL* - Convert Natural into Real
| OP | Name        | Arg0 | Arg1 | Arg2 | Arg3 |
|----|-------------|------|------|------|------|
| A9 | NATREAL     |  -   |  -   |  -   |  -   |
| AA | NATREALLEFT |  -   |  -   |  -   |  -   |

#### Stack Arguments (NATREAL)
| Offset | Type  | Name     |
|--------|-------|----------|
|      0 | nat4  | value    |

#### Stack Returns (NATREAL)
| Offset | Type  | Name     |
|--------|-------|----------|
|      0 | real8 | result   |

#### Stack Arguments (NATREALLEFT)
| Offset | Type  | Name     |
|--------|-------|----------|
|      0 | real8 | preserve |
|      8 | nat4  | value    |

#### Stack Returns (NATREALLEFT)
| Offset | Type  | Name     |
|--------|-------|----------|
|      0 | real8 | preserve |
|      8 | real8 | result   |

#### Description
Converts ```value``` into an real8, storing the result in  ```result```. Also 
allocates enough space to accomodate the real8.

#### Exceptions
None

#### Operation
```
result := (real8)value
```

-----
### NATSTR - Convert Natural into String
| OP | Name   | Arg0 | Arg1 | Arg2 | Arg3 |
|----|--------|------|------|------|------|
| AB | NATSTR |  -   |  -   |  -   |  -   |

#### Stack Arguments
| Offset | Type    | Name    |
|--------|---------|---------|
|      0 | addrint | destStr |
|      4 | int4    |  base   |
|      8 | int4    |  width  |
|     12 | nat4    |  value  |

#### Stack Returns
| Offset | Type    | Name    |
|--------|---------|---------|
|      0 | addrint | destStr |

#### Description
Converts ```value``` into a string using the specified conversion arguments 
(```width``` and ```base```), and stores the string at ```destStr```.

```width``` specifies the minimum number of characters in the string. If there 
are less digits than what is specified in ```width```, it is padded with spaces.

```base``` specifies the integer base to use during conversion.

#### Exceptions
- Quit (excpNegativeFieldWidthSpecified): If ```width``` is negative.
- Quit (excpResultStringTooBig): If the string generated in the conversion
  exceeds the length of MAX_STRING_LEN.
- Quit (excpIllegalStringConversionBase): If ```base``` is outside of the range
  2 .. 36 (inclusive).

#### Operation
```
// TODO: NATSTR
```

-----
### NEG* - Flip Number Sign
| OP | Name   | Arg0 | Arg1 | Arg2 | Arg3 |
|----|--------|------|------|------|------|
| AC | NEGINT |  -   |  -   |  -   |  -   |
| AD | NEGREAL |  -   |  -   |  -   |  -   |

#### Stack Arguments
| Offset | Type  | Name  |
|--------|-------|-------|
|      0 | value | value |

#### Stack Returns
| Offset | Type  | Name  |
|--------|-------|-------|
|      0 | value | value |

#### Type Definition
| Instruction | type  |
|-------------|-------|
| NEGINT      | int4  |
| NEGREAL     | real8 |

#### Description
Stores ```value``` with its sign flipped back into ```value```.

#### Exceptions
##### NEGINT
- Abort (50) "Overflow in Integer expression":
  If ```value``` is equal to <b>minint</b>, as it has no representable positive
  value.

##### NEGREAL
None

#### Operation
```
if (value == minint) then
  Abort (50) // Overflow in Integer expression.
fi

value := -value
```

-----
### NEW - Allocate Heap Memory
| OP | Name  | Arg0 | Arg1 | Arg2 | Arg3 |
|----|-------|------|------|------|------|
| AE | NEW   |  -   |  -   |  -   |  -   |

#### Stack Arguments
| Offset | Type    | Name        |
|--------|---------|-------------|
|      0 | addrint | uninitPat   |
|      4 | nat4    | size        |
|      8 | addrint | addressSave |

#### Stack Returns
None

#### Description
Allocates a new heap object with ```size```.

```addressSave``` points to a PointerDescriptor, where the ```address```
will be set to the location of the new heap memory, and ```cookie``` will be
updated to the cookie value at the new heap memory.

If ```uninitPat``` is not <b>nil</b>, then the new heap memory is uninitialized
using the given pattern at the address.

If the allocation fails, then the new heap memory is not uninitialized using
the given ```uninitPat```, and ```address``` of the pointer descriptor at
```addressSave``` is set to zero.

#### Exceptions
None

-----
### NEWARRAY - Resize Flexible Array
| OP | Name     | Arg0 | Arg1 | Arg2 | Arg3 |
|----|----------|------|------|------|------|
| AF | NEWARRAY |  -   |  -   |  -   |  -   |

#### Stack Arguments
| Offset | Type    | Name       |
|--------|---------|------------|
|      0 | addrint | descriptor |
|      4 | addrint | uninitPat  |
|     +8 | int4    | newUppers  |

#### Stack Returns
None

#### Description
Resizes the flexible array given in ```descriptor```, adjusting the upper
bounds of each dimension. The new upper bounds are given in ```newUppers```
which is a variable-length list of int4's.

Also reallocates the memory used to store the array data.

If ```uninitPat``` is non-zero, it points to an unitialization pattern to use
for the new elements.

For TProlog, a non-zero resizing of any dimension but the first one is not
supported.

#### Exceptions
- ExecutionError ("New index range for flexible array is negative"):
  If the new computed range for a given dimension is less than zero (i.e. lower
  bound > upper bound)
- ExecutionError ("Complex multi-dimensioned flexible array reallocation not implemented yet - sorry"):
  (TProlog) If an attempt is made to resize any dimension which is not the
  first one
- ExecutionError ("Not enough memory for flexible array reallocation"):
  If not enough memory is available to support the reallocation

-----
### NEWCLASS - Allocate Class Instance
| OP | Name     | Arg0 | Arg1 | Arg2 | Arg3 |
|----|----------|------|------|------|------|
| B0 | NEWCLASS |  -   |  -   |  -   |  -   |

#### Stack Arguments
| Offset | Type    | Name            |
|--------|---------|-----------------|
|      0 | addrint | classDescriptor |

#### Stack Returns
| Offset | Type    | Name          |
|--------|---------|---------------|
|      0 | addrint | classInstance |
|      4 | addrint | initRoutine   |
|      8 | int4    | constantOne   |
|     12 | addrint | self          |

#### Description
Allocates a new class instance for the given ```classDescriptor```.

The allocated class instance is given back in ```classInstance```, and the init
routine address is returned in ```initRoutine```. If there is no associated
routine, it points to a stub routine.

If allocation of the class instance fails, ```classInstance``` is zero, and
```initRoutine``` points to the stub routine.

#### Exceptions
- ExecutionError ("Class implement-by(s) have not been initialized"):
  If the associated ```classDescriptor``` has no base class.

-----
### NEWU - Allocate Raw Memory
| OP | Name  | Arg0 | Arg1 | Arg2 | Arg3 |
|----|-------|------|------|------|------|
| B1 | NEWU  |  -   |  -   |  -   |  -   |

#### Stack Arguments
| Offset | Type    | Name      |
|--------|---------|-----------|
|      0 | addrint | uninitPat |
|      4 | nat4    | size      |
|      8 | addrint | pointer   |

#### Stack Returns
None

#### Description
Allocates a new memory area with ```size``` bytes.

```pointer``` points to a pointer variable, where the pointer variable will be
set to the location of the new heap memory.

If ```uninitPat``` is not <b>nil</b>, then the new memory is uninitialized
using the given pattern at the address.

If the allocation fails, then the new heap memory is not uninitialized using
the given ```uninitPat```, and the variable at ```store``` is set to zero.

-----
### NOT - Boolean NOT
| OP | Name  | Arg0 | Arg1 | Arg2 | Arg3 |
|----|-------|------|------|------|------|
| B2 | NOT   |  -   |  -   |  -   |  -   |

#### Stack Arguments
| Offset | Type | Name |
|--------|------|------|
|      0 | int4 | lval |

#### Stack Returns
| Offset | Type | Name |
|--------|------|------|
|      0 | int4 | flip |

#### Description
Changes the boolean value at ```lval``` into its inverted version at ```flip```.
In essence, it changes a 0 to a 1, and a 1 to a 0.

This instruction does not work for any other values, and will do nothing if
such an attempt is made. If a bitwise logical NOT is preferred, a XOR
instruction with 0xFFFFFFFF as the ```rval``` should be used.

#### Exceptions
None

#### Operation
```
flip = !lval
```

-----
### NUMARRAYELEMENTS - Get Number of Array Elements
| OP | Name             | Arg0 | Arg1 | Arg2 | Arg3 |
|----|------------------|------|------|------|------|
| B3 | NUMARRAYELEMENTS |  -   |  -   |  -   |  -   |

#### Stack Arguments
| Offset | Type    | Name     |
|--------|---------|----------|
|      0 | addrint | array    |

#### Stack Returns
| Offset | Type    | Name     |
|--------|---------|----------|
|      0 | int4    | numElems |

#### Description
Gets the number of array elements in ```array```, storing the result in
```numElems```

#### Exceptions
None

#### Operation
```
numElems := PointerTo (ArrayDescriptor, array) -> elementCount
```

-----
### OBJCLASS - Get Object's Class
| OP | Name     | Arg0 | Arg1 | Arg2 | Arg3 |
|----|----------|------|------|------|------|
| B4 | OBJCLASS |  -   |  -   |  -   |  -   |

#### Stack Arguments
| Offset | Type    | Name     |
|--------|---------|----------|
|      0 | addrint | instance |

#### Stack Returns
| Offset | Type    | Name            |
|--------|---------|-----------------|
|      0 | addrint | classDescriptor |

#### Description
Gets the class descriptor associated with ```instance```, storing the 
descriptor address in ```classDescriptor```.

#### Exceptions
None

#### Operation
```
if instance != nullptr then
  classDescriptor := (addrint) (instance->descriptor)
fi
```

-----
### OPEN - Open File Handle
| OP | Name | Arg0            | Arg1            | Arg2 | Arg3 |
|----|------|-----------------|-----------------|------|------|
| B5 | OPEN | openKind (nat1) | openMode (nat1) |  -   |  -   |

#### Stack Arguments (```openKind``` == OldOpen)
| Offset | Type    | Name     |
|--------|---------|----------|
|      0 | addrint | fileMode |
|      4 | addrint | filePath |
|      8 | addrint | streamNo |

#### Stack Arguments (```openKind``` == NormalOpen)
| Offset | Type    | Name     |
|--------|---------|----------|
|      0 | addrint | filePath |
|      4 | addrint | streamNo |

#### Stack Arguments (```openKind``` == ArgumentFile)
| Offset | Type    | Name     |
|--------|---------|----------|
|      0 | int4    | argNo    |
|      4 | addrint | streamNo |

#### Description
Performs a file open operation, of the specific kind ```openKind```, with in
mode specified in ```openMode```. The new file descriptor is stored at the
address of ```streamNo```.

Depending on ```openKind``` a specific operation may be performed:

| ```openKind```   | Operation Description                        |
|------------------|----------------------------------------------|
| 0 - OldOpen      | Open file at path, with mode as a string     |
| 1 - NormalOpen   | Open file                                    |
| 2 - ArgumentFile | File path is specified in a program argument |

For the ```NormalOpen``` open kind, if the prefix for ```filePath``` is
of "%window", "%printer", or "%net", the open is treated as a device file open.
Otherwise, if the prefix is "%oot", ```filePath``` is relative to the Turing
installation directory.

Otherwise, ```filePath``` is resolved to the file at the current working
directory.

#### Exceptions
- Abort (25): If ```openKind``` is not equal to 0, 1, or 2.
- Quit (excpIllegalOpenMode):
  If ```openMode``` does not have any of the "get", "put", "read", or "write" 
  bits set, or if the text modes ("get", "put") are mixed with the binary modes
  ("read", "write").
- Quit (excpImplicitOpenFailed):
  If an error occurred while opening the file.

##### ```openKind``` == NormalFile:
- Quit (excpIllegalOpenMode):
  If ```fileMode``` does not have "r" or "w" as the first character. 

##### ```openKind``` == ArgumentFile:
- ExecutionError ("Open argument ```argNo``` does not exist"):
  If ```argNo``` is less than zero or is greater than ARGC
- ExecutionError ("Open argument ```argNo``` is already open"):
  If the file pointed to by ```argNo``` has already been opened

#### Operation
```
if (openKind != 0) && (openKind != 1) && (openKind != 2) then
  Abort (25) // Case selector value is out of range
fi

case openKind of
  label 0: // Old open
    *streamNo := OpenFile (filePath, fileMode)
  label 1: // Normal Open
    *streamNo := OpenFile (filePath, openMode)
  label 2: // ArgumentPath File
    *streamNo := OpenFile (ProgramArguments[argNo], fileMode)
esac
```

-----
### OR - Bitwise Or
| OP | Name  | Arg0 | Arg1 | Arg2 | Arg3 |
|----|-------|------|------|------|------|
| B6 | OR    |  -   |  -   |  -   |  -   |

#### Stack Arguments
| Offset | Type | Name |
|--------|------|------|
|      0 | nat4 | rval |
|      4 | nat4 | lval |

#### Stack Returns
| Offset | Type | Name   |
|--------|------|--------|
|      0 | nat4 | result |

#### Description
Performs a bitwise logical-OR operation between ```lval``` and ```rval```, and 
stores the result in ```result```

#### Exceptions
None

#### Operation
```
result := lval | rval
```

-----
### ORD - Ordinal Of String Character
| OP | Name  | Arg0 | Arg1 | Arg2 | Arg3 |
|----|-------|------|------|------|------|
| B7 | ORD   |  -   |  -   |  -   |  -   |

#### Stack Arguments
| Offset | Type    | Name   |
|--------|---------|--------|
|      0 | addrint | string |

#### Stack Returns
| Offset | Type | Name    |
|--------|------|---------|
|      0 | int4 | ordinal |

#### Description
Gets the character inside ```string``` and returns the integer ordinal value
in ```ordinal```.

Note: Invocations of the <b>ord</b> command on single characters and enums
automatically get folded by the compiler into integer constants.

#### Exceptions
- Abort (37)
- If the length of ```string``` is not equal to one.

#### Operation
```
if (LengthOf (string) != 1) then
  Abort (37) // Parameter to "ord" is not of length one
fi

ordinal := CastTo(int4, string[0])
```

-----
### PAUSE - Pause and Wait for Simulated Time
| OP | Name  | Arg0 | Arg1 | Arg2 | Arg3 |
|----|-------|------|------|------|------|
| B8 | PAUSE |  -   |  -   |  -   |  -   |

#### Stack Arguments
| Offset | Type | Name  |
|--------|------|-------|
|      0 | nat4 | delay |

#### Stack Returns
None

#### Description
Waits for ```delay``` units of simulated time to pass.
The ```pause``` instruction advances the interpreter's internal simulation time
counter, allowing for the usage of Turing as a simulation platform.

Advancing the simulation time counter may also cause processes to timeout
while waiting on a timeout condition.

See ```pause``` documentation inside of the Turing Help File for more
information on the ```pause``` command and other related commands.

#### Exceptions
None

#### Operation
```
Pause (delay)
```

-----
### PRED - Predecessor of
| OP | Name  | Arg0 | Arg1 | Arg2 | Arg3 |
|----|-------|------|------|------|------|
| B9 | PRED  |  -   |  -   |  -   |  -   |

#### Stack Arguments
| Offset | Type | Name   |
|--------|------|--------|
|      0 | int4 | number |

#### Stack Returns
| Offset | Type | Name |
|--------|------|------|
|      0 | int4 | pred |

#### Description
Reduces ```number``` by 1 and store the result in ```pred```.

#### Exceptions
- Abort (50) "Overflow in Integer expression":
  If ```number``` is equal to 0x80000000 or <b>minint</b>

#### Operation
```
if number == minint then
  Abort (50) // Overflow in integer expression
fi

pred := number - 1
```

-----
### PROC - Begin Procedure
| OP | Name  | Arg0               | Arg1 | Arg2 | Arg3 |
|----|-------|--------------------|------|------|------|
| BA | PROC  | localSize (offset) |  -   |  -   |  -   |

#### Stack Returns
| Offset | Type    | Name         |
|--------|---------|--------------|
|  ...   |   ...   | localsArea   |
|      0 | addrint | framePointer |
|      4 | nat2    | fileNo       |
|      6 | nat2    | lineNo       |

#### Stack Arguments
None

#### Stack Returns
None

#### Description
Sets up the stack to be ready for the work of the procedure. The previous
frame pointer and file location information is stored on the stack, and
above the stack pointer, ```localSize``` bytes of space are allocated for
local variables. The ```localsArea``` is also cleared before usage.
The frame pointer position is preserved in GlobalFP, later to be restored by 
```return```

#### Description

#### Operation
```
PerformOverflowCheck (localsSize)
Push (nat2, lineNo)
Push (nat2, fileNo)
Push (addrint, GlobalFP)

// Save the location of the stack
GlobalFP = GlobalSP

if (localSize != 0) then
  GlobalSP -= localSize
  ZeroMemory (GlobalSP, localSize)
fi
```

-----
### PUSH* - Push Value onto stack
| OP | Name         |  Arg0          | Arg1           | Arg2 | Arg3 |
|----|--------------|----------------|----------------|------|------|
| BB | PUSHADDR     | imm (addrint)  |   -            |   -  |   -  |
| BC | PUSHADDR1    | link (offset)  | imm (addrint)  |   -  |   -  |
| BD | PUSHCOPY     |   -            |   -            |   -  |   -  |
| BE | PUSHINT      | imm (int4)     |   -            |   -  |   -  |
| BF | PUSHINT1     | imm (int1)     |   -            |   -  |   -  |
| C0 | PUSHINT2     | imm (int2)     |   -            |   -  |   -  |
| C1 | PUSHREAL     | imm (real8)    |   -            |   -  |   -  |
| C2 | PUSHVAL0     |   -            |   -            |   -  |   -  |
| C3 | PUSHVAL1     |   -            |   -            |   -  |   -  |

#### Stack Arguments (PUSHCOPY)
| Offset | Type        | Name        |
|--------|-------------|-------------|
|      0 | <i>type</i> | copiedValue |

#### Stack Returns (PUSHCOPY)
| Offset | Type        | Name        |
|--------|-------------|-------------|
|      0 | <i>type</i> | value       |
|      4 | <i>type</i> | copiedValue |

#### Stack Returns (Not PUSHCOPY)
| Offset | Type        | Name  |
|--------|-------------|-------|
|      0 | <i>type</i> | value |

#### Description
Pushes a value on to the stack.

PUSHADDR, PUSHADDR1, PUSHINT, PUSHINT1, PUSHINT2, and PUSHREAL pushes immediate
values from the instruction on to the stack.

PUSHVAL0 and PUSHVAL1 push literal '0' and '1' values respectively on to
the stack.

PUSHCOPY duplicates the value currently on top of the stack, leading to two of
the same values on to the stack.

The type for ```value``` is dependent on what the specific opcode is. Below
is a table showing the instructions and the relavent type for ```value```

#### Type Definitions
| Instruction  | Type    |
|--------------|---------|
| PUSHADDR     | addrint |
| PUSHADDR1    | addrint |
| PUSHCOPY     | nat4 \* |
| PUSHINT      | int4    |
| PUSHINT1     | int1    |
| PUSHINT2     | int2    |
| PUSHREAL     | real8   |
| PUSHVAL0     | int4    |
| PUSHVAL1     | int4    |

\* PUSHCOPY can only copy values on the stack that are <b>sizeof</b> nat4 or smaller

#### Exceptions
None

#### Operation
##### Generic
```
Push (type, imm)
```

##### PUSHCOPY
```
Temp := Peek (type, 0)
Push (type, Temp)
```
-----
### PUT - Put Characters Into Stream
| OP | Name | Arg0      | Arg1 | Arg2 | Arg3 |
|----|------|-----------|------|------|------|
| C4 | PUT  | i1PutKind |  -   |  -   |  -   |

#### Stack Arguments
| Offset | Type | Name |
|--------|------|------|
|      0 | type | name |

#### Stack Returns
| Offset | Type | Name |
|--------|------|------|
|      0 | type | name |

#### Description

-----
### QUIT - Issue Program Quit
| OP | Name | Arg0 | Arg1 | Arg2 | Arg3 |
|----|------|------|------|------|------|
| C5 | QUIT |  -   |  -   |  -   |  -   |

#### Stack Arguments
| Offset | Type | Name     |
|--------|------|----------|
|      0 | int4 | quitType |
|      4 | int4 | quitCode |

#### Stack Returns
None

#### Description
Issues the current program to perform a quit. Depending on the ```quitType```,
the program quit may be handled differently:

| ```quitType``` | Behaviour                                                          |
|----------------|--------------------------------------------------------------------|
|             -1 | Propagate the quit condition to the next abort handler             |
|              0 | The quit location is at the location of the invocation             |
|              1 | Quit occurred inside of the abort handler, reset handler chain     |
|              * | Invalid ```quitType```, abort with "Case selector is out of range" |

If no abort handler is found, or if none of the abort handlers handle the quit,
the program exits with the exit code corresponding to the ```quitCode```

#### Exceptions
None

#### Operation
```
case (quitType) of
  label -1:
    // Propagate Error
    SourcePos.FileNo = (GlobalFP + 4)
    SourcePos.LineNo = (GlobalFP + 4) + 2
    QuitCode = 0
  label  0:
    // Quit normally
    QuitCode = 0
  label  1:
    // Restart the handler chain
    SourcePos = CurrentHandler.SourcePos
    Peek (int4, 0) = CurrentHandler.QuitCode
  label default: Abort (15) // Case selector is out of range
esac

InvokeAbortHandler (QuitCode)
```

-----
### READ - Read Bytes In
| OP | Name  | Arg0 | Arg1 | Arg2 | Arg3 |
|----|-------|------|------|------|------|
| C6 | READ  |  -   |  -   |  -   |  -   |

#### Stack Arguments
| Offset | Type | Name |
|--------|------|------|
|      0 | type | name |

#### Stack Returns
| Offset | Type | Name |
|--------|------|------|
|      0 | type | name |

#### Description


-----
### REALDIVIDE - Divide Reals
| OP | Name       | Arg0 | Arg1 | Arg2 | Arg3 |
|----|------------|------|------|------|------|
| C7 | REALDIVIDE |  -   |  -   |  -   |  -   |

#### Stack Arguments
| Offset | Type  | Name     |
|--------|-------|----------|
|      0 | real8 | divisor  |
|      8 | real8 | dividend |

#### Stack Returns
| Offset | Type  | Name     |
|--------|-------|----------|
|      0 | real8 | quotient |

#### Description
Divides the ```dividend``` by the ```divisor``` and stores the result in
```quotient```.

#### Exceptions
- Abort(113) "Division (or Modulus) by zero in Real Expression":
  If ```divisor``` is zero.

- Abort(51) "Overflow in Real Expression":
  If the result of the operation is not a finite value.

#### Operation
```
if divisor == 0 then
  Abort (113) // "Division (or Modulus) by zero in Real Expression"
fi

quotient := dividend / divisor

if !IsFinite(quotient) then
  Abort (51) // "Overflow in Real Expression"
fi
```

------
### REMINT - Remainder of Integers
| OP | Name   | Arg0 | Arg1 | Arg2 | Arg3 |
|----|--------|------|------|------|------|
| C8 | REMINT |  -   |  -   |  -   |  -   |

#### Stack Arguments
| Offset | Type  | Name     |
|--------|-------|----------|
|      0 |  int4 | divisor  |
|      8 |  int4 | dividend |

#### Stack Returns
| Offset | Type  | Name      |
|--------|-------|-----------|
|      0 |  int4 | remainder |

#### Description
Divides the ```dividend``` by the ```divisor``` and stores the remainder in
```remainder```.

#### Exceptions
- Abort(33) "Division (or Modulus) by zero in Integer Expression":
  If ```divisor``` is zero.

#### Operation
```
if divisor == 0 then
  Abort (33) // "Division (or Modulus) by zero in Integer Expression"
fi

remainder := dividend - (dividend / divisor)
```

-----
### REMREAL - Remainder of Reals
| OP | Name    | Arg0 | Arg1 | Arg2 | Arg3 |
|----|---------|------|------|------|------|
| C9 | REMREAL |  -   |  -   |  -   |  -   |

#### Stack Arguments
| Offset | Type  | Name     |
|--------|-------|----------|
|      0 | real8 | divisor  |
|      8 | real8 | dividend |

#### Stack Returns
| Offset | Type  | Name      |
|--------|-------|-----------|
|      0 | real8 | remainder |

Divides the ```dividend``` by the ```divisor``` and stores the remainder in
```remainder```.

#### Exceptions
- Abort(113) "Division (or Modulus) by zero in Real Expression":
  If ```divisor``` is zero.

- ExecutionError ("Integer overflow in real8 div")
  If ```wholeDivide``` is too large to fit inside of an int4.

- ExecutionError ("Overflow in real 'mod' operation"):
  If the result of the expression overflows.

#### Operation
```
if divisor == 0 then
  Abort (113) // "Division (or Modulus) by zero in Real Expression"
fi

wholeDivide := RoundToZero (dividend / divisor) // Can overflow
remainder := dividend - (divisor * wholeDivide)
```

-----
### RESOLVEDEF - Resolve Procedure Definition
| OP | Name       | Arg0             | Arg1 | Arg2 | Arg3 |
|----|------------|------------------|------|------|------|
| CA | RESOLVEPTR | descOff (offset) |  -   |  -   |  -   |

#### Stack Arguments
| Offset | Type    | Name     |
|--------|---------|----------|
|      0 | addrint | instance |

#### Stack Returns
| Offset | Type    | Name     |
|--------|---------|----------|
|      0 | addrint | procAddr |

#### Description
Resolves the requested procedure address for the given class instance 
```instance```, storing the procedure address in procAddr

#### Exceptions
None

#### Operation
```
procAddr := (instance -> descriptor) + descOffset
```

-----
### RESOLVEPTR - Resolve Pointer
| OP | Name       | Arg0 | Arg1 | Arg2 | Arg3 |
|----|------------|------|------|------|------|
| CB | RESOLVEPTR |  -   |  -   |  -   |  -   |

#### Stack Arguments
| Offset | Type    | Name |
|--------|---------|------|
|      0 | addrint | addr |

#### Stack Returns
| Offset | Type    | Name |
|--------|---------|------|
|      0 | addrint | addr |

#### Description
Resolves the pointer ```addr```'s address and verifies if ```addr``` points to a 
valid location in memory.

#### Exceptions
- ExecutionError ("Reference through nil pointer"):
  If the address of ```addr``` is equal to nil / zero

#### Operation
```
ptr := ptr
```

-----
### RESTORESP - Restore Stack Pointer
| OP | Name      | Arg0 | Arg1 | Arg2 | Arg3 |
|----|-----------|------|------|------|------|
| CC | RESTORESP |  -   |  -   |  -   |  -   |

#### Stack Arguments
| Offset | Type    | Name    |
|--------|---------|---------|
|      0 | addrint | saveLoc |

#### Stack Returns
None

#### Description
Restores the current stack pointer to the address specified in ```saveLoc```.

#### Exceptions
None

#### Operation
```
GlobalSP := *saveLoc
```

-----
### RETURN - Return From Procedure
| OP | Name   | Arg0 | Arg1 | Arg2 | Arg3 |
|----|--------|------|------|------|------|
| CD | RETURN |  -   |  -   |  -   |  -   |

#### Stack Arguments
None

#### Stack Returns
Previous Stack State

#### Description
Returns from a procedure. Uses the frame pointer setup by a previous ```proc```
instruction to restore the stack state before the call of the procedure.

Before the invocation of ```return```, the current frame pointer structure at
GlobalFP is as follows:

| Offset | Type    | Name    | Pushed By? |
|--------|---------|---------|------------|
|      0 | addrint | lastFP  | ```proc``` |
|      4 | nat2    | fileNo  | ```proc``` |
|      6 | nat2    | lineNo  | ```proc``` |
|      8 | addrint | retAddr | ```call``` |

If ```lastFP``` is zero, the ```return``` is treated as a ```sysexit``` and the
current process is terminated.

Otherwise, the previous stack state is restored, and program execution resumes
after the invocation to ```call```

#### Exceptions
None

#### Operation
```
GlobalSP := GlobalFP
GlobalFP := lastFP

if (GlobalFP == 0) then
  ExitProcess ()
fi

CurrentFileNo := fileNo
CurrentLineNo := lineNo
GlobalPC := retAddr

// Remove the old frame pointer
Pop (12)

if SavedInitPC == GlobalPC then
  // Returned to init code, zero saved PC & FP
  SavedInitPC := 0
  SavedInitFP := 0
fi

// Perform implementation specific return handling here //
```

-----
### RTS - Return From Subroutine
| OP | Name | Arg0 | Arg1 | Arg2 | Arg3 |
|----|------|------|------|------|------|
| CE | RTS  |  -   |  -   |  -   |  -   |

#### Stack Arguments
| Offset | Type    | Name    |
|--------|---------|---------|
|      0 | addrint | retAddr |

#### Stack Returns
None

#### Description
Jumps to the address specified in ```retAddr```. Unlike RETURN, it does not
clean up after a function, and it only pops off ```retAddr```

#### Exceptions
None

#### Operation
```
GlobalPC := retAddr
```

-----
### SAVESP - Save Stack Pointer
| OP | Name   | Arg0 | Arg1 | Arg2 | Arg3 |
|----|--------|------|------|------|------|
| CF | SAVESP |  -   |  -   |  -   |  -   |

#### Stack Arguments
| Offset | Type    | Name    |
|--------|---------|---------|
|      0 | addrint | saveLoc |

#### Stack Returns
None

#### Description
Stores the current stack pointer (after popping off ```saveLoc```) into the
location specified by ```saveLoc```

#### Exceptions
None

#### Operation
```
*saveLoc := GlobalSP
```

-----
### SEEK - Seek to
| OP | Name  | Arg0 | Arg1 | Arg2 | Arg3 |
|----|-------|------|------|------|------|
| D0 | SEEK  |  -   |  -   |  -   |  -   |

#### Stack Arguments
| Offset | Type | Name     |
|--------|------|----------|
|      0 | int4 | streamNo |
|      4 | int4 | seekPos  |

#### Stack Returns
None

#### Description

-----
### SEEKSTAR - Seek to End
| OP | Name     | Arg0 | Arg1 | Arg2 | Arg3 |
|----|----------|------|------|------|------|
| D1 | SEEKSTAR |  -   |  -   |  -   |  -   |

#### Stack Arguments
| Offset | Type | Name     |
|--------|------|----------|
|      0 | int4 | streamNo |

#### Stack Returns
None

#### Description

-----
### SETALL - Fill Set
| OP | Name   | Arg0           | Arg1        | Arg2 | Arg3 |
|----|--------|----------------|-------------|------|------|
| D2 | SETALL | setSize (nat4) | tail (nat2) |  -   |  -   |

#### Stack Arguments
| Offset | Type    | Name    |
|--------|---------|---------|
|      0 | addrint | set     |

#### Stack Returns
| Offset | Type    | Name    |
|--------|---------|---------|
|      0 | addrint | result  |

#### Description
Fills ```set```.

Sets in Turing are represented in two ways
 - As a single set32 / nat4
 - As an array of set16 / nat2's

However, as SETALL only applies to variable length sets, the types of sets do
not need to be discriminated.

```setSize``` indicates the length of the entire set to fill, and ```tail```
forms a part of the tail end of the set.

#### Exceptions
None

#### Operation
```
// Fill the set
Lset := PointerTo (set16, set)
Words := setSize >> 1 // Get set size in words
WordIdx := 2          // Start from 2 to allow for sets smaller than 16 entries

if WordIdx <= Words then
  loop
   *Lset := set16_filled
    Lset += 2
    if Words == WordIdx then
      break
    fi
    Words += 1
  pool
fi

// Append tail
*Lset := tail

// Give back new set
result := set
```

-----
### SETCLR - Clear Set
| OP | Name   | Arg0           | Arg1 | Arg2 | Arg3 |
|----|--------|----------------|------|------|------|
| D3 | SETCLR | setSize (nat4) |  -   |  -   |  -   |

#### Stack Arguments
| Offset | Type    | Name    |
|--------|---------|---------|
|      0 | addrint | set     |

#### Stack Returns
| Offset | Type    | Name    |
|--------|---------|---------|
|      0 | addrint | result  |

#### Description
Clears ```set```.

Sets in Turing are represented in two ways
 - As a single set32 / nat4
 - As an array of set16 / nat2's

However, as SETCLR only applies to variable length sets, the types of sets do
not need to be discriminated.

```setSize``` indicates the length of the entire set to clear.

#### Exceptions
None

#### Operation
```
// Clear the set
Lset := PointerTo (set16, set)
Words := setSize >> 1 // Get set size in words
WordIdx := 1          // Start from 2 to allow for sets smaller than 16 entries

// TProlog performs a check if 'WordIdx' <= 'Words', but because 'Words' can
// only be greater than 0, the check is not needed

if WordIdx <= Words then
  loop
   *Lset := set16_cleared
    Lset += 2
    if Words == WordIdx then
      break
    fi
    Words += 1
  pool
fi

// Give back new set
result := set
```

-----
### SETELEMENT - Make Set with Element
| OP | Name       | Arg0           | Arg1      | Arg2      | Arg3 |
|----|------------|----------------|-----------|-----------|------|
| D4 | SETELEMENT | setSize (nat4) | lo (int4) | hi (int4) |  -   |

#### Stack Arguments
##### Form 1
| Offset | Type    | Name    |
|--------|---------|---------|
|      0 | set32   | set     |
|      4 | set32   | element |

##### Form 2
| Offset | Type    | Name    |
|--------|---------|---------|
|      0 | addrint | set     |
|      4 | addrint | element |


#### Stack Returns
##### Form 1
| Offset | Type    | Name    |
|--------|---------|---------|
|      0 | set32   | result  |

##### Form 2
| Offset | Type    | Name    |
|--------|---------|---------|
|      0 | addrint | result  |

#### Description
Clears ```set``` and adds ```element``` to ```set```, storing the changed set
at ```result```.

Sets in Turing are represented in two ways
 - As a single set32 / nat4
 - As an array of set16 / nat2's

Due to these two representations, two forms of parameter input are required to
insert ```element``` correctly.

```setSize``` discriminates between these two types, as it determines the
length of the set in bytes.

If ```setSize``` less than or equal to 4 bytes, parameter 'Form 1' is used and
the element add for ```element``` and ```set``` is performed for the
first set32.

Otherwise, 'Form 2' is used, and the element add is performed at the
```set```'s corresponding word index for ```element```.

#### Exceptions
- Abort (29) "Set element is out of range":
  If ```element``` is outside of the bounds of ```lo``` .. ```hi```.

#### Operation
```
if element < lo || element > hi then
  Abort (29) // Set element is out of range
fi 

// Bit offset
BitSel := (element - lo) & 0x1F
// Byte offset
WordSel := (element - lo) >> 4

if setSize <= 4 then
  // Clear set & add element
  result := set32_cleared
  result := result | (1 << BitSel)
else
  // Clear the set
  Words := setSize >> 1 // Get set size in words
  WordIdx := 1

  // TProlog performs a check if 'WordIdx' <= 'Words', but because 'Words' can
  // only be greater than 0, the check is not needed

  loop
    *Lset := set16_cleared
    Lset += 2

    if Words == WordIdx then
      break
    fi

    Words += 1
  pool

  // Access the specific word and add the element there
  Lset := PointerTo (set16, set + WordSel)
  *Lset := *Lset | (1 << BitSel)

  // Give back new set
  result := set
fi
```

-----
### SETFILENO - Set File & Line Number
| OP | Name      | Arg0          | Arg1          | Arg2 | Arg3 |
|----|-----------|---------------|---------------|------|------|
| D5 | SETFILENO | fileNo (nat2) | lineNo (nat2) |  -   |  -   |

#### Stack Arguments
None

#### Stack Returns
None

#### Description
Sets the current file & line number. Used for debugging purposes.

#### Exceptions
None

#### Operation
```
FileNo := fileNo
LineNo := lineNo
```

-----
### SETLINENO - Set Line Number
| OP | Name      | Arg0          | Arg1 | Arg2 | Arg3 |
|----|-----------|---------------|------|------|------|
| D6 | SETLINENO | lineNo (nat2) |  -   |  -   |  -   |

#### Stack Arguments
None

#### Stack Returns
None

#### Description
Sets the current line number. Used for debugging purposes.

#### Exceptions
None

#### Operation
```
LineNo := lineNo
```

-----
### SETPRIORITY - Set Process Priority
| OP | Name        | Arg0 | Arg1 | Arg2 | Arg3 |
|----|-------------|------|------|------|------|
| D7 | SETPRIORITY |  -   |  -   |  -   |  -   |

#### Stack Arguments
| Offset | Type    | Name     |
|--------|---------|----------|
|      0 |    nat2 | priority |

#### Stack Returns
None

#### Description
Updates the current process's priority value to ```priority```.

#### Exceptions
None

#### Operation
```
UpdatePriority(CurrentProcess, priority)
```
-----
### SETSTDSTREAM - Set Standard IO Stream
| OP | Name         | Arg0        | Arg1 | Arg2 | Arg3 |
|----|--------------|-------------|------|------|------|
| D8 | SETSTDSTREAM | kind (int1) |  -   |  -   |  -   |

#### Stack Arguments
| Offset | Type    | Name |
|--------|---------|------|
|      0 | addrint | dest |

#### Stack Returns
None

#### Description
Gets the current Standard IO stream and stored the stream handle at ```dest```.
The stream handle stored in ```dest``` depends on whether the streams are
redirected and the value of ```kind```.

If the Standard IO streams are not redirected, the value stored at ```dest```
is as follows:

| ```kind``` | StreamNo    |
|------------|-------------|
|          1 | -2 (stdin)  |
|          2 | -1 (stdout) |


Otherwise if the streams are redirected, the redirection stream handle is stored 
at ```dest```.

#### Exceptions
- AssertFail ():
  If ```kind``` is not one of the accepted values: 1, 2.

#### Operation
```
if RedirectStreams != 0 then
  *PointerTo (nat4, dest) := RedirectStreams
else
  StdStream := 0

  case kind of
    label 1:
      StdStream := -2 // stdin
    label 2:
      StdStream := -1 // stdout
  esac

  *PointerTo (nat4, dest) := StdStream
fi
```

-----
### SETSTREAM - Set Stream
| OP | Name      | Arg0        | Arg1 | Arg2 | Arg3 |
|----|-----------|-------------|------|------|------|
| D9 | SETSTREAM | kind (int1) |  -   |  -   |  -   |

#### Stack Arguments
| Offset | Type    | Name         |
|--------|---------|--------------|
|      0 | addrint | status       |
|      4 | addrint | storeNo      |
|      8 | addrint | streamStatus |
|     12 | int4    | streamNo     |

#### Stack Returns
None

#### Description
Sets the current stream handle & updates the status store address & the stream
handle to pass to other IO operations.

#### Exceptions
- AssertFail ():
  If ```kind``` is not in the range 0 .. 5
- ExecutionError ("I/O ", streamNo):
  If ```streamNo``` is less than stdin (-2)
- ExecutionError ("I/O attempted on illegal stream number ```streamNo```"):
  If ```streamNo``` is greater than MAX_STREAM_NO

#### Operation
```
MAX_STREAM_NO := 20
*PointerTo(addrint, status) := streamStatus
*PointerTo(int4, storeNo) := streamNo

ExternalStreamNo := streamNo

if streamNo < -2 then
  ExecutionError ("I/O ", streamNo)
elseif streamNo > MAX_STREAM_NO
  ExecutionError ("I/O attempted on illegal stream number " + streamNo)
fi

// Check if corresponding streamNo is an argument stream
if IsArgumentStream (streamNo) then
  // Build open mode
  if kind < 0 || kind > 5 then
    AssetFail ()
  fi

  OpenMode := 1 << kind

  if kind == 0 then
    // Have the stream be both "put"able & "get"able
    OpenMode |= 0x20
  fi

  XSN := OpenFile (ProgramArguments[streamNo], OpenMode)
fi
```

-----
### SHL - Arithmetic Shift Left
| OP | Name | Arg0 | Arg1 | Arg2 | Arg3 |
|----|------|------|------|------|------|
| DA | SHL  |  -   |  -   |  -   |  -   |

#### Stack Arguments
| Offset | Type    | Name   |
|--------|---------|--------|
|      0 |    nat4 | shift  |
|      4 |    nat4 | rvalue |

#### Stack Returns
| Offset | Type    | Name   |
|--------|---------|--------|
|      0 |    nat4 | lvalue |

#### Description
Shifts ```rvalue``` left ```shift``` bits, and stores the result in
```lvalue```.

#### Exceptions
- Abort (50) "Overflow in Integer expression":
  If integer overflow is detected to occur.

#### Operation
```
lvalue := rvalue << shift
```

-----
### SHR - Arithmetic Shift Right
| OP | Name | Arg0 | Arg1 | Arg2 | Arg3 |
|----|------|------|------|------|------|
| DB | SHR  |  -   |  -   |  -   |  -   |

#### Stack Arguments
| Offset | Type    | Name   |
|--------|---------|--------|
|      0 |    nat4 | shift  |
|      4 |    nat4 | rvalue |

#### Stack Returns
| Offset | Type    | Name   |
|--------|---------|--------|
|      0 |    nat4 | lvalue |

#### Description
Shifts ```rvalue``` right ```shift``` bits, and stores the result in ```lvalue```.

#### Exceptions
None

#### Operation
```
lvalue := rvalue >> shift
```

-----
### SIGNAL - Signal Condition
| OP | Name   | Arg0       | Arg1 | Arg2 | Arg3 |
|----|--------|------------|------|------|------|
| DC | SIGNAL | i1CondKind |  -   |  -   |  -   |

#### Stack Arguments
| Offset | Type    | Name         |
|--------|---------|--------------|
|      0 | addrint | conditionVar |

#### Stack Returns
None

#### Description
Signals ```conditionVar```, waking up the next process waiting on this
condition.

Depending on ```i1CondKind```, the appropriate signal action is performed:
| ```i1CondKind``` | Action                                                          |
|------------------|-----------------------------------------------------------------|
|     Normal   (0) | Wakes up the first waiting process, blocking the current one    |
|     Priority (1) | Wakes up the highest priority process, blocking the current one |
|     Deferred (2) | Wakes up the first waiting process after exiting the monitor    |
|     Timeout  (3) | Wakes up the first waiting process after exiting the monitor    |

There is no specific guarrantee for exactly which process gets woken up.

#### Exceptions
- Quit (25):
  If ```i1CondKind``` is not in the range of 0 .. 3 (inclusive).

-----
### STRINT - Convert String to Integer
| OP | Name   | Arg0 | Arg1 | Arg2 | Arg3 |
|----|--------|------|------|------|------|
| DD | STRINT |  -   |  -   |  -   |  -   |

#### Stack Arguments
| Offset | Type    | Name   |
|--------|---------|--------|
|      0 |    int4 | base   |
|      4 | addrint | string |

#### Stack Returns
| Offset | Type    | Name   |
|--------|---------|--------|
|      0 |    int4 | result |

#### Description
Convertes the requested ```string``` into an int4 of the specified ```base```,
storing the number in ```result```.

The valid characters for a base are selected from the following alphabet:
```0123456789abcdefghijklmnopqrstuvwxyz```.
The conversion process is case-insensitive, so capital letters map to the same
lower case letters.

Also, ```+-``` are accepted at the beginning of the number string to indicate
the desired signedness of the number.

#### Exceptions
- TLQUIT ("Illegal conversion base passed to 'strint'", excpIllegalStringConversionBase):
  If ```base``` is not in the valid range of 2 .. 36 (inclusive)
- TLQUIT ("Illegal character in string passed to 'strint'", excpStringFormatIncorrect):
  If a digit in ```string``` is not a valid character for the given ```base```
- TLQUIT ("Overflow in result of 'strint'", excpIntegerOverflow)
  If the conversion process will result in a number that is too large to fit
  inside of an int4

#### Operation
```
// TODO: STRINT
```

-----
### STRINTOK - Check String-to-Integer Conversion
| OP | Name     | Arg0 | Arg1 | Arg2 | Arg3 |
|----|----------|------|------|------|------|
| DE | STRINTOK |  -   |  -   |  -   |  -   |

#### Stack Arguments
| Offset | Type    | Name   |
|--------|---------|--------|
|      0 |    int4 | base   |
|      4 | addrint | string |

#### Stack Returns
| Offset | Type    | Name   |
|--------|---------|--------|
|      0 | addrint | string |
|      0 |    int4 | valid  |

#### Description
Checks if the requested ```string``` can safely be converted into an int4 of
the specified ```base```

If one of the following is true:
- The base is not between the range of 2 .. 36 (inclusive)
- A digit in ```string``` is not a valid character for the given ```base```
- The conversion process will result in a number that is too large to fit
  inside of an int4

then ```valid``` is set to zero. Otherwise, ```valid``` is set to one.

#### Exceptions
- None

#### Operation
```
// TODO: STRINTOK
```

-----
### STRNAT - Convert String to Natural
| OP | Name   | Arg0 | Arg1 | Arg2 | Arg3 |
|----|--------|------|------|------|------|
| DF | STRNAT |  -   |  -   |  -   |  -   |

#### Stack Arguments
| Offset | Type    | Name   |
|--------|---------|--------|
|      0 |    int4 | base   |
|      4 | addrint | string |

#### Stack Returns
| Offset | Type    | Name   |
|--------|---------|--------|
|      0 |    nat4 | result |

#### Description
Convertes the requested ```string``` into a nat4 of the specified ```base```,
storing the number in ```result```.

The valid characters for a base are selected from the following alphabet:
```0123456789abcdefghijklmnopqrstuvwxyz```.
The conversion process is case-insensitive, so capital letters map to the same
lower case letters.

Also, ```+``` is accepted at the beginning of the number string to indicate
the desired signedness of the number (only positives for nat4's).

#### Exceptions
- TLQUIT ("Illegal conversion base passed to 'strnat'", excpIllegalStringConversionBase):
  If ```base``` is not in the valid range of 2 .. 36 (inclusive)
- TLQUIT ("Illegal character in string passed to 'strnat'", excpStringFormatIncorrect):
  If a digit in ```string``` is not a valid character for the given ```base```
- TLQUIT ("Overflow in result of 'strnat'", excpIntegerOverflow)
  If the conversion process will result in a number that is too large to fit
  inside of a nat4

#### Operation
```
// TODO: STRNAT
```

-----
### STRNATOK - Check String-to-Natural Conversion
| OP | Name     | Arg0 | Arg1 | Arg2 | Arg3 |
|----|----------|------|------|------|------|
| E0 | STRNATOK |  -   |  -   |  -   |  -   |

#### Stack Arguments
| Offset | Type    | Name   |
|--------|---------|--------|
|      0 |    int4 | base   |
|      4 | addrint | string |

#### Stack Returns
| Offset | Type    | Name   |
|--------|---------|--------|
|      0 | addrint | string |
|      0 |    int4 | valid  |

#### Description
Checks if the requested ```string``` can safely be converted into a nat4 of
the specified ```base```

If one of the following is true:
- The base is not between the range of 2 .. 36 (inclusive)
- A digit in ```string``` is not a valid character for the given ```base```
- The conversion process will result in a number that is too large to fit
  inside of a nat4

then ```valid``` is set to zero. Otherwise, ```valid``` is set to one.

#### Exceptions
- None

#### Operation
```
// TODO: STRNATOK
```

-----
### STRTOCHAR - Convert String Into Character
| OP | Name      | Arg0 | Arg1 | Arg2 | Arg3 |
|----|-----------|------|------|------|------|
| E1 | STRTOCHAR |  -   |  -   |  -   |  -   |

#### Stack Arguments
| Offset | Type    | Name   |
|--------|---------|--------|
|      0 | addrint | string |

#### Stack Returns
| Offset | Type | Name |
|--------|------|------|
|      0 | char | chr  |

#### Description
Extracts a single character from the string at ```string``` and puts it into
```chr```

#### Exceptions
- ExecutionError ("String coerced to char is not length 1"): If ```string``` is
  not of length 1

#### Operation
```
if StrLen(string) != 1 then
  ExecutionError ("String coerced to char is not length 1")
fi

chr := string[0] // Get the first character
```

----
### SUB* - Subtract
| OP | Name      | Arg0 | Arg1 | Arg2 | Arg3 |
|----|-----------|------|------|------|------|
| E2 | SUBINT    |  -   |  -   |  -   |  -   |
| E3 | SUBINTNAT |  -   |  -   |  -   |  -   |
| E4 | SUBNAT    |  -   |  -   |  -   |  -   |
| E5 | SUBNATINT |  -   |  -   |  -   |  -   |
| E6 | SUBREAL   |  -   |  -   |  -   |  -   |

#### Stack Arguments
| Offset      | Type  | Name       |
|-------------|-------|------------|
|           0 | suba  | subtrahend |
| sizeof suba | subb  | minuend    |

#### Stack Returns
| Offset | Type | Name       |
|--------|------|------------|
|      0 | subb | difference |

#### Type Definition
| Instruction | suba  | subb  |
|-------------|-------|-------|
| SUBINT      | int4  | int4  |
| SUBINTNAT   | nat4  | int4  |
| SUBNAT      | nat4  | nat4  |
| SUBNATINT   | int4  | nat4  |
| SUBREAL     | real8 | real8 |

#### Description
Subtracts ```subtrahend``` from ```minuend``` and stores the result in
```difference```.

#### Exceptions
SUBINT, SUBINTNAT, SUBNAT*, SUBNATINT
- Aborts with Abort (50) "Overflow in Integer expression" if ```difference```
  is too large to fit into the requested type.

\* Note: In TProlog, SUBNAT aborts with Abort (36) "Illegal Parameter to 'chr'".
   This is assumed to be a typo, and thus is included with the other instructions.

SUBREAL*
- Aborts with Abort (51) "Overflow in Real expression" if ```difference```
  is too large to fit into the requested type.

\* Note: In TProlog, SUBREAL aborts with Abort (38) "Pred applied to the first 
   value of the enumeration". This is assumed to be a type, and thus aborts
   with Abort (51).

#### Operation

SUBINT, SUBINTNAT, SUBNAT, SUBNATINT
````
difference := minuend - subtrahend

if CheckOverflow (difference, typeof subb) then
  Abort (50) // Overflow in Integer expression
fi
````

SUBREAL
````
difference := minuend - subtrahend

if CheckOverflow (difference, real8) then
  Abort (51) // Overflow in Real expression
fi
````

-----
### SUBSCRIPT - Subscript Array
| OP | Name      | Arg0 | Arg1 | Arg2 | Arg3 |
|----|-----------|------|------|------|------|
| E7 | SUBSCRIPT |  -   |  -   |  -   |  -   |

#### Stack Arguments
| Offset | Type    | Name            |
|--------|---------|-----------------|
|      0 | addrint | arrayDescriptor |
|      4 | int4    | subscriptN      |
|      8 | int4    | subscriptN-1    |
|        |         | ...             |
|     +4 | int4    | subscript1      |
|     +8 | int4    | subscript0      |
|    +12 | addrint | arrayBase       |

#### Stack Returns
| Offset | Type    | Name     |
|--------|---------|----------|
|      0 | addrint | elemAddr |

#### Description
Computes the ```elemAddr``` for the given array in ```arrayDescriptor```.

The subscript is computed starting from the last dimension back to the first
due to the way that TProlog compiles files. Each dimension subscript is
adjusted to start from zero, and then multiplied by the range of the dimension.
This dimension subscript is accumulated together with the other dimension
subscripts to compute the final element offset.

```arrayBase``` points to the very first element in the array, and is adjusted
by the computed offset to point to the respective element's address. This
address is stored as ```elemAddr```.

#### Exceptions
- Quit(11):
  If the ```subscriptN``` for the given dimension is outside of the
  dimension's range.

-----
### SUBSET - Subtract Sets / Inverted Intersection of Sets
| OP | Name   | Arg0           | Arg1 | Arg2 | Arg3 |
|----|--------|----------------|------|------|------|
| E8 | SUBSET | setSize (nat4) |  -   |  -   |  -   |

#### Stack Arguments
##### Form 1
| Offset | Type    | Name   |
|--------|---------|--------|
|      0 | set32   | rvalue |
|      4 | set32   | lvalue |

##### Form 2
| Offset | Type    | Name   |
|--------|---------|--------|
|      0 | addrint | tvalue |
|      4 | addrint | rvalue |
|      8 | addrint | lvalue |


#### Stack Returns
##### Form 1
| Offset | Type    | Name   |
|--------|---------|--------|
|      0 | set32   | result |

##### Form 2
| Offset | Type    | Name   |
|--------|---------|--------|
|      0 | addrint | tvalue |

#### Description
Computes the subtraction of the two sets ```lvalue``` and ```rvalue```.
All elements in ```rvalue``` are removed from ```lvalue```.

Sets in Turing are represented in two ways
 - As a single set32 / nat4
 - As an array of set16 / nat2's

Due to these two representations, two forms of parameter input are required to
merge each set appropriately.

```setSize``` discriminates between these two types, as it determines the
length of the set in bytes.

If ```setSize``` less than or equal to 4 bytes, parameter
'Form 1' is used and a remove operation (A & ~B) of ```lvalue``` and
```rvalue``` is performed. The resulting set is stored in ```result```

Otherwise, 'Form 2' is used, and the remove operation is performed between all
set values of ```lvalue``` and ```rvalue```, stored in the same location inside
```tvalue```.

```setSize``` applies to the length of both sets, as merging two sets of
different length is not supported in Turing.

#### Exceptions
None

#### Operation
```
if setSize <= 4 then
  // Perform element removal of single word sets
  result := lvalue & (~rvalue)
else
  // Perform word-by-word union of sets
  Tset := PointerTo (set16, tvalue)
  Rset := PointerTo (set16, rvalue)
  Lset := PointerTo (set16, lvalue)

  Words := setSize >> 1 // Get set size in words
  WordIdx := 1

  // TProlog performs a check if 'WordIdx' <= 'Words', but because 'Words' can
  // only be greater than 0, the check is not needed

  loop
    *Tset := (*Lset & ~(*Rset))

    Tset += 2
    Rset += 2
    Lset += 2

    if Words == WordIdx then
      break
    fi

    Words += 1
  pool
fi
```


----
### SUBSTR1 - Substring of String from Bound to End
| OP | Name    | Arg0        | Arg1 | Arg2 | Arg3 |
|----|---------|-------------|------|------|------|
| E9 | SUBSTR1 | kind (nat1) |  -   |  -   |  -   |

#### Stack Arguments
| Offset | Type    | Name   |
|--------|---------|--------|
|      0 | addrint | target |
|      4 | int4    | index  |
|      8 | addrint | source |

#### Stack Returns
| Offset | Type    | Name   |
|--------|---------|--------|
|      0 | addrint | target |

#### Description
Takes a substring from ```source``` using ```index```.
The starting point of the substring depends on the value of ```kind```:

| ```kind```    | Description                                                             |
|---------------|-------------------------------------------------------------------------|
| FromStart (0) | Computes the start index from the start of ```source```, going forwards |
| FromEnd   (1) | Computes the start index from the end of ```source```, going backwards  |

The resulting string subsection is stored in ```target```.

Note: if ```kind``` is FromEnd, ```index``` must be a negative value.

#### Exceptions
- AssertFail (): If ```kind``` is not 0 or 1
- ExecutionError ("Substring index is less than 1"):
  If the computed substring index goes beyond the start of ```source```
- ExecutionError ("Substring index is greater than length of string"):
  If the computed substring index goes beyond the end of ```source```

#### Operation
```
StartIdx := -1
Length := StrLen (source)

if kind == 0 then
  // FromStart
  StartIdx := index
else if kind == 1 then
  // FromEnd
  // !!! NOTE: index must passed as a negative number !!!
  StartIdx := index + Length
else
  Assert (False)
fi

if StartIdx < 0 then
  ExecutionError ("Substring index is less than 1")
elseif StartIdx >= length then
  ExecutionError ("Substring index is greater than length of string")
fi

StrCpy (target, source + StartIdx)
```

-----
### SUBSTR2 - Substring of String from Left Bound To Right Bound
| OP | Name    | Arg0        | Arg1 | Arg2 | Arg3 |
|----|---------|-------------|------|------|------|
| EA | SUBSTR2 | kind (nat1) |  -   |  -   |  -   |

#### Stack Arguments
| Offset | Type    | Name      |
|--------|---------|-----------|
|      0 | addrint | target    |
|      4 | int4    | toIndex   |
|      8 | int4    | fromIndex |
|     12 | addrint | source    |

#### Stack Returns
| Offset | Type    | Name   |
|--------|---------|--------|
|      0 | addrint | target |

#### Description
Takes a substring from ```source``` and copies it to ```target```, using
```fromIndex``` and ```toIndex```

The computed substring indices depends on the value of ```kind```:

| ```kind```        | Description                                          |
|-------------------|------------------------------------------------------|
| BothFromStart (2) | Compute both from the start                          |
| EndFromEnd    (3) | Compute the end index from the end of ```source```   |
| StartFromEnd  (4) | Compute the start index from the end of ```source``` |
| BothFromEnd   (5) | Compute both from the end of ```source```            |

Note: If the selected index is computed based off of the end of ```source```,
      the corresponding value must be negative.

#### Exceptions
- ExecutionError ("Left bound of substring is less than 1"):
  If the computed start index goes beyond the start of ```source```.
- ExecutionError ("Left bound of substring exceeds right bound by more than 1"):
  If the computed start index goes beyond the end of ```source```.
- ExecutionError ("Right bound of substring is greater than length of string"):
  If the computed end index goes beyond the end of ```source```.
- Quit (25):
  If ```kind``` is not one of the accepted values.

#### Operation
```

StartIdx := fromIndex
EndIdx := toIndex
Length := StrLen (source)

case kind of
  label 2: // BothFromStart
    StartIdx += 0
    EndIdx += 0
  label 3: // EndFromEnd
    StartIdx += 0
    EndIdx += Length
  label 4: // StartFromEnd
    StartIdx += Length
    EndIdx += 0
  label 5: // BothFromEnd
    StartIdx += Length
    EndIdx += Length
esac

if StartIdx < 0 then
  ExecutionError ("Left bound of substring is less than 1")
elseif StartIdx >= Length then
  ExecutionError ("Left bound of substring exceeds right bound by more than 1")
elseif EndIdx >= Length then
  ExecutionError ("Right bound of substring is greater than length of string")
fi

// Compute substring length
SubstrLen := EndIdx - StartIdx - 1
StrnCopy (target, source + StartIdx, SubstrLen)

```

-----
### SUCC - Successor of
| OP | Name | Arg0 | Arg1 | Arg2 | Arg3 |
|----|------|------|------|------|------|
| EB | SUCC |  -   |  -   |  -   |  -   |

#### Stack Arguments
| Offset | Type | Name  |
|--------|------|-------|
|      0 | int4 | value |

#### Stack Returns
| Offset | Type | Name  |
|--------|------|-------|
|      0 | int4 | next  |

#### Description
Gets the successor value of ```value```, storing the result in ```next```.

#### Exceptions
- Abort (50): If ```value``` is equal to <b>maxint</b>

#### Operation
```
if value == maxint then
  Abort (50) // Overflow in Integer expression
fi

next := value + 1
```

----
### TAG - Tag Union
| OP | Name | Arg0           | Arg1 | Arg2 | Arg3 |
|----|------|----------------|------|------|------|
| EC | TAG  | tagSize (int1) |  -   |  -   |  -   |

#### Stack Arguments
| Offset | Type    | Name            |
|--------|---------|-----------------|
|      0 | addrint | unionDescriptor |
|      4 | addrint | uninit          |
|      8 | int4    | tagValue        |
|     12 | addrint | union           |

#### Stack Returns
None

#### Description
Updates the tag of ```union``` to the value of ```tagValue```.
Also uninitializes the union with the specified ```uninit``` pattern.

```tagSize``` determines the size of the tag, in bytes.

#### Exceptions
- Abort (14):
  If ```tagValue``` is outside the accepted range of tag values for the union.
- Abort (25):
  If ```tagSize``` is not one of the accepted values: 1, 2, 4.

#### Operation
```
if tagValue < unionDescriptor.lowest || tagValue > unionDescriptor.highest then
  Abort (14) // Tag value is out of range
fi

case tagSize of
  label 1:
    *PointerTo(int1, union) := tagValue
  label 2:
    *PointerTo(int2, union) := tagValue
  label 4:
    *PointerTo(int4, union) := tagValue
esac

// Fetch the specific uninit pattern
Pattern := GetUninitPatternForUnion (
    unionDescriptor,
    uninit,
    tagValue - unionDescriptor.lowest)

UninitArea (union, Pattern)
```

----
### TELL - Tell Current File Offset
| OP | Name  | Arg0 | Arg1 | Arg2 | Arg3 |
|----|-------|------|------|------|------|
| ED | TELL  |  -   |  -   |  -   |  -   |

#### Stack Arguments
| Offset | Type    | Name     |
|--------|---------|----------|
|      0 | int4    | streamNo |
|      0 | addrint | tellTo   |

#### Stack Returns
None

#### Description

-----
### UFIELD - Get Union Field
| OP | Name   | Arg0              | Arg1               | Arg2           | Arg3 |
|----|--------|-------------------|--------------------|----------------|------|
| EE | UFIELD | fieldOff (offset) | expectMap (offset) | tagSize (nat4) |  -   |

#### Stack Arguments
| Offset | Type    | Name       |
|--------|---------|------------|
|      0 | addrint | descriptor |
|      4 | addrint | instance   |

#### Stack Returns
| Offset | Type    | Name      |
|--------|---------|-----------|
|      0 | addrint | fieldAddr |

#### Description
Computes the address to access a specific field in a union by adding
```fieldOff``` to the base ```instance``` address, storing the address in
```fieldAddr```.

Before the computation of the field address occurs, validation of the access
is performed. If the expected mapping of the union does not match
```expectMap```, the access fails.

#### Exceptions
- Abort (34):
  If the tag value for ```instance``` has not been set yet.
- Abort (13):
  If ``expectMap`` does not match the expected mapping for the ```instance```'s
  tag
- Abort (25):
  If ```tagSize``` is not one of the accepted values: 1, 2, 4.

#### Operation
```
// Validate field access
TagValue := 0

case tagSize of
  label 1:
    TagValue := *PointerTo (int1, instance)
  label 2:
    TagValue := *PointerTo (int2, instance)
  label 4:
    TagValue := *PointerTo (int4, instance)
esac

if TagValue < descriptor.lowest || TagValue > descriptor.highest then
  Abort (34) // Union tag has not been set
fi

TagIndex := TagValue - descriptor.lowest
TagMapping := descriptor -> mappings [TagIndex]

if TagMapping != expectMap then
  Abort (13) // Union field is not in alternative selected by current tag value
fi

fieldAddr := fieldOff + instance
```

----
### UNINIT - Unitialize Area
| OP | Name          | Arg0 | Arg1 | Arg2 | Arg3 |
|----|---------------|------|------|------|------|
| EF | UNINIT        |  -   |  -   |  -   |  -   |

#### Stack Arguments
| Offset | Type    | Name    |
|--------|---------|---------|
|      0 | addrint | pattern |
|      4 | addrint | varAddr |

#### Stack Returns
None

#### Description
Uninitializes the memory area pointed to by ```varAddr``` using the pattern
given in ```pattern```.

See "tlib/TL/TLB/TLBUNR.ch" from the Turing+ source code for more details on
the specifics of ``pattern``.

#### Exceptions

#### Operation
```
// TODO: UNINIT
```
-----
### UNINIT* - Unitialize Variable
| OP | Name          | Arg0 | Arg1 | Arg2 | Arg3 |
|----|---------------|------|------|------|------|
| F0 | UNINITADDR    |  -   |  -   |  -   |  -   |
| F1 | UNINITBOOLEAN |  -   |  -   |  -   |  -   |
| F2 | UNINITINT     |  -   |  -   |  -   |  -   |
| F3 | UNINITNAT     |  -   |  -   |  -   |  -   |
| F4 | UNINITREAL    |  -   |  -   |  -   |  -   |
| F5 | UNINITSTR     |  -   |  -   |  -   |  -   |

#### Stack Arguments
| Offset | Type    | Name    |
|--------|---------|---------|
|      0 | addrint | varAddr |

#### Stack Returns
None

#### Description
Uninitializes the variable pointed to at by the address
Each instruction writes its own uninitialized value:

| Instruction   | Uninit Value        |
|---------------|---------------------|
| UNINITADDR    | 0xFFFFFFFF          |
| UNINITBOOLEAN | 0xFF                |
| UNINITINT     | 0x80000000          |
| UNINITNAT     | 0xFFFFFFFF          |
| UNINITREAL    | 0x80000000_80000000 |
| UNINITSTR     | 0x80 0x00           |

#### Exceptions
None

#### Operation
```
*varAddr := UninitValue
```

-----
### UNLINKHANDLER - Unlink Handler
| OP | Name          | Arg0 | Arg1 | Arg2 | Arg3 |
|----|---------------|------|------|------|------|
| F6 | UNLINKHANDLER |  -   |  -   |  -   |  -   |

#### Stack Arguments
None

#### Stack Returns
None

#### Description
Restores the previous exception handler before the execution of the 
BEGINHANDLER instruction.

#### Exceptions
None

#### Operation
```
ActiveHandler := ActiveHandler -> nextHandler
```

-----
### VSUBSCRIPT - Vector Subscript (Single Dimension Arrays)
| OP | Name       | Arg0      | Arg1     | Arg2     | Arg3 |
|----|------------|-----------|----------|----------|------|
| F7 | VSUBSCRIPT | oElemSize | iLoBound | iHiBound |  -   |

#### Stack Arguments
| Offset | Type    | Name        |
|--------|---------|-------------|
|      0 | int4    | arrayIndex  |
|      4 | addrint | arrayMemory |

#### Stack Returns
| Offset | Type    | Name        |
|--------|---------|-------------|
|      0 | addrint | subscriptAt |

#### Description
Computes the array address corresponding to the given subscript
```arrayIndex``` and ```arrayMemory```. The computed address is stored in
```subscriptAt```.

```arrayIndex``` is the requested array index to subscript.

```arrayMemory``` points to the memory area of the coressponding array.

```subscriptAt``` points to the computed address of the selected array element

The address is computed as follows:
```
subscriptAt := arrayMemory + (arrayIndex - iLoBound) * oElemSize
```

#### Exceptions
- Abort(11) "Array subscript is out of range":
  If ```arrayIndex``` is outside of the range of ```iLoBound``` .. ```iHiBound```

-----
### WAIT - Wait
| OP | Name  | Arg0       | Arg1 | Arg2 | Arg3 |
|----|-------|------------|------|------|------|
| F8 | WAIT  | i1CondKind |  -   |  -   |  -   |

#### Stack Arguments (i1CondKind == Normal, Deferred)
| Offset | Type    | Name         |
|--------|---------|--------------|
|      0 | addrint | conditionVar |

#### Stack Arguments (i1CondKind == Priority)
| Offset | Type    | Name         |
|--------|---------|--------------|
|      0 | addrint | conditionVar |
|      4 | int4    | priority     |

#### Stack Arguments (i1CondKind == Timeout)
| Offset | Type    | Name         |
|--------|---------|--------------|
|      0 | addrint | conditionVar |
|      4 | int4    | timeout      |

#### Stack Returns
None

#### Description
Waits on the given ```conditionVar``` and blocks the current process.
The current process is added to the ```conditionVar```'s waiting queue.

Valid values of ```i1CondKind``` are
| ```i1CondKind``` | Name     | Deferred Wakeup |
|------------------|----------|-----------------|
|                0 | Normal   | No              |
|                1 | Priority | No              |
|                2 | Deferred | Yes             |
|                3 | Timeout  | Yes             |

A process may either be woken up immediately upon a corresponding SIGNAL
execution, or woken up after the process exits the condition's associated
monitor. Condition kinds where the wakeup is deferred are indicated in the
table above.

The order in which processes wake up also depends on ```i1CondKind```.
If ```i1CondKind``` is 'Priority', processes with a higher priority will be
woken up first, and the priority of the wakeup is given in ```priority```.
Otherwise, processes are woken up in the order in which they waited in.

If ```i1CondKind``` is 'Timeout', a process can also wake up after a specified
period of time before a corresponding SIGNAL execution. The timeout length is
specified in ```timeout```.
Timeout durations are measured in reference to the internal simulation time
instead of real time, and are thus dependent on executions of the PAUSE command.

#### Exceptions
None

-----
### WRITE - Write Bytes Out
| OP | Name  | Arg0 | Arg1 | Arg2 | Arg3 |
|----|-------|------|------|------|------|
| F9 | WRITE |  -   |  -   |  -   |  -   |

#### Stack Arguments
| Offset | Type | Name |
|--------|------|------|
|      0 | type | name |

#### Stack Returns
| Offset | Type | Name |
|--------|------|------|
|      0 | type | name |

#### Description

-----
### XOR - Bitwise eXclusive-OR
| OP | Name  | Arg0 | Arg1 | Arg2 | Arg3 |
|----|-------|------|------|------|------|
| FA | XOR   |  -   |  -   |  -   |  -   |

#### Stack Arguments
| Offset | Type | Name |
|--------|------|------|
|      0 | nat4 | rval |
|      4 | nat4 | lval |

#### Stack Returns
| Offset | Type | Name   |
|--------|------|--------|
|      0 | nat4 | result |

#### Description
Performs a bitwise logical-XOR operation between ```lval``` and ```rval```, and 
stores the result in ```result```

#### Exceptions
None

#### Operation
```
result := lval ^ rval
```

-----
### XORSET - eXclusive-OR Of Sets
| OP | Name   | Arg0           | Arg1 | Arg2 | Arg3 |
|----|--------|----------------|------|------|------|
| FB | XORSET | setSize (nat4) |  -   |  -   |  -   |

#### Stack Arguments
##### Form 1
| Offset | Type    | Name   |
|--------|---------|--------|
|      0 | set32   | rvalue |
|      4 | set32   | lvalue |

##### Form 2
| Offset | Type    | Name   |
|--------|---------|--------|
|      0 | addrint | tvalue |
|      4 | addrint | rvalue |
|      8 | addrint | lvalue |


#### Stack Returns
##### Form 1
| Offset | Type    | Name   |
|--------|---------|--------|
|      0 | set32   | result |

##### Form 2
| Offset | Type    | Name   |
|--------|---------|--------|
|      0 | addrint | tvalue |

#### Description
Computes the exclusive-or of the two sets ```lvalue``` and ```rvalue```.

Sets in Turing are represented in two ways
 - As a single set32 / nat4
 - As an array of set16 / nat2's

Due to these two representations, two forms of parameter input are required to
merge each set appropriately.

```setSize``` discriminates between these two types, as it determines the
length of the set in bytes.

If ```setSize``` less than or equal to 4 bytes, parameter
'Form 1' is used and a simple bitwise XOR ```lvalue``` and ```rvalue``` is
performed. The resulting set is stored in ```result```

Otherwise, 'Form 2' is used, and the bitwise XOR is performed between all set
values of ```lvalue``` and ```rvalue```, stored in the same location inside
```tvalue```.

```setSize``` applies to the length of both sets, as merging two sets of
different length is not supported in Turing.

#### Exceptions
None

#### Operation
```
if setSize <= 4 then
  // Perform xor of single word sets
  result := lvalue ^ rvalue
else
  // Perform word-by-word union of sets
  Tset := PointerTo (set16, tvalue)
  Rset := PointerTo (set16, rvalue)
  Lset := PointerTo (set16, lvalue)

  Words := setSize >> 1 // Get set size in words
  WordIdx := 1

  // TProlog performs a check if 'WordIdx' <= 'Words', but because 'Words' can
  // only be greater than 0, the check is not needed

  loop
    *Tset := (*Lset ^ *Rset)

    Tset += 2
    Rset += 2
    Lset += 2

    if Words == WordIdx then
      break
    fi

    Words += 1
  pool
fi
```

-----
### BREAK - Breakpoint
| OP | Name  | Arg0 | Arg1 | Arg2 | Arg3 |
|----|-------|------|------|------|------|
| FC | BREAK |  -   |  -   |  -   |  -   |

#### Stack Arguments
None

#### Stack Returns
None

#### Description
Causes the execution of the current program to stop, and begins the entry into
the debug context.

#### Exceptions
None

#### Operation
```
Breakpoint()
```

-----
### SYSEXIT - Quit Program
| OP | Name    | Arg0 | Arg1 | Arg2 | Arg3 |
|----|---------|------|------|------|------|
| FD | SYSEXIT |  -   |  -   |  -   |  -   |

#### Stack Arguments
| Offset | Type | Name     |
|--------|------|----------|
|      0 | nat4 | exitCode |

#### Stack Returns
None

#### Description
Quits the entire program, and stops all execution of code. The reason for 
quitting the program is given in the ```exitCode``` argument.

If the exit code is not zero, the program quit is treated as an error and 
handled appropriately.

#### Exceptions
None

#### Operation
```
SystemExit (exitCode)
```

-----
### ILLEGAL - Illegal Opcode
| OP | Name    | Arg0 | Arg1 | Arg2 | Arg3 |
|----|---------|------|------|------|------|
| FE | ILLEGAL |  -   |  -   |  -   |  -   |

#### Stack Arguments
None

#### Stack Returns
None

#### Description
Triggers an execution error indicatine that an illegal opcode was encountered.

#### Exceptions
- ExecutionError("Undecodable opcode #254"):
  Whenever this opcode is encountered

#### Operation
```
ExecutionError("Undecodable opcode #254")
```
