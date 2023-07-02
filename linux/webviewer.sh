./dgate "--luastart:package.path=Global.basedir..[[lua/?.lua]];dofile(Global.basedir..[[lua/ladle.lua]])"
sensible-browser 127.0.0.1:8086/app/newweb
./dgate "--luastart:package.path=Global.basedir..[[lua/?.lua]];dofile(Global.basedir..[[lua/quitladle.lua]])"
