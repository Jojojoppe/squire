#include <general/string.h>

char * strchr(const char * s, int c){
	for (; *s != '\0'; ++s)
		if (*s == c)
			return (char *) s;

	return 0;
}

void strcpy(char * dest, const char * src){
	int i = 0;
	while (1) {
		dest[i] = src[i];
		if (dest[i] == '\0') break;
		i++;
	}
}

unsigned strlen(const char * s){
	unsigned len=0;
	while(s[len++]);
	return len;
}

int strcmp (const char *s1, const char *s2) {
    const unsigned char *p1 = (const unsigned char *)s1;
    const unsigned char *p2 = (const unsigned char *)s2;

    while (*p1 != '\0') {
        if (*p2 == '\0') return  1;
        if (*p2 > *p1)   return -1;
        if (*p1 > *p2)   return  1;

        p1++;
        p2++;
    }

    if (*p2 != '\0') return -1;

    return 0;
}

void * memset(void * ptr, int value, unsigned num){
	unsigned char *temp = (unsigned char *)ptr;
	for( ; num != 0; num--, temp[num] = value);
	return (void *) temp;
}

void * memcpy(void *dest, const void *src, unsigned count){
	const char *sp = (const char *)src;
	char *dp = (char *)dest;
	for(; count != 0; count--) *dp++ = *sp++;
	return dest;
}