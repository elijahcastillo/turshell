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

### `nativeInit`
- **Description**: Initializes an array of a specified size with an initializer.
- **Arguments**:
  - `array` (ArrayValue): The array to initialize.
  - `size` (IntValue): Size of the array.
  - `initializer` (RuntimeVal): The value to initialize each element of the array.
- **Returns**: None (void function).

### `nativeHash`
- **Description**: Computes a hash value for the given string.
- **Arguments**:
  - `str` (StringValue): The string to hash.
- **Returns**: `IntValue` (the hash value).

### `nativeCopy`
- **Description**: Performs a deep copy of the passed argument.
- **Arguments**:
  - `value` (RuntimeVal): The value to copy.
- **Returns**: `RuntimeVal` (a copy of the argument).

### `nativePrint`
- **Description**: Prints arguments separated by space, ending with a newline.
- **Arguments**:
  - `args` (various): The values to print.
- **Returns**: None (void function).

### `nativePrintAddr`
- **Description**: Prints out memory addresses of variables.
- **Arguments**:
  - `args` (various): The variables whose addresses are to be printed.
- **Returns**: None (void function).

### `nativeType`
- **Description**: Prints out the types of passed variables.
- **Arguments**:
  - `args` (various): The variables whose types are to be printed.
- **Returns**: None (void function).

### `nativeStringSplit`
- **Description**: Splits a string by a delimiter into an array of strings.
- **Arguments**:
  - `str` (StringValue): The string to split.
  - `delimiter` (StringValue): The delimiter to use for splitting.
- **Returns**: `ArrayValue` (array of split strings).

### `nativeAssert`
- **Description**: Asserts a condition and optionally throws an error with a message.
- **Arguments**:
  - `condition` (BoolValue): The condition to assert.
  - `message` (StringValue, optional): The error message if the assertion fails.
- **Returns**: `BoolValue` (true if the assertion passes).

### `nativeToString`
- **Description**: Converts any type to its string representation.
- **Arguments**:
  - `value` (RuntimeVal): The value to convert to string.
- **Returns**: `StringValue` (the string representation).

### `nativeRegexMatch`
- **Description**: Matches a string against a regular expression.
- **Arguments**:
  - `str` (StringValue): The string to match.
  - `regexPattern` (StringValue): The regular expression pattern.
- **Returns**: `BoolValue` (true if the string matches the regex).

### `nativeTimeNow`
- **Description**: Returns the current time in milliseconds since the epoch.
- **Arguments**: None.
- **Returns**: `FloatValue` (current time in milliseconds).

### `nativeSleep`
- **Description**: Pauses execution for a specified number of milliseconds.
- **Arguments**:
  - `millis` (IntValue): Number of milliseconds to pause.
- **Returns**: `BoolValue` (true, dummy return value).
