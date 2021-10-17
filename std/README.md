## Standardization

### Arguments
Functions take arguments starting with `rax` then `rbx` until `rdx` then `r8` to `r11` and so on. After this they're passed on the stack.
<br>
### Return
Functions return on `rsi` and `rdi` then `r12` to `r14` if needed. There is currently no planned way to return more than 4 without being in an array or tuple.
<br>
### Args
The pointer to argc is always in `r15` so this value should not ever be modified.
<br>
### Polymorphism
std library functions do not contain polymorphism instead they are split separete files that will be contained in language files.
<br>
### Namespace functions
In assembly functions that are prefix with a namespace, e.g. `mem.allocate`, are represented with a `#` e.g. `mem#allocate`. <br>
Additionally functions that are object oriented, e.g. `int~print`, are represented with a `~` e.g. `int~print`. These function always take the object they're operating on as rax.
<br>
### Registers
std library function are never allowed to modify the register values outside of the ones used for arguments and return values. <br>
Note: Some functions do modify registers especially the ones that use syscalls. This is not intenional and should be treated as a bug.
