# Balloon-script

A new script language and the subset of nasal

# Why i create this language:

Before i decided to write this interpreter, i had tried almost about three months to make Nasal-script interpreter.

But making an interpreter for nasal whose grammar is tha same as ECMA6 is totally a impossible work for one developer like me.

So i decided to make a simpler language but is the subset of Nasal-script.

And finally i made it.Then this language was born.Hope you will love this simple language.

Also, i hope the interpreter's codes will give you different inspiration to make your own interpreter.

You can still see the code in Nasal-Interpreter: [Nasal-Interpreter ValKmjolnir](https://github.com/ValKmjolnir/Nasal-Interpreter)

# How to use balloon:

value type:

  number (double)
  
  string (std::string)
  
  array  (std::list)
  
  hash   (std::list)
  
  function (std::list & class:abstract_syntax_tree)

operators:

  `+` (add)
  
  `-` (sub)
  
  `*` (mul)
  
  `/` (div)
  
  `~` (link:used to link two strings)
  
  `!` (nor)
  
  `>`
  
  `>=`
  
  `<`
  
  `<=`
  
  `==` (equal)
  
  `!=` (not equal)
  
  `and` (used like this: a<=b and b<=c,it is not used in bit operation)
  
  `or` (used like this: a!=b or b!=c,it is not used in bit operation)
  
  `()` (you know how to use it)
  
  `=`  (definition/assignment)
  
  `+=` (assignment)
  
  `-=` (assignment)
  
  `*=` (assignment)
  
  `/=` (assignment)
  
  `~=` (assignment)

## 1.Definition:

All new variables must be declared by this way:

`var a=1;

var b='str';

var c=[];

var d={m:0,n:"str"};

var f=func(){return 0;};`

Don't forget to add a ';' after each statement.

## 2.Assignment:

Variables can change their value by using operators like =,+=,-=,*=,/=,~=

`a+=1;

b~="hello world!";`

## 3.Call identifiers

Call array's member by this way: `array_id[number];`

Call hash's member by this way: `hash_id.member_name;`

Call function by this way: `function_name(parameters);`

## 4.Choose Statement

Framework is like this:

`if(condition){}

elsif(condition){}

else if(condition){}

else{}`

Remember not to forget braces, and the same for loop statement

## 5.Loop

There are two types of loops in this language:

`while(condition){}

for(;;){}`

The usage of the two loops is the same as that in C++

## 6.Notes

Notes begin with char '#'

## 7.Lib Functions

### In basics.nas:

`append(vector,elements...)`:append is used to add a new variable into an array

`subvec(vector,begin,length)`:subvec is used to get a new sub array from an exist array

`int(value)`:change a float number into integer

`num(str)`:change a string into a number

`size(vector)`:get the number of members in an array

`rand()`:you know 

`chars(str)`: split a string into characters

`substr(str,start,length)`:get a substring from a exist string

### In io.nas:

`print(args...)`:print informations on the screen

`input()`:gets istream from keyboard and returns a string

`read(filename)`:reads strings from a file and returns an array

`write(filename,vector)`:output strings or numbers in this array into a file

### In math.nas:

`pi`:you know

`ln_2`:you know

`ln_10`:you know

`ln(x)

log(a,x)

exp(x)

pow(x,number)

sigmoid(x)

sinh(x)

cosh(x)

tanh(x)

relu(x)

sin(x)

cos(x)

tan(x)

sqrt(x)

asin(x)

acos(x)

atan(x)`
