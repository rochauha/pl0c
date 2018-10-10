# pl0c
My attempt at writing a compiler for the PL/0 programming language by Niklaus Wirth. <br>
Initially, the aim was to write the entire compiler from scratch. While working on it, the focus slowly shifted to writing an LLVM frontend. <br>
See [GRAMMAR.md](https://github.com/ronakchauhan97/pl0c/blob/master/GRAMMAR.md) for details on the language spec. <br>

## Brief implementation details
- Recursive descent parser returns AST
- AST implemented with nodes having a list of nodes as children (CLRS 10.4)
- Symbol table is an unordered linked list
- I/O uses wrapper functions written in C. This makes it easier than handling variadic functions (which now clang can handle for us). These wrappers are implemented in examples/io.c


## Building
```
$ make
```

## Usage
```
$ ./pl0c <file_name>.pl0
```
- `pl0c` outputs a `.ll` file containing LLVM IR corresponding to the source. <br>
- Use `llc` to get an object file and `clang` to get an executable.
- If the source uses `print` and/or `scan` statements, you'll need compile _io.c_ and link with it.<br>
_io.c_ contains wrappers with the following signatures:
```
void print64(int64_t)
int64_t scan64()
```
<br>

_examples/build.sh_ automates all of this. Usage described below.
```
# get corresponding exectable named <filename>
$ ./build.sh <filename>.pl0

# get executable along with <filename>.ll containing LLVM IR
$ ./build.sh -emit-llvm <filename>.pl0   
```

<br>

__Demo__ : running examples <br>
```
$ make
$ cd examples
$ ./build.sh -emit-llvm fibonacci.pl0  # get executable named fibonacci and fibonacci.ll (contains IR)
$ ./fibonacci
10
55
$
$ ./build.sh natural_recurse.pl0  # get executable named natural_recurse
$ ./natural_recurse
5
1
2
3
4
5
$
```

## License
All `pl0c` source code is licensed under the terms of the [MIT License](https://github.com/ronakchauhan97/pl0c/blob/master/LICENSE).