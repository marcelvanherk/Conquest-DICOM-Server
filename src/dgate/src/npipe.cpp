// Named Pipe facility somewhat similar to local named pipes
// available under Win32
// This is implemented for Unix SysV where named pipes are available
// the package could be implemented using BSD UNIX Domain sockets
// if SysV named pipes were not available

// 20050111	mvh	Blocked out sys/conf.h; compiles with linux but gives 
//			linker warning: fattach not implemented and will always fail
// 20051217	mvh	Return (unsigned) 0xFFFFFFFF instead of signed -1
// 20070308	bcb	Change things for BSD and DARWIN.
// 20070316	bcb	Some more fixes
// 20091229	mvh	Completely disabled call to fattach, ioctl, and isastream as per 
//              	http://www.image-systems.biz/forum/viewtopic.php?f=33&t=1666&start=0#p6879
// 20100111	mvh	Some layout
// 20100619	bcb	Fix gcc4 warnings.
// 20100717	mvh	Merged

#include "npipe.hpp"
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <stdlib.h>
#include <signal.h>
#ifndef BSD
//#include <stropts.h>
#endif //BSD
//#include <sys/conf.h>
#include <string.h>
#include <sys/ioctl.h>

NamedPipe::NamedPipe(char *theName) :
	itsName(NULL),
	itsCreatedHere(FALSE),
	itsControlReadFd(-1),
	itsControlWriteFd(-1),
#ifndef BSD
	itsConnectionFd(-1)
#else //BSD
	itsConnectionFd(NULL)
#endif //BSD
{
	int aLength = theName ? strlen(theName) : 0;
	itsName = theName ? new char[aLength + 1] : NULL;
	if(theName)
	{
		strncpy(itsName, theName, aLength + 1);
	}
}

NamedPipe::~NamedPipe()
{
	Destroy();
}

void
NamedPipe::Destroy()
{
	if(itsName)
	{
		if(itsCreatedHere)
		{
			(void)unlink(itsName);
		}
		delete [] itsName;
		itsName = NULL;
		if(itsControlReadFd >= 0)
		{
#ifndef BSD //Doesn't have a read field
			close(itsControlReadFd);
#endif //BSD
			itsControlReadFd = 0;
		}
		if(itsControlWriteFd >= 0)
		{
#ifndef BSD //Doesn't have a write field
			close(itsControlWriteFd);
#endif //BSD
			close(itsControlWriteFd);
			itsControlWriteFd = 0;
		}
	}
#ifndef BSD
	if(itsConnectionFd >= 0)
	{
		close(itsConnectionFd);
		itsConnectionFd = 0;
	}
#else //BSD uses fclose for a stream
	if(itsConnectionFd != NULL)
	{
		fclose(itsConnectionFd);
		itsConnectionFd = NULL;
	}
#endif //BSD
	itsCreatedHere = FALSE;
}

BOOL
NamedPipe::Create()
{
#ifndef BSD //Not used by dgate, no need to port
	if(itsCreatedHere)
	{
		// already created

		return TRUE;
	}

	if(!itsName)
	{
		// call it invalid argument...
		errno = EINVAL;
		return FALSE;
	}

	// see if the name exists in the file system
	// if not, create it error checking done via fattach() below...

	int aNamedFd = creat(itsName, 0770);

	if(aNamedFd >= 0)
	{
		// just needed the name in the file system...

		close(aNamedFd);
	}

	int anFdList[2];

	if(pipe(anFdList))
	{
		// can't create a pipe

		return FALSE;
	}

	itsControlReadFd = anFdList[0];
	itsControlWriteFd = anFdList[1];
	itsConnectionFd = -1;

	// bind the desired name to the end of pipe used by writer
	// fails if name doesn't exist or is already bound to a stream

	//if(fattach(itsControlWriteFd, itsName))
	if (0)
	{
		// error binding to desired name -- name probably in use already
		// as a named pipe

		int anError = errno;
		close(itsControlWriteFd);
		itsControlWriteFd = -1;
		close(itsControlReadFd);
		itsControlReadFd = -1;
		errno = anError;
		return FALSE;
	}

	// push the connection-oriented line discipline module onto the
	// writer's end of the stream -- this will have the effect of
	// creating a new stream each time an open is done on the named
	// object, and sending a file descriptor to the new stream to us
	// via the original stream we created -- thus yielding a separate
	// stream for each connection established (like mulitple instances
	// of an NT named pipe)

	//if(ioctl(itsControlWriteFd, I_PUSH, "connld"))
	if (0)
	{
		int anError = errno;
		close(itsControlWriteFd);
		itsControlWriteFd = -1;
		close(itsControlReadFd);
		itsControlReadFd = -1;
		errno = anError;
		return FALSE;
	}

	itsCreatedHere = TRUE;
#endif //BSD

	return TRUE;
}

size_t
NamedPipe::Read(unsigned char *theBuffer, size_t theLength)
{
	if(!itsName || !Connect())
	{
		errno = EBADF;
		return 0xFFFFFFFF;
	}

	int aReadCount;

#ifndef BSD
	aReadCount = read(itsConnectionFd, theBuffer, theLength);
#else //BSD uses fread to read streams
	rewind(itsConnectionFd);
	aReadCount = fread(theBuffer,1, theLength, itsConnectionFd);	
#endif //BSD
	return aReadCount;
}

size_t
NamedPipe::Write(unsigned char *theBuffer, size_t theLength)
{
	if(!itsName || !Connect())
	{
		errno = EBADF;
		return 0xFFFFFFFF;
	}

	int aWriteCount;

	void (*aPreviousDisposal)(int) = signal(SIGPIPE, SIG_IGN);
#ifndef BSD
	aWriteCount = write(itsConnectionFd, theBuffer, theLength);
#else //BSD uses fwrite for streams.
	fseek(itsConnectionFd, 0, SEEK_END);
	aWriteCount = fwrite(theBuffer,1, theLength, itsConnectionFd);
#endif //BSD
	signal(SIGPIPE, aPreviousDisposal);

	return aWriteCount;
}

BOOL
NamedPipe::Wait(size_t theWaitSeconds)
{
	BOOL aWaitForeverFlag = (theWaitSeconds == 0 ? TRUE : FALSE);
	unsigned char aBuffer;

	do
	{
		// remain in loop when waiting until it happens...

		if(aWaitForeverFlag)
		{
			++theWaitSeconds;
		}

		if(Connect() && Write(&aBuffer, 0) == 0)
		{
			// connected and succeeded in writing 0 bytes
			// which actually do get transmitted as a 0 byte message

			Disconnect();
			return TRUE;
		}
			
		// sleep 1 second then try again...
		// unless time is up

		if(theWaitSeconds > 0)
		{
			sleep(1);
		}
	}
	while(theWaitSeconds-- > 0);

	// times up...

	return FALSE;
}

BOOL
NamedPipe::Connect()
{
#ifndef BSD //Darwin uses fopen to open a stream.
	if(itsConnectionFd >= 0)
	{
		// already connected...

		return TRUE;
	}

	// client or server depending upon whether or not we created it

	if(itsCreatedHere)
	{
		// server style -- wait to receive a connection via our
		// control read fd (this ioctl returns a new fd for a
		// connection when one occurs)

		//if(ioctl(itsControlReadFd, I_RECVFD, &itsConnectionFd))
		if (0)
		{
			return FALSE;
		}
	}
	else
	{
		// client style

		itsConnectionFd = open(itsName, O_RDWR | O_NONBLOCK);

		if(itsConnectionFd == -1)
		{
			return FALSE;
		}

		// ensure that its a stream...

		//if(!isastream(itsConnectionFd))
		if (1)
		{
			close(itsConnectionFd);
			itsConnectionFd = -1;
			return FALSE;
		}
	}
#else //Darwin uses fopen to open a stream.

	if(itsConnectionFd != NULL)
	{
		// already connected...

		return TRUE;
	}

	itsConnectionFd = fopen(itsName, "w+");

	if(itsConnectionFd == NULL)
	{
		return FALSE;
	}

#endif //BSD
	return TRUE;
}

void
NamedPipe::Disconnect()
{
#ifndef BSD
	if(itsConnectionFd >= 0)
	{
		close(itsConnectionFd);	
		itsConnectionFd = -1;
	}
#else //BSD uses fclose.
	if(itsConnectionFd != NULL)
	{
		fclose(itsConnectionFd);	
		itsConnectionFd = NULL;
	}
#endif //BSD
}
