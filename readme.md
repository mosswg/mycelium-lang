 # Mycelium
 Mycelium will be a language built with scripting principles like lists, automatic types, multiple return values, etc. but the language will ultimately be converted to assembly to be compiled and run natively. Mycelium will be a compiled dynamic language.

 ## std library
 The Mycelium std library has now been removed and will be rewritten in Mycelium on the language is useable. <br>
 The old version of std library has been moved to [Mycore](https://github.com/mossx-dev/Mycore)

## Important Update
Mycelium is going to be completely rewritten in C++. The reason for this change is the same reason there have been barely any updates to the project in months. I have been very busy and every time I try to code in assembly I can't get anything meaningful done. So hopefully now I can get back to working on this project again.

## Milestones
- [x] tokenizer
- [ ] parser
  - [x] variable creation
  - [x] constant creation
  - [ ] math evaluation
  - [ ] condition calls
  - [ ] condition definitions
  - [ ] condition declaration
  - [x] operator calls
  - [x] operator declaration
  - [ ] operator definitions
  - [x] function calls
  - [x] function declarations
  - [x] function definitions
  - [ ] class definitions
- [ ] interpreter
  - [x] std functions (e.g. print)
  - [x] user defined functions
  - [x] std operations (e.g. adding ints)
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
Variables are decalred with a [type](#Types) which define how they function. 
e.g.

``` c++
string my_string;
int my_int = 5;
```

There are two special types in the std library which can be used as an alternative.
They are `any` and `auto`. `any` will allow a variable to hold any type. `any` requires a type to be provided when first declared and when the type is changed. `auto` is the same as any but will decide its type based on assignment and does not require explicit type declaration. Both `any` and `auto` require an extra byte to store the current type.

e.g. of `any`
``` c++
any<string> var1 = "Hello";
var1<int> = 5;
```

e.g. of `auto`
``` c++
auto var1 = "Hello";

var1 = 5;
```


### Functions
Functions are decalred with the `func` keyword. 
Then return types and amount are defined. 
This can be omitted if the function does not return anything. 
Next the function name is defined. Then the arguments.

e.g. a function that takes two `int`s and returns two `int`s
``` c++
func<int, int> my_func(int a, int b) {
	...
}
```
Functions can also use the `any` and `auto` types
e.g.

``` c++
func<any> my_other_func(auto a) {
	...
}
```
A function that returns nothing and takes no arguments could be defined as.

``` c++
func my_third_func() {
	...
}
```
Or defined as.

``` c++
func<> my_other_third_func() {
	...
}
```
#### Arguments
A function can also use custom argument syntax
e.g. a function that uses semi-colons instead of commas

``` c++
pattern func my_semi_colon_func(int a; int b) {
	...
}
```
or a function could take arguments in the form of a string

``` c++
pattern func my_patterned_func(My arguments are int a and string b) {
	...
}

my_patterned_func(My arguments are 5 and "Test")
```
#### Conditionals
A function can use the special keyword `cn` and then is a conditional which takes a function and any other arguments. 
It should return a boolean to indicate that the condition was true. This can be used to define custom if statements
e.g. a conditional that takes a string and executes the code if the string is empty
``` c++
cond if_empty_string(func func, string str) {
	if (str.is_empty()) {
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
Using the conditional function type and the conditional argument type a loop can be made.
Note: infinite recursion will not result in a stack overflow but will cause errors after 18 quintillion times (when the recursion counter goes negative). 

e.g. A loop that runs n number of times
```c++
cond loop_n_times(func func, int counter) {
	if (counter != 0) {
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

### Pattern Match
Using the tokens make from the tokenizer a pattern match can be made for example:
```
{token("int"), token("a"), token("++")}
```
Can be turned into:
```
{variable(int), string("++")}
```
Then when a variable is already defined it can be used like so:
```c++
int value = 1

value++
```
Which turns into
```
{variable(int), string("++")}
```
This is exactly the same so we can say they match. This does pose an issue. Let's say we have the pattern:
```
{variable(int), string("+"), variable(int))}
```
If we then have the program:
```c++
int value = 1

value + value - 1
```
We want to say this matches because ``value - 1`` can be turned into an integer however, if we use our primitive pattern matching we get:
```
{variable(int), string("+"), variable(int), string("-"), variable(int)}
```
The solution to this is instead of creating two independent patterns we can use one to create the other.
By useing expressions rather than variables and finding landmark of the first pattern we can check not only if the current state of the program match but also if any other lossless conversion could match.
For Example with the same pattern:
```
{variable(int), string("+"), variable(int))}
```
And the same program:
```c++
int value = 1

value + value - 1
```
We can find landmarks in the pattern which in this case would just be the "+":
```
{string("+")}
```
Then we can look for everything before the landmark in the program:
```
{variable(int)}
```
Then we can look for everything after the landmark:
```
{variable(int), string("-"), variable(int)}
```
We can then check if the proceeding part can be turning into an expression that results in an integer. In this case subtracting two intergers gives an integer so we can convert that to an expression:
```
{expression(int - int, returns: int)}
```
Then our final pattern from the program is:
{variable(int), string("+"), expression(int - int, returns: int)}


## Planned Features
### Note
All syntax here is in development and is most likely going to change.
### Polymorphism
A function can be declared multiple times with different arguments and do different things based on which arguments it are used.
e.g. two print functions that take a string and an int could be decalred as. 
``` c++
func print(string str) {
    ...
}
```
and
``` c++
func print(int num) {
    ...
}
```
They would compile to

``` asm
print_ret_void_arg_string:
```
and 

``` asm
print_ret_void_arg_int:
```

Then when they're called the compiler will decide which to use when called.
<br>
A function can also be decalred with different return types. Note that the contents of the function should be changed as little as possible to avoid confusion.
e.g. two functions that divide an integer where one returns just the quotent and the other returns the quotent and the remainder.

``` c++
func<int> div(int a, int b) {
	...
	return result;
}
```
and

``` c++
func<int, int> div(int a, int b) {
	...
	return result, remainder;
}
```
They would compile to

```asm
div_ret_int_arg_int_int:
	...
	ret
```
and 

``` asm
div_ret_int_int_arg_int_int:
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
oper<pair><this + pair b> {
	...
}
```
This would compile into

// TODO: Update the syntax of this

``` asm
class_pair_op_043:
	...
```

Then the `+` operator can be used like you would with anything else e.g.

``` c++
pair first_pair;
pair second_pair;

first_pair + second_pair;
```

### Native Assembly Functions
A function can be declared then defined in an attached assembly file
e.g. a function `foo` can be defined as.

``` c++
asm<int> foo(int a) {
	...
	ret
}
```
Or could be decalred as.
``` c++
asm<int> foo(int a);
```
and defined in a separate file as
``` asm
foo_ret_int_arg_int:
    ...
    ret
```
Where the constant in rax is the argument passed and rsi is the return value


### Inline Functions
A function can be declared with the keyword inline and after being turned into assembly code it will directly be placed wherever it is called
e.g. a function `add` can be defined as.
``` c++
func<int> add(int a, int b) {
    return a + b;
}
```
It will compile into
``` asm
add_ret_int_arg_int_int:
    add     rax, rbx
    mov     rsi, rax
    ret
```
However, if defined like this
``` c++
inline func<int> add(int a, int b) {
    return a + b;
}
```
And called like this
``` c++
add(1, 1);
```
The whole thing would compile into
``` asm
mov     rax, 1
mov     rbx, 1
add     rax, rbx
```

### Special Case 
#### This might not be implemented
a function can be defined as normal, then a special case can be added to do something when a certain value is passed
e.g. a function to get the digits in a number can be defined as
``` c++
int digits(int num) {
    ...
}
```
Then a special case can be defined when zero is passed as
``` c++
sc int digits(num = 0) {
	return 1;
}
```



## How to Contribute
[![PRs Welcome](https://img.shields.io/badge/PRs-welcome-brightgreen.svg)](http://makeapullrequest.com)<br>
- Please submit any issues you have [here](https://github.com/mossx-dev/Mycelium/issues/new/choose)
- If you need to contact me directly send an email to <moss@mossx.net>



 ## Contributors
 - Moss Gallagher - [mossx-dev](https://github.com/mossx-dev)
 
 
