
# Turshell Programming Language


<div align="center">
  <img src="turshell.png" alt="Turshell Logo" width="500">
</div>

## Introduction

Welcome to Turshell, a powerful and intuitive programming language inspired by the elegance and efficiency of turtles. Turshell combines simplicity with versatility, making it suitable for a wide range of applications, from scripting to software development.

## Features

- **Turtle-inspired Syntax**: Turshell's syntax is designed to be user-friendly and easy to understand, like guiding a turtle through your code.

- **Efficient**: Turshell is designed for performance and efficiency, ensuring your code runs smoothly.

- **Cross-Platform**: Turshell is cross-platform and works seamlessly on Windows, macOS, and Linux.

## Getting Started

To get started with Turshell, you'll need to install the Turshell interpreter and compiler. Follow these steps:

<!-- 1. [Download the Turshell installer](https://turshell-lang.com/downloads). -->
<!--  -->
<!-- 2. Run the installer and follow the installation instructions. -->
<!--  -->
<!-- 3. Once installed, you can start writing and executing Turshell code right away! -->

# Lets learn how to write some turshell code!
## Types

There are 3 default data types in turshell these are:
 - int
 - string
 - bool

## Variables
You can define variables with these types ,    **these types are enforced at runtime**
```
int a = 2024;
string b = "turshell is awesome!"
bool c = true;
```

## Comments


## Control Flow
If you have ever written in other languages like javascript, go, c++ these will be familiar


- If Statements
```
int a = 3;
if(a == 3){
  print(a)
}
``` 

- While Loops
```
int b = 0;
while(b <= 10){
	if(b % 2 == 0){
		print(b)
	}
	b = b + 1;
}
```

## Functions
Here is an example of how you would declare and call a function in turshell
```
func isEven(int n) bool {
  if(n % 2 == 0){
    return true;
  }

  return false;
}

int a = 4;
print(isEven(a))
```
