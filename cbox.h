/*
    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <https://www.gnu.org/licenses/>.
*/
/*
   CBOX.H

   Started in: 06-03-88 

   Useful C functions.

   Copyright (c) 1988, 1997, 2019 Scott Beasley.
   Released into the Public Domain

   Scott Beasley. 
   scottbeasley@gmail.com
*/

/* Useful Macros. */
#define mstrncpy(dest,src,len) {strncpy(dest,src,len);*(dest+len)='\0';}
#define cstrcpy(dest,src) {*dest=src;*(dest+1)='\0';}
#define cstrcat(dest,src) {*(dest+strlen(dest))=src;*(dest+strlen(dest)+1)='\0';}
#define DeleteChar(strbuf,pos) strcpy((strbuf+pos),(strbuf+pos+1))
#define DeleteChars(strbuf,pos,cnt) strcpy((strbuf+pos),(strbuf+pos+cnt))
#define GetCommandLineArg(argnum) ((argnum)<=(argc-1))?argv[argnum]:""
#define NumOfCmdArgs() ((argc-1)>0?argc-1:0)
#define CheckArgCnt(reqnum) ((argc-1)>=(reqnum)?1:0)
#define MakeArray(arrname,type,size) (arrname)=(type)malloc((size)*sizeof((type)))
#define FreeArray(arrname) free(arrname)

#define CPY 0
#define PTR 1

#define BUFFER_SIZE 8192

/*
   Removes known chars from a string.
*/
char *RemoveChars (char *strbuf, char *chrstorm);

/*
   Subsitutes known char(s) in a string with another.
*/
char *ReplaceCharsWith (char *strbuf, char *chrstorm, char chartorlcwith);

/*
   Inserts a string into another string.
*/
char *InsertString (char *strbuf, char *chrstoins, int pos);

/*
   Inserts a char into a string.
*/
char *InsertChar (char *strbuf, char chrtoins, int pos);

/*
   Pads a string to the left, to a know length.
*/
char *LeftFill (char *strbuf, char chrtofill, unsigned len);

/*
   Pads a string to the right, to a know length.
*/
char *RightFill (char *strbuf, char chrtofill, unsigned len);

/*
   Eats the white spaces from the left side of a string.
   This function maintains a proper pointer head.
*/
char *TrimLeft (char *strbuf);

/*
   Eats the white spaces from the right side of a string.
*/
char *TrimRight (char *strbuf);

/*
   Eats the white spaces from the left and right sides of a string.
*/
char *Trim (char *strIn);

/* 
   A case insensitive Search and Replace.
   Searches for all occurances of a string, and replaces 
   it with another string.
*/
char *SearchReplace (char *strbuf, char *strtofnd, char *strtoins);

/* 
   Deletes all occurances of one string, in another string.
*/
char *DeleteString (char *strbuf, char *strtodel);

/* 
   Gets a sub-string from a formated string. Nice strtok replacement.
*/
char *GetStrFld (char *strbuf, int fldno, int ofset, char *sep, char *retstr);

/* 
   Inserts a string into a fomated string.
*/
char *SetStrFld (char *strbuf, int fldno, int ofset, char *sep, char *strToIns);

/* 
   Get the length of as a field in a string.  Used mainly for getting
   the len to malloc mem to call GetStrFld with.
*/
int GetStrFldLen (char *strbuf, int fldno, int ofset, char *sep);

/* 
   Sets a string to upper case.
*/
char *Uppercase (char *strbuf);

/* 
   Sets a string to lower case.
*/
char *Lowercase (char *strbuf);

/* 
   Sees if two strings are equal.
*/
int ismatch (char *str1, char *str2);

/* 
   Find a string inside a text file.
*/
char *FindStrInFile (FILE *fp, char *strToFind, char *strRetStr, int bcpy);

/* 
   Support function for .ini stuff.
*/
char *MatchStrInProfile (FILE *fp, char *strToFind, char *strRetStr,
                          int bsec, int nlen);

/* 
   Read nlen bytes from a stream, or to eof/eol.
*/
char *ReadLine (FILE *fp, char *strRetStr, int nlen);

/* 
   Get the everything on a line past a '='.
*/
char *GetEquVal (char *strLine, char *strRetStr);

/* 
    Gets the next line in a file, and trims the white spaces from it. 
*/
char *GetCleanLine (FILE *fp, char *strRetStr, int nlen);

/* 
    Structured Config file reader.  (Windows .ini style.) 
*/
int GetProfileStr (char *strSection, char *strEntry, char *strDefault,
                    char *strRetStr, int nlen, char *strFile);

/* 
    Nice little function to compare a string with a set of strings. 
*/
int MatchTable (char *strbuf, char *strmatch, char *strsept, int ncase);

/* 
    Cheap access function (Just for seeing if a file is there, no mode checking.). 
*/
int isfile (char *strFile);

/* 
    Inserts a string into a file. 
*/
int InsertStrInFile (FILE *fp, char *strToIns, long pos);

/* 
   Deletes a string from inside a file. 
*/
int DeleteStrInFile (FILE *fp, long stpos, int len);

/* 
   Nifty little debug or data loging function.
*/
int WriteLogFile (char *strSect, char *strEntry, char *strLogEntry, char *strLogFile);

/*
   Cats a set of strings together.
*/
char *String (int argcnt, char *strdest, ...);

/*
   This function searches for known strings, and replaces them with
   another string. 

   StringReplace (strFileName, "sqv|sqr,ruv|run,h_v|h", 0);

   This example would replace all occurences of sqv, with sqr, ruv with
   run and h_v with h.
*/
char *StringReplace (char *strbuf, char *strPattern);

/*
   Wild Card strcmp.   
   ? = Any char.
   * = Anything past that point.

   str2 is the pattern to match with.
*/
int strwildcmp (const char *str1, const char *str2);

/*
   Wild Card strncmp.   
   ? = Any char.
   * = Anything past that point.

   str2 is the pattern to match with.
*/
int strwildncmp (const char *str1, const char *str2, int len);

/*
   Cmps a string to a char.
*/
int strcharcmp (const char *str1, char chr);

int strcharncmp (const char *str1, char chr, int len);

int strtemplatecmp (const char *str1, const char *strPat);

long fcopy (char *dest, char *source);

/*
   Simple string word wrap format.
*/
char *WordWrapStr (char *strBuff, int iWid);

char *GetFileName (char *strFullPath);
char *GetPathName (char *strFullPath, char *strRetPath);

int fdebug (char *strDebugFile, char *strText1, char *strText2);
int flog (char *strFile, char *strbuff);
int fclrlog (char *strFile);

char ** split (char* str, char sep, int *list_size);
void free_array (char ** array, int size);