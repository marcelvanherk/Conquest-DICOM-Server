/* Lua language header file, designed for dynamic DLL loading. 
   This file is not part of the standard distribution, but was
   generated from original files lua.h, lauxlib.h, lualib.h */

#ifndef LUA_DYNAMIC_H
#define LUA_DYNAMIC_H

#ifdef __cplusplus
extern "C" {
#endif
#ifndef _WIN32
#define __cdecl
#define GetProcAddress dlsym
#endif

#define LUA_NUMBER double
#define LUA_IDSIZE 60
#define LUAL_BUFFERSIZE BUFSIZ
#define LUA_COMPAT_OPENLIB 
#define LUA_INTEGER ptrdiff_t

#define LUA_PREFIX LuaFunctions.

/******************************************************************************
* Copyright (C) 1994-2007 Lua.org, PUC-Rio.  All rights reserved.
*
* Permission is hereby granted, free of charge, to any person obtaining
* a copy of this software and associated documentation files (the
* "Software"), to deal in the Software without restriction, including
* without limitation the rights to use, copy, modify, merge, publish,
* distribute, sublicense, and/or sell copies of the Software, and to
* permit persons to whom the Software is furnished to do so, subject to
* the following conditions:
*
* The above copyright notice and this permission notice shall be
* included in all copies or substantial portions of the Software.
*
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
* EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
* MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
* IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
* CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
* TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
* SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
******************************************************************************/

#include <stdarg.h>
#include <stddef.h>

#define LUA_VERSION	"Lua 5.1"
#define LUA_RELEASE	"Lua 5.1.5"
#define LUA_VERSION_NUM	501
#define LUA_COPYRIGHT	"Copyright (C) 1994-2007 Lua.org, PUC-Rio"
#define LUA_AUTHORS 	"R. Ierusalimschy, L. H. de Figueiredo & W. Celes"

#define	LUA_SIGNATURE	"\033Lua"

#define LUA_MULTRET	(-1)

#define LUA_REGISTRYINDEX	(-10000)
#define LUA_ENVIRONINDEX	(-10001)
#define LUA_GLOBALSINDEX	(-10002)
#define lua_upvalueindex(i)	(LUA_GLOBALSINDEX-(i))

#define LUA_YIELD	1
#define LUA_ERRRUN	2
#define LUA_ERRSYNTAX	3
#define LUA_ERRMEM	4
#define LUA_ERRERR	5

typedef struct lua_State lua_State;

typedef int (*lua_CFunction) (lua_State *L);

typedef const char * (*lua_Reader) (lua_State *L, void *ud, size_t *sz);

typedef int (*lua_Writer) (lua_State *L, const void* p, size_t sz, void* ud);

typedef void * (*lua_Alloc) (void *ud, void *ptr, size_t osize, size_t nsize);

#define LUA_TNONE		(-1)

#define LUA_TNIL		0
#define LUA_TBOOLEAN		1
#define LUA_TLIGHTUSERDATA	2
#define LUA_TNUMBER		3
#define LUA_TSTRING		4
#define LUA_TTABLE		5
#define LUA_TFUNCTION		6
#define LUA_TUSERDATA		7
#define LUA_TTHREAD		8

#define LUA_MINSTACK	20

typedef LUA_NUMBER lua_Number;

typedef LUA_INTEGER lua_Integer;

typedef lua_State * (__cdecl *lua_newstate_t) (lua_Alloc f, void *ud);
typedef void (__cdecl *lua_close_t) (lua_State *L);
typedef lua_State * (__cdecl *lua_newthread_t) (lua_State *L);

typedef lua_CFunction (__cdecl *lua_atpanic_t) (lua_State *L, lua_CFunction panicf);

typedef int (__cdecl *lua_gettop_t) (lua_State *L);
typedef void (__cdecl *lua_settop_t) (lua_State *L, int idx);
typedef void (__cdecl *lua_pushvalue_t) (lua_State *L, int idx);
typedef void (__cdecl *lua_remove_t) (lua_State *L, int idx);
typedef void (__cdecl *lua_insert_t) (lua_State *L, int idx);
typedef void (__cdecl *lua_replace_t) (lua_State *L, int idx);
typedef int (__cdecl *lua_checkstack_t) (lua_State *L, int sz);

typedef void (__cdecl *lua_xmove_t) (lua_State *from, lua_State *to, int n);

typedef int (__cdecl *lua_isnumber_t) (lua_State *L, int idx);
typedef int (__cdecl *lua_isstring_t) (lua_State *L, int idx);
typedef int (__cdecl *lua_iscfunction_t) (lua_State *L, int idx);
typedef int (__cdecl *lua_isuserdata_t) (lua_State *L, int idx);
typedef int (__cdecl *lua_type_t) (lua_State *L, int idx);
typedef const char     * (__cdecl *lua_typename_t) (lua_State *L, int tp);

typedef int (__cdecl *lua_equal_t) (lua_State *L, int idx1, int idx2);
typedef int (__cdecl *lua_rawequal_t) (lua_State *L, int idx1, int idx2);
typedef int (__cdecl *lua_lessthan_t) (lua_State *L, int idx1, int idx2);

typedef lua_Number (__cdecl *lua_tonumber_t) (lua_State *L, int idx);
typedef lua_Integer (__cdecl *lua_tointeger_t) (lua_State *L, int idx);
typedef int (__cdecl *lua_toboolean_t) (lua_State *L, int idx);
typedef const char     * (__cdecl *lua_tolstring_t) (lua_State *L, int idx, size_t *len);
typedef size_t (__cdecl *lua_objlen_t) (lua_State *L, int idx);
typedef lua_CFunction (__cdecl *lua_tocfunction_t) (lua_State *L, int idx);
typedef void	       * (__cdecl *lua_touserdata_t) (lua_State *L, int idx);
typedef lua_State      * (__cdecl *lua_tothread_t) (lua_State *L, int idx);
typedef const void     * (__cdecl *lua_topointer_t) (lua_State *L, int idx);

typedef void (__cdecl *lua_pushnil_t) (lua_State *L);
typedef void (__cdecl *lua_pushnumber_t) (lua_State *L, lua_Number n);
typedef void (__cdecl *lua_pushinteger_t) (lua_State *L, lua_Integer n);
typedef void (__cdecl *lua_pushlstring_t) (lua_State *L, const char *s, size_t l);
typedef void (__cdecl *lua_pushstring_t) (lua_State *L, const char *s);
typedef const char * (__cdecl *lua_pushvfstring_t) (lua_State *L, const char *fmt,
                                                      va_list argp);
typedef const char * (__cdecl *lua_pushfstring_t) (lua_State *L, const char *fmt, ...);
typedef void (__cdecl *lua_pushcclosure_t) (lua_State *L, lua_CFunction fn, int n);
typedef void (__cdecl *lua_pushboolean_t) (lua_State *L, int b);
typedef void (__cdecl *lua_pushlightuserdata_t) (lua_State *L, void *p);
typedef int (__cdecl *lua_pushthread_t) (lua_State *L);

typedef void (__cdecl *lua_gettable_t) (lua_State *L, int idx);
typedef void (__cdecl *lua_getfield_t) (lua_State *L, int idx, const char *k);
typedef void (__cdecl *lua_rawget_t) (lua_State *L, int idx);
typedef void (__cdecl *lua_rawgeti_t) (lua_State *L, int idx, int n);
typedef void (__cdecl *lua_createtable_t) (lua_State *L, int narr, int nrec);
typedef void * (__cdecl *lua_newuserdata_t) (lua_State *L, size_t sz);
typedef int (__cdecl *lua_getmetatable_t) (lua_State *L, int objindex);
typedef void (__cdecl *lua_getfenv_t) (lua_State *L, int idx);

typedef void (__cdecl *lua_settable_t) (lua_State *L, int idx);
typedef void (__cdecl *lua_setfield_t) (lua_State *L, int idx, const char *k);
typedef void (__cdecl *lua_rawset_t) (lua_State *L, int idx);
typedef void (__cdecl *lua_rawseti_t) (lua_State *L, int idx, int n);
typedef int (__cdecl *lua_setmetatable_t) (lua_State *L, int objindex);
typedef int (__cdecl *lua_setfenv_t) (lua_State *L, int idx);

typedef void (__cdecl *lua_call_t) (lua_State *L, int nargs, int nresults);
typedef int (__cdecl *lua_pcall_t) (lua_State *L, int nargs, int nresults, int errfunc);
typedef int (__cdecl *lua_cpcall_t) (lua_State *L, lua_CFunction func, void *ud);
typedef int (__cdecl *lua_load_t) (lua_State *L, lua_Reader reader, void *dt,
                                        const char *chunkname);

typedef int (__cdecl *lua_dump_t) (lua_State *L, lua_Writer writer, void *data);

typedef int (__cdecl *lua_yield_t) (lua_State *L, int nresults);
typedef int (__cdecl *lua_resume_t) (lua_State *L, int narg);
typedef int (__cdecl *lua_status_t) (lua_State *L);

#define LUA_GCSTOP		0
#define LUA_GCRESTART		1
#define LUA_GCCOLLECT		2
#define LUA_GCCOUNT		3
#define LUA_GCCOUNTB		4
#define LUA_GCSTEP		5
#define LUA_GCSETPAUSE		6
#define LUA_GCSETSTEPMUL	7

typedef int (__cdecl *lua_gc_t) (lua_State *L, int what, int data);

typedef int (__cdecl *lua_error_t) (lua_State *L);

typedef int (__cdecl *lua_next_t) (lua_State *L, int idx);

typedef void (__cdecl *lua_concat_t) (lua_State *L, int n);

typedef lua_Alloc (__cdecl *lua_getallocf_t) (lua_State *L, void **ud);
typedef void (__cdecl *lua_setallocf_t) (lua_State *L, lua_Alloc f, void *ud);

#define lua_pop(L,n)		lua_settop(L, -(n)-1)

#define lua_newtable(L)		lua_createtable(L, 0, 0)

#define lua_register(L,n,f) (lua_pushcfunction(L, (f)), lua_setglobal(L, (n)))

#define lua_pushcfunction(L,f)	lua_pushcclosure(L, (f), 0)

#define lua_strlen(L,i)		lua_objlen(L, (i))

#define lua_isfunction(L,n)	(lua_type(L, (n)) == LUA_TFUNCTION)
#define lua_istable(L,n)	(lua_type(L, (n)) == LUA_TTABLE)
#define lua_islightuserdata(L,n)	(lua_type(L, (n)) == LUA_TLIGHTUSERDATA)
#define lua_isnil(L,n)		(lua_type(L, (n)) == LUA_TNIL)
#define lua_isboolean(L,n)	(lua_type(L, (n)) == LUA_TBOOLEAN)
#define lua_isthread(L,n)	(lua_type(L, (n)) == LUA_TTHREAD)
#define lua_isnone(L,n)		(lua_type(L, (n)) == LUA_TNONE)
#define lua_isnoneornil(L, n)	(lua_type(L, (n)) <= 0)

#define lua_pushliteral(L, s)	\
	lua_pushlstring(L, "" s, (sizeof(s)/sizeof(char))-1)

#define lua_setglobal(L,s)	lua_setfield(L, LUA_GLOBALSINDEX, (s))
#define lua_getglobal(L,s)	lua_getfield(L, LUA_GLOBALSINDEX, (s))

#define lua_tostring(L,i)	lua_tolstring(L, (i), NULL)

#define lua_open()	luaL_newstate()

#define lua_getregistry(L)	lua_pushvalue(L, LUA_REGISTRYINDEX)

#define lua_getgccount(L)	lua_gc(L, LUA_GCCOUNT, 0)

#define lua_Chunkreader		lua_Reader
#define lua_Chunkwriter		lua_Writer

#define LUA_HOOKCALL	0
#define LUA_HOOKRET	1
#define LUA_HOOKLINE	2
#define LUA_HOOKCOUNT	3
#define LUA_HOOKTAILRET 4

#define LUA_MASKCALL	(1 << LUA_HOOKCALL)
#define LUA_MASKRET	(1 << LUA_HOOKRET)
#define LUA_MASKLINE	(1 << LUA_HOOKLINE)
#define LUA_MASKCOUNT	(1 << LUA_HOOKCOUNT)

typedef struct lua_Debug lua_Debug;  

typedef void (*lua_Hook) (lua_State *L, lua_Debug *ar);

typedef int (__cdecl *lua_getstack_t) (lua_State *L, int level, lua_Debug *ar);
typedef int (__cdecl *lua_getinfo_t) (lua_State *L, const char *what, lua_Debug *ar);
typedef const char * (__cdecl *lua_getlocal_t) (lua_State *L, const lua_Debug *ar, int n);
typedef const char * (__cdecl *lua_setlocal_t) (lua_State *L, const lua_Debug *ar, int n);
typedef const char * (__cdecl *lua_getupvalue_t) (lua_State *L, int funcindex, int n);
typedef const char * (__cdecl *lua_setupvalue_t) (lua_State *L, int funcindex, int n);

typedef int (__cdecl *lua_sethook_t) (lua_State *L, lua_Hook func, int mask, int count);
typedef lua_Hook (__cdecl *lua_gethook_t) (lua_State *L);
typedef int (__cdecl *lua_gethookmask_t) (lua_State *L);
typedef int (__cdecl *lua_gethookcount_t) (lua_State *L);

struct lua_Debug {
  int event;
  const char *name;	
  const char *namewhat;	
  const char *what;	
  const char *source;	
  int currentline;	
  int nups;		
  int linedefined;	
  int lastlinedefined;	
  char short_src[LUA_IDSIZE]; 
  
  int i_ci;  
};



#include <stddef.h>
#include <stdio.h>

#define luaL_getn(L,i)          ((int)lua_objlen(L, i))
#define luaL_setn(L,i,j)        ((void)0)  

#define LUA_ERRFILE     (LUA_ERRERR+1)

typedef struct luaL_Reg {
  const char *name;
  lua_CFunction func;
} luaL_Reg;

typedef void (__cdecl *luaL_openlib_t) (lua_State *L, const char *libname,
                                const luaL_Reg *l, int nup);
typedef void (__cdecl *luaL_register_t) (lua_State *L, const char *libname,
                                const luaL_Reg *l);
typedef int (__cdecl *luaL_getmetafield_t) (lua_State *L, int obj, const char *e);
typedef int (__cdecl *luaL_callmeta_t) (lua_State *L, int obj, const char *e);
typedef int (__cdecl *luaL_typerror_t) (lua_State *L, int narg, const char *tname);
typedef int (__cdecl *luaL_argerror_t) (lua_State *L, int numarg, const char *extramsg);
typedef const char * (__cdecl *luaL_checklstring_t) (lua_State *L, int numArg,
                                                          size_t *l);
typedef const char * (__cdecl *luaL_optlstring_t) (lua_State *L, int numArg,
                                          const char *def, size_t *l);
typedef lua_Number (__cdecl *luaL_checknumber_t) (lua_State *L, int numArg);
typedef lua_Number (__cdecl *luaL_optnumber_t) (lua_State *L, int nArg, lua_Number def);

typedef lua_Integer (__cdecl *luaL_checkinteger_t) (lua_State *L, int numArg);
typedef lua_Integer (__cdecl *luaL_optinteger_t) (lua_State *L, int nArg,
                                          lua_Integer def);

typedef void (__cdecl *luaL_checkstack_t) (lua_State *L, int sz, const char *msg);
typedef void (__cdecl *luaL_checktype_t) (lua_State *L, int narg, int t);
typedef void (__cdecl *luaL_checkany_t) (lua_State *L, int narg);

typedef int (__cdecl *luaL_newmetatable_t) (lua_State *L, const char *tname);
typedef void * (__cdecl *luaL_checkudata_t) (lua_State *L, int ud, const char *tname);

typedef void (__cdecl *luaL_where_t) (lua_State *L, int lvl);
typedef int (__cdecl *luaL_error_t) (lua_State *L, const char *fmt, ...);

typedef int (__cdecl *luaL_checkoption_t) (lua_State *L, int narg, const char *def,
                                   const char *const lst[]);

typedef int (__cdecl *luaL_ref_t) (lua_State *L, int t);
typedef void (__cdecl *luaL_unref_t) (lua_State *L, int t, int ref);

typedef int (__cdecl *luaL_loadfile_t) (lua_State *L, const char *filename);
typedef int (__cdecl *luaL_loadbuffer_t) (lua_State *L, const char *buff, size_t sz,
                                  const char *name);
typedef int (__cdecl *luaL_loadstring_t) (lua_State *L, const char *s);

typedef lua_State * (__cdecl *luaL_newstate_t) (void);

typedef const char * (__cdecl *luaL_gsub_t) (lua_State *L, const char *s, const char *p,
                                                  const char *r);

typedef const char * (__cdecl *luaL_findtable_t) (lua_State *L, int idx,
                                         const char *fname, int szhint);

#define luaL_argcheck(L, cond,numarg,extramsg)	\
		((void)((cond) || luaL_argerror(L, (numarg), (extramsg))))
#define luaL_checkstring(L,n)	(luaL_checklstring(L, (n), NULL))
#define luaL_optstring(L,n,d)	(luaL_optlstring(L, (n), (d), NULL))
#define luaL_checkint(L,n)	((int)luaL_checkinteger(L, (n)))
#define luaL_optint(L,n,d)	((int)luaL_optinteger(L, (n), (d)))
#define luaL_checklong(L,n)	((long)luaL_checkinteger(L, (n)))
#define luaL_optlong(L,n,d)	((long)luaL_optinteger(L, (n), (d)))

#define luaL_typename(L,i)	lua_typename(L, lua_type(L,(i)))

#define luaL_dofile(L, fn) \
	(luaL_loadfile(L, fn) || lua_pcall(L, 0, LUA_MULTRET, 0))

#define luaL_dostring(L, s) \
	(luaL_loadstring(L, s) || lua_pcall(L, 0, LUA_MULTRET, 0))

#define luaL_getmetatable(L,n)	(lua_getfield(L, LUA_REGISTRYINDEX, (n)))

#define luaL_opt(L,f,n,d)	(lua_isnoneornil(L,(n)) ? (d) : f(L,(n)))

typedef struct luaL_Buffer {
  char *p;			
  int lvl;  
  lua_State *L;
  char buffer[LUAL_BUFFERSIZE];
} luaL_Buffer;

#define luaL_addchar(B,c) \
  ((void)((B)->p < ((B)->buffer+LUAL_BUFFERSIZE) || luaL_prepbuffer(B)), \
   (*(B)->p++ = (char)(c)))

#define luaL_putchar(B,c)	luaL_addchar(B,c)

#define luaL_addsize(B,n)	((B)->p += (n))

typedef void (__cdecl *luaL_buffinit_t) (lua_State *L, luaL_Buffer *B);
typedef char * (__cdecl *luaL_prepbuffer_t) (luaL_Buffer *B);
typedef void (__cdecl *luaL_addlstring_t) (luaL_Buffer *B, const char *s, size_t l);
typedef void (__cdecl *luaL_addstring_t) (luaL_Buffer *B, const char *s);
typedef void (__cdecl *luaL_addvalue_t) (luaL_Buffer *B);
typedef void (__cdecl *luaL_pushresult_t) (luaL_Buffer *B);

#define LUA_NOREF       (-2)
#define LUA_REFNIL      (-1)

#define lua_ref(L,lock) ((lock) ? luaL_ref(L, LUA_REGISTRYINDEX) : \
      (lua_pushstring(L, "unlocked references are obsolete"), lua_error(L), 0))

#define lua_unref(L,ref)        luaL_unref(L, LUA_REGISTRYINDEX, (ref))

#define lua_getref(L,ref)       lua_rawgeti(L, LUA_REGISTRYINDEX, (ref))

#define luaL_reg	luaL_Reg



#define LUA_FILEHANDLE		"FILE*"

#define LUA_COLIBNAME	"coroutine"
typedef int (__cdecl *luaopen_base_t) (lua_State *L);

#define LUA_TABLIBNAME	"table"
typedef int (__cdecl *luaopen_table_t) (lua_State *L);

#define LUA_IOLIBNAME	"io"
typedef int (__cdecl *luaopen_io_t) (lua_State *L);

#define LUA_OSLIBNAME	"os"
typedef int (__cdecl *luaopen_os_t) (lua_State *L);

#define LUA_STRLIBNAME	"string"
typedef int (__cdecl *luaopen_string_t) (lua_State *L);

#define LUA_MATHLIBNAME	"math"
typedef int (__cdecl *luaopen_math_t) (lua_State *L);

#define LUA_DBLIBNAME	"debug"
typedef int (__cdecl *luaopen_debug_t) (lua_State *L);

#define LUA_LOADLIBNAME	"package"
typedef int (__cdecl *luaopen_package_t) (lua_State *L);

typedef void (__cdecl *luaL_openlibs_t) (lua_State *L); 

#define luaL_addlstring         LUA_PREFIX AddlstringL
#define luaL_addstring          LUA_PREFIX AddstringL
#define luaL_addvalue           LUA_PREFIX AddvalueL
#define luaL_argerror           LUA_PREFIX ArgerrorL
#define luaL_buffinit           LUA_PREFIX BuffinitL
#define luaL_callmeta           LUA_PREFIX CallmetaL
#define luaL_checkany           LUA_PREFIX CheckanyL
#define luaL_checkinteger       LUA_PREFIX CheckintegerL
#define luaL_checklstring       LUA_PREFIX ChecklstringL
#define luaL_checknumber        LUA_PREFIX ChecknumberL
#define luaL_checkoption        LUA_PREFIX CheckoptionL
#define luaL_checkstack         LUA_PREFIX CheckstackL
#define luaL_checktype          LUA_PREFIX ChecktypeL
#define luaL_checkudata         LUA_PREFIX CheckudataL
#define luaL_error              LUA_PREFIX ErrorL
#define luaL_findtable          LUA_PREFIX FindtableL
#define luaL_getmetafield       LUA_PREFIX GetmetafieldL
#define luaL_gsub               LUA_PREFIX GsubL
#define luaL_loadbuffer         LUA_PREFIX LoadbufferL
#define luaL_loadfile           LUA_PREFIX LoadfileL
#define luaL_loadstring         LUA_PREFIX LoadstringL
#define luaL_newmetatable       LUA_PREFIX NewmetatableL
#define luaL_newstate           LUA_PREFIX NewstateL
#define luaL_openlib            LUA_PREFIX OpenlibL
#define luaL_openlibs           LUA_PREFIX OpenlibsL
#define luaL_optinteger         LUA_PREFIX OptintegerL
#define luaL_optlstring         LUA_PREFIX OptlstringL
#define luaL_optnumber          LUA_PREFIX OptnumberL
#define luaL_prepbuffer         LUA_PREFIX PrepbufferL
#define luaL_pushresult         LUA_PREFIX PushresultL
#define luaL_ref                LUA_PREFIX RefL
#define luaL_register           LUA_PREFIX RegisterL
#define luaL_typerror           LUA_PREFIX TyperrorL
#define luaL_unref              LUA_PREFIX UnrefL
#define luaL_where              LUA_PREFIX WhereL
#define lua_atpanic             LUA_PREFIX Atpanic
#define lua_call                LUA_PREFIX Call
#define lua_checkstack          LUA_PREFIX Checkstack
#define lua_close               LUA_PREFIX Close
#define lua_concat              LUA_PREFIX Concat
#define lua_cpcall              LUA_PREFIX Cpcall
#define lua_createtable         LUA_PREFIX Createtable
#define lua_dump                LUA_PREFIX Dump
#define lua_equal               LUA_PREFIX Equal
#define lua_error               LUA_PREFIX Error
#define lua_gc                  LUA_PREFIX Gc
#define lua_getallocf           LUA_PREFIX Getallocf
#define lua_getfenv             LUA_PREFIX Getfenv
#define lua_getfield            LUA_PREFIX Getfield
#define lua_gethook             LUA_PREFIX Gethook
#define lua_gethookcount        LUA_PREFIX Gethookcount
#define lua_gethookmask         LUA_PREFIX Gethookmask
#define lua_getinfo             LUA_PREFIX Getinfo
#define lua_getlocal            LUA_PREFIX Getlocal
#define lua_getmetatable        LUA_PREFIX Getmetatable
#define lua_getstack            LUA_PREFIX Getstack
#define lua_gettable            LUA_PREFIX Gettable
#define lua_gettop              LUA_PREFIX Gettop
#define lua_getupvalue          LUA_PREFIX Getupvalue
#define lua_insert              LUA_PREFIX Insert
#define lua_iscfunction         LUA_PREFIX Iscfunction
#define lua_isnumber            LUA_PREFIX Isnumber
#define lua_isstring            LUA_PREFIX Isstring
#define lua_isuserdata          LUA_PREFIX Isuserdata
#define lua_lessthan            LUA_PREFIX Lessthan
#define lua_load                LUA_PREFIX Load
#define lua_newstate            LUA_PREFIX Newstate
#define lua_newthread           LUA_PREFIX Newthread
#define lua_newuserdata         LUA_PREFIX Newuserdata
#define lua_next                LUA_PREFIX Next
#define lua_objlen              LUA_PREFIX Objlen
#define lua_pcall               LUA_PREFIX Pcall
#define lua_pushboolean         LUA_PREFIX Pushboolean
#define lua_pushcclosure        LUA_PREFIX Pushcclosure
#define lua_pushfstring         LUA_PREFIX Pushfstring
#define lua_pushinteger         LUA_PREFIX Pushinteger
#define lua_pushlightuserdata   LUA_PREFIX Pushlightuserdata
#define lua_pushlstring         LUA_PREFIX Pushlstring
#define lua_pushnil             LUA_PREFIX Pushnil
#define lua_pushnumber          LUA_PREFIX Pushnumber
#define lua_pushstring          LUA_PREFIX Pushstring
#define lua_pushthread          LUA_PREFIX Pushthread
#define lua_pushvalue           LUA_PREFIX Pushvalue
#define lua_pushvfstring        LUA_PREFIX Pushvfstring
#define lua_rawequal            LUA_PREFIX Rawequal
#define lua_rawget              LUA_PREFIX Rawget
#define lua_rawgeti             LUA_PREFIX Rawgeti
#define lua_rawset              LUA_PREFIX Rawset
#define lua_rawseti             LUA_PREFIX Rawseti
#define lua_remove              LUA_PREFIX Remove
#define lua_replace             LUA_PREFIX Replace
#define lua_resume              LUA_PREFIX Resume
#define lua_setallocf           LUA_PREFIX Setallocf
#define lua_setfenv             LUA_PREFIX Setfenv
#define lua_setfield            LUA_PREFIX Setfield
#define lua_sethook             LUA_PREFIX Sethook
#define lua_setlocal            LUA_PREFIX Setlocal
#define lua_setmetatable        LUA_PREFIX Setmetatable
#define lua_settable            LUA_PREFIX Settable
#define lua_settop              LUA_PREFIX Settop
#define lua_setupvalue          LUA_PREFIX Setupvalue
#define lua_status              LUA_PREFIX Status
#define lua_toboolean           LUA_PREFIX Toboolean
#define lua_tocfunction         LUA_PREFIX Tocfunction
#define lua_tointeger           LUA_PREFIX Tointeger
#define lua_tolstring           LUA_PREFIX Tolstring
#define lua_tonumber            LUA_PREFIX Tonumber
#define lua_topointer           LUA_PREFIX Topointer
#define lua_tothread            LUA_PREFIX Tothread
#define lua_touserdata          LUA_PREFIX Touserdata
#define lua_type                LUA_PREFIX Type
#define lua_typename            LUA_PREFIX Typename
#define lua_xmove               LUA_PREFIX Xmove
#define lua_yield               LUA_PREFIX Yield
#define luaopen_base            LUA_PREFIX Open_base
#define luaopen_debug           LUA_PREFIX Open_debug
#define luaopen_io              LUA_PREFIX Open_io
#define luaopen_math            LUA_PREFIX Open_math
#define luaopen_os              LUA_PREFIX Open_os
#define luaopen_package         LUA_PREFIX Open_package
#define luaopen_string          LUA_PREFIX Open_string
#define luaopen_table           LUA_PREFIX Open_table

typedef struct lua_All_functions
{
  luaL_addlstring_t       AddlstringL;
  luaL_addstring_t        AddstringL;
  luaL_addvalue_t         AddvalueL;
  luaL_argerror_t         ArgerrorL;
  luaL_buffinit_t         BuffinitL;
  luaL_callmeta_t         CallmetaL;
  luaL_checkany_t         CheckanyL;
  luaL_checkinteger_t     CheckintegerL;
  luaL_checklstring_t     ChecklstringL;
  luaL_checknumber_t      ChecknumberL;
  luaL_checkoption_t      CheckoptionL;
  luaL_checkstack_t       CheckstackL;
  luaL_checktype_t        ChecktypeL;
  luaL_checkudata_t       CheckudataL;
  luaL_error_t            ErrorL;
  luaL_findtable_t        FindtableL;
  luaL_getmetafield_t     GetmetafieldL;
  luaL_gsub_t             GsubL;
  luaL_loadbuffer_t       LoadbufferL;
  luaL_loadfile_t         LoadfileL;
  luaL_loadstring_t       LoadstringL;
  luaL_newmetatable_t     NewmetatableL;
  luaL_newstate_t         NewstateL;
  luaL_openlib_t          OpenlibL;
  luaL_openlibs_t         OpenlibsL;
  luaL_optinteger_t       OptintegerL;
  luaL_optlstring_t       OptlstringL;
  luaL_optnumber_t        OptnumberL;
  luaL_prepbuffer_t       PrepbufferL;
  luaL_pushresult_t       PushresultL;
  luaL_ref_t              RefL;
  luaL_register_t         RegisterL;
  luaL_typerror_t         TyperrorL;
  luaL_unref_t            UnrefL;
  luaL_where_t            WhereL;
  lua_atpanic_t           Atpanic;
  lua_call_t              Call;
  lua_checkstack_t        Checkstack;
  lua_close_t             Close;
  lua_concat_t            Concat;
  lua_cpcall_t            Cpcall;
  lua_createtable_t       Createtable;
  lua_dump_t              Dump;
  lua_equal_t             Equal;
  lua_error_t             Error;
  lua_gc_t                Gc;
  lua_getallocf_t         Getallocf;
  lua_getfenv_t           Getfenv;
  lua_getfield_t          Getfield;
  lua_gethook_t           Gethook;
  lua_gethookcount_t      Gethookcount;
  lua_gethookmask_t       Gethookmask;
  lua_getinfo_t           Getinfo;
  lua_getlocal_t          Getlocal;
  lua_getmetatable_t      Getmetatable;
  lua_getstack_t          Getstack;
  lua_gettable_t          Gettable;
  lua_gettop_t            Gettop;
  lua_getupvalue_t        Getupvalue;
  lua_insert_t            Insert;
  lua_iscfunction_t       Iscfunction;
  lua_isnumber_t          Isnumber;
  lua_isstring_t          Isstring;
  lua_isuserdata_t        Isuserdata;
  lua_lessthan_t          Lessthan;
  lua_load_t              Load;
  lua_newstate_t          Newstate;
  lua_newthread_t         Newthread;
  lua_newuserdata_t       Newuserdata;
  lua_next_t              Next;
  lua_objlen_t            Objlen;
  lua_pcall_t             Pcall;
  lua_pushboolean_t       Pushboolean;
  lua_pushcclosure_t      Pushcclosure;
  lua_pushfstring_t       Pushfstring;
  lua_pushinteger_t       Pushinteger;
  lua_pushlightuserdata_t Pushlightuserdata;
  lua_pushlstring_t       Pushlstring;
  lua_pushnil_t           Pushnil;
  lua_pushnumber_t        Pushnumber;
  lua_pushstring_t        Pushstring;
  lua_pushthread_t        Pushthread;
  lua_pushvalue_t         Pushvalue;
  lua_pushvfstring_t      Pushvfstring;
  lua_rawequal_t          Rawequal;
  lua_rawget_t            Rawget;
  lua_rawgeti_t           Rawgeti;
  lua_rawset_t            Rawset;
  lua_rawseti_t           Rawseti;
  lua_remove_t            Remove;
  lua_replace_t           Replace;
  lua_resume_t            Resume;
  lua_setallocf_t         Setallocf;
  lua_setfenv_t           Setfenv;
  lua_setfield_t          Setfield;
  lua_sethook_t           Sethook;
  lua_setlocal_t          Setlocal;
  lua_setmetatable_t      Setmetatable;
  lua_settable_t          Settable;
  lua_settop_t            Settop;
  lua_setupvalue_t        Setupvalue;
  lua_status_t            Status;
  lua_toboolean_t         Toboolean;
  lua_tocfunction_t       Tocfunction;
  lua_tointeger_t         Tointeger;
  lua_tolstring_t         Tolstring;
  lua_tonumber_t          Tonumber;
  lua_topointer_t         Topointer;
  lua_tothread_t          Tothread;
  lua_touserdata_t        Touserdata;
  lua_type_t              Type;
  lua_typename_t          Typename;
  lua_xmove_t             Xmove;
  lua_yield_t             Yield;
  luaopen_base_t          Open_base;
  luaopen_debug_t         Open_debug;
  luaopen_io_t            Open_io;
  luaopen_math_t          Open_math;
  luaopen_os_t            Open_os;
  luaopen_package_t       Open_package;
  luaopen_string_t        Open_string;
  luaopen_table_t         Open_table;
} lua_All_functions;

extern int luaL_loadfunctions(void* hModule, lua_All_functions* functions, size_t size_struct);

#ifdef __cplusplus
}
#endif

#endif
