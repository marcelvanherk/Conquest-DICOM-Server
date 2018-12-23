./dgate "--luastart:package.path=[[lua/?.lua]];dofile[[lua/ladle.lua]]"
firefox 127.0.0.1:8086/cgi-bin/dgate?mode=start
./dgate "--luastart:package.path=[[lua/?.lua]];dofile[[lua/quitladle.lua]]"
