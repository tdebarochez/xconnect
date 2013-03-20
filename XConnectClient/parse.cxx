/*
This program is free software: you can redistribute it and/or modify it under
the terms of the GNU Lesser General Public License as published by the Free
Software Foundation, either version 3 of the License, or (at your option) any
later version.

This program is distributed in the hope that it will be useful, but WITHOUT
ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public License for more
details.

You should have received a copy of the GNU Lesser General Public License along
with this program. If not, see <http://www.gnu.org/licenses/>.
*/

#include <string.h>
#include <assert.h>
#include "parse.h"
#include <sys/stat.h>

#ifdef IBMCPLUSPLUS
#include <strings.h>
#endif

// global variables 
// character read from the file

token_type TOKENTYPE= is_eof;
const int TOKENMAXSIZE = 20000;
char TOKENBUFFER[TOKENMAXSIZE] = {'\0'};
char g_parsingPath[512] = {'\0'};
int g_parsingLineNb = 1;

void SetParsingFileName(const char *path)
{
	strcpy(g_parsingPath, path);
	g_parsingLineNb = 1;
}

const char* GetParsingFileName()
{
	return g_parsingPath;
}

int GetParsingLineNumber()
{
	return g_parsingLineNb;
}

int TokenIs(const char *s2)
{
#if defined _WIN32 && !defined WINDU_AIS
	return (_stricmp(TOKENBUFFER, s2) == 0);
#else
	return (strcasecmp(TOKENBUFFER, s2) == 0);
#endif
}

int TokenTypeIs(token_type ty)
{
	return (TOKENTYPE == ty);
}

token_type SkipBehindToken (FILE *fp, token_type ty)
{
	while (!(TOKENTYPE == is_eof || TOKENTYPE == ty ))
		TOKENTYPE = ReadToken(fp);
	if (TOKENTYPE == ty)
		TOKENTYPE = ReadToken(fp);
	return TOKENTYPE;
}

token_type SkipToToken (FILE *fp, token_type ty)
{
	while (!(TOKENTYPE == is_eof || TOKENTYPE == ty ))
		TOKENTYPE = ReadToken(fp);
	//if (TOKENTYPE == ty)
	//  TOKENTYPE = ReadToken(fp);
	return TOKENTYPE;
}

static int car_read = ' ';

token_type ReadToken(FILE *fp)
{ // skip leading blanks
	char *buf;
	buf = TOKENBUFFER;
	fpos_t fpos;
	fgetpos(fp, &fpos);
	// if new file, init car_read
	if (fpos == 0) car_read = ' ';
	while ((car_read <= ' ') && !(feof (fp)))
	{
		car_read = getc(fp);

		if (car_read == '\n') {
			++g_parsingLineNb;
		}

		// Skip commentlines (lines thar begin with a # but not begin with "#>>").
		if (car_read == '#')
		{  
			car_read = getc(fp);
			if (car_read == '>')
			{
				car_read = getc(fp);
				if (car_read == '>')
					car_read = ' ';
				else
				{
					ungetc(car_read,fp);
					fgets(TOKENBUFFER, 999, fp);
					++g_parsingLineNb;
					car_read = ' ';
				}
			}
			else // skips line
			{
				ungetc(car_read,fp);
				fgets(TOKENBUFFER, 999, fp);
				++g_parsingLineNb;
				car_read = ' ';
			}
		}
	}

	// It's end of file.
	if (feof(fp))
	{
		strcpy(TOKENBUFFER, "EOF");
		TOKENTYPE = is_eof; 
		return TOKENTYPE;
	}

	// Could be brace.
	TOKENTYPE = is_unknown;
	switch (car_read)
	{
	case '(':
		TOKENTYPE =  is_open_parenthesis;
		break;
	case ')':
		TOKENTYPE = is_close_parenthesis;
		break;
	case '[':
		TOKENTYPE =  is_open_bracket;
		break;
	case ']':
		TOKENTYPE =  is_close_bracket;
		break;
	case '{':
		TOKENTYPE = is_open_akk;
		break;
	case '}':
		TOKENTYPE = is_close_akk;
		break;
	case '<':
		TOKENTYPE = is_smaller;
		break;
	case '>':
		TOKENTYPE = is_greater;
		break;
	default:
		break;
	} // end switch	special characters
	if (TOKENTYPE != is_unknown) {
		*(buf++) = (char)car_read;
		*buf = 0;
		car_read = getc(fp);
		if (car_read == '\n')
			++g_parsingLineNb;
		return TOKENTYPE;
	}
	
	// It's an identifier.
	if ((car_read >= 'A' &&
		car_read <= 'Z' ) ||
		(car_read >= 'a' &&
		car_read <= 'z' )
		)
	{
		*(buf++) = (char)car_read;
		while(1)
		{
			car_read = getc(fp);
			if ( (car_read >= '0' &&
				car_read <= '9') ||
				(car_read >= 'A' &&
				car_read <= 'Z')  ||
				(car_read >= 'a' &&
				car_read <= 'z') ||
				car_read == '-' ||
				car_read == '.' ||
				car_read == '_'
				) {
				assert(buf - TOKENBUFFER < TOKENMAXSIZE * sizeof(char));
				*(buf++) = (char)car_read;
			}
			else {
				if (car_read == '\n')
					++g_parsingLineNb;
				*buf = 0;
				TOKENTYPE = is_identifier;
				return TOKENTYPE;
			}
		}
	}

	// It's a string
	if (car_read == '"') {
		car_read = getc(fp);
		while (car_read != '"' && !feof(fp)) {
			if (car_read == '\n') {
				++g_parsingLineNb;
			}
			assert(buf - TOKENBUFFER < TOKENMAXSIZE * sizeof(char));
			*(buf++) = (char)car_read;
			car_read = getc(fp);
		}
		if (car_read == '"') {
			car_read = getc(fp);
		}
		if (car_read == '\n')
			++g_parsingLineNb;
		*buf = 0;
		TOKENTYPE = is_string_value;
		return TOKENTYPE;
	}
	
	// It's a number.
	if ((car_read >= '0' &&
		car_read <= '9' ) ||
		(car_read == '+') ||
		(car_read == '-')) {
		assert(buf - TOKENBUFFER < TOKENMAXSIZE * sizeof(char));
		*(buf++) = (char)car_read;
		while (1) {
			car_read = getc(fp);
			if ((car_read >= '0' && car_read <= '9')  ||
				(car_read == '.') ||
				(car_read == '+') ||
				(car_read == '-') ||
				(car_read == 'e') ||
				(car_read == 'E')) {
				assert(buf - TOKENBUFFER < TOKENMAXSIZE * sizeof(char));
				*(buf++) = (char)car_read;
			}
			else {
				if (car_read == '\n')
					++g_parsingLineNb;
				*buf = 0;
				TOKENTYPE = is_number_value;
				return TOKENTYPE;
			}
		}
	}
	// non significant char, skip it
	car_read = getc(fp);
	if (car_read == '\n')
		++g_parsingLineNb;
	return ReadToken(fp);
}

const char* GetTokenBuffer()
{
	return TOKENBUFFER;
}

void Writeln(FILE *str, int offset)
{
	fprintf(str, "\n");
	for (int i = 0; i < offset; i++) fprintf(str, " ");
}

float stringToFloat(const char * s)
{
	float result;
	return (sscanf(s, "%f", &result) > 0 )?result:0;
}

char* TrimRight( char *szSource )
{
	
	if (!szSource) return NULL;
	// This method removes the trailing blanks at the end of a string
	char *pszLastChar=szSource-1;
	char *pszCurrent=szSource;
		
	// Set pointer to end of string to point to the character just
	//  before the 0 at the end of the string.
//	pszEOS = szSource + strlen( szSource ) - 1;
	
	while(*pszCurrent != '\0')
	{
		if (*pszCurrent!=' ')
			pszLastChar = pszCurrent;
		*pszCurrent++;
	}
	*(++pszLastChar)='\0';
	return szSource;
}

char* TrimLeft( char *szSource )
{
	
	if (!szSource) return NULL;
	// This method removes the trailing blanks at the end of a string
	while ((*szSource)==' ')
		szSource++;
	return szSource;
}

time_t getFileCreationTime(const char* path)
{
	struct _stat fileStatus;
	int result = _stat( path, &fileStatus );
	
	/* Check if statistics are valid: */
	if( result != 0 )
	{
		printf( "Bad file path\n" );
		return 0;
	}
	else
		return fileStatus.st_ctime;
}

time_t getFileModificationTime(const char* path)
{
	struct _stat fileStatus;
	int result = _stat( path, &fileStatus );
	
	/* Check if statistics are valid: */
	if( result != 0 )
	{
		printf( "Bad file path\n" );
		return 0;
	}
	else
		return fileStatus.st_mtime;
}

long getFileSize(const char* path)
{
	struct _stat fileStatus;
	int result = _stat( path, &fileStatus );
	
	/* Check if statistics are valid: */
	if( result != 0 )
	{
		printf( "Bad file path\n" );
		return 0;
	}
	else
		return fileStatus.st_size;
}
