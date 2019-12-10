# IntCode assembler

This is an IntCode assembler (short: icc :smirk:) that compiles IntCode assembly
into IntCode. It's probably pretty buggy right now because i haven't cleaned up the
code yet. Just run in in the command line and pass the input and output file as
command line arguments.

### Compiling

You'll have to compile it yourself, for that you can do whatever you want or use my CMake file:
```bash
$ cmake -H. -Bbuild
$ cmake --build build
$ ./bin/icc.elf <infile> <outfile>
```

### IntCode assembly

IntCode assembly is very similar to intel assembly. Instruction are read line by line and can contain comments.
Everything after a semicolon is a comment and is ignored. A parameters mode is denoted by a $ sign or a % sign before the value. The $ sign
sets the parameter to immediate mode (1). The % sign sets the parameter to relative mode (2).

### Example (Fibonacci generator by u/mkeeter, translated into IntCode assembly):

The IntCode assembly:
```
in 3            ;This is a comment
jz $1, $24
out $1
add $1, $0, 6
add 8, $0, 9
add 6, $0, 8
add 3, $-1, 3
jnz $99, $2
```

Compiles into:
```
3,3,1106,1,24,104,1,1101,1,0,6,1001,8,0,9,1001,6,0,8,1001,3,-1,3,1105,99,2
```

### Instruction set

| instruction | example            | explanation                                                                               | mode info                   |
|-------------|--------------------|-------------------------------------------------------------------------------------------|-----------------------------|
| add         | add 5, $-0x5d, 9   | adds two numbers                                                                          | param 3 expects mode 0 or 2 |
| mul         | mul 5, $0xABCD, 10 | multiplies two numbers                                                                    | param 3 expects mode 0 or 2 |
| in          | in 10              | stores input at address                                                                   | param 3 expects mode 0 or 2 |
| out         | out 10             | writes value at address to stdout                                                         | param 3 expects mode 0 or 2 |
| jnz         | jnz 5, 0x10        | jumps if first param is not 0                                                             |                             |
| jz          | jz $0, 0           | jumps if first param is 0                                                                 |                             |
| slt         | slt $5, 10, 15     | sets value at param 3 to 1 if param 1<br>is less than param 2 otherwise<br>it is set to 0 | param 3 expects mode 0 or 2 |
| se          | se $5, 10, 15      | sets value at param 3 to 1 if param 1<br>is equal to param 2 otherwise<br>it is set to 0  | param 3 expects mode 0 or 2 |
| hlt         | hlt                | halts execution                                                                           |                             |
| crb.........| crb %-2            | changes the relative base.................................................................|.............................|