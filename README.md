 # Mycelium
 Mycelium is a language built with scripting principles like lists, automatic types, multiple return values, etc. but the language will ultimately be converted to assembly to be compiled and ran natively. Currently the std library of the language is being written with work on the compiler coming later. The plan is to bootstrap the language in assembly to interface with the std library and once it is complete enough it will be converted to self hosting. However, this is subject to change.

 ## std library
 The Mycelium std library is entirely written in assembly for x86_64 linux with no dependencies. Eventually it may be expanded to include other platforms or architectures. <br>
 Currently the std library is the only part of the project. Once the std library is extensive enough work on the actual language will begin. <br>

 ## How to Contribute
  [![PRs Welcome](https://img.shields.io/badge/PRs-welcome-brightgreen.svg)](http://makeapullrequest.com)<br>
  - Please submit any issues you have [here](https://github.com/mossx-dev/Mycelium/issues/new/choose)
  - If you need to contact me directly send an email to <moss@mossx.net>

## Planned Features
### Polymorphism
A function can be declared multiple times with different arguments and do different things based on which arguments it are used.
e.g. two print functions that take a string and an int could be decalred as. 
``` c++
void print(string str) {
    ...
}
```
and
``` c++
void print(int num) {
    ...
}
```
Then when they're called the compiler will decide which to use when called. (might add functionallity for return based polymorhism)

### Native Assembly Functions
A function can be declared then defined in an attached assembly file
e.g. a function `foo` can be declared as.

``` c++
int asm foo(int a);
```
and defined as.

``` asm
foo:
    ...
    ret
```
Where the value in rax is the argument passed and rsi is the return value


### Inline Functions
A function can be decalred with the keyword inline and after being turn into assembly code it will directly be placed wherever it is called
e.g. a function `add` can be defined as.
``` c++
int add(int a, int b) {
    return a + b;
}
```
It will compile into
``` asm
add:
    add     rax, rbx
    mov     rsi, rax
    ret
```
However if defined like this
``` c++
inline add(int a, int b) {
    return a + b;
}
```
And called like 
``` c++
int a = 1;
int b = 1;
add(a, b);
```
The whole thing would compile into
``` asm
mov     rax, 1
mov     rbx, 1
add     rax, rbx
```

### Special Replace 
a function can be define as normal then a special case can be added in assembly to do something when a certain value is passed
e.g. a function to get the digits in a number can be defined as
``` c++
void digits(int num) {
    ...
}
```
Then a special case can be defined when zero is passed as
``` c++
PLACEHOLDER digits(0) {
    mov     rsi, 1
    ret
}
```
Meta_Note: PLACEHOLDER should eventually be replaced with the proper keyword once it is decided.



 ## Contributors
 - Moss Gallagher - [mossx-dev](https://github.com/mossx-dev)
 
 
