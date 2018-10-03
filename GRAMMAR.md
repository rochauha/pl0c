Here's the grammer followed by `pl0c`, which is a slightly modified version of the original grammar found on [Wikipedia](https://en.wikipedia.org/wiki/PL/0 "link to the Wikipedia page"). <br>

Even though the grammar supports nested functions, this implementation does not.

Changes:
- Consecutive statements in a particular scope must be enclosed within a BEGIN-END block
- `DO` and `THEN` are no longer valid keywords
- Procedure definitions, `IF`, `ELSE`, `WHILE` are followed by a colon
- Added keywords `PRINT` and `SCAN` for I/O

```
program = block .

block = [ "CONST" ident "=" number {"," ident "=" number} ";"]
        [ "VAR" ident {"," ident} ";"]
        {"PROCEDURE" ident ":" block } statement_block .

statement_block = "BEGIN" {statement} "END" | statement .

statement = [ ident "=" expression ";"
               | "CALL" ident ";"
               | "IF" condition ":" statement_block ["ELSE" ":" statement_block]
               | "WHILE" condition ":" statement_block
               | "PRINT" ident | NUM ";"
               | "SCAN" ident ";" ] .

condition = "ODD" expression 
            | expression ("=="|"!="|"<"|"<="|">"|">=") expression .

expression = ["+"|"-"] term {("+"|"-") term} .

term = factor {("*"|"/") factor} .

factor = ident | number | "(" expression ")" .

ident = (letter | "_") {letter | digit | "_"} .

letter = "A"|"B"| ... |"Z"|"a"|"b"|...|"z" .

digit = "0"|"1"|"2"|"3"|... |"9"| .

```


Based on Wirth’s EBNF notation, we have the following rules:
- `{ }` denotes 0 or more repetitions
- `[ ]` denotes an optionality
- `( )` denotes grouping of symbols
- A period is used to indicate the end of a production rule

For more details on Wirth's EBNF notation, head over to 
[this page](https://en.wikipedia.org/wiki/Wirth_syntax_notation "Wikipedia link to Wirth syntax notation"). <br>
Note that in this implementation, all keywords are case insensitive.
