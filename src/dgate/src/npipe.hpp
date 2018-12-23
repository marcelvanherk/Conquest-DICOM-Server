// npipe.hpp -- define UNIX implementation of a minimal
// windows NT named pipe service
// 20070308	bcb	Added changes for BSD and DARWIN
// 20100619	bcb	Added no-copy to the NamedPipe class.
// 20100717	mvh	Merged

#ifndef NPIPE_HPP
#define NPIPE_HPP
#include <sys/types.h>
#include "dicom.hpp"

class NamedPipe
{
	public:
		NamedPipe(char *theName);
		~NamedPipe();
		BOOL Create();
		void Destroy();
		size_t Write(unsigned char *theBuffer, size_t theLength);
		size_t Read(unsigned char *theBuffer, size_t theLength);
		BOOL   Wait(size_t theWaitTime);
		BOOL Connect();			// establish connection
		void Disconnect();		// terminate connection

	private:

		char *itsName;
		BOOL itsCreatedHere;
		int itsControlReadFd;	// connections received here
		int itsControlWriteFd;	// this FD bound to name in filesystem
#ifndef BSD
		int itsConnectionFd;	// this is the FD used for a connection
								// which has been established
#else	//BSD
		FILE *itsConnectionFd;
#endif
#ifdef __GNUC__
private:// This will prevent it from being copied (it has a pointer)
	NamedPipe(const NamedPipe&);
	const	NamedPipe & operator = (const NamedPipe&);
#endif
};

#endif
