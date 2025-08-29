#include <stdio.h>
#include <string.h>

int main() {
	char obf[] = "\x51\x7d\x7c\x75\x60\x73\x66\x67\x7e\x73\x66\x7b\x7d\x7c\x61\x33";
	char result[17] = {0};

	for (int i = 0; i < 128; i++) {
		for (int j = 0; j < 16; j++)
			result[j] = obf[j] ^ i;
		result[16] = '\0';
		if (strcmp(result, "Congratulations!") == 0)
			printf("i: %d = 0x%x\n", i, i);
	}
}
