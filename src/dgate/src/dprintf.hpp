#ifndef	_DPRINTF_H_
#	define	_DPRINTF_H_

/*
20000211	ljz	Removed MajorEvent and MinorEvent instances of 'Debug'
			Added UserLog and TroubleLog
			Added bAddTimeStamps member
20011110        mvh     Replaced MessagePipeName by FilePipeName
                        Blocked out critical section
20091231	bcb 	Changed char* to const char* for gcc4.2 warnings
20100111	mvh	Merged
20100619	bcb	Fix gcc4.0 warnings and prevented coping of clases with pointers.
20100717	mvh	Merged
20181219	mvh	Added OnTCP
*/

class	Debug
{
private:
	int			BDebug;
	FILE*			Debugfp;
//	CRITICAL_SECTION	CriticalFile;
	int			CloseOnOff;
	int			UseMessagePipe;
	char			FilePipeName[256];
	int			UseUDPTCP;
	char			bAddTimeStamps;
public:
	Socket			DebugSocket;	
public:
	Debug();
	~Debug();
	BOOL	DescribeSource;
	void	AddTimeStamps(char YesOrNo) {bAddTimeStamps = YesOrNo;};
	void	On(FILE	*);
	void	On();
	void	On(char	*filename);
	void	OnMsgPipe(char	*MsgPipeName);
	void	OnMsgPipe(char	*MsgPipeName, BOOL);
	void	OnUDP(char *Host, const char *IP);
	void	OnTCP(char *Host, const char *IP);
	void	Off();
	int	printf(const char *, ...);
#ifdef __GNUC__
private:// This will prevent it from being copied (it has pointers)
	Debug(const Debug&);
	Debug & operator = (const Debug &D);
#endif
};

extern	Debug	SystemDebug;
extern	Debug	OperatorConsole;
extern	Debug	AccessUpdate;
//extern	Debug	MajorEvent;
//extern	Debug	MinorEvent;
extern	Debug	UserLog;
extern	Debug	TroubleLog;

#endif
