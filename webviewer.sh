./dgate "--luastart:package.path=Global.basedir..[[lua/?.lua]];dofile(Global.basedir..[[lua/ladle.lua]])"
sensible-browser 127.0.0.1:8086/cgi-bin/dgate?mode=start
./dgate "--luastart:package.path=Global.basedir..[[lua/?.lua]];dofile(Global.basedir..[[lua/quitladle.lua]])"
