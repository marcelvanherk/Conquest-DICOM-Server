/* mvh  19990827: added (limited) wildcard mapping in GetACRNema: AE, port or IP may end on '*' 
   mvh  20001105: replaced m-alloc by new
   mvh  20011109: Made AE mapping case insensitive
   ljz  20020524: In function 'GetACRNema', first try case SENSITIVE then INSENSITIVE
   mvh  20030701: Added compression column (with this code is this column is not optional!)
   mvh  20030703: KNOWN BUG: number of columns may not be 3 (crashes)
   ljz  20030709: Solved above problem; rewritten parsing of Acrnema.map
   ljz  20030711: Fixed trailing spaces in acrnema.map
   ljz  20031118: Fixed leak InitACRNemaAddressArray
*/
   

#ifndef	WHEDGE
#	include	"dgate.hpp"
#else
#	include	"master.h"
#endif


BYTE	ACRNEMAMAP [ 256 ] = "acrnema.map";
Array < ACRNemaAddress * >	ACRNemaAddressArray;



BOOL
InitACRNemaAddressArray(void)
{ FILE*			f;
  char*			pBuffer;
  int			i, iSize;
  char			*pSrc, *pDest;
  ACRNemaAddress*	pACR;
  int			iNbAddresses;

  /* Open AcrNema.map */
  f = fopen((char*)ACRNEMAMAP, "rb");
  if (!f)
    return FALSE;

  /* Read it in memory */
  fseek(f, 0, SEEK_END);
  iSize = ftell(f);
  fseek(f, 0, SEEK_SET);
  pBuffer = (char*)malloc(iSize + 2);
  if (!pBuffer)
  { fclose(f);
    return FALSE;
  }
  if (fread(pBuffer, 1, iSize, f) != (unsigned int)iSize)
  { fclose(f);
    free(pBuffer);
    return FALSE;
  }
  fclose(f);

  /* Make it a CR and zero terminated string */
  pBuffer[iSize++] = '\r';
  pBuffer[iSize]   = '\0';

  /* Get rid of all comments like this one */
  pSrc = pBuffer;
  pDest = pBuffer;
  i = 0;
  while (i<iSize)
  { if ((pSrc[i] == '/') && (pSrc[i + 1] == '*'))
    { /* Found the start of a comment */
      i += 2;
      while (i<iSize)
      { if ((pSrc[i] == '*') && (pSrc[i + 1] == '/'))
	{ i++;
	  break;
	}
	i++;
      }
    }
    else
      *pDest++ = pSrc[i];
    i++;
  }
  *pDest = 0;

  /* Replace all LF by CR, and all '\t' by ' ' */
  iSize = strlen(pSrc);
  for (i=0; i<iSize; i++)
    if (pSrc[i] == '\n')
      pSrc[i] = '\r';
    else if (pSrc[i] == '\t')
      pSrc[i] = ' ';

  /* The characters '#', ';' and '//' are one-line comments */
  pDest = pSrc;
  i = 0;
  while (i<iSize)
  { if ((pSrc[i] == '/') && (pSrc[i + 1] == '/'))
    { /* Found the start of a comment */
      i += 2;
      while (i<iSize)
      { if (pSrc[i] == '\r')
	  break;
	i++;
      }
    }
    else if ((pSrc[i] == '#') || (pSrc[i + 1] == ';'))
    { /* Found the start of a comment */
      i += 1;
      while (i<iSize)
      { if (pSrc[i] == '\r')
	  break;
	i++;
      }
    }
    else
      *pDest++ = pSrc[i];
    i++;
  }
  *pDest = 0;

  /* Eliminate empty lines, double space... */
  iSize = strlen(pSrc);
  pDest = pSrc;
  i = 0;
  while (i<iSize)
  { if (pSrc[i] == ' ')
    { if (pDest == pSrc)
      { /* Skip leading space */
        i++;
	continue;
      }
      else if ((pDest[-1] == ' ') || (pDest[-1] == '\r'))
      { /* Skip space when the previous was space or CR */
        i++;
	continue;
      }
      else if (pSrc[i+1] == '\r')
      { /* Skip space when the next is CR */
        i++;
	continue;
      }
    }
    else if (pSrc[i] == '\r')
    { if (pDest == pSrc)
      { /* Skip leading CR */
        i++;
	continue;
      }
      else if (pDest[-1] == '\r')
      { /* Skip CR when the previous was a CR */
        i++;
	continue;
      }
    }
    *pDest++ = pSrc[i];
    i++;
  }
  *pDest = 0;

  /* Count the iNbAddresses */
  iSize = strlen(pSrc);
  iNbAddresses = 0;
  for (i=0; i<iSize; i++)
    if (pSrc[i] == '\r')
      iNbAddresses++;

  /* Now fill the ACRNemaAddressArray */
  pDest = pSrc;
  for (i=0; i<iNbAddresses; i++)
  { pACR = new ACRNemaAddress;
    strcpy(pACR->Name, "");
    strcpy(pACR->IP, "");
    strcpy(pACR->Port, "");
    strcpy(pACR->Compress, "un");

    /* Get a line */
    *strchr(pDest, '\r') = 0;
    sscanf(pDest, "%s %s %s %s",
	pACR->Name,
	pACR->IP,
	pACR->Port,
	pACR->Compress);

    ACRNemaAddressArray.Add(pACR);
    pDest = pDest + strlen(pDest) + 1;
  }

  free(pBuffer);
  return TRUE;
}


BOOL
CloseACRNemaAddressArray()
	{
	while ( ACRNemaAddressArray.GetSize() )
		{
		delete ACRNemaAddressArray.Get(0);
		ACRNemaAddressArray.RemoveAt(0);
		}
	return ( TRUE );
	}


static
BOOL
__iswhitespacea(char	ch)
	{
	switch ( ch )
		{
		case	' ':
		case	'\t':
		case	'\r':
		case	'\n':
		case	0:
			return ( TRUE );
		}
	return ( FALSE );
	}


BOOL
GetACRNema(char* ACRNema, char* ip, char* port, char* compress)
	{
	UINT	Index;
	char	s[20];
	char	*t;

	// trim the ACRNema address

        memset(s, 0, 20);

	strcpy(s, ACRNema);
	if(!strlen(s))
		return ( FALSE );
	while(__iswhitespacea(s[strlen(s)-1]))
		{
		s[strlen(s)-1] = '\0';
		if(!strlen(s))
			return ( FALSE );
		}


	Index = 0;
	while ( Index < ACRNemaAddressArray.GetSize())
		{
		t = ACRNemaAddressArray.Get(Index)->Name;

		// try wildcard mapping, e.g., s=NG12, t=NG*, IP=101.11.11.*, Port=56*
		if (t[strlen(t)-1] == '*')
			{
			if (!memicmp(t, s, strlen(t)-1))
				{
				strcpy(ip, ACRNemaAddressArray.Get(Index)->IP);
				if (ip[strlen(ip)-1]=='*')
					strcpy(ip+strlen(ip)-1, s+strlen(t)-1);

				strcpy(port, ACRNemaAddressArray.Get(Index)->Port);
				if (port[strlen(port)-1]=='*')
					strcpy(port+strlen(port)-1, s+strlen(t)-1);
				strcpy(compress, ACRNemaAddressArray.Get(Index)->Compress);
				return ( TRUE );
				}
			}

		// try exact match (case sensitive)
		if (!strcmp(t, s))
			{
			strcpy(ip, ACRNemaAddressArray.Get(Index)->IP);
			strcpy(port, ACRNemaAddressArray.Get(Index)->Port);
			strcpy(compress, ACRNemaAddressArray.Get(Index)->Compress);
			return ( TRUE );
			}
		++Index;
		}

	Index = 0;
	while ( Index < ACRNemaAddressArray.GetSize())
		{
		t = ACRNemaAddressArray.Get(Index)->Name;

		// try exact match (case insensitive)
		if (!stricmp(t, s))
			{
			strcpy(ip, ACRNemaAddressArray.Get(Index)->IP);
			strcpy(port, ACRNemaAddressArray.Get(Index)->Port);
			strcpy(compress, ACRNemaAddressArray.Get(Index)->Compress);
			return ( TRUE );
			}
		++Index;
		}

	return ( FALSE );
	}


