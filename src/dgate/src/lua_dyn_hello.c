#include "lua_dyn.h"
#ifdef _WIN32
#include <windows.h>
#else
#include <dlfcn.h>
#endif

#define LUA_PREFIX LuaFunctions.
lua_All_functions LuaFunctions;

int main(int argc, char* argv[])
{
	lua_State* L;
	/* Load lua.dll dynamically */
#ifdef _WIN32
	HMODULE module = LoadLibrary("lua5.1.dll");
#else
	void* module = dlopen("liblua-5.1.so", RTLD_LAZY);
#endif
	if(!luaL_loadfunctions(module, &LuaFunctions, sizeof(LuaFunctions)))
	{
		printf("Error loading Lua DLL!\n");
		return (1);
	}
	
	/* From now on, the usual Lua API is available */
	L = lua_open();
	luaL_openlibs(L);
	luaL_dostring(L, "print 'Hello World'");
	lua_close(L);
	return 0;
}
