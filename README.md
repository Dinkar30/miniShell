# MiniShell

A small Unix shell written in C for learning process creation, signals, job control, and pipes.

## Build

```sh
gcc -o minishell *.c
```

Run it from the project directory:

```sh
./minishell
```

The shell reads external command names from `external_commands.txt`, so that file must remain available from the current working directory.

## Features

- Execute supported external commands.
- Built-in commands:
  - `cd`
  - `pwd`
  - `exit`
  - `jobs`
  - `bg`
  - `fg`
  - `fg <pid>`
- Run commands in the background:

  ```sh
  sleep 10 &
  ```

- Track multiple background and stopped jobs.
- Resume stopped jobs with `bg`.
- Bring jobs to the foreground with `fg`.
- Handle `SIGINT`, `SIGTSTP`, and `SIGCHLD`.
- Reap completed background processes and display their exit status.
- Execute foreground pipelines with multiple stages:

  ```sh
  ls | wc
  ls -l /dev | grep tty | wc -l
  ```

## Job control examples

Start a background job and inspect it:

```sh
sleep 20 &
jobs
```

Stop and resume a foreground job:

```text
sleep 20
Ctrl-Z
bg
jobs
fg
```

## Current limitations

- Input is kept to 24 characters by the current input buffer.
- Command arguments and pipeline stages are limited by fixed-size arrays.
- Pipelines currently support foreground execution only.
- Quoting and escaping are not implemented.
- Redirection (`>`, `<`, `>>`, and similar operators) is not implemented.
- Shell expansion is not implemented, including wildcard, variable, command, and tilde expansion.
- Commands are recognized using the static `external_commands.txt` list rather than a full `PATH` lookup.
- Built-in parsing is basic and does not provide complete shell-compatible argument validation.
- Error handling and terminal behavior may vary when the shell is run without an interactive terminal.
- The shell is intended for educational use and is not a POSIX-compatible shell.

## Project structure

- `main.c`: program entry point.
- `scan_input.c`: input loop, command dispatch, process groups, and job cleanup.
- `execute_internal_commands.c`: built-in command implementations.
- `execute_pipeline.c`: foreground pipeline creation and execution.
- `signal_handler.c`: signal handling.
- `check_commands.c`: command classification.
- `external_commands.txt`: list of recognized external commands.

## Notes

Run the shell in an interactive terminal when testing job control. Use `Ctrl-Z` to stop a foreground process and `Ctrl-C` to interrupt one.
