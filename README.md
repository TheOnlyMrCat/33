#  33

33 is an esolang created to use letter characters instead of the strange symbols other esolangs use. Yes, there are some
symbols, such as [] for loops, {} for functions, and "" and '' for strings, but they're more standard than the stuff some other
esolangs use. There are 33 distinct letter commands in the language.

## Installation
Get the latest release from the [releases page](https://github.com/TheOnlyMrCat/33/releases) of this repository. Put it on
your $PATH if need be.

## Running a program
After installing the interpreter, execute it with `-f <filename>` to execute a file. Adding the `-d` flag will show a debugger.

### The debugger
The debugger allows you to step through the program, one instruction at a time, and see the internal values of the registers.
It prints the values stored in the accumulator, counter, source string, destination string, and mutable list (in that order).

## Input/Output
There are 3 output commands:

* `p`: Prints the string in the source string register to standard output
* `o`: Prints the value in the accumulator to standard output
* `i`: Prints a newline

There are three input commands:

* `P`: Sets the accumulator to the value of the next character read from stdin
* `O`: Sets the accumulator to the next integer read from stdin
* `I`: Sets the destination string to the next line read from stdin

## Memory
There are 2 integer memory registers, a general integer memory, two string memory registers, and 2 list memory registers.

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

#### List operations
* `b`: Sets the destination string to a string in the mutable list, specified by the counter
* `f`: Sets a value in the mutable list, specified by the counter, to the source string
* `u`: Swaps the mutable and backup lists
* `v`: Appends the backup list to the mutable list
* `w`: Sets the accumulator to the length of the mutable list
* `y`: Appends the source string to the mutable list
* `Z`: Clears the mutable list

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

## Program termination
When the program reaches the end of input, the interpreter terminates. No printing is done after this. The program can be
terminated early with the `@` operator.

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
* `I`: Sets the destination string to the next line read from stdin
* `j`: Sets the accumulator to the value of a character in the source string, referenced by the counter
* `k`: Sets a character in the destination string, referenced by the counter, to the character represented by the value in the accumulator.
* `l`: Loads the value in memory referenced by the source string register into the accumulator
* `m`: Subtracts the counter from the accumulator
* `n`: Performs the next operation if the accumulator is equal to 0.
* `N`: Performs the next operation if the accumulator is not equal to 0.
* `o`: Prints the value of the accumulator to standard output
* `O`: Sets the accumulator to the next integer input
* `p`: Prints the string in the source string register to standard output
* `P`: Sets the accumulator to the value of the next character read from stdin
* `q`: Calls the function from the source string
* `r`: Divides the accumulator by the counter but keeps the remainder
* `s`: Stores the value of the accumulator in memory referenced by the destination string
* `t`: Swaps the source and destination strings
* `u`: Swaps the mutable and backup lists
* `v`: Appends the backup list to the mutable list
* `w`: Sets the accumulator to the length of the mutable list
* `x`: Multiplies the accumulator by the counter
* `y`: Appends the source string to the mutable list
* `z`: Sets the counter to 0
* `Z`: Clears the mutable list
