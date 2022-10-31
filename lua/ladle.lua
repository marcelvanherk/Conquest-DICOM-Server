-----------------------------------------------------
-- Ladle web server
-- Version 0.1.2
-- Original work Copyright (c) 2008 Samuel Saint-Pettersen (original author)
-- Modified work Copyright (c) 2015 Daniel Rempel
-- Released under the MIT License

-- mvh 20180113 Amalgation by Marcel van Herk; made compatible with Lua 5.1 and Luasocket 2.02
-- mvh 20180118 added dgatecgi handler
-- mvh 20180120 Added most conquest specific functions to lua Env
-- mvh 20180202 Added quit option; do not crash on invalid dgate.exe modes
-- mvh 20180204 Catch quit error and show Ladle stopped
-- mvh 20180204 Detect fail to start; timeout to 1s (helps hang on IE); print errors
-- mvh 20181110 Release 1.4.19c
-- mvh 20181115 Fix default of BaseDir
-- mvh 20181117 Use / in folder for linux compatibility
-- mvh 20181218 Added loadstring and DicomObject for new serialized data
-- mvh 20220828 Added dicomecho as trusted function; move used newweb source to htdocs/app
-- mvh 20220828 Note: does not support POST multipart; cannot upload yet
-- mvh 20220906 Responds to redirect.lua by sending all requests to there; 
--              Added redirect method; Fixed include method (pass handleIt); 
--              give luascript handler access to require, dofile, string
--              close files when done
-- mvh 20220909 Make it ladle_routes.lua; include and call router; 
--              disable 'quit' for debugging; print error message
--              added ipairs, table, print, tostring, JSON, servercommand to luascript handler
-- mvh 20220911 Redirect for dgate.exe uses folder
--              Default root is basedir/webserver/htdocs/
-- mvh 20220912 Returned to default root to /webserver/htdocs/app/newweb/ for compatibility
--              Added webroot as arg2 to startup; reenabled 'quit'; added pairs to Env
-- mvh 20220913 Added math to luascript handler
-- mvh 20220915 prepMain parses post or put message into request; CGI('') returns payload
-- mvh 20220916 CGI() also returns payload; correct size of payload
-- mvh 20220920 Added mjs to mimetypes
-- mvh 20221018 Added wasm; fix post without filename; added unlink writefile tempfile to env
-- mvh 20221018 Fixed STOW (needs multipart/related); writefile must be binary
-- mvh 20221022 Replaced some \n outputs by \r\n (tripped Quirt HTTP_GET module)

-----------------------------------------------------

-- lfs = require('lfs')

local mimetypes = {}

function mimetypes.getMime(ext)
	if ext == "" or mimetypes['mconf'][ext] == nil
	then
		return "application/octet-stream"
	else
		return mimetypes['mconf'][ext]['mime']
	end
end
function mimetypes.isBinary(ext)
	if ext == "" or mimetypes['mconf'][ext] == nil
	then
		return false
	else
		return mimetypes['mconf'][ext]['bin']
	end
end

mimetypes["mconf"] = {

	[".html"] = {
		["mime"] = "text/html",
		["bin"] = false,
		},
	[".xml"] = {
		["mime"] = "application/xml",
		["bin"] = false,
		},
	[".txt"] = {
		["mime"] = "text/plain",
		["bin"] = false,
		},
	[".css"] = {
		["mime"] = "text/css",
		["bin"] = false,
		},
	[".js"] = {
		["mime"] = "application/x-javascript",
		["bin"] = false,
		},
	[".mjs"] = {
		["mime"] = "application/x-javascript",
		["bin"] = false,
		},
	[".jpg"] = {
		["mime"] = "image/jpeg",
		["bin"] = true,
		},
	[".jpeg"] = {
		["mime"] = "image/jpeg",
		["bin"] = true,
		},
	[".png"] = {
		["mime"] = "image/png",
		["bin"] = true,
		},
	[".gif"] = {
		["mime"] = "image/gif",
		["bin"] = true,
		},
	[".ico"] = {
		["mime"] = "image/x-icon",
		["bin"] = true,
		},
	[".wasm"] = {
		["mime"] = "application/web-assembly",
		["bin"] = true,
		},
}

--------------------------------

local ladleutil = {}

function ladleutil.scandir(directory)
    local i, t, popen = 0, {}, io.popen
    for filename in popen('dir /b "'..directory..'"'):lines() do
        i = i + 1
        t[i] = filename
    end
    return t
end

function ladleutil.getRequestedFileInfo(request)
	local file = request["uri"]

	-- retrieve mime type for file based on extension
	local ext = string.match(file, "%.%l%l%l?%l?$") or ""
	local mimetype = mimetypes.getMime(ext)

	local flags
	if mimetypes.isBinary(ext) == false then
		-- if file is ASCII, use just read flag
		flags = "r"
	else
		-- otherwise file is binary, so also use binary flag (b)
		-- note: this is for operating systems which read binary
		-- files differently to plain text such as Windows
		flags = "rb"
	end

	return file, mimetype, flags
end

function ladleutil.fileExists(filename)
	local f = io.open(filename, 'r')
	if f then
		f:close()
		return true
	else
		return false
	end
end

function ladleutil.receiveHTTPRequest(client)
	local request = ""
	local line,err = ""
	repeat
		local line, err = client:receive("*l")
		if err then print(err) end
		if line
		then
			request = request .. "\r\n" .. line
		end
	until not line or line:len()==0 or err
	return request,err
end

function ladleutil.parseQueryString(query_string)
	-- From lua-wiki
	local urldecode = function (str)
	str = string.gsub (str, "+", " ")
	str = string.gsub (str, "%%(%x%x)",
	function(h) return string.char(tonumber(h,16)) end
			  )
	str = string.gsub (str, "\r\n", "\n")
	return str
	end

	local retval = {}

	while query_string:len()>0 do
		local a,b = query_string:find('=')
		local c = nil
		local index = query_string:sub( 0, a-1 )
		b,c = query_string:find('&')
		local value = ""
		if b
		then
			value = query_string:sub( a+1, b-1 )
			query_string = query_string:sub(b+1)
		else
			value = query_string:sub( a+1 )
			query_string = ""
		end

		index = urldecode(index)

		retval[index] = urldecode(value)
	end
	return retval
end

function ladleutil.parseRequest(request)
	local request_table = {}
	local request_text = request

	local line = ""

	local a,b = request_text:find("\r*\n")
	if not a or not b
	then
		--ladleutil.trace("ladleutil.parseRequest(request):")
		--ladleutil.trace("Suspicious request:")
		--ladleutil.trace(request)
		--ladleutil.trace("=======================================================")
		ladleutil.trace("Newlines (\\r\\n) not found in request: "..request)

		return {}
	end

	repeat
		local a,b = request_text:find("\r*\n")
		line = request_text:sub(0,a-1)
		request_text = request_text:sub(b+1)
	until line:len() > 0

	request_table["method"],request_table["url"],request_table["protocol"] = line:match("^([^ ]-) +([^ ]-) +([^ ]-)$")

	while request_text:len() > 0 do
		local a,b = request_text:find("\r*\n")
		local line = request_text:sub(0,a-1)
		request_text = request_text:sub(b+1)

		if line:len()>0
		then
			local key, value = line:match("^([^:]*): +(.+)$")
			request_table[key] = value
		end
	end

	query_string = (request_table["url"]):match("^/[^?]*%??(.*)$") or ""
	uri = (request_table["url"]):match("^/([^?]*)%??.*$") or ""

	request_table["query_string"] = query_string -- TODO: base64 decode?
	request_table["query"] = ladleutil.parseQueryString(query_string)
	request_table["uri"] = uri

	return request_table
end

-- decides which prep to run etc
function ladleutil.prepMain(request, client)
        -- simplistic multipart data parser, assumes one filename and must be the last parameter
	if request.method=='POST' or request.method=='PUT' then
		boundary = string.match(request["Content-Type"], '.+boundary=(.+)$')
		local len = request["Content-Length"]
		local data, err, partial = client:receive(len)
		local line = ''
		for i=1, 100 do
			line, data = string.match(data, "(.-)\r\n(.+)")
			if not line then
			        break
			end
			if string.find(line, 'filename=') then 
				local fn= string.match(line, '; filename="(.-)"')
				local nam= string.match(line, '; name="(.-)"')
				_, data = string.match(data, "(.-)\r\n(.+)")
				_, data = string.match(data, "(.-)\r\n(.+)")
				request.query["filename"]=fn
				request.query["_upload_"]=data:sub(1, -#boundary-9)
				--local f=io.open(fn,'wb') f:write(request["_upload_"]) f:close()
				break 
			elseif string.find(line, 'name=') then 
				local nam= string.match(line, 'name="(.-)"')
				_, data = string.match(data, "(.-)\r\n(.+)")
				val, data = string.match(data, "(.-)\r\n(.+)")
				request.query[nam]=val
			elseif string.find(line, 'Content') then 
				_, data = string.match(data, "(.-)\r\n(.+)")
				request.query["_upload_"]=data:sub(1, -#boundary-9)
				break 
			end
		end
	end
end

-- display error message and server information
-- used while serving pages (for different errors like 404, 500 etc)
function ladleutil.err(message,client)
	client:send(message)
	client:send("\n\nLadle web server\r\n")
end

function ladleutil.trace(message)
	print(os.date() .. ": " .. message)
end

--------------------------------

generic = {}

function generic.handler(request, client, config)
	local file, mimetype, flags = ladleutil.getRequestedFileInfo(request)

	local served = io.open(config["webroot"] .. file, flags)
	if served ~= nil then
		client:send("HTTP/1.1 200/OK\r\nServer: Ladle\r\n")
		client:send("Content-Type:" .. mimetype .. "\r\n\r\n")

		local content = served:read("*all")
		client:send(content)
                served:close()
	else
		-- TODO : link to ladle's generic err
		ladleutil.trace(("generic: %s not found"):format(config["webroot"] .. file))
		client:send("HTTP/1.1 404 Not Found\r\nServer: Ladle\r\n")
		client:send("Content-Type: text/plain\r\n\r\n")

		ladleutil.err("Not found!", client)
	end
end

function generic.match(filename)
	return true
end

generic.id = "generic"
generic.name = "Generic/Static file handler"

--------------------------------

luascript = {}

function luascript.executeLua(code, Env)
	local func, message = loadstring(code, "code") --load(code, "code","bt", Env)
	if not func
	then
		return nil, message
	end
  setfenv(func, Env)
	return xpcall(func, debug.traceback)
end

function luascript.parseLuaPage(page, Env)
	local a,b,c
	while page:len() > 0 do
		a,b = page:find("%<%?")

		if a
		then
			a= a-1
		
			Env.write(page:sub(0,a))
			page = page:sub(b+1)
					
			a,c = page:find("%?%>")
			if not a
			then
				return nil, "matching '?>' not found!"
			else
				a=a-1
				local code = page:sub(1,a)
								
				local retval, err = luascript.executeLua(code, Env)
				if err
				then
					return nil, err
				end
				page = page:sub(c+1)
			end
		else
			Env.write(page)
			page = ""
		end
	end
end

function luascript.handleIt(filename, Env)
	local uri = filename:match("[^/]+$")
	-- ladleutil.trace(("luascript: handleIt(%s)" ):format(uri, filename))
	local file_l = io.open(filename, "r")
	if not file_l
	then
		ladleutil.trace(("luascript: failed to open %s"):format(filename))
		return nil, "HTTP/1.1 500 Internal Server Error\r\nServer: Ladle\r\n" ..
					"Content-Type: text/plain\r\n\r\n" ..
					"500 Internal Server Error\r\n"
	end

	local fileContents = file_l:read("*all")
        file_l:close()
	
	-- either lua script
	-- or anything else
	if filename:match(".+%.lua$") ~= nil
	then
		local retval, err = luascript.executeLua(fileContents, Env)
		if err
		then
			Env.onerror("luascript: " .. uri .. ": " .. err)
		end
	else
		local retval, err = luascript.parseLuaPage(fileContents, Env)
		if err
		then
			Env.onerror("luascript: " .. uri .. ": " .. err)
		end
	end
end

function luascript.genEnv(_Env, request, config, handleIt, client)
	local Env = _Env
	Env.__tmp_output_buffer = ""
	Env.write	= function (text)
					Env.__tmp_output_buffer = Env.__tmp_output_buffer .. (text or '')
				end
	Env.trace	= ladleutil.trace
	Env.onerror	= function (text)
						ladleutil.trace(text)
						if not text:match("\n$")
						then
							text = text .. "\n"
						end
						Env.write(text)
						
				end
	Env.request = request
	Env.config = config

	Env.include	= function (filename)
					handleIt(Env.config.webroot .. "/" .. filename, Env)
				end
  
	Env.redirect	= function (uri)
                                        req = request
                                        req.uri = uri
					servedirect(req, client)
					print('redirect', uri)
			end

        Env.require = require
        Env.dofile = dofile
        Env.string = string
        Env.servercommand = servercommand
	Env.print=print
	Env.tostring=tostring
	Env.ipairs=ipairs
	Env.pairs=pairs
	Env.table=table
	Env.math=math
	Env.tempfile=tempfile
	Env.writefile=function(nam, dat) local f=io.open(nam, 'wb') f:write(dat) f:close() end
	Env.unlink=function(nam) os.remove(nam) end
	Env.JSON=require('json')
	
	local router = require 'router'
	Env.routes = router.new()

	return Env
end

function luascript.handler(request, client, config)
	local _ENV = {}
        -- setfenv(1, _ENV) 
	local Env = luascript.genEnv(_ENV, request, config, luascript.handleIt, client)
	
	if not ladleutil.fileExists(config.webroot .. request.uri)
	then
		-- TODO : link to ladle's generic err
		ladleutil.trace(("luascript: %s not found"):format(config.webroot .. request.uri))
		client:send("HTTP/1.1 404 Not Found\r\nServer: Ladle\r\n")
		client:send("Content-Type: text/plain\r\n\r\n")
		client:send("404 Not Found\r\n")
		return
	end
	
	--local cdir = lfs.currentdir()
	--lfs.chdir(config.webroot)
	luascript.handleIt(config.webroot .. request.uri, Env)
	Env.routes:execute(request.method, string.gsub(request.orguri, 'index.lua', ''))

	client:send(Env.__tmp_output_buffer)
	--lfs.chdir(cdir)
end

function luascript.match(filename)
	return (filename:match(".+%.lp$") ~= nil) or (filename:match(".+%.lua$") ~= nil)
end

luascript.id = "luascript"
luascript.name = "Lua script and page handler"

--------------------------------

dgatecgi = {}

dgatecgi.executeLua = luascript.executelua
dgatecgi.parseLuaPage = luascript.parseLuaPage
dgatecgi.handleIt = luascript.handleIt
dgatecgi.genEnv = luascript.genEnv
function dgatecgi.handler(request, client, config)
  local folder = string.match(request.orguri, '(.-)/dgate.exe') or ''
  if folder~='' then folder = folder .. '/' end
  local _ENV = {}
  local Env = luascript.genEnv(_ENV, request, config)
  
  -- inherited from luascript, need not reimport most functions anymore
  Env.tostring = tostring
  Env.tonumber = tonumber
  Env.table = table
  Env.math = math
  Env.unpack = unpack
  Env.ipairs = ipairs
  Env.pairs = pairs
  Env.string = string
  Env.io = io
  Env.loadstring = loadstring
  Env.require = require
  Env.dofile = dofile

  -- conquest specific
  Env.get_amap = get_amap
  Env.dicomquery = dicomquery
  Env.dicomquery2 = dicomquery2
  Env.dicommove = dicommove
  Env.newdicomobject = newdicomobject
  Env.Global = Global
  Env.servercommand = servercommand
  Env.DicomObject = DicomObject
  Env.dicomecho = dicomecho

  Env.print = function(...) 
    Env.write(...) 
    Env.write('\r\n') 
  end
  Env.CGI = function(a, b)
    if (a or '')=='' then
      return request.query["_upload_"] or (b or '')
    else
      return request.query[a] or (b or '')
    end
  end
  Env.gpps = function(a, b, c)
    if b=='viewer' then return 'wadoseriesviewer' end
    if b=='studyviewer' then return 'wadostudyviewer' end
    if b=='TCPPort' then return servercommand('lua:return Global.TCPPort') end
    if b=='WebServerFor' then return '127.0.0.1' end
    print('unhandled gpps request, returning default', a, b)
    return c
  end
  Env.HTML = 
    function (a, b, c, d, e, f, g, h) 
      if Env.__tmp_output_buffer=='' then
        Env.__tmp_output_buffer = "HTTP/1.1 200/OK\r\nServer: Ladle\r\n"
      end
      Env.write(string.format(a, b or '', c or '', d or '', e or '', f or '', g or '', h or ''))
      Env.write('\r\n') 
    end
  
  Env.script_name = 'dgate.exe'
  Env.extra = 'ladle=1'
  Env.version = Global.DGATEVERSION or 'unknown'
  Env.size = 512
  Env.iconsize = 56
  Env.graphic = 'jpg'
  Env.study2 = request.query.study
  Env.series2 = request.query.series

  if request.query['mode']=='quit' then 
    error('quiting ladle server')
  end
  request.uri = (request.query['mode'] or 'xxxx')..'.lua' 
	if not ladleutil.fileExists(config.webroot .. folder .. request.uri) then
	  request.uri = (request.query['requestType'] or 'xxxx')..'.lua' 
	elseif not ladleutil.fileExists(config.webroot ..  folder .. request.uri)
	then
		-- TODO : link to ladle's generic err
		ladleutil.trace(("luascript: %s not found"):format(config.webroot .. request.uri))
		client:send("HTTP/1.1 404 Not Found\r\nServer: Ladle\r\n")
		client:send("Content-Type: text/plain\r\n\r\n")
		client:send("404 Not Found\r\n")
		return
	end
	
	--local cdir = lfs.currentdir()
	--lfs.chdir(config.webroot)
	luascript.handleIt(config.webroot .. folder .. request.uri, Env)
	client:send(Env.__tmp_output_buffer)
	--lfs.chdir(cdir)
end
function dgatecgi.match(filename)
	return (filename:match("dgate.exe") ~= nil) or (filename:match("dgate") ~= nil)
end
dgatecgi.id = "dgatecgi"
dgatecgi.name = "Lua script and page handler to emulate dgate CGI mode"

---------------------

Server = "Ladle"
ServerVersion = "0.1.3"

-- load required modules
socket = require('socket')
server = assert(socket.tcp())

-- load configuration file
-- TODO: have a default array with config and merge with the loaded one
if ladleutil.fileExists('config.lua') then
	config = require('config')
else
	config = {
	["hostname"] = "*",
	["port"] = 8086,
	["webroot"] = (servercommand('lua:return Global.BaseDir') or '../')..[[webserver/htdocs/app/newweb/]],
  }
end

-- load extensions
extensions = {}
extensions.generic = generic
extensions.luascript = luascript
extensions.dgatecgi = dgatecgi

function getHandler(request)
	local handler = nil
	for k,ext in pairs(extensions) do
		if ext["id"] ~= "generic" and ext.match(request["uri"])
		then
			handler = ext.handler
			break
		end
	end
	-- no specific match, use generic handler
	if not handler
	then
		handler = extensions["generic"].handler
	end
	return handler
end

-- checks whether the root index file is requested and finds an appropriate
-- one if needed
function checkURI(uri)
	if ladleutil.fileExists(config["webroot"] .. "ladle_routes.lua") then
          uri = "ladle_routes.lua"
        end
                
	-- if index file was requested
	-- loop til' the first index.* file found
	if(uri == "")
	then
		if ladleutil.fileExists(config["webroot"] .. "index.html")
		then
			uri = "index.html"
		else
			local wrootIndex = ladleutil.scandir(config["webroot"])
			local chosenIndex = ""

			for k,v in pairs(wrootIndex) do
			if v:match("^index.*")
			then
				chosenIndex = "" .. v
				break
			end
			end
			uri = chosenIndex
		end
	end
	return uri
end

function serve(request, client)
	request["orguri"] = request["uri"]
	request["uri"] = checkURI(request["uri"])
  
	-- find an appropriate handler in extensions
	local handler = getHandler(request)

	-- Got a handler, run it
	handler(request, client, config)

	-- done with client, close request
	client:close()
end

function servedirect(request, client)
	request["orguri"] = request["uri"]
	-- request["uri"] = checkURI(request["uri"])
  
	-- find an appropriate handler in extensions
	local handler = getHandler(request)

	-- Got a handler, run it
	handler(request, client, config)

	-- done with client, close request
	client:close()
end

function clientHandler(client)
	-- set timeout - 1 sec.
	client:settimeout(1)
	-- receive request from client
	local request_text, err = ladleutil.receiveHTTPRequest(client)

	-- if there's no error, begin serving content or kill server
	if not err then
		-- parse request
		local request = ladleutil.parseRequest(request_text)
    if request.uri==nil then client:close() return end
		-- run a prep [e.g. for POST with files - download files]
		ladleutil.prepMain(request, client)
		-- begin serving content
		serve(request, client)
	end
end

-- TODO: maybe implement keepalive?
function waitReceive()
	-- loop while waiting for a client request
	while 1 do
		-- accept a client request
		local client = server:accept()
		clientHandler(client)
	end
end

-- start web server
function main(arg1, arg2)
	-- command line argument overrides config file entry:
	local port = arg1
	-- if no port specified on command line, use config entry:
	if port == nil then port = config['port'] end
	-- if still no port, fall back to default port, 80:
	if port == nil then port = 80 end

	-- load hostname from config file:
	local hostname = config['hostname']
	if hostname == nil then hostname = '*' end -- fall back to default
	
	if arg2 then 
	  config["webroot"] = arg2
	else
	  if config["webroot"] == "" or config["webroot"] == nil
	  then
		config["webroot"] = "www/"
	  end
	end

	-- display initial program information
	ladleutil.trace(("%s web server v%s (c) 2008 S Saint-Pettersen (c) 2015 D Rempel"):format(Server,ServerVersion))

	-- create tcp socket on localhost:$port
	-- local server = socket.bind(hostname, port)
	server:bind(hostname, port)
	if not server:listen(5) then
	  ladleutil.trace("Failed to listen on given hostname:port, Ladle already running?")
	  return
	end
  
        if not server
	then
	  ladleutil.trace("Failed to bind to given hostname:port")
	end

	-- display message to web server is running
	ladleutil.trace(("Serving on %s:%d"):format(hostname,port))
  local a, b = pcall(waitReceive) -- begin waiting for client requests
  print(b)
  ladleutil.trace("Ladle web server stopped")
end

-- invoke program starting point:
-- parameter is command-line argument for port number and webroot
main((arg or {})[1], (arg or {})[2])
