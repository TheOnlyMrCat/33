#  Ska

Ska is an esolang created to use letter characters instead of the strange symbols other esolangs use. Yes, there are some
symbols, such as [] for loops, {} for functions, and "" and '' for strings, but they're more standard than the stuff some other
esolangs use.

## Installation
I am unable to make binaries for the interpreter at this point, so to use this you'll have to get `main.cpp` and `cxxopts.hpp`.
Compile using C++17.

## Output
There are 3 output commands:

* `p`: Prints the string in the source string register to standard output
* `o`: Prints the value in the accumulator to standard output
* `i`: Prints a newline

## Input
There is currently no way to get user input.

## Memory
There are 2 integer memory registers, a general integer memory, two string memory registers, and *(unimplemented)* 2 list memory registers.

### Integers
Of the two integer memory registers, the accumulator is the one that can have mathematical operations performed on it, the
one that can be checked conditionally, and the one that can be read from/written to general memory. The counter is the one that can 
have values put into it directly.

The general memory is accessed via strings.

#### Arithmetic operations

* `0-9`: Multiplies the counter by 10, then adds itself. For example: `50coc7co` will output `50507`
* `a`: Adds the counter to the accumulator
* `m`: Subtracts the counter from the accumulator
* `x`: Multiplies the accumulator by the counter
* `d`: Divides the accumulator by the counter
* `r`: Sets the accumulator to the remainder of the accumulator divided by the counter
* `z`: Sets the counter to 0
* `c`: Swaps the counter and the accumulator

#### Memory operations
* `s`: Stores the accumulator's current value at the location in the destination string register
* `l`: Loads the value in memory referenced by the source string register into the accumulator

### Strings
There are two string registers, a source, and a destination. Their functions are described in other sections. The source is set by
enclosing a string in double quotes (`""`), and the destination by single quotes (`''`).

#### Modification operations
* `t`: Swaps the source string and the destination string
* `j`: Sets the accumulator to the value of a character in the source string, referenced by the counter
* `k`: Sets a character in the destination string, referenced by the counter, to the character represented by the value in the accumulator.
	* Note: if the destination string isn't long enough to accommodate this character, any extra space is filled in with space characters (` `).

### Lists
There are two list registers. One that you can perform operations on, and one for simply storing a second list. The list is initially filled in with `argv`

## Flow control
### Conditionals
Conditional operators skip the next operation if the condition is not met.
* `n`: Performs the next operation if the accumulator is equal to 0.
* `N`: Performs the next operation if the accumulator is not equal to 0.
* `g`: Performs the next operation if the accumulator is greater than 0.
* `G`: Performs the next operation if the accumulator is not greater than 0 (i.e. less than or equal to 0).
* `h`: Performs the next operation if the accumulator is less than 0.
* `H`: Performs the next operation if the accumulator is not less than 0 (i.e. greater than or equal to 0).

### Functions
Functions use the string registers to create and call. The destination register is queried when creating a function, and the source
when calling. Creating a function is as simple as setting the destination register, then enclosing a block of code in braces, for
example: `'Print'{"Hello, World!'p}`.

To call a function, you must set the source register, then perform the `q` operation. Following on from the previous example, that
would be: `"Print"q`.

Register values are kept when changing stack frames. This means you can do stuff like this:

```
{"Hello"p}'Hello'{", "p}', '{"World"p}'World'{"!"pi}qqqq
```

to print "Hello, World!"

### Loops
Loops are notated using square brackets (`[]`). When a closing bracket is found, the code jumps back to the matching opening 
bracket if the accumulator is not 0. For example: This code `10az1[oim]"Done."pi` will count down from 10 until it reaches 1,
then it will print "Done.". To make it count down until it reaches 0, this code would be used: `11az1[moi]"Done."pi`.

## Synopsis:
* `a`: Adds the counter to the accumulator
* `b`: Sets the destination string to a string in the mutable list, specified by the counter
* `c`: Swaps the counter and the accumulator
* `d`: Divides the accumulator by the counter
* `e`: Appends the source string to the destination string
* `f`: Sets a value in the mutable list, specified by the counter, to the source string
* `g`: Performs the next operation if the accumulator is greater than 0.
* `G`: Performs the next operation if the accumulator is not greater than 0.
* `h`: Performs the next operation if the accumulator is less than 0.
* `H`: Performs the next operation if the accumulator is not less than 0.
* `i`: Prints a newline to standard output
* `j`: Sets the accumulator to the value of a character in the source string, referenced by the counter
* `k`: Sets a character in the destination string, referenced by the counter, to the character represented by the value in the accumulator.
* `l`: Loads the value in memory referenced by the source string register into the accumulator
* `m`: Subtracts the counter from the accumulator
* `n`: Performs the next operation if the accumulator is equal to 0.
* `N`: Performs the next operation if the accumulator is not equal to 0.
* `o`: Prints the value of the accumulator to standard output
* `p`: Prints the string in the source string register to standard output
* `q`: Calls the function from the source string
* `r`: Divides the accumulator by the counter but keeps the remainder
* `s`: Stores the value of the accumulator in memory referenced by the destination string
* `t`: Swaps the source and destination strings
* `u`: Swaps the mutable and backup lists
* `v`: Appends the backup list to the mutable list
* `w`: Sets the accumulator to the length of the list
* `x`: Multiplies the accumulator by the counter
* `y`: Sets the list to the value of the source string split by the regex value in the destination string
* `z`: Sets the counter to 0
* `Z`: Clears the mutable list
