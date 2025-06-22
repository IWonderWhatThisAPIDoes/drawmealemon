/**
 * @file main.c
 * 
 * Template file for the wrapper that executes a parser
 * with the desired input
 */

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "%parser%.h"
#include "%parser%.c"

const int inputTokens[] = { %tokens% };

int main(void) {
    ParseTrace(stdout, "");
    void* parser = ParseAlloc(malloc);
    for (size_t i = 0; i < sizeof(inputTokens) / sizeof(inputTokens[0]); ++i) {
        Parse(parser, inputTokens[i], NULL);
    }
    ParseFree(parser, free);
}
