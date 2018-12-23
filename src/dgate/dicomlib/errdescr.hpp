#ifndef _ERRDESCR_H_
#define _ERRDESCR_H_

#include "dicom.hpp"

// class to encapsulate description of an error condtion
// used in a temporary fashion since it DOES NOT copy
// the character strings, dicom object, etc. when created or set

class ErrorDescription
{
	public:

	// type of error

	typedef enum
		{
		NoError,
		InvalidArgument,
		PDUReadError,
		PDUWriteError,
		UnexpectedDIMSENResponse,
		DIMSENWriteError,
		DIMSENReadError,
		DIMSENWarningStatus,
		DIMSENErrorStatus,
		UnexpectedDIMSENStatus

		} ErrorType;

	// failed operation

	typedef enum
		{
		NoOperation,
		UnknownOperation,
		PDUReadOperation,
		PDUWriteOperation,
		NActionRQRead,
		NActionRQWrite,
		NActionRSPRead,
		NActionRSPWrite,
		NCreateRQRead,
		NCreateRQWrite,
		NCreateRSPRead,
		NCreateRSPWrite,
		NDeleteRQRead,
		NDeleteRQWrite,
		NDeleteRSPRead,
		NDeleteRSPWrite,
		NEventReportRQWrite,
		NEventReportRQRead,
		NEventReportRSPRead,
		NEventReportRSPWrite,
		NGetRQRead,
		NGetRQWrite,
		NGetRSPRead,
		NGetRSPWrite,
		NSetRQRead,
		NSetRQWrite,
		NSetRSPRead,
		NSetRSPWrite

		} Operation;

	// ctor

	ErrorDescription(
		const char*	theFile = "",
		int	theLine = 0,
		const char* theClass = "",
		const char* theMethod = "",
		const char *theMessage = "",
		ErrorType theError = NoError,
		Operation theOperation = UnknownOperation,
		UINT16 theStatus = 0,
		const DICOMCommandObject *theResponseCommand = NULL,
		const DICOMCommandObject *theResponseDataset = NULL) :
		itsFile(theFile ? theFile : ""),
		itsLine(theLine),
		itsClass(theClass ? theClass : ""),
		itsMethod(theMethod ? theMethod : ""),
		itsMessage(theMessage ? theMessage : ""),
		itsError(theError),
		itsOperation(theOperation),
		itsDICOMStatus(theStatus),
		itsResponseCommand(theResponseCommand),
		itsResponseDataset(theResponseDataset)
		{
		}

	// copy ctor

	ErrorDescription(const ErrorDescription& theDescription)
		{
			InternalAssign(theDescription);
		}

	// assignment operator

	ErrorDescription&
	operator= (const ErrorDescription& theDescription)
		{
			InternalAssign(theDescription);
			return *this;
		}

	// dtor

	~ErrorDescription()
		{
		}
		
	// Get/Set name of class encountering error

	const char* GetClass() const
		{
		return itsClass;
		}
	void SetClass(const char *theClass)
		{
			itsClass = theClass ? theClass : "";
		}

	// Get/Set name of method encountering error

	const char* GetMethod() const
		{
		return itsMethod;
		}
	void SetMethod (const char *theMethod)
		{
			itsMethod = theMethod ? theMethod : "";
		}

	// Get/Set message string

	const char* GetMessage() const
		{
		return itsMessage;
		}
	void SetMessage (const char *theMessage)
		{
			itsMessage = theMessage ? theMessage : "";
		}

	// Get/Set File / Line
	const	char	*	GetFile() const
		{
		return itsFile;
		}
	
	void	SetFile (const char *theFile)
		{
			itsFile = theFile ? theFile : "";
		}
	int		GetLine () const
		{
		return itsLine;
		}
	void	SetLine (const	int	theLine) 
		{
		itsLine = theLine;
		}
		
	// Get/Set type of error encountered

	ErrorType GetErrorType() const
		{
		return itsError;
		}
	void SetErrorType (ErrorType theError)
		{
			itsError = theError;
		}

	// Get/Set type of operation encountering error

	Operation GetOperation() const
		{
		return itsOperation;
		}
	void SetOperation (Operation theOperation)
		{
			itsOperation = theOperation;
		}

	// Get/Set status returned in a DICOM response (if applicable)

	UINT16 GetDICOMStatus() const
		{
		return itsDICOMStatus;
		}
	void SetDICOMStatus (UINT16 theDICOMStatus)
		{
			itsDICOMStatus = theDICOMStatus;
		}

	// Get/Set DICOMCommandObject containing list of attributes returned in
	// command response dataset

	const DICOMObject* GetResponseCommand() const
		{
		return itsResponseCommand;
		}
	const DICOMObject* GetResponseDataset() const
		{
		return itsResponseDataset;
		}
	void SetResponseCommand (const DICOMObject* theResponseCommand)
		{
			itsResponseCommand = theResponseCommand;
		}
	void SetResponseDataset (const DICOMObject* theResponseDataset)
		{
			itsResponseDataset = theResponseDataset;
		}

	protected:

	// internal implementation of copy ctor/assignment operator

	void InternalAssign(const ErrorDescription& theDescription)
		{
		itsFile = theDescription.itsFile;
		itsLine = theDescription.itsLine;
		itsClass = theDescription.itsClass;
		itsMethod = theDescription.itsMethod;
		itsMessage = theDescription.itsMessage;
		itsOperation = theDescription.itsOperation;
		itsError = theDescription.itsError;
		itsDICOMStatus = theDescription.itsDICOMStatus;
		itsResponseCommand = theDescription.itsResponseCommand;
		itsResponseDataset = theDescription.itsResponseDataset;
		}

		// its data
		const char*	itsFile;
		int	itsLine;
		const char* itsClass;		// name of class reporting error
		const char* itsMethod;		// name of method reporting error
		const char* itsMessage;		// additional message if any
		Operation itsOperation;
		ErrorType itsError;
		UINT16 itsDICOMStatus;		// status returned in a response
		const DICOMObject* itsResponseCommand; // pointer to response command
											   // object or NULL if none
		const DICOMObject* itsResponseDataset; // pointer to response dataset
											   // or NULL if none
};
#endif // !defined(_ERRDESCR_H_)
