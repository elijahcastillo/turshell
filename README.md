
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


## Language Basics

### Data Types


Primary data types in Turshell:
- **int**: Integer numbers.
- **string**: Textual data.
- **bool**: Boolean values (true/false).
- **float**: 3.1415
- **array**: store data of the same type

### Variable Declaration
Strongly typed variables in Turshell:
```turshell
int year = 2024;
string message = "Turshell rocks!";
bool isValid = true;
float pi = 3.1415;
array<int> arr = [1,2,3];
```

### Control Flow
Turshell includes familiar control structures:
```turshell
// If Statements
int temp = 22;
if(temp > 20){
  print("Warm weather")
}

// While Loops
int count = 0;
while(count < 3){
  print("Count is " + count)
  count = count + 1;
}
```

### Functions
Defining and invoking functions:
```turshell
func add(int a, int b) int {
  return a + b;
}

int result = add(5, 3);
print("Result: " + result)
```

### Structs and Methods
Object-oriented features in Turshell:
```turshell
struct Vehicle {
  string model;
  int year;
};

impl Vehicle -> displayInfo(Vehicle self) void {
  print(self.model + " made in " + self.year)
}

Vehicle myCar = {model: "TurboX", year: 2023};
myCar.displayInfo();
```

### Advanced Features
Turshell supports recursion, enhanced control flow, struct operations, native functions, and string handling, enabling complex and efficient programming solutions.

### Sample Programs
Discover various algorithms and problem-solving techniques with Turshell through our sample programs section.

## Community and Support
Engage with the Turshell community for support, collaboration, and knowledge sharing. Visit [Turshell Community](https://turshell-lang.com/community) for more.

---

Embark on a coding adventure with Turshell and unlock new programming possibilities! 🐢🚀
