 # Mycelium
 Mycelium is a language built with scripting principles like lists, automatic types, multiple return values, etc. but the language will ultimately be converted to assembly to be compiled and ran natively. Currently the std library of the language is being written with work on the compiler coming later. The plan is to bootstrap the language in assembly to interface with the std library and once it is complete enough it will be converted to self hosting. However, this is subject to change.

 ## std library
 The Mycelium std library is entirely written in assembly for x86_64 linux with no dependencies. Eventually it may be expanded to include other platforms or architectures. <br>
 Work on the language has begun. The std library is still being developed but more focus is currently on the compiler. <br>

## Important Update
Mycelium is going to be completely rewritten in C++. The reason for this change is the same reason there have been barely any updates to the project in months. I have been very busy and every time I try to code in assembly I can't get anything meaningful done. So hopefully now I can get back to working on this project again. As for the std library I figure someone might find it useful so I created a [repo for it](https://github.com/mossx-dev/Mycore) but it will not be actively maintained.

## Milestones
- [ ] tokenizer
- [ ] parser
- [ ] interpreter
  - [ ] std functions (e.g. print)
  - [ ] user defined functions
  - [ ] std operations (e.g. adding ints)
  - [ ] user defined classes
  - [ ] user defined operations
- [ ] type checker
- [ ] compiler
  - [ ] std functions (e.g. print)
  - [ ] user defined functions
  - [ ] std operations (e.g. adding ints)
  - [ ] user defined classes
  - [ ] user defined operations
- [ ] self-compilation


## Syntax
Note: This is a work in progress and is completely subject to change.
### Variables
Variables are decalred with a [type](Types) which define how they function. 
e.g.

``` c++
string my_string;
int my_int = 5;
```

There are two special types in the std library which can be used as an alternative.
They are `any` and `auto`. `any` will allow a variable to hold any type. `any` requires a type to be provided when first declared and when the type is changed. `auto` is the same as any but will decide it's type based on assignment and does not require explicit type declaration. Both `any` and `auto` require an extra byte to store the current type.

e.g. of `any`
``` c++
// TODO: Figure out how this syntax will work
```

e.g. of `auto`
``` c++
auto var1 = "Hello";

var1 = 5;
```


### Functions
Functions are decalred with the `fn` keyword. 
Then return types and amount are defined. 
This can be omitted if the function does not return anything. 
Next the function name is defined. Then the arguments.

e.g. a function that takes two `int`s and returns two `int`s
``` c++
fn<int, int> my_func(int a, int b) {
	...
}
```
Functions can also use the `any` and `auto` types
e.g.

``` c++
fn<any> my_other_func(auto a) {
	...
}
```
A function that returns nothing and takes no arguments could be defined as.

``` c++
fn my_third_func() {
	...
}
```
Or defined as.

``` c++
fn<> my_other_third_func() {
	...
}
```
#### Arguments
A function can also use custom argument syntax
e.g. a function that uses semi-colons instead of commas

``` c++
fn my_semi_colon_func(int a; int b) {
	...
}
```
or a function could take arguments in the form of a string

``` c++
// TODO: Change this syntax
fn my_weird_func("My arguments are" auto a "and" auto b) {
	...
}
```
#### Conditionals
A function can use the special keyword `cn` and then is a conditional which takes a function and any other arguments. 
It should return a boolean to indicate that the condition was true. This can be used to define custom if statements
e.g. a conditional that takes a string and executes the code if the string is empty
``` c++
cn if_empty_string(fn func, string str) {
	if (str.is_empty) {
		func.run();
		return true;
	}
	return false;
}


if_empty_string("") {
	println("String is empty");
}
else {
	println("String is not empty");
}
```
#### Loops
Using the conditional function type and the cond argument type a loop can be made.
Note: infinite recursion will not result in a stack overflow but will cause errors after 18 quintillion times (when the recursion counter goes negative). 

e.g. A loop that runs n number of times
```c++
cn loop_n_times(fn func, int counter) {
	if (n != 0) {
		func.run();
		loop_n_times(func, counter - 1);
	}
	return false;
}


loop_n_times(5) {
	println("loop");
}
```


### Classes


### Types


## Planned Features
### Note
All syntax here is in development and is most likely going to change.
### Polymorphism
A function can be declared multiple times with different arguments and do different things based on which arguments it are used.
e.g. two print functions that take a string and an int could be decalred as. 
``` c++
fn print(string str) {
    ...
}
```
and
``` c++
fn print(int num) {
    ...
}
```
They would compile to

``` asm
print(string):
```
and 

``` asm
print(int):
```

Then when they're called the compiler will decide which to use when called.
<br>
A function can also be decalred with different return types. Note that the contents of the function should be changed as little as possible to avoid confusion.
e.g. two functions that divide an integer where one returns just the quotent and the other returns the quotent and the remainder.

``` c++
fn<int> div(int a, int b) {
	...
	return result;
}
```
and

``` c++
fn<int, int> div(int a, int b) {
	...
	return result, remainder;
}
```
They would compile to

```asm
int_div(int_int):
	...
	ret
```
and 

``` asm
int_int_div(int_int):
	...
	ret
```


### Operator Overloading
A class can be defined as.

``` c++
class pair {
	...
}
```
Then the `+` operator could be defined with

``` c++
op<this + pair b> {
	...
}
```
This would compile into

``` asm
pair.op_043:
	...
```

Then the `+` operator can be use a you would with anything else e.g.

``` c++
pair first_pair;
pair second_pair;

first_pair + second_pair;
```

### Native Assembly Functions
A function can be declared then defined in an attached assembly file
e.g. a function `foo` can be defined as.

``` c++
int asm foo(int a) {
	...
	ret
}
```
Or could be decalred as.
``` c++
int asm foo(int a);
```
and defined in a separate file as
``` asm
int_foo(int):
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
int_add(int_int):
    add     rax, rbx
    mov     rsi, rax
    ret
```
However if defined like this
``` c++
inline int add(int a, int b) {
    return a + b;
}
```
And called like 
``` c++
add(1, 1);
```
The whole thing would compile into
``` asm
mov     rax, 1
mov     rbx, 1
add     rax, rbx
```

### Special Replace 
#### This might not be implemented
a function can be define as normal then a special case can be added to do something when a certain value is passed
e.g. a function to get the digits in a number can be defined as
``` c++
int digits(int num) {
    ...
}
```
Then a special case can be defined when zero is passed as
``` c++
PLACEHOLDER digits(0) {
	return 1;
}
```
Note: PLACEHOLDER should eventually be replaced with the proper keyword once it is decided.



## How to Contribute
[![PRs Welcome](https://img.shields.io/badge/PRs-welcome-brightgreen.svg)](http://makeapullrequest.com)<br>
- Please submit any issues you have [here](https://github.com/mossx-dev/Mycelium/issues/new/choose)
- If you need to contact me directly send an email to <moss@mossx.net>



 ## Contributors
 - Moss Gallagher - [mossx-dev](https://github.com/mossx-dev)
 
 
