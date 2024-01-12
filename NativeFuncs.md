## File Manipulation & Access

### `nativeFileWrite`

- **Description**: Writes data to a file.
- **Arguments**:
  - `filePath` (string): Path to the file.
  - `data` (string): Data to be written to the file.
- **Returns**: `BoolValue` (true if successful).

### `nativeFileAppend`

- **Description**: Appends data to an existing file.
- **Arguments**:
  - `filePath` (string): Path to the file.
  - `data` (string): Data to be appended to the file.
- **Returns**: `BoolValue` (true if successful).

### `nativeFileExists`

- **Description**: Checks if a file exists.
- **Arguments**:
  - `filePath` (string): Path to the file.
- **Returns**: `BoolValue` (true if the file exists).

### `nativeFileRead`

- **Description**: Reads and returns the content of a file.
- **Arguments**:
  - `filePath` (string): Path to the file.
- **Returns**: `StringValue` (content of the file).

### `nativeDirectoryList`

- **Description**: Lists files and directories in a specified directory.
- **Arguments**:
  - `directoryPath` (string): Path to the directory.
- **Returns**: `ArrayValue` (list of files and directories).

---

## General Utilities

### `init`
- **Description**: Initializes an array of a specified size with an initializer.
- **Arguments**:
  - `array` (array): The array to initialize.
  - `size` (int): Size of the array to initilzie
  - `initializer` (any): The value to initialize each element of the array.
- **Returns**: None (void function).

### `input`
- **description**: prompts user for input 
- **arguments**:
  - `promptString` (string)
- **Returns**: `int | string` (the users input).

### `random`
- **description**: generates a random integer between 0 and INT_MAX
- **arguments**:
  - None
- **Returns**: `int` (the random number).

### `len`
- **description**: computes the length of an array or string
- **arguments**:
  - `value` (string | array)
- **Returns**: `int` (the length of the value).

### `hash`
- **description**: computes a hash value for the given string.
- **arguments**:
  - `value` (any): the value to hash.
- **returns**: `int` (the hash value).

### `copy`
- **Description**: Performs a deep copy of the passed argument.
- **Arguments**:
  - `value` (any): The value to copy.
- **Returns**: `any` (a copy deep of the argument).

### `print`
- **Description**: Prints arguments separated by space, ending with a newline.
- **Arguments**:
  - `args` (any): The values to print.
- **Returns**: None (void function).

### `addr`
- **Description**: Prints out memory addresses of variables.
- **Arguments**:
  - `args` (any): The variables whose addresses are to be printed.
- **Returns**: None (void function).

### `type`
- **Description**: Prints out the types of passed variables.
- **Arguments**:
  - `args` (any): The variables whose types are to be printed.
- **Returns**: None (void function).

### `split`
- **Description**: Splits a string by a delimiter into an array of strings.
- **Arguments**:
  - `str` (string): The string to split.
  - `delimiter` (string): The delimiter to use for splitting.
- **Returns**: `array<string>` (array of split strings).

### `assert`
- **Description**: Asserts a condition and optionally throws an error with a message.
- **Arguments**:
  - `condition` (bool): The condition to assert.
  - `message` (string, optional): The error message if the assertion fails.
- **Returns**: None (void)

### `toString`
- **Description**: Converts any type to its string representation.
- **Arguments**:
  - `value` (any): The value to convert to string.
- **Returns**: `string` (the string representation).

### `regexMatch`
- **Description**: Matches a string against a regular expression.
- **Arguments**:
  - `str` (string): The string to match.
  - `regexPattern` (string): The regular expression pattern.
- **Returns**: `bool` (true if the string matches the regex).

### `timeNow`
- **Description**: Returns the current time in milliseconds since the epoch.
- **Arguments**: None.
- **Returns**: `float` (current time in milliseconds).

### `sleep`
- **Description**: Pauses execution for a specified number of milliseconds.
- **Arguments**:
  - `millis` (int): Number of milliseconds to pause.
- **Returns**: None (void function)

---

## Math Functions

### `sqrt`
- **Description**: Calculates the square root of a number
- **Arguments**:
  - `value` (int): The integer to square root
- **Returns**: `int` (Maybe convert to float in the future)

### `abs`
- **Description**: Calculates the absolute value of a number
- **Arguments**:
  - `value` (int): The integer to take the abs of
- **Returns**: `int` 

### `pow`
- **Description**: Calculates a interger raised to a power
- **Arguments**:
  - `base` (int): The base number
  - `power` (int): The value to raise the base to
- **Returns**: `int` 
