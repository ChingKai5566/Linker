## Linker

This project is a two-pass linker. It takes individually compiled code/object modules and creates a single executable by resolving external symbol references (e.g. variables and functions) and module relative addressing by assigning global addresses after placing the modules’ object code at global addresses.

Assume a target machine with the following properties:

- word addressable
- addressable memory of 512 words
- each valid word is represented by an integer (<10000)
