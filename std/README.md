## Standardization

### Arguments
Functions take arguments starting with `rax` then `rbx` until `rdx` then `r8` to `r11` and so on.
<br>
### Return
Functions return on `rdi` and `rsi` then `r12` to `r14` if needed.
<br>
### Args
The pointer to argc is always in r15 so this value should not be modified.
<br>
### Polymorphism
std library functions do not contain polymorphism instead they are split separete files that will be contained in language files.
<br>
### Namespace functions
in assembly functions that are prefix with a namespace e.g. `num.print` are represented with a `~` e.g. `num~print`.
<br>
### Registers
std library function are never allowed to modify the register values outside of the ones used for arguments and return values.
