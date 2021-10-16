## std/arr
### More Metadata
Currently arrays hold two metadata values, size and data type. They are stored before the user data in the format of [size, data type, ...]. <br>
#### Planned Changes
 * [ ] Change size to used size
 * [ ] Add memory size for more better resize and push performance
 * [ ] Add array type for 2d, const, and other planned array types
 * [ ] Add a type specific extra metadata size (the offset from that data to the user data)
 
## std/out
### Buffered Output
* [ ] Currently everytime any output is written it is done when the function is called. This should be replaced with a buffered output where the buffer is written when it reaches the end or a special character is output e.g. `\n`

## std/exception
### Exception Array
* [ ] Currently all exceptions are declared individually and retrieved with a switch statement. This should be changed to an indexable array similar to the one used in std/type
### Throws with Arguments
* [ ] Currently the same error message is printed every time an exception is printed. It would be nice if an error argument could be pass. For example with a bad array index it would be nice to print the bad index and the size of the array. 
### Stacktrace
* [ ] Currently when an error is thrown there is no way to know where the error occurred. This would help debug and make error messages seem more important.
