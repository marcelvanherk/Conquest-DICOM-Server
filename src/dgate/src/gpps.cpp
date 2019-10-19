// GetPrivateProfileString() -- approximate implementation of
// Windows NT System Services version of GetPrivateProfileString()
// probably doesn't handle the NULL key for section name or value key
// correctly also, doesn't provide Microsoft backwards compatability
// wrt TAB characters in the value string -- Microsoft terminates value
// at the first TAB, but I couldn't discover what the behavior should
// be regarding TABS in quoted strings so, I treat tabs like any other
// characters -- NO comments following value string separated by a TAB
// are allowed (that is an anachronism anyway)

// 20040111	mvh	Fixes: defaults now work, allow DOS type files, case insensitive
// 20070330	mvh	Change by Mark Pearson: also allow tabs instead of spaces around =
// 20090620	jf	Include file stuff
// 20091231	bcb	Changed char* to const char* and added blank[]="" for gcc4.2 warnings
// 20100111	mvh	Merged
// 20100124	mvh	Fix: blank item (e.g., password = ) returned default not empty
// 20100124	mvh	Added cache for ini items: accelerates server with cached disk data 7x!!!!
// 20100124	mvh	ini file is tested for newer version every second
// 20100309	bcb	Added double parentheses (gcc4.2 Warnings)
// 20100703	mvh	Merged	
// 20110320	mvh	Removal of trailing blanks in value also removed one character after blank
// 20110331	mvh	Added FlushPrivateProfileStringCache
// 20110407	mvh	Extended FlushPrivateProfileStringCache to reset timestamp
//			Fixed that was reading beyond current section
// 20191019	mvh     Attemp to fix buffer overrun when reading long entries

int      gpps=0, gppstime=0;

#include <stdio.h>
#ifdef UNIX
#include <unistd.h>
#endif
#include <sys/types.h>
#include <sys/stat.h>
#include <string.h>
#include <time.h>
#include "gpps.hpp"
#include "dicom.hpp"
#include "dprintf.hpp"	// Debug output

#ifndef WIN32
#define stricmp(s1, s2) strcasecmp(s1, s2) //DUCKHEAD92
#endif

#define NINICACHE 1000

char			IniCache[NINICACHE][64];
char			IniSections[NINICACHE][64];
char			IniValues[NINICACHE][1024];
int			IniCheckSums[NINICACHE];
CRITICAL_SECTION 	IniCritical;
int			IniTop=0, IniBottom=0;
time_t			IniTm=0, lasttime=0;

int into_IniCache(const char *section, const char *in, char *value)
	{ 
	int i, sum, r;

	const char *p = in;
	sum = 0;
	while(*p) sum+=*p++;
	p = section;
	while(*p) sum+=*p++;

	if (IniTop==0 && IniBottom==0)
		InitializeCriticalSection(&IniCritical);

	EnterCriticalSection(&IniCritical);

	/* clear cache if it is empty */
	if (IniTop==0 && IniBottom==0)
		for (i=0; i<NINICACHE; i++)
			IniCache[i][0]=0;

	if ((IniTop+1)%NINICACHE == IniBottom)
		IniBottom = (IniBottom+1)%NINICACHE;	// delete oldest entry

	strcpy(IniCache[IniTop], in);
	strcpy(IniSections[IniTop], section);
	strcpy(IniValues[IniTop], value);
        IniCheckSums[IniTop] = sum;
        r = IniTop;

      	IniTop = (IniTop+1)%NINICACHE;

	LeaveCriticalSection(&IniCritical);

	return r;
  	}

int isin_IniCache(const char *section, const char *in, char *file)
	{
	int i, sum;
	const char *p = in;
	struct stat statbuf;

	if (IniTop==0 && IniBottom==0) return -1;

	sum = 0;
	while(*p) sum+=*p++;
	p = section;
	while(*p) sum+=*p++;

	EnterCriticalSection(&IniCritical);

	// retest for changed ini file once per second
	if (lasttime!=time(NULL))
	  {
	  lasttime = time(NULL);
	  if (stat(file, &statbuf) != -1)
		if (statbuf.st_mtime != IniTm)
		{
		IniTop=0;
		IniBottom=0;
		IniTm = statbuf.st_mtime;
		lasttime = time(NULL);
	        LeaveCriticalSection(&IniCritical);
		return -1;
		}
	  }

	for (i=IniTop; i!=IniBottom; i=(i==0 ? NINICACHE-1 : i-1))
		{
		if (IniCheckSums[i] == sum)
			if (strcmp(IniCache[i], in)==0 && strcmp(IniSections[i], section)==0)
				{
				LeaveCriticalSection(&IniCritical);
				return i;
				}
		}
				
	LeaveCriticalSection(&IniCritical);
	return -1;
	}

DWORD
MyGetPrivateProfileString(const char *theSection,	// section name
	const char *theKey,			// search key name
	const char *theDefault,			// default value if not found
	char *theReturnBuffer,			// return valuse stored here
	size_t theReturnBufferLength,		// byte length of return buffer
	char *theIniFileName)			// pathname of ini file to search
{
	int i, t = time(NULL);
	gpps++;

	// try to find it in the cache
	if ((i = isin_IniCache(theSection, theKey, theIniFileName))>=0)
	{ strcpy(theReturnBuffer, IniValues[i]);
  	  gppstime += time(NULL)-t;
	  return strlen(theReturnBuffer);
	}

	FILE *aFile = theIniFileName ? fopen(theIniFileName, "r") : NULL;
	size_t aLength = (theDefault == NULL) ? 0 : strlen(theDefault);
	BOOL matched = FALSE;

	if(theReturnBufferLength == 0 || theReturnBuffer == NULL)
	{
		if(aFile)
		{
			fclose(aFile);
		}
		return 0;
	}

	if(aFile == NULL)
	{
		// no ini file specified, return the default

		++aLength;	// room for NULL char
		aLength = theReturnBufferLength < aLength ?
			theReturnBufferLength : aLength;
		strncpy(theReturnBuffer, theDefault, aLength);
		theReturnBuffer[aLength - 1] = '\0';
		return aLength - 1;
	}

	char aLine[2048];
	char *aValue;
	char *aString;
	char blank[] = "";
	size_t aLineLength;
	size_t aReturnLength = 0;
	BOOL aSectionFound = FALSE;

	while(fgets(aLine, sizeof(aLine), aFile) != NULL)
	{
		aLineLength = strlen(aLine);
		// strip final '\n'
		if(aLineLength > 0 && aLine[aLineLength - 1] == '\n')
		{
			aLine[aLineLength - 1] = '\0';
			aLineLength--;
		}
		// strip final '\r'
		if(aLineLength > 0 && aLine[aLineLength - 1] == '\r')
		{
			aLine[aLineLength - 1] = '\0';
			aLineLength--;
		}
		switch(*aLine)
		{
			case ' ': // blank line
			case ';': // comment line
				continue;
			break;
			
			case '[':	// section marker

				if((aString = strchr(aLine, ']')))
				{
					*aString = '\0';

					// accept as matched if NULL key or exact match

					if(!theSection || !stricmp(aLine + 1, theSection))
					{
						aSectionFound = TRUE;
					}
					else if (theSection && stricmp(aLine + 1, theSection))
					{
						aSectionFound = FALSE;
					}
				}

			break;

			default:

				// try to match value keys if in proper section

				if(aSectionFound)
				{
					// try to match requested key
	
					if((aString = aValue = strchr(aLine, '=')))
					{
						*aValue = '\0';
						++aValue;

						// strip leading blanks in value field

						while((*aValue == ' ' || *aValue == '\t') && aValue < aLine + sizeof(aLine))
						{
							*aValue++ = '\0';
						}
						if(aValue >= aLine + sizeof(aLine))
						{
							aValue = blank;
						}
					}
					else
					{
						aValue = blank;
					}

					// strip trailing blanks from key

					if(aString)
					{
						while(--aString >= aLine && (*aString == ' ' || *aString == '\t'))
						{
							*aString = '\0';
						}
					}

					// see if key is matched
					
					if(theKey == NULL || !stricmp(theKey, aLine))
					{
						// matched -- first, terminate value part
						matched = TRUE;
						aLineLength = strlen(aValue);

						// remove trailing blanks from aValue if any

						aString = aValue + aLineLength;
						
						while(--aString > aValue && (*aString == ' ' || *aString == '\t'))
						{
							*aString = '\0';
							--aLineLength;
						}

						// unquote value if quoted

						if(aLineLength >= 2 && aValue[0] == '"' &&
							aValue[aLineLength - 1] == '"')
						{
							// string quoted with double quotes

							aValue[aLineLength - 1] = '\0';
							++aValue;
							aLineLength -= 2;
						}
						else
						{
							// single quotes allowed also...

							if(aLineLength >= 2 && aValue[0] == '\'' &&
								aValue[aLineLength - 1] == '\'')
							{
								aValue[aLineLength - 1] = '\0';
								++aValue;
								aLineLength -= 2;
							}
						}

						// compute maximum length copyable

						aLineLength = (aLineLength <
							theReturnBufferLength - aReturnLength) ? aLineLength :
							theReturnBufferLength - aReturnLength;

						// do the copy to return buffer

						if(aLineLength>=0)
						{
							strncpy(&theReturnBuffer[aReturnLength],
								aValue, aLineLength);
							aReturnLength += aLineLength;
							if (aReturnLength >= theReturnBufferLength)
							{
								aReturnLength = theReturnBufferLength-1;
								OperatorConsole.printf("***Ini file item '%s:%s' trucated\n", theSection, theKey);
							}								
							if(aReturnLength < theReturnBufferLength)
							{
								theReturnBuffer[aReturnLength] = '\0';
								++aReturnLength;
								
							}
						}
					}
				}

			break;
		}
	}

	if(aFile)
	{
		fclose(aFile);
	}

	if (aReturnLength==0 && !matched)
	{
		++aLength;	// room for NULL char
		aLength = theReturnBufferLength < aLength ?
			theReturnBufferLength : aLength;
		strncpy(theReturnBuffer, theDefault, aLength);
		theReturnBuffer[aLength - 1] = '\0';
		// store into cache
		into_IniCache(theSection, theKey, theReturnBuffer);
                gppstime += time(NULL)-t;
		return aLength - 1;
	}

	// store into cache
	into_IniCache(theSection, theKey, theReturnBuffer);
	gppstime += time(NULL)-t;
	return aReturnLength > 0 ? aReturnLength - 1 : 0;
}

void FlushPrivateProfileStringCache(void)
{ lasttime = 0;
  IniTm    = 0;
}
