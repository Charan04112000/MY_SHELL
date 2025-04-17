SHELL Project Implementation
==========================

Overview
--------
This project implements a basic command-line interpreter (shell) in C, named MY_SHELL, as per the specified requirements. The shell provides a user-friendly interface to interact with the operating system, supporting built-in commands, external command execution, input/output redirection, pipelines, and command history.

Project Files
------------
- my_shell.c: Main source file containing the shell implementation.
- my_shell.h.h: Header file with structure definitions and function declarations.
- my_shell.makefile: Makefile for building the project.
- README.txt: This file, documenting the project and usage instructions.

Features
--------
1. **Prompt Display**: Shows command sequence number and hostname (e.g., `<1 srvr2>`).
2. **Built-in Commands**:
   - `hist`: Displays the last 10 commands.
   - `curPid`: Shows the current process ID.
   - `pPid`: Shows the parent process ID.
   - `cd [directory]`: Changes the current directory.
   - `quit`: Exits the shell.
3. **External Commands**: Executes programs using fork/exec (e.g., `ls`, `pwd`).
4. **Command Separation**: Supports semicolon-separated commands (e.g., `ls; pwd`).
5. **Pipelines**: Supports piping commands (e.g., `ls -l | grep txt`).
6. **Input/Output Redirection**: Supports `<` for input and `>` for output (e.g., `ls > output.txt`).
7. **Command History**: Stores up to 10 non-empty command lines.
8. **Single Command Mode**: Supports `-c` option for single command execution (e.g., `./PROG1 -c "ls -l"`).

Compilation
-----------
1. **Prerequisites**:
   - Ensure `gcc` and `make` are installed (e.g., `sudo apt-get install build-essential` on Ubuntu).
2. **Steps**:
   - Navigate to the project directory:
     ```
     cd ~/linux/miniproject
     ```
   - Compile using the Makefile:
     ```
     make -f prog01.makefile
     ```
   - This creates the executable `PROG1`.
3. **Clean Up**:
   - Remove object files and executable:
     ```
     make -f prog01.makefile clean
     ```

Usage
-----
1. **Interactive Mode**:
   - Run the shell:
     ```
     ./PROG1
     ```
   - At the prompt (e.g., `<1 srvr2>`), enter commands like:
     ```
     ls -l
     hist
     cd /tmp
     ls -l | grep txt
     ls > output.txt
     quit
     ```
2. **Single Command Mode**:
   - Execute a single command and exit:
     ```
     ./PROG1 -c "ls -l"
     ```
3. **Examples**:
   - Run multiple commands:
     ```
     ls; pwd; date
     ```
   - Use pipelines and redirection:
     ```
     cat file.txt | grep keyword > result.txt
     ```

Implementation Details
---------------------
- **Structure**: Uses `Command` and `CommandList` structs in `prog01.h` for command parsing.
- **History**: Stores up to 10 commands in a `History` struct array.
- **Parsing**: Supports tokens separated by spaces/tabs, with special handling for `;`, `|`, `<`, and `>`.
- **Execution**: Uses `fork` and `execvp` for external commands, `pipe` for pipelines, and `dup2` for redirection.
- **Error Handling**: Displays appropriate error messages for failed commands (e.g., invalid paths, failed exec).
- **Limitations**:
   - Maximum input line: 128 characters.
   - Maximum tokens per command: 20.
   - Maximum pipes: 5.
   - Maximum semicolon-separated commands: 10.
   - Maximum history entries: 10.

GitHub Repository
----------------
The project is hosted on https://github.com/Charan04112000/MY_SHELL/edit/main/readme.txt#L10C8


Deliverables
------------
- Source files: `my_shell.c`, `my_shell.h`
- Makefile: `my_shell.makefile`
- Documentation: `README.txt`
- To create a tarball for submission:
  ```
  tar -czf MY_SHELL.tar.gz prog01.c prog01.h prog01.makefile README.txt
  ```

Notes
-----
- The project was developed and tested on a Linux environment.
- Files were renamed from `miniproject.c` and `miniproject.makefile` to `prog01.c` and `prog01.makefile` to match project requirements.
- Compiled files (`miniproject.o`, `PROG1`) are ignored in `.gitignore` for version control.
- The MIT License is included for open-source distribution.

Contact
-------
For issues or questions, refer to the GitHub repository or contact the project owner.
