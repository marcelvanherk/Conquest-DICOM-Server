// 20091231	bcb	Changed char* to const char* for gcc4.2 warnings
// 20100111	mvh	Merged
// 20100619	bcb	Added no-copy to the classes.
// 20100717	mvh	Merged

#ifndef	_LEX_H
#	define	_LEX_H

typedef	struct	_TOKENSTRUCT
{
	int	val;
	const char	*token;
}	TOKENSTRUCT;

#define	TOKEN_END	-2
#define	TOKEN_USER	0
#define	NEW_LINE	-3

class	TK
	{
	public:
		int		Value;
		char	*Str;
		TK ();
		TK (const	int);
		TK (const	int, int, char *);

		~TK ();
#ifdef __GNUC__
	private:// This will prevent it from being copied (it has a pointer)
		TK(const TK&);
		const	TK & operator = (const TK&);
#endif
	};

class	Lex
	{
		FILE	*fp;
		void	*Tree;
		TK		Tk;
	public:
		Lex();
		~Lex();
		BOOL	Start (char	*filename, TOKENSTRUCT	*TKS);
		BOOL	Stop ();
		int		Peek (TK	*);
		int		Get (TK	*);
		int		GetString ( TK *, char );
#ifdef __GNUC__
	private:// This will prevent it from being copied (it has pointers)
		Lex(const Lex&);
		const	Lex & operator = (const Lex&);
#endif
	};	

#define	TOKEN_END	-2
#define	TOKEN_USER	0
#define	kNEW_LINE	-3

#endif
