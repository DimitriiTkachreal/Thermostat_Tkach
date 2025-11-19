/*	CHARACTER TYPES HEADER
 *	Copyright (c) 2006 by COSMIC Software
 */
#pragma misra system
#ifndef __CTYPE
#define __CTYPE	1

/*	function declarations
 */
_Bool isalnum(char c);
_Bool isalpha(char c);
_Bool iscntrl(char c);
_Bool isdigit(char c);
_Bool isgraph(char c);
_Bool islower(char c);
_Bool isprint(char c);
_Bool ispunct(char c);
_Bool isspace(char c);
_Bool isupper(char c);
_Bool isxdigit(char c);
char tolower(char c);
char toupper(char c);

/*	macro definitions
 */
#define isdigit(c)	(((c) >= '0') && ((c) <= '9'))
#define islower(c)	(((c) >= 'a') && ((c) <= 'z'))
#define isupper(c)	(((c) >= 'A') && ((c) <= 'Z'))

#endif
