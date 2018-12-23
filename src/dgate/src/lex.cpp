/*
20000822	ljz	Created
20000823	ljz	Support cases where the input-file is empty or contains
			only white-space.
20010716	ljz	Fix: Allow TOKENs that are substrings of other TOKENs.
			The largest TOKEN that fits will be the result.
20050103	mvh	Changed include file to lower case (for linux)
20050108	mvh	Removed windows.h for linux compile
20050118	mvh	GNU C++ knows BOOL etc, do not redefine
20090620	jf	Include file stuff
20100120	mvh	Fixed const issue detected with ms8
20100309	bcb	Changed int to unsigned int, add ui as unsigned int in for loop (gcc4.2 Warnings)
20100619	bcb	Fix gcc4 warnings, improve speed and UNUSED_ARGUMENT.
20100717	bcb	Merged
*/

/*
   Lex.cpp contains the implementation of two classes:
   - TK
   - Lex

   These two classes are defined in Lex.hpp

   The original UCDMC code was shipped with only an object-file 'lex.o', and
   not with the source-code of these classes.

   The only compiler that could cope with lex.o was M$VC++.
   However: project-settings had to include a statement to exclude a standard
   library (msvcrt.lib??). This caused crashes when dgate was not launched by
   ConquestDicomServer.exe, but was running stand-alone.
   Print-statements to the console crashed the program...

   For this reason, and in order to be able to compile dgate with other
   compilers, lex.cpp had to be written.

   Lex.hpp has been left 'as is'

  The code is used in:
  - amap.cpp: read acrnema.map
  - parse.cpp: read dicom.sql

  The class TK is just a dummy.
*/

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "dgate.hpp"
#include "lex.hpp"

#ifndef UNUSED_ARGUMENT
#define UNUSED_ARGUMENT(x) (void)x
#endif

TK::TK()
#ifdef __GNUC__ //Faster with member initialization.
:Value(0),
Str(NULL)
#endif
{
}

TK::TK (const	int i)
#ifdef __GNUC__ //Faster with member initialization.
:Value(0),
Str(NULL)
#endif
{
	UNUSED_ARGUMENT(i);
}

TK::TK (const	int i, int j, char* s)
#ifdef __GNUC__ //Faster with member initialization.
:Value(0),
Str(NULL)
#endif
{
	UNUSED_ARGUMENT(i);
	UNUSED_ARGUMENT(j);
	UNUSED_ARGUMENT(s);
}

TK::~TK ()
{
}


/*---------------------------------------------------------*/

typedef struct TOKEN_LIST
{
  int		val;
  char*		token;
  TOKEN_LIST*	pNext;
} TOKEN_LIST;


Lex::Lex()
#ifdef __GNUC__ //Faster with member initialization.
:fp(NULL),
Tree(NULL),
Tk()
{
#else
{
  fp = NULL;		/* Not used */
  Tree = NULL;		/* Mis-used as a pointer to the root of TOKEN_LIST */
#endif
  Tk.Str = NULL;	/* Mis-used as a ponter to the current TOKEN_LIST element */
}

Lex::~Lex()
{
  TOKEN_LIST*	pCur;
  TOKEN_LIST*	pNext;

  pCur = (TOKEN_LIST*)Tree;
  while (pCur)
  { if (pCur->token)
      free(pCur->token);
    pNext = pCur->pNext;
    free(pCur);
    pCur = pNext;
  }
}


BOOL Lex::Start (char	*filename, TOKENSTRUCT	*TKS)
{
  FILE*		f;
  int i, j,  iFileLength;
  unsigned int ui;
  char*		pBuf;
  int		iNbTokens;
  TOKENSTRUCT*	pToken;
  char**	ppPos;
  char*		pCurPos;
  int		iFirst, iFirstToken;
  TOKEN_LIST*	pCur = NULL;
//  TOKEN_LIST*	pPrev = NULL;		/* LJ */
  char		cSave = 0;
  char		bWasWhiteSpace;
  const char*	pTmp;

  /* Sensible parameters? */
  if ((!filename) || (!TKS))
    return FALSE;

  /* How many tokens are to be identified in the file? */
  pToken = TKS;
  iNbTokens = 0;
  while (TRUE)
  { if (pToken->val != 0)
      if ((pToken->token == NULL) || (pToken->token[0] == 0))
	return FALSE;	/* A search-string must be supplied */
      else
	iNbTokens++;
    else
      if (!iNbTokens)
	return FALSE;	/*  At least one token must be supplied */
      else
	break;
    pToken++;
  }

  /* Read the file in memory */
  f = fopen(filename, "rb");
  if (!f)
    return FALSE;

  fseek(f, 0, SEEK_END);
  iFileLength = ftell(f);
  fseek(f, 0, SEEK_SET);

  pBuf = (char*)malloc(iFileLength + 1);
  if (!pBuf)
  { fclose(f);
    return FALSE;
  }

  if (fread(pBuf, 1, iFileLength, f) != (unsigned int)iFileLength)
  { fclose(f);
    free(pBuf);
    return FALSE;
  }
  fclose(f);
  pBuf[iFileLength] = 0;

  /* Search all tokens */
  ppPos = (char**)malloc(iNbTokens * sizeof(char*));
  if (!ppPos)
  { fclose(f);
    free(pBuf);
    return FALSE;
  }
  pToken = TKS;
  for (i=0; i<iNbTokens; i++)
  { ppPos[i] = strstr(pBuf, (pToken + i)->token);
    while (ppPos[i])
    { /* Is this token a substring of another one? */
      for (j=0; j<iNbTokens; j++)
      { if (j == i)
	  continue;
	pTmp = strstr((pToken + j)->token, (pToken + i)->token);
	if (pTmp)
	  /* Yep, current token is a substring of another one...
	     Does the larger token also match here? If so,
	     continue searching.
	  */
	  if (strncmp(ppPos[i] - (pTmp - (pToken + j)->token),
	    (pToken + j)->token, strlen((pToken + j)->token)) == 0)
	  { ppPos[i] = strstr(ppPos[i] + 1, (pToken + i)->token);
	    break;
	  }
      }
      if (j == iNbTokens)
	break;
    }
    
  }

  /* What is the first found token? */
  iFirst = iFileLength + 1;
  iFirstToken = -1;
  for (i=0; i<iNbTokens; i++)
    if (ppPos[i] && (ppPos[i] - pBuf < iFirst))
    { iFirst = ppPos[i] - pBuf;
      iFirstToken = i;
    }

  pCurPos = pBuf;
  while (TRUE)
  {
    /* A word may not start with white-space */
    while ((*pCurPos == '\r') || (*pCurPos == '\n') ||
	   (*pCurPos == '\t') || (*pCurPos == ' '))
      pCurPos++;
    if (*pCurPos == 0)
      break;	/* Finished */

    /* Add a new entry */
    if (!pCur)
    { Tree = malloc(sizeof(TOKEN_LIST));
      pCur = (TOKEN_LIST*)Tree;
    }
    else
    { pCur->pNext = (TOKEN_LIST*)malloc(sizeof(TOKEN_LIST));
      pCur = pCur->pNext;
    }

    if (pCurPos < pBuf + iFirst)
    { /* A USER entry */
      pCur->val = TOKEN_USER;
      /* Entry is terminated at the start of the next TOKEN... */
      if (iFirstToken != -1)
      { cSave = pBuf[iFirst];
        pBuf[iFirst] = 0;
      }
      /* ... or at the first occurance of white-space */
      bWasWhiteSpace = 0;
      for (ui=0; ui<strlen(pCurPos); ui++)
        if ((pCurPos[ui] == '\r') || (pCurPos[ui] == '\n') ||
	    (pCurPos[ui] == '\t') || (pCurPos[ui] == ' '))
	{ pCurPos[ui] = 0;
	  bWasWhiteSpace = 1;
	  break;
	}
      pCur->token = strdup(pCurPos);
      if (bWasWhiteSpace)
	pCurPos[ui] = ' ';
      pCurPos += strlen(pCur->token);
      if (iFirstToken != -1)
        pBuf[iFirst] = cSave;
    }
    else
    { /* A TOKEN entry */
      pCur->val = (pToken + iFirstToken)->val;
      pCur->token = NULL;
      pCurPos += strlen((pToken + iFirstToken)->token);

      /* Update the next occurance of this token */
      ppPos[iFirstToken] = strstr(pCurPos, (pToken + iFirstToken)->token);
      while (ppPos[iFirstToken])
      { /* Is this token a substring of another one? */
	for (j=0; j<iNbTokens; j++)
	{ if (j == iFirstToken)
	    continue;
	  pTmp = strstr((pToken + j)->token, (pToken + iFirstToken)->token);
	  if (pTmp)
	    /* Yep, current token is a substring of another one...
	       Does the larger token also match here? If so,
	       continue searching.
	    */
	    if (strncmp(ppPos[iFirstToken] - (pTmp - (pToken + j)->token),
	      (pToken + j)->token, strlen((pToken + j)->token)) == 0)
	    { ppPos[iFirstToken] = strstr(ppPos[iFirstToken] + 1, (pToken + iFirstToken)->token);
	      break;
	    }
        }
	if (j == iNbTokens)
	  break;
      }

      /* Update the first token */
      iFirst = iFileLength + 1;
      iFirstToken = -1;
      for (i=0; i<iNbTokens; i++)
	if (ppPos[i] && (ppPos[i] - pBuf < iFirst))
	{ iFirst = ppPos[i] - pBuf;
	  iFirstToken = i;
	}
    }
    pCur->pNext = NULL;
  }

  /* Finally add an END token */
  if (!pCur)
  { Tree = malloc(sizeof(TOKEN_LIST));
    pCur = (TOKEN_LIST*)Tree;
  }
  else
  { pCur->pNext = (TOKEN_LIST*)malloc(sizeof(TOKEN_LIST));
    pCur = pCur->pNext;
  }
  pCur->val = TOKEN_END;
  pCur->token = NULL;
  pCur->pNext = NULL;


  /* Cleanup */
  free(ppPos);
  free(pBuf);

  return TRUE;
}

BOOL Lex::Stop ()
{
  return TRUE;
}

int Lex::Peek (TK* pTK)
{
  TOKEN_LIST*	pCur;

  pCur = (TOKEN_LIST*)Tk.Str;
  if (!pCur)
  { /* No entries in token-list */
    pTK->Value = TOKEN_END;
    pTK->Str = NULL;
  }
  else
  { pTK->Value = pCur->val;
    pTK->Str = pCur->token;
  }
  return pTK->Value;
}

int Lex::Get (TK* pTK)
{
  TOKEN_LIST*	pCur;

  pCur = (TOKEN_LIST*)Tk.Str;
  if (!pCur)
    /* The first time */
    pCur = (TOKEN_LIST*)Tree;
  else
    /* Get next token, or stay at EOF */
    if (pCur->pNext)
      pCur = pCur->pNext;
  Tk.Str = (char*)pCur;

  if (!pCur)
  { /* No entries in token-list */
    pTK->Value = TOKEN_END;
    pTK->Str = NULL;
  }
  else
  { pTK->Value = pCur->val;
    pTK->Str = pCur->token;
  }

  return pTK->Value;
}

int Lex::GetString (TK* pTK, char c)
{
	UNUSED_ARGUMENT(pTK);
	UNUSED_ARGUMENT(c);
  /* Not implemented */
	return 0;
}

