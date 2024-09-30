# Custom C-shell Project

This project presents a C implementation of the Bash shell, designed to emulate the functionality of Bash in Linux environments.

## Description

For more detailed information, please refer to the [Project PDF](https://github.com/mihikasanghi/OSN-custom-C-shell/blob/main/Specifications.pdf).

## Functionalities

1. **Display Information**: Displays the current user and hostname of the system.

2. **Input Handling**: Handles user input while taking care of edge cases.

3. **warp**: Changes the current directory of the shell.
   - Handles non-existent directory cases.
   - Supports ".", "..", "~", and "-" flags.
   - Executes warp sequentially for multiple arguments.
   - Defaults to the home directory if no argument is provided.

4. **peek**: Lists files and directories in specified directories in lexicographic order.
   - Supports "-a" and "-l" flags, similar to the bash `ls` command.

5. **pastevents**: Similar to the `history` command in bash.
   - `pastevents purge`: Clears the command history.
   - `pastevents execute <index>`: Executes a specific command from history.
   - Displays all stored commands (up to 15) if no argument is given.

6. **system commands**: Capable of executing other Bash system commands (e.g., emacs, gedit).
   - Supports foreground and background processing with the "&" symbol.

7. **proclore**: Retrieves information about processes.
   - If no argument is specified, it displays information about the shell process.

8. **seek**: Searches for files/directories in the specified target directory.
   - Supports flags: -d (directories only), -f (files only), -e (execute/change directory).
   - Example usage: `seek -d -e <directory> <name>`

## Program Structure

1. **main.c**: Contains the main function, shell loop, initialization, exit code, and signal handlers.

2. **input_handler.c**: Processes user input, handles command history, and parses commands.

3. **function_handler.c**: Handles individual commands and routes them to appropriate functions.

4. **syscall.c**: Executes system commands using `execvp()`.

5. **functions/**: Contains implementations for custom commands (warp, peek, pastevents, proclore, seek).

## Build and Run

1. Clone the repository:
   ```
   git clone <repository_url>
   ```

2. Navigate to the project directory:
   ```
   cd <project_directory>
   ```

3. Compile the project:
   ```
   make
   ```

4. Run the shell:
   ```
   ./shell
   ```

## Usage

Once the shell is running, you can use the custom commands as described in the Functionalities section above. For system commands, use them as you would in a regular Bash shell.

## Assumptions

The implementation aims to closely mimic Bash functionality. In ambiguous situations, Bash behavior is the reference standard.
