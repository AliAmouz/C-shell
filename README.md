# C-Shell

## Features
- Supports built-in commands:
  - `echo`: Prints the given input to the console with normalized spaces.
  - `type`: Identifies if a command is a built-in shell command or an external executable.
  - `cd`: Changes the current working directory.
  - `pwd`: Prints the current working directory.
  - `exit`: Exits the shell.
- Executes external commands using `execvp()`.
- Handles spaces and input normalization.

## Compilation
To compile the program, use:
```sh
gcc shell.c -o shell
```

## Usage
Run the shell using:
```sh
./shell
```
You will see a `$` prompt where you can enter commands.

### Examples
```sh
$ echo Hello    world!
Hello world!
$ pwd
/home/user
$ cd /tmp
$ pwd
/tmp
$ type ls
ls is /bin/ls
$ exit
```

## File Structure
- **shell.c**: Main source code for the shell implementation.
- **Makefile** (optional): Automates the compilation process.

## Limitations
- No support for pipes (`|`), redirections (`>`, `<`), or background execution (`&`).
- Maximum argument count of 9 for external commands.
- Uses `strtok()`, which modifies the input string.

## Future Improvements
- Add support for command chaining (`&&`, `||`).
- Implement history and tab-completion.
- Improve memory management to reduce redundant allocations.

## License
This project is open-source. You can modify and distribute it freely.

## Author
Created by **Ali Amouz**

