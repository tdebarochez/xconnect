#ifndef TOKENTYPE_H
#define TOKENTYPE_H

typedef enum {
	is_identifier, // Token that begin with a letter [a-z]
	is_number_value, // Token that begin with a numbers [0-9] or a sign (+ or -)
	is_string_value, // Token that begin with " and end with "
	is_open_parenthesis, // Token (
	is_close_parenthesis, // Token )
	is_open_bracket, // Token [
	is_close_bracket, // Token ]
	is_open_akk, // Token {
	is_close_akk, // Token }
	is_smaller, // Token <
	is_greater, // Token >
	is_eof,
	is_unknown,
} token_type;

#endif