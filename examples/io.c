#include <stdint.h>
#include <stdio.h>

/* Wrappers around printf and scanf */

void print64(int64_t num)
{
    printf("%ld\n", num);
}

int64_t scan64()
{
    int64_t num;
    scanf("%ld", &num);
    return num;
}