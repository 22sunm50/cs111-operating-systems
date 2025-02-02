## UID: 706001504

## Pipe Up

This program imitates the pipe operator (|) by taking in command lines as arguments.

## Building

```
make
```

## Running

Show an example run of your program, using at least two additional arguments, and what to expect

Here is an example run of my program on the cs111 VM, where we can expect the output of both lines to be the same.
```
cs111@cs111 Desktop/lab1 » ls | cat | wc                                                        
      7       7      63
cs111@cs111 Desktop/lab1 » ./pipe ls cat wc
      7       7      63
```

## Cleaning up

To clean up all binary files:
```
make clean
```
