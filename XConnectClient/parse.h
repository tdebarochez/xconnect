#ifndef __PARSE_H__
#define __PARSE_H__

#include <stdio.h>
#include <sys/types.h>

#include "TokenType.h"

void SetParsingFileName(const char *path);
const char* GetParsingFileName();

int GetParsingLineNumber();

token_type ReadToken(FILE *fp);

int TokenIs(const char *s1);
int TokenTypeIs(token_type ty);

token_type SkipBehindToken(FILE *fp, token_type ty);
token_type SkipToToken(FILE *fp, token_type ty);

const char* GetTokenBuffer();

void Writeln(FILE *str, int offset);
float stringToFloat(const char *s);
char* TrimRight(char *szSource );
char* TrimLeft(char *szSource );

time_t getFileCreationTime(const char *path);
time_t getFileModificationTime(const char *path);
long getFileSize(const char *path);

#endif __PARSE_H__
