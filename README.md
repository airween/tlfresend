# This is a small piece of code of Tlf

The main goal of this repository is to help emulate the `RESEND_CALL` behavior. If somebody wants to play with the code, he does not need to understand the whole code and logic.

## How can you use

### Requirements

Before you start to try it, you have to be sure that you have

* a C compiler (`gcc` or `clang`)
* glib
* `make`
* `git`

### Try it

Grab the code and build it

```
$ git checkout https://github.com/airween/tlfresend
$ cd tlfresend
$ make
$ src/tlftest 
 Status | CALL 1st            | CALL 2nd            | Result              | Got                 
--------+---------------------+---------------------+---------------------+---------------
 OK     | WB6A                | W6BA                | W6B                 | W6B                 
 OK     | HA2OH               | HA2OS               | OS                  | OS                  
 OK     | HA2MS               | HA2OS               | OS                  | OS                  
 OK     | HA2MH               | HA2OS               | OS                  | OS                  
 OK     | OK2FHI              | OK1FHI              | OK1                 | OK1                 
 OK     | OK1FH               | OK1FHI              | FHI                 | FHI                 
 OK     | OK2FH               | OK1FHI              | 1FHI                | 1FHI                
 OK     | WB6AB               | WB6A                | 6A                  | 6A                  

```

### Modify the logic

You can find the responsible function in [src/utils.c](src/utils.c#L52). Modify as you want, and send a PR :).


73, Ervin
HA2OS

