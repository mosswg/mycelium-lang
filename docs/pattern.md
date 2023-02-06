# Patterns

## Introduction
Patterns are the heart and soul of mycelium. Functions, operators, and conditionals all rely on patterns. For the purpose of this document patterns will be in the format `<[pattern]>` where [pattern] is replaced by whatever pattern is being discussed.
Pattern can contain control sequences. These are surronded by curly brackets.

## Basic patterns
A basic pattern is made up of strings and variables. This is enough to get by most of the time however it can be very limiting. An example of a basic pattern for add two integers is: `<int a + int b>`.

## Patterns with lists
Patterns can have an arbritrary number of arguments using lists. For example a pattern that matches for two or more integers added together `<int a + int b {<+ int> ...ints}>`. This pattern requires `a` and `b` but does not require any other integers. All other integers would be placed into a list named `ints`. Matching this pattern with `1 + 2 + 3 + 4` would yeild `a = 1`, `b = 2`, `ints = {3, 4}`.

## Patterns arbitrary types
A pattern that matches to multiple types can be created using the `any` special type. For example a pattern that matches anything added to an integer would look like `<any a + int b>`.

## Patterns with templated types
Patterns can use an argument to define the types of everything. This type can be defined with the control sequence `{type [name]}` where name is a name given to the type. It used with the control sequence `{[name]}`. For example asigning an to a value in a list `<list<{type t}> a[int b] = {t} c`
