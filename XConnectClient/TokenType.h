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