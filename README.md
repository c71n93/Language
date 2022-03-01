# С(ringe)
The *С(ringe)* language is a simplified copy of the *C* language. 

The source code is compiled via AST into assembly code, that can be execute by [Processor](https://github.com/RomanKorostinskiy/Processor).


## С(ringe) grammar:
I used [EBNF](https://en.wikipedia.org/wiki/Extended_Backus%E2%80%93Naur_form) syntax to describe grammar of my language.

**Grammar for recursive descent:**
```
G  		::= St+, '\0' ;

St 		::= [Cond | As | E | FuncDef], ';' ;

Blk 	::= '{', St+, '}' ;

Cond 	::= 'if', '(', E, ')', Blk, ['else', '(', ')', Blk] ;

As 		::= Vas, '=', E ;

FuncDef ::= 'def', '!', Func, Blk ;

Func 	::= Name, '(', ')' ;

StdFunc ::= ('print' | 'sqrt' | 'scan'), '(', E, ')' ;

E 		::= T ((+ | -), T)* ;

T 		::= P ((* | /), P)* ;

P 		::= ('(', E, ')') | N | Var | StdFunc | Func ;

N 		::= FloatingPointNumber ;

Var 	::= Name ;
```

**Grammar for tokenizer:**
```
Name   ::= Letter, (Letter | Digit | "_")* ;

Letter ::= ('a'-'z') | ('A'-'Z') ;
```
```
FloatingPointNumber ::= ['-'], ('0' | NaturalNumber), '.', NaturalNumber ;

NaturalNumber       ::= DigitExcludingZero, Digit* ;

Digit               ::= '0' | DigitExcludingZero ;

DigitExcludingZero  ::= ('1'-'9');
```

## How to use Language:
**Input** - is a program in С(ringe) language. You should pass a path to source code in the first argument of programm.

**Output** - is a assembly code. You should pass a path to place where you want to see your assembly code in the second argument of programm.