![OS support](https://img.shields.io/badge/OS-macOS%20Linux%20-red)

# TERMINAL SCREEN

## Introduction.

Terminal screen is a program that takes as its input a stream of bytes in the following format

```
+--------------+-------------+-------------+-------------+--- ··· ---+----------------+
| Command Byte | Length Byte | Data Byte 0 | Data Byte 1 | ··· | Data Byte n-1 |
+--------------+-------------+-------------+-------------+--- ··· ---+----------------+
```

and draws the output of the operation that this stream of bytes encodes to a screen in the terminal window from which the program was launched. This stream of bytes acts as a means of communication between two active processes, a concept that plays a role in the design of the program.

The data format of the byte stream is an array of bytes categorized into sections that start with a command byte, which specifies the desired operation, a length byte that captures how many arguments the command takes and finally a sequence of data bytes that act as the arguments to the command.

> [!WARNING]
> Terminal Screen is not production ready. Do not use this in production environments.

## Architectural design.

The format presented within the introduction is reminiscent of bytecode generated by interpreted languages like **Python**. As such, terminal screen is broken down into two sections:

## Execution.

### Requiremets.

1. gcc version 14.2.0
2. make
3. Unix-like OS.
4. C23

> [!NOTE]  
> Terminal Screen was tested on Mac OS. It uses gcc as the compiler of choice. Ensure the environment variable CC on your system points to gcc version 14.2.0 and above otherwise the program will not compile as it uses expressions only available on this version.

### Demo.

Run the following to build terminal screen and generate two executables:

```bash
make
```

If you encounter an error while building the progam, ensure you have a C compiler that supports the features introduced in the C23 standard like **_constexpr_**, **_nullptr_**, **_bool_** without needing to include the **stdbool** header file, etc.

With a successful build, open two instances of your terminal and run the two executables:

```bash
./build/screen
```

```bash
./build/stream
```

In the terminal window in which you executed the **_./build/stream_** executable, a REPL wiil be presented using a chevron, i.e:

```bash
>>>
```

Run the following commands in the REPL, one after the other:

1. Setup a screen with a width of 40 characters, a height of 25 characters, and supporting 8-bit colour.

```bash
>>> screen(40, 25, 2);
```

2. Render the text "HELLO, WORD!!!" starting at point (19, 12), using the colour index 196.

```bash
>>> render(19, 25, 196, "HELLO, WORD!!!");
```

3. Clear the screen.

```bash
>>> clear();
```

These and other commands are supported in the REPL.

## Supported commands.

| Name           | Command                                        | Description                                                   |
| -------------- | ---------------------------------------------- | ------------------------------------------------------------- |
| Setup Screen   | screen(width, height, mode);                   | Defines the dimensions and colour setting of the screen.      |
| Draw Character | draw(x, y, colour, ASCII character);           | Places a character at the specified coordinate of the screen. |
| Draw Line      | line(x0, y0, x1, y1, colour, ASCII character); | Draws a line from one coordinate of the screen to another.    |
| Render Text    | render(x, y, colour, "text to render");        | Renders a string starting from the specified position.        |
| Move cursor    | move(x, y);                                    | Moves the cursor without drawing on the screen.               |
| Draw at cursor | dcur(ASCII character, colour);                 | Draws a character at the cursor location.                     |
| Clear Screen.  | clear();                                       | Clears the screen.                                            |

> [!NOTE]  
> Given that the stream is an array of bytes, the dimensions and coordinates can only be between 0 and 255.
