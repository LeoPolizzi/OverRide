I used `Ghidra` to analyze the binary and see what it does:

It waits for two inputs, a login and a serial. It then performs some checks on the serial to see if it is correct.

I checked the function that verifies the serial and saw that it always uses the same algorithm to compute a hash from the login and compare it to the serial.

Nothing is random, therefore I can just run the program with a random login and see what the final computed serial is, then use that serial to authenticate.

I'm not going to go deep into how I retrieved the addresses for the breakpoints/jump/hash, they can all be seen within the `auth` function using GDB.

```bash
level06@OverRide:~$ gdb ./level06
(gdb) b *0x80487ae <==== Break before ptrace call to skip it
Breakpoint 1 at 0x80487ae
(gdb) b *0x8048877 <==== Break after the serial check to get the value of the final hash
Breakpoint 2 at 0x8048877
(gdb) r
Starting program: /home/users/level06/level06
***********************************
*               level06           *
***********************************
-> Enter Login: random
***********************************
***** NEW ACCOUNT DETECTED ********
***********************************
-> Enter Serial: 123456

Breakpoint 1, 0x080487ae in auth ()
(gdb) jump *0x080487ed <==== Jump over the ptrace call
Continuing at 0x80487ed.

Breakpoint 2, 0x08048877 in auth ()
(gdb) x/d $ebp-0x10 <==== The final hash is stored at ebp-0x10, print it as a decimal number
0xffffdbb8:     6230777
```

```bash
level06@OverRide:~$ ./level06
***********************************
*               level06           *
***********************************
-> Enter Login: random
***********************************
***** NEW ACCOUNT DETECTED ********
***********************************
-> Enter Serial: 6230777
Authenticated!
$ whoami
level07
```
