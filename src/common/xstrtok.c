/*
**		Xstrtok()  -  extended strtok
**
**		see comments below for documentation.
*/


#include <string.h>

#if 0
/* define MAIN to !=0 to get a test main function. Set to 0
** if used in a library.
*/
#define MAIN 1

/* external prototype -- copy the following line into favourite .h file */
char *Xstrtok(char*,char*);
#endif


/* Macros:
**
** QUOTE is the character used for grouping tokens.
** BACKSL is the character to quote QUOTE with.
** IsQuote determins if *x is beginning/end of quoted token.
**
** If required, change QUOTE and BACKSL _only_! Do not change IsQuote.
*/
#define QUOTE '\"'				/* " makes C-mode happy */
#define BACKSL '\\'
#define IsQuote(x) (*(x)==QUOTE && *((x)-1)!=BACKSL)	/* char *x !!!	*/


/************************************************************************

	Xstrtok: replacement for strtok(3)

 Xstrtok is used exactly the same way as strtok. The difference
 is that Xstrtok allows tokens to be grouped in ". So,
 	`This "is a" string'
 would yield the tokens `This', `"is', `a"' and `string' with strtok
 but `This', `is a' and `string' with Xstrtok. The " may themselves
 be quoted by a prefixed backslash, so
 	`This \"is a string\"'
 will yield `This', `"is', `a' and `string"'.
 For an example see the main() at the end of this module.

 Should compile on any Ansi system. Change function declarations to
 old style to compile on pre-Ansi compilers.
 Uses lib functions strlen and strchr.

 Written with PureC on Atari ST.
 This function originates from StrFirst/StrNext() of the Okami Shell
 sources.

 This function may be used freely, but please don't forget to
 mention my name and uucp adress in the program or documentation,
 and, if possible, drop me a line.
 
 	16/10/1991	Wolfram Roesler		wr@bara.oche.de

*************************************************************************/
						/* " makes C-mode happy */


char *xstrtok(char *s,char *Space)
{
  static struct
  {
    char *Cur;		/* current position in string */
    char *End;		/* pointer to end of current token */
    char *Last;		/* pointer to end of string */
  } Str;
  static void KillChar(register char *St,register c);

  if (s)
  {
    Str.Cur   = s;
    Str.Last  = s+strlen(s)-1;
    while (strchr(Space,*Str.Last)) Str.Last--;
    *++Str.Last='\0';
  }
  else
  {
    if (Str.End==Str.Last)
      return 0;
    Str.Cur=Str.End+1;
  }

  while (strchr(Space,*Str.Cur)) Str.Cur++;

  if (Str.Cur>Str.Last) return 0;

  Str.End=Str.Cur;

  for (;*Str.End && !strchr(Space,*Str.End);Str.End++)
    if (IsQuote(Str.End))
    {
      for (Str.End++;*Str.End && !IsQuote(Str.End);Str.End++);
      if (!*Str.End)
        break;
    }
  *Str.End='\0';
  KillChar(Str.Cur,QUOTE);

  return Str.Cur;
} 

static void KillChar(register char *s,register int c)
{
  register char *Ptr;
  for (Ptr=s;*s;s++)
    if (*s!=c)
      *Ptr++ = *s;
    else
      if (*(s-1)==BACKSL)
        *(Ptr-1) = *s;
  *Ptr='\0';
}

#if MAIN
#include <stdio.h>
main(void)
{
  static char s[]="This \\\"is\\\" a \"string\" being \"taken apart\"",*p;
  puts(s);
  for(p=Xstrtok(s," ");p;p=Xstrtok((char*)0," "))
    puts(p);
  return 0;
}
#endif
