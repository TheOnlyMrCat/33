#  Ska

Ska is an esolang created to use letter characters instead of the strange symbols other esolangs use. Yes, there are some
symbols, such as [] for loops, {} for functions, and "" and '' for strings, but they're more standard than the stuff some other
esolangs use.

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

* `0-9`: Multiplies the counter by 10, then adds itself. For example: `50o7o` will output `50507`
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
enclosing a string in double quotes (`""`), and the destination by single quotes (`''`). They are swapped by the operation `t`.

### Lists
*To be added*

## Flow control
### Conditionals
Conditional operators skip the next operation if the condition is not met.
* `n`: Performs the next operation if the accumulator is 0.
* `N`: Performs the next operation if the accumulator is not 0.
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
