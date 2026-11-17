Image Steganography Using C

Overview

This project implements image steganography using the C programming language. It allows users to encode secret messages into image files and subsequently decode them to retrieve the hidden information.
The project demonstrates the application of file handling, bitwise operations, pointers, and modular programming in C.

Features

- Encoding secret text messages into image files.
- Decoding hidden messages from encoded images.
- Modular implementation using separate encoding and decoding modules.
- File handling for reading input data and generating output files.

Technologies Used

- Programming Language: C
- Concepts: File Handling, Pointers, Bitwise Operations, Structures, Modular Programming.

Project Structure

| `common.h` | Common definitions and declarations |
| `types.h` | Data type definitions |
| `encode.c` | Encoding implementation |
| `encode.h` | Encoding function declarations |
| `decode.c` | Decoding implementation |
| `decode.h` | Decoding function declarations |
| `main.c` | main file |

How It Works

The project uses image files as carriers for secret information. The encoding module embeds the input message into an image, generating an encoded image. The decoding module extracts the hidden message from the encoded image.

Sample Input and Output

Sample image files and text files are included in this repository to demonstrate the encoding and decoding process.

How to Run

Instructions for compiling and executing the project will be provided here.

Author: Asmila Ali
