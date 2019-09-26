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
   CBOX.C

   Started in: 06-03-88 

   Useful C functions.

   Copyright (c) 1988, 1997, 2019 Scott Beasley.
   Released into the Public Domain

   Scott Beasley.    
   scottbeasley@gmail.com
*/

static int  sstricmp          (const char *str1, const char *str2);
static int  sstrnicmp         (const char *str1, const char *str2, int len);
static char *sstristr        (const char *str1, const char *str2);

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <ctype.h>
#include <time.h>
#include "cbox.h"

/*
   Removes known chars from a string.
*/
char *RemoveChars ( char *strbuf, char *chrstorm )
{
   char *offset;

   offset = ( char * ) NULL;

   while ( *strbuf ) {
      offset = strpbrk ( strbuf, chrstorm );
      if ( offset ) {
         strcpy ( offset, ( offset + 1 ) );
      }

      else
         break;
   }

   return strbuf;
}

/*
   Subsitutes known char(s) in a string with another.
*/
char *ReplaceCharsWith ( char *strbuf, char *chrstorm, char chartorlcwith )
{
   char *offset;

   offset = ( char * ) NULL;

   while ( *strbuf ) {
      offset = strpbrk ( strbuf, chrstorm );
      if ( offset ) {
         * ( offset ) = chartorlcwith;
      }

      else
         break;
   }

   return strbuf;
}

/*
   Inserts a string into another string.
*/
char *InsertString ( char *strbuf, char *chrstoins, int pos )
{
   memmove ( ( strbuf + pos ) + strlen ( chrstoins ),
           ( strbuf + pos ), strlen ( ( strbuf + pos ) ) + 1 );
   memcpy ( ( strbuf + pos ), chrstoins, strlen ( chrstoins ) );

   return strbuf;
}

/*
   Inserts a char into a string.
*/
char *InsertChar ( char *strbuf, char chrtoins, int pos )
{
   memmove ( ( strbuf + pos ) + 1,
           ( strbuf + pos ), strlen ( ( strbuf + pos ) ) + 1 );
   *( strbuf + pos ) = chrtoins;

   return strbuf;
}

/*
   Pads a string to the left, to a know length.
*/
char *LeftFill ( char *strbuf, char chrtofill, unsigned len )
{
   while ( strlen ( strbuf ) < len ) {
      InsertChar ( strbuf, chrtofill, 0 );
   }

   return strbuf;
}

/*
   Pads a string to the right, to a known length.
*/
char *RightFill ( char *strbuf, char chrtofill, unsigned len )
{
   while ( strlen ( strbuf ) < len ) {
      InsertChar ( strbuf, chrtofill, strlen ( strbuf ) );
   }

   return strbuf;
}

/*
   Eats the white spaces from the left side of a string.
   This function maintains a proper pointer head.
*/
char *TrimLeft ( char *strbuf )
{
   while ( isspace ( ( int ) *strbuf ) ) {
      DeleteChar ( strbuf, 0 );
   }

   return strbuf;
}

/*
   Eats the white spaces from the right side of a string.
*/
char *TrimRight ( char *strbuf )
{
   while ( isspace ( ( int ) *( strbuf + strlen ( strbuf ) - 1 ) ) ) {
      *( strbuf + strlen ( strbuf ) - 1 ) = 0;
   }

   return strbuf;
}

/*
   Eats the white spaces from the left and right sides of a string.
*/
char *Trim ( char *strIn )
{
   TrimLeft ( strIn );
   TrimRight ( strIn );

   return strIn;
}

/* 
   A case insensitive Search and Replace.
   Searches for all occurances of a string, and replaces 
   it with another string.
*/
char *SearchReplace ( char *strbuf, char *strtofnd, char *strtoins )
{
   char *offset, *strbase;

   offset = strbase = ( char * ) NULL;

   while ( *strbuf ) {
      offset = sstristr ( !offset ? strbuf : strbase, strtofnd );
      if ( offset ) {
         strbase = ( offset + strlen ( strtoins ) );
         strcpy ( offset, ( offset + strlen ( strtofnd ) ) );
         memmove ( offset + strlen ( strtoins ),
                   offset, strlen ( offset ) + 1 );
         memcpy ( offset, strtoins, strlen ( strtoins ) );
      }

      else
         break;
   }

   return strbuf;
}

/* 
   Deletes all occurances of one string, in another string.
*/
char *DeleteString ( char *strbuf, char *strtodel )
{
   char *offset;

   offset = ( char * ) NULL;

   while ( *strbuf ) {
      offset = strstr ( strbuf, strtodel );
      if ( offset ) {
         strcpy ( offset, ( offset + strlen ( strtodel ) ) );
      }
      else
         break;
   }

   return strbuf;
}

/* 
   Gets a sub-string from a formated string. Nice strtok replacement.
*/
char *GetStrFld ( char *strbuf, int fldno, int ofset, char *sep, char *retstr )
{
   char *offset, *strptr;
   int curfld;

   offset = strptr = ( char * ) NULL;
   curfld = 0;

   strbuf += ofset;

   while ( *strbuf ) {
      strptr = !offset ? strbuf : offset;
      offset = strpbrk ( ( !offset ? strbuf : offset ), sep );

      if ( offset )
         offset++;
      else if ( curfld != fldno ) {
         *retstr = ( char ) 0;
         break;
      }

      if ( curfld == fldno ) {
         strncpy ( retstr, strptr,
         ( int )( !offset ? strlen ( strptr ) + 1 : ( int ) ( offset - strptr ) ) );
         if ( offset )
            retstr[offset - strptr - 1] = 0;

         break;
      }
      curfld++;
   }   
   return retstr;
}

/* 
   Inserts a string into a fomated string.
*/
char *SetStrFld ( char *strbuf, int fldno, int ofset, char *sep, char *strToIns )
{
   char *offset, *strptr, *strHead;
   int curfld;

   offset = strptr = ( char * ) NULL;
   curfld = 0;

   strHead = strbuf;
   strbuf += ofset;

   while ( *strbuf ) {
      strptr = !offset ? strbuf : offset;
      offset = strpbrk ( ( !offset ? strbuf : offset ), sep );

      if ( offset )
         offset++;

      if ( curfld == fldno ) {
         InsertString ( strptr, strToIns,
                     ( int )( !offset ? strlen ( strptr ) : ( int ) ( offset - strptr ) ) );
         break;
      }
      curfld++;
   }   

   return strHead;
}

/* 
   Get the length of as a field in a string.  Used mainly for getting
   the len to malloc mem to call GetStrFld with.
*/
int GetStrFldLen ( char *strbuf, int fldno, int ofset, char *sep )
{
   char *offset, *strptr;
   int curfld, retlen = 0;

   offset = strptr = ( char * ) NULL;
   curfld = 0;

   strbuf += ofset;

   while ( *strbuf ) {
      strptr = !offset ? strbuf : offset;
      offset = strpbrk ( ( !offset ? strbuf : offset ), sep );

      if ( offset )
         offset++;
      else if ( curfld != fldno ) {
         retlen = 0;
         break;
      }

      if ( curfld == fldno ) {
         retlen = ( int )( !offset ? strlen ( strptr ) + 1 : ( int ) ( offset - strptr ) );
         break;
      }
      curfld++;
   }   
   return retlen;
}

/* 
   Sets a string to upper case.
*/
char *Uppercase ( char *strbuf )
{
   char *strbase;

   strbase = strbuf;   

   while ( *strbuf ) {
      *strbuf = *strbuf <= 'z' && *strbuf >= 'a' ?
                *strbuf + 'A' - 'a' : *strbuf;
      strbuf++;
   }

   return strbase;
}

/* 
   Sets a string to lower case.
*/
char *Lowercase ( char *strbuf )
{
   char *strbase;

   strbase = strbuf;   

   while ( *strbuf ) {
      *strbuf = *strbuf <= 'Z' && *strbuf >= 'A' ?
                *strbuf + 'a' - 'A' : *strbuf;
      strbuf++;
   }

   return strbase;
}

/* 
   Sees if two strings are equal.
*/
int ismatch ( char *str1, char *str2 )
{
    int ret = 1;

    while ( *str1 ) {
       if ( toupper ( ( int ) *str1 ) == toupper ( ( int ) *str2 ) ) {
          str1++;
          str2++;
       }

       else
          break;
    }

    if ( !*str1 )
       ret = 0;

    if ( !ret && !isspace ( *str2 ) && *str2 && isalnum ( *str2 ) )
       ret = 1;

    return ret;
}

/* 
   Find a string inside a text file.
*/
char *FindStrInFile ( FILE *fp, char *strToFind, char *strRetStr, int bcpy )
{
   char strLine[1025];
   int nfnd = 0;

   if ( bcpy )
      *strRetStr = 0;

   while ( 1 ) {
      fgets ( strLine, 1023, fp );
      Trim ( strLine );

      Uppercase ( strLine );
      Uppercase ( strToFind );
      if ( strstr ( strLine, strToFind ) ) {
         if ( bcpy )
            strcpy ( strRetStr, strLine );
         nfnd = 1;
         break;
      }

      if ( feof ( fp ) ) {
         break;
      }
   }         
 
   if ( bcpy )
      return strRetStr;
   else
      return ( char * ) NULL;
}

/* 
   Support function for .ini stuff.
*/
char *MatchStrInProfile ( FILE *fp, char *strToFind, char *strRetStr,
                          int bsec, int nlen )
{
   char *strLine;

   strLine = ( char * ) malloc ( nlen + 1 );

   while ( 1 ) {
      ReadLine ( fp, strLine, nlen );
      Trim ( strLine );

      if ( !ismatch ( strToFind, strLine ) ) {
         strcpy ( strRetStr, strLine );
         break;
      }

      if ( !bsec && *strLine == '[' ) {
         char *stroffset;
         stroffset = strchr ( strLine, ']' );
         if ( stroffset ) {
            *strRetStr = 0;
            break;
         }
      }
  
      if ( feof ( fp ) ) {
         *strRetStr = 0;
         break;
      }
   }         

   free ( strLine );
   return strRetStr;
}

/* 
   Read nlen bytes from a stream, or to eof/eol.
*/
char *ReadLine ( FILE *fp, char *strRetStr, int nlen )
{
   char ch;
   char *strBase = strRetStr;

   while ( !feof ( fp ) && nlen ) {
     ch = ( char ) getc ( fp ); 

     if ( ch != '\n' && ch != EOF ) {
        *strRetStr = ch;
        strRetStr++;
        nlen--;
     }
     else 
        break;
   }

   *strRetStr = ( char ) 0;

   return strlen ( strBase ) ? strBase : ( char * ) NULL;
}

/* 
   Get the everything on a line past a '='.
*/
char *GetEquVal ( char *strLine, char *strRetStr )
{
   char *strOffset;

   strOffset = strstr ( strLine, "=" );

   if ( strOffset )
      TrimLeft ( ( strOffset + 1 ) );

   return strcpy ( strRetStr,
                 ( strOffset && *( strOffset + 1 ) ) ? ( strOffset + 1 ) : "" );
}

/* 
    Gets the next line in a file, and trims the white spaces from it. 
*/
char *GetCleanLine ( FILE *fp, char *strRetStr, int nlen )
{
   *strRetStr = 0;

   ReadLine ( fp, strRetStr, nlen );
   TrimLeft ( strRetStr );
   TrimRight ( strRetStr );

   return strRetStr;
}

/* 
    Structured Config file reader.  ( Windows .ini style. ) 
    Returns length of entry.  A 0 if no entry is found, or -1 if
    the file could not be opened, and a -2 if a malloc err occurs.
*/
int GetProfileStr ( char *strSection, char *strEntry, char *strDefault,
                    char *strRetStr, int nlen, char *strFile )
{
   FILE *fp;
   int nRetLen = 0;
   char *ptrSection;

   ptrSection = ( char * ) malloc ( strlen ( strSection ) + 3 );
   fp = fopen ( strFile, "rb" );

   if ( !ptrSection || !fp ) {
      strcpy ( strRetStr, strDefault );
      if ( ptrSection )
         free ( ptrSection );
      if ( fp ) 
         fclose ( fp );
      else 
         return -1;
      return -2;
   }

   strcpy ( ptrSection, strSection );
   InsertChar ( ptrSection, '[', 0 );
   InsertChar ( ptrSection, ']', strlen ( ptrSection ) );

   MatchStrInProfile ( fp, ptrSection, ptrSection, 1, nlen );
   
   if ( *ptrSection ) {
      MatchStrInProfile ( fp, strEntry, strRetStr, 0, nlen );
      nRetLen = strlen ( strRetStr );
   }
   else {
      nRetLen = 0;
   }

   if ( !nRetLen ) {
      strcpy ( strRetStr, strDefault );
   }

   fclose ( fp );
   free ( ptrSection );
   return nRetLen;
}

/* 
    Nice little function to compare a string with a set of strings. 
    Returns >= 0 if match is found.  A -1 if no match is found.
*/
int MatchTable ( char *strbuf, char *strmatch, char *strsept, int ncase )
{
   int nstate = -1, cnt = 0, icmpres;
   int iLen;
   char *strtemp; //[61];

   Trim ( strbuf );
   while ( 1 ) {
      iLen = GetStrFldLen ( strmatch, cnt, 0, strsept );
      strtemp = ( char * ) malloc ( sizeof ( char ) * iLen + 1 );
      GetStrFld ( strmatch, cnt, 0, strsept, strtemp );
      if ( *strtemp ) {
         Trim ( strtemp );

         if ( !ncase ) {
            icmpres = sstricmp ( strbuf, strtemp );
         }
         
         else {
            icmpres = strcmp ( strbuf, strtemp );
         }

         if ( !icmpres ) {
            nstate = cnt;
            free ( strtemp );
            break; 
         }
        
         else {
            if ( !strcmp ( strbuf, strtemp ) ) {
               nstate = cnt;
               free ( strtemp );
               break; 
            }
         }
      }         
      else {
         nstate = -1;
         free ( strtemp );
         break; 
      }
      cnt++;
      free ( strtemp );
   }
   
   return nstate;
}

/* 
    Nice little function to compare a string with a set of strings. 
    Returns >= 0 if match is found.  A -1 if no match is found.
*/
int StrStrMatch ( char *strbuf, char *strmatch, char *strsept, int ncase )
{
   int nstate = -1, cnt = 0, icmpres;
   char *strtemp;
   
   strtemp = ( char * ) malloc ( strlen ( strbuf ) * 2 );
   while ( 1 ) {
      GetStrFld ( strmatch, cnt, 0, strsept, strtemp );
      if ( *strtemp ) {
         TrimLeft ( strtemp );
         TrimRight ( strtemp );

         if ( !ncase ) {
            icmpres = sstristr ( strbuf, strtemp ) == NULL ? 0 : 1;
         }
         
         else {
            icmpres = strstr ( strbuf, strtemp ) == NULL ? 0 : 1;
         }

         if ( !icmpres ) {
            nstate = cnt;
            break; 
         }
        
         else {
            if ( !strstr ( strbuf, strtemp ) ) {
               nstate = cnt;
               break; 
            }
         }
      }         
      else {
         nstate = -1;
         break; 
      }
      cnt++;
   }
   
   free ( strtemp );
   return nstate;
}

/* 
    Cheap access function ( Just for seeing if a file is there, no mode checking. ). 
*/
int isfile ( char *strFile )
{
   FILE *fp;

   fp = fopen ( strFile, "r" );

   if ( !fp ) {
      return 0;
   }
   else {
      fclose ( fp );
      return 1;
   }     
}

/* 
    Inserts a string into a file. 
*/
int InsertStrInFile ( FILE *fp, char *strToIns, long pos )
{
   long linspos, lendpos;
   char *strbuff;

   if ( !pos ) {
      linspos = 0;
   }

   if ( pos == -1 ) {
      linspos = ftell ( fp );
   }

   else {
      linspos = pos;
   }

   fseek ( fp, 0L, SEEK_END );
   lendpos = ftell ( fp );

   if ( ( lendpos - linspos ) < 1 )
      return 0;

   strbuff = ( char * ) malloc ( ( size_t ) ( lendpos - linspos ) + 512 );
   if ( !strbuff ) {
      return 0;
   }

   fseek ( fp, linspos, SEEK_SET );
   memset ( strbuff, 0, ( size_t ) ( lendpos - linspos ) + 512 );
   fread ( strbuff, sizeof ( char ), ( size_t ) ( lendpos - linspos ), fp );
   fseek ( fp, linspos, SEEK_SET );

   fputs ( strToIns, fp );
   fflush ( fp );
   fwrite ( strbuff, sizeof ( char ), ( size_t ) ( lendpos - linspos ), fp );

   free ( strbuff );
   return 1;   
}

/* 
   Deletes a string from inside a file. 
*/
int DeleteStrInFile ( FILE *fp, long stpos, int len )
{
   long linspos, lendpos;
   char *strbuff;
   char strTmp[256];

   puts ( "TP 554" );
   if ( stpos == -1 ) {
      linspos = ftell ( fp );
   }
   else {
      linspos = stpos;
   }
     
   fseek ( fp, 0L, SEEK_END );
   lendpos = ftell ( fp );

   if ( ( lendpos - linspos ) < 1 )
      return 0;

   strbuff = ( char * ) malloc ( ( size_t ) ( lendpos - linspos ) + len + 1 );
   if ( !strbuff ) {
      return 0;
   }

   puts ( "TP 5544" );
   fseek ( fp, linspos + len - 1, SEEK_SET );
   fgets ( strTmp, 255, fp );
   printf ( "<%s>\n", strTmp );
   fseek ( fp, linspos + len - 1, SEEK_SET );
   fread ( strbuff, sizeof ( char ), ( size_t ) ( lendpos - linspos ) - ( len - 1 ), fp );
   fseek ( fp, linspos, SEEK_SET );
   puts ( strbuff );
   fwrite ( ( strbuff + len ), 
          sizeof ( char ), ( size_t ) ( lendpos - linspos ) - ( len - 1 ), fp );

   free ( strbuff );
   puts ( "TP 555" );
   return 1;   
}

/* 
   Nifty little debug or data loging function.
*/
int WriteLogFile ( char *strSect, char *strEntry, char *strLogEntry, char *strLogFile )
{
   FILE *fp;
   time_t tmt;
   struct tm *tstct;
   char *ptrSection;
   char *strRetStr;

   time ( &tmt );
   tstct = localtime ( &tmt );
   ptrSection = ( char * ) malloc ( strlen ( strSect ) + 3 );
   strcpy ( ptrSection, strSect );
   InsertChar ( ptrSection, '[', 0 );
   InsertChar ( ptrSection, ']', strlen ( ptrSection ) );

   if ( !isfile ( strLogFile ) ) {
      fp = fopen ( strLogFile, "wb+" );
      fprintf ( fp, "%s\n%s\n%s  %s\n", ptrSection, strEntry,
                strLogEntry, TrimRight ( asctime ( tstct ) ) );
      fclose ( fp );
      return 1;
   }
   else {
      fp = fopen ( strLogFile, "rb+" );
   }

   if ( !ptrSection || !fp ) {
      if ( ptrSection )
         free ( ptrSection );
      if ( fp ) 
         fclose ( fp );
      else 
         return -1;
      return -2;
   }

   strRetStr = ( char * ) malloc ( 4093 );

   if ( !strRetStr ) {
      return -1;
   }

   MatchStrInProfile ( fp, ptrSection, strRetStr, 1, 132 );
   
   if ( *strRetStr ) {
      MatchStrInProfile ( fp, strEntry, strRetStr, 0, 4092 );
      if ( *strRetStr ) {
         strcpy ( strRetStr, strLogEntry );
         strcat ( strRetStr, "  " );
         strcat ( strRetStr, TrimRight ( asctime ( tstct ) ) );
         strcat ( strRetStr, "\n" );
         InsertStrInFile ( fp, strRetStr, 0L );
      }
      else {
         fprintf ( fp, "\n%s\n%s  %s\n", strEntry, strLogEntry,
                   TrimRight ( asctime ( tstct ) ) );      
      }
   }
   else {
      fseek ( fp, 0L, SEEK_END );
      fprintf ( fp, "\n%s\n%s\n%s  %s\n", ptrSection, strEntry,
                strLogEntry, TrimRight ( asctime ( tstct ) ) );
   }

   fclose ( fp );
   free ( ptrSection );
   free ( strRetStr );
   return 1;
}

/*
   Cats a set of strings together.
*/
char *String ( int argcnt, char *strdest, ... )
{
   va_list args;

   va_start ( args, strdest );
   
   //*strdest = 0;
   while ( argcnt-- ) {
      strcat ( strdest, va_arg ( args, char * ) );
   }
   
   va_end ( args );

   return strdest;
}

/*
   This function searches for known strings, and replaces them with
   another string. 

   StringReplace ( strFileName, "sqv|sqr,ruv|run,h_v|h" );

   This example would replace all occurences of sqv, with sqr, ruv with
   run and h_v with h.
*/
char *StringReplace ( char *strbuf, char *strPattern )
{
   int ilen, ifld = 0;
   char *strSrch, *strRpl, *strPat;

//   if ( !strPattern )
//      return strbuf;

   while ( 1 ) {
      ilen = GetStrFldLen ( strPattern, ifld, 0, "," );
      if ( !ilen )
         break;
      strPat = ( char * ) malloc ( ilen + 1 );
      GetStrFld ( strPattern, ifld, 0, ",", strPat );
      ifld++;

      ilen = GetStrFldLen ( strPat, 0, 0, "|" );
      strSrch = ( char * ) malloc ( ilen + 1 );
      GetStrFld ( strPat, 0, 0, "|", strSrch );

      ilen = GetStrFldLen ( strPat, 1, 0, "|" );
      strRpl = ( char * ) malloc ( ilen + 1 );
      GetStrFld ( strPat, 1, 0, "|", strRpl );

      SearchReplace ( strbuf, strSrch, strRpl );

      free ( strSrch );
      free ( strRpl );
      free ( strPat );
   }

   return strbuf;
}

/*
   Wild Card strcmp.
   ? = Any char.
   * = Anything past that point.

   str2 is the pattern to match with.
*/
int strwildcmp ( const char *str1, const char *str2 )
{
   char c1, c2;
   
   while ( *str1 && *str2 ) {
      c1 = *str1;
      c2 = *str2;

      if ( c2 == '?' ) {
         str1++;
         str2++;
         continue;
      }

      if ( c2 == '*' ) {
         c2 = c1;
         break;
      }

      if ( c1 != c2 ) {
         break;
      }

      str1++;
      str2++;
   }

   return c1 - c2;
}

/*
   Wild Card strncmp.
   ? = Any char.
   * = Anything past that point.

   str2 is the pattern to match with.
*/
int strwildncmp ( const char *str1, const char *str2, int len )
{
   char c1, c2;
   
   while ( *str1 && *str2 ) {
      c1 = *str1;
      c2 = *str2;
      len--;

      if ( c2 == '?' ) {
         str1++;
         str2++;
         continue;
      }

      if ( c2 == '*' ) {
         c2 = c1;
         break;
      }

      if ( c1 != c2 || !len ) {
         break;
      }

      str1++;
      str2++;
   }

   return c1 - c2;
}

/*
   Cmps a string to a char.
*/
int strcharcmp ( const char *str1, char chr )
{
   int ires = 1;

   while ( *str1 ) {
      if ( *str1 != chr ) {
         ires = 0;
         break;
      }

      str1++; 
   }

   return ires;
}

/*
   Cmps a string to a char.
*/
int strcharncmp ( const char *str1, char chr, int len )
{
   int ires = 1;

   while ( *str1 ) {
      if ( !len ) {
         break;
      }

      if ( *str1 != chr ) {
         ires = 0;
         break;
      }

      len--;
      str1++; 
   }

   return ires;
}

long fcopy ( char *dest, char *source )
{
   FILE *fpd, *fps;
   char *buffer;
   size_t incount;
   long totcount = 0L;

   fps = fopen ( source, "rb" );
   if ( !fps )
      return -1L;

   fpd = fopen ( dest, "wb" );
   if ( !fpd ) {
      fclose ( fps );
      return -1L;
   }

   buffer = ( char * ) malloc ( BUFFER_SIZE );
   if ( !buffer ) {
      fclose ( fps );
      fclose ( fpd );
      return -1L;
   }

   incount = fread ( buffer, sizeof ( char ), BUFFER_SIZE, fps );

   while ( !feof ( fps ) ) {
      totcount += ( long ) incount;
      fwrite ( buffer, sizeof ( char ), incount, fpd );
      incount = fread ( buffer, sizeof ( char ), BUFFER_SIZE, fps );
   }

   totcount += ( long ) incount;
   fwrite ( buffer, sizeof ( char ), incount, fpd );

   free ( buffer );
   fclose ( fps );
   fclose ( fpd );

   return totcount;
}

char *WordWrapStr ( char *strBuff, int iWid )
{
   char *strTmp = strBuff;
   int iCnt = 0;

   ReplaceCharsWith ( strBuff, "\n", ' ' );   
   while ( *strTmp ) {
      if ( ( int ) strlen ( strTmp ) > ( int ) iWid ) {
         iCnt = iWid; 
         while ( *( strTmp + iCnt ) ) {
            if ( strchr ( " .?;!,", *( strTmp + iCnt ) ) ) {
               TrimLeft ( ( strTmp + iCnt ) );
               InsertChar ( strTmp, '\n', iCnt );
               strTmp += iCnt + 1; 
               break;
            }
            iCnt--;

            if ( !iCnt ) {
               if ( strchr ( " .?;!,", *( strTmp + iCnt ) ) ) {
                  TrimLeft ( ( strTmp + iWid ) );
                  InsertChar ( strTmp, '\n', iWid );
                  strTmp += iWid + 1; 
                  break;
               }
            }
         }
      }
   
      else
         break;
   }

   return strBuff;
}

/* 
    Structured Config file writer.  ( Windows .ini style. ) (NEEDS work!!!)
*/
int WriteProfileStr ( char *strSect, char *strEntry, char *strLogEntry, char *strLogFile )
{
   FILE *fp;
   char *ptrSection;
   char *strRetStr;

   ptrSection = ( char * ) malloc ( strlen ( strSect ) + 3 );
   strcpy ( ptrSection, strSect );
   InsertChar ( ptrSection, '[', 0 );
   InsertChar ( ptrSection, ']', strlen ( ptrSection ) );

   if ( !isfile ( strLogFile ) ) {
      fp = fopen ( strLogFile, "wb+" );
      fprintf ( fp, "%s\n%s=%s\n", ptrSection, strEntry, strLogEntry );
      fclose ( fp );
      return 1;
   }
   else {
      fp = fopen ( strLogFile, "rb+" );
   }

   if ( !ptrSection || !fp ) {
      if ( ptrSection )
         free ( ptrSection );
      if ( fp ) 
         fclose ( fp );
      else 
         return -1;
      return -2;
   }

   strRetStr = ( char * ) malloc ( 512 );

   if ( !strRetStr ) {
      return -1;
   }

   MatchStrInProfile ( fp, ptrSection, strRetStr, 1, 132 );
   
   if ( *strRetStr ) {
      MatchStrInProfile ( fp, strEntry, strRetStr, 0, 512 );
      if ( *strRetStr ) {
         fseek ( fp, ( ftell ( fp ) - strlen ( strRetStr ) ), SEEK_SET );
         DeleteStrInFile ( fp, -1, strlen ( strRetStr ) + 2 );
         fseek ( fp, ( ftell ( fp ) - strlen ( strRetStr ) ) - 1, SEEK_SET );
         strcpy ( strRetStr, strEntry );
         strcat ( strRetStr, "=" );
         strcat ( strRetStr, strLogEntry );
         strcat ( strRetStr, "\n" );
         InsertStrInFile ( fp, strRetStr, -1 );
      }

      else {
         fprintf ( fp, "%s=%s\n", strEntry, strLogEntry );      
      }
   }
   else {
      fseek ( fp, 0L, SEEK_END );
      fprintf ( fp, "%s\n%s=%s\n", ptrSection, strEntry, strLogEntry );
   }

   fclose ( fp );
   free ( ptrSection );
   free ( strRetStr );
   return 1;
}

char *GetFileName ( char *strFullPath )
{
   int iLen;
   char *strTmp;

   iLen = strlen ( strFullPath ); 

   strTmp = ( strFullPath + strlen ( strFullPath ) );
   while ( 1 ) {
      if ( *strTmp == '\\' || !iLen )
         break;
      strTmp--;
      iLen--;
   }

   if ( *strTmp == '\\' )
      strTmp++;

   return strTmp;
}

char *GetPathName ( char *strFullPath, char *strRetPath )
{
   int iLen;
   char *strTmp;

   strcpy ( strRetPath, strFullPath );
   iLen = strlen ( strRetPath ); 

   strTmp = ( strRetPath + iLen );
   while ( 1 ) {
      if ( *strTmp == '\\' || !iLen )
         break;
      strTmp--;
      iLen--;
   }

   if ( *strTmp == '\\' ) {
      *strTmp = ( char ) 0;
      strTmp++;
   }

   return strRetPath;
}

int fdebug ( char *strDebugFile, char *strText1, char *strText2 )
{
   FILE *fp;
   time_t tmt;
   struct tm *tstct;

   if ( !strDebugFile || !*strDebugFile )
      return 0;      

   time ( &tmt );
   tstct = localtime ( &tmt );

   if ( !strDebugFile || !*strDebugFile )
      return 0;

   fp = fopen ( strDebugFile, "a+" );
   if ( !fp )
      return 1;

   fprintf ( fp, "%s %s %s\n", strText1, strText2, Trim ( asctime ( tstct ) ) );
   fclose ( fp );
   return 0;  
}

int flog ( char *strFile, char *strbuff )
{
   FILE *fp;

   if ( !strFile || !*strFile )
      return 0;      

   fp = fopen ( strFile, "a+" );
   if ( !fp )
      return 1;
   fprintf ( fp, "%s\n", strbuff );
   fclose ( fp );  
   return 0;
}

int fclrlog ( char *strFile )
{
   FILE *fp;

   if ( !strFile || !*strFile )
      return 0;      

   fp = fopen ( strFile, "w" );
   if ( !fp )
      return 1;
   fclose ( fp );  
   return 0;
}

/*  ---------------------------------------------------------------------[<]-
    Function: stricmp

    Synopsis:
    A case insensitive strcmp.  Returns a pointer to head of the str1.
    ---------------------------------------------------------------------[>]-*/

static int 
sstricmp ( 
    const char *str1, 
    const char *str2 )
{
   register char c1, c2;

   while ( *str1 ) 
     {
       c1 = toupper ( ( int ) *str1++ );
       c2 = toupper ( ( int ) *str2++ );

       if ( c1 < c2 ) 
         {
           return -1;
         }
       else if ( c1 > c2 ) 
         {
           return 1;
         }
     }

   if ( *str2 ) 
     {
       return -1;
     } 
   else 
     {
       return 0;
     }
}

/*  ---------------------------------------------------------------------[<]-
    Function: strincmp

    Synopsis:
    A case insensitive strncmp.  Returns a pointer to head of the str1.
    ---------------------------------------------------------------------[>]-*/

static int 
sstrnicmp ( 
    const char *str1, 
    const char *str2, 
    int len )
{
   register char c1, c2;

   while ( *str1 ) 
     {
       c1 = toupper ( ( int ) *str1++ );
       c2 = toupper ( ( int ) *str2++ );
       len--;

       if ( c1 < c2 ) 
         {
           return -1;
         }
       else if ( c1 > c2 ) 
         {
           return 1;
         }

       if ( !len ) 
         {
           return 0;
         }
     }

   if ( *str2 || *str2 ) 
     {
       return -1;
     } 
   else 
     {
       return 0;
     }
}

/*  ---------------------------------------------------------------------[<]-
    Function: stristr

    Synopsis:
    A case insensitive strstr.  Returns a pointer to head of the str1.
    ---------------------------------------------------------------------[>]-*/

static char *
sstristr ( 
    const char *str1, 
    const char *str2 )
{
   char *strtmp = ( char * ) str1;
   int iret = 1;

   while ( *strtmp ) 
     {
       if ( strlen ( strtmp ) >= strlen ( str2 ) ) 
         {
//           iret = strnicmp ( strtmp, str2, strlen ( str2 ) );
           iret = strncmp ( strtmp, str2, strlen ( str2 ) );
         }
       else 
         {
           break;
         }

       if ( !iret ) 
         {
           break;
         }

       strtmp++;
     }

   return !iret ? strtmp : ( char * ) NULL;
}

char ** split (char* str, char sep, int *list_size)
{
    int list_len = 0, seg_len = 0;
    // Start with 20 in the array.
    char **list = malloc (20 * sizeof (char **));
    char *x, *last_stop = str;
    
    while (1) {
      if (*str == sep || *str == 0x00) {
          // Realloc every 20 added.
          if (*list_size % 20 == 0) {
              char *temp_holder = NULL;
              temp_holder = realloc (*list, (list_len + 20) * sizeof (char **));

              // Reset the array head if the realloc was good.
              if (temp_holder != NULL) {
                  *list = temp_holder;
              } else {
                  // If the realloc failed, bail and let the caller know.
                  free (*list);
                  *list_size = 0;
                  return (char**) NULL;
              }
          }

          // Alloc the memory for the field element and add to the array.
          list[list_len] = malloc ((seg_len + 1) * sizeof (char));
          strncpy (list[list_len], last_stop, seg_len);
          //printf ("<%s>\n", list[list_len]);
          last_stop += seg_len + 1;
          seg_len = 0;
          list_len++;
      } else {
          seg_len++;
      }

      if (*str == 0x00) {
         break;
      }
      str++;
    }

    // Set the final array length and return the list.
    *list_size = list_len;
    // Adjust the array to the final size.
    char *temp_holder = NULL;
    temp_holder = realloc (*list, list_len * sizeof (char **));
    if (temp_holder != NULL) {
        *list = temp_holder;
    } else {
        // If the realloc failed, then bail and let the caller know.
        free (*list);
        *list_size = 0;
        *list = NULL;
    }
    
    return list;
}

void free_array (char ** array, int size)
{
  int x = 0;

  for (x = 0; x < size; x++) {
    if (array[x] != NULL) {
        free (array[x]);
        array[x] = NULL;
    }
  }

  if (*array)
      free (*array);
}
