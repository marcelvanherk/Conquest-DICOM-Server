-- Rserve client - original from https://github.com/albhasan/luaRserve
--
-- 20160903 mvh   Remove separator from splitstring output
--                Strip 0 bytes from string outputs
--                Corrected output from printtable and made it more lua compatible
-- 20160904 mvh   Add lua vector, matrix, frame, factor objects representing their R counterparts
--                Simplify code; add rclient(string) and rclient.evalVoid(string)
-- 20160905 mvh   Added R(), R.connect, R.a and R.a=value; csv interface to frame
--                Fix NA for boolean
-- 20160906 mvh   Constructors of matrix and factor autogenerate dims or levels if not passed
--                Allow passing simple tables back and forth to R as vectors (NA not supported yet)
-- 20160907 mvh   R() now maps to evaluateVoid, use R.evaluate() to get return values, renamed internals to start with _
--                Do not read response in evaluateVoid calls to avoid timeout on those calls
--                known bugs: tcpcall:read*('*a) works on timeout, factor.fromR gives numerical levels as string
--                Transpose frame.contents for readCSV and writeCSV; use evaluateVoid for _set
--                Always read 32 byte response and then hangup unless reading data; much faster
-- 20160910 mvh   TCP now interprets length of response and no longer works on timeout; timout to 30s
--                Print '-- R >' and '-- R <' for evaluateVoid and evaluate to show direction of data
--                Added support for 3D and 4D matrices; embed CSV interface here
--                Code cleanup using ZBS evaluate; test million numbers: Lua->R 6s; R->Lua 50s
--                evaluateVoid returns nothing
-- 20160910 mvh   Use Convert (Delphi in my case) to speed up R --> Lua conversion 25 times
-- 20160911 mvh   Renamed to ConvertBinaryData; added storeArray and using in R.var = matrix
--                Note: does not work for matrix of booleans yet; fix 4D matrix toR
--                Make sure string to R is multiple of 4 characters
--                test million numbers: Lua->R <1s; R->Lua <1s; NOTE: max matrix size currently ~1 million (24 bits size)
-- 20160917 mvh   Added limited vstruct emulation based on ConvertBinaryData -> optional vstruct independent
--                Did require some simplifying of formats used
-- 20160921 mvh   Fix length of last format passed to ConvertBinaryData
-- 20161019 mvh	  Block out print statements
-- 20161022 mvh	  Added frame:fromCSV
-- 20170226 mvh   Note: cannot assign to frame.name

--[[
  R client software with native Lua representation of some R objects. 
  
  Sample usage R side:
  
  install.packages("Rserve")
  library(Rserve)
  Rserve() or Rserve(debug=T)
  
  Sample usage Lua side (assumes e.g.: ../lua/?/ini.lua is part of package.path)
  
  R = require "rclient"
  R.connect()
  R("library(survival)")  -- same as R.evaluateVoid("library(survival)")
  a = R.evaluate('d <- c(1,2,3,NA); e <- c("red", "white", NA, "blue"); f <- c(NA,TRUE,TRUE,FALSE); mydata <- data.frame(d,e,f)')
  printtable(a)
  
  f = frame:new({{1,5,3},{'a','b','c'},{0,1,2}}, {'time','letters','status'})
  --f:readCSV(filename)
  R.f = f
  R('f$SurvObj <- with(f, Surv(time, status == 2))')
  printtable(R.f)
  
  m = matrix:new({{1,1},{1,-1},{-1,1},{-1,-1}})
  R.m = m
  R('m = solve(m)')
  printtable(R.m)
  
  f = factor:new('red', 'green', 'blue', 'red')
  printtable(f)
  R.f = f
  printtable(R.f)
  
  v = {1,2,3,4}
  R.v = v
  printtable(R.v)
]]

local rclient = {}
local socket  = require("socket")
local vstruct

-------------------------------------
-- Split the given string given the separator. 
-- Regular split function does not work on string with 0 separators
-- @param str  string to be split
-- @param sep  separator character (string)
-------------------------------------
local function splitstring(str, sep)
  local res = {}
  local counter = 1
  local pos = 1
  for i = 1, #str do
    if (string.sub(str, i, i) == sep) then
      res[counter] = string.sub(str, pos, i-1)
      pos  = i + 1
      counter = counter + 1
    end
  end
  return res
end

-- faster (but limited) implementation of vstruct using C function ConvertBinaryData
if ConvertBinaryData then
vstruct = {}
  vstruct.write = function(format, t)
    local o = ''
    local f = splitstring(format..' ', ' ')
    for _, v in ipairs(f) do
      if #f==1 then
        o = ConvertBinaryData(v, t)
      else
        o = o .. ConvertBinaryData(v, t)
        table.remove(t, 1)
      end
    end
    return o
  end
  vstruct.read = function(format, s)
    local o = {}
    local f = splitstring(format..' ', ' ')
    local i = 1
    for k, v in ipairs(f) do
      local j=#s+1
      if k~=#f then j=i+string.sub(v, #v, #v) end  -- only supports single short elements except for last
      t = ConvertBinaryData(v, string.sub(s, i, j-1))
      for _, v2 in ipairs(t) do 
        table.insert(o, v2)
      end
      if k~=#f then i = i+string.sub(v, #v, #v) end
    end
    return o
  end 
else
  vstruct = require("vstruct")
end

-------------------------------------
-------------------------------------
-------------------------------------
-- function readCSV(filename, itemnames)
-- read CSV file if itemnames is set, use the first line as item name
-- returns table[1..nlines] of tables with items, 
-- and returns table with ordered heading (needed to rewrite)

-- function writeCSV(t, filename, heads)
-- write table t as CSV file; if heads passed use heads as first line and
-- each row in t contains names items; if not it contains numbered items

-- mvh 20151126 Fix for tables with empty header cells
-- mvh 20151130 Fix for tables with booleans
-- mvh/AM 20160329 Fix for using ulua
-- mvh 20160907 Convert to number if possible, local f; support reading bool and nil

function escapeCSV (s)
  if s==nil or s=='nil' then
    return nil
  end
  
  if type(s)~='string' then
    s = tostring(s)
  end
  if string.find(s, '[,"]') then
    s = '"' .. string.gsub(s, '"', '""') .. '"'
  end
  return s
end

-- Convert from CSV string to table (converts a single line of a CSV file)
function fromCSV (s)
  s = s .. ','        -- ending comma
  local t = {}        -- table to collect fields
  local fieldstart = 1
  repeat
    -- next field is quoted? (start with `"'?)
    if string.find(s, '^"', fieldstart) then
      local a, c
      local i  = fieldstart
      repeat
        -- find closing quote
        a, i, c = string.find(s, '"("?)', i+1)
      until c ~= '"'    -- quote not followed by quote?
      if not i then error('unmatched "') end
      local f = string.sub(s, fieldstart+1, i-1)
      if f=='nil' then f='' end
      table.insert(t, (string.gsub(f, '""', '"')))
      fieldstart = string.find(s, ',', i) + 1
    else                -- unquoted; find next comma
      local nexti = string.find(s, ',', fieldstart)
      local f = string.sub(s, fieldstart, nexti-1) or ''
      if f=='nil'   then f = '' end
      if f=='TRUE'  then f = true end
      if f=='FALSE' then f = false end
      table.insert(t, f)
      fieldstart = nexti + 1
    end
  until fieldstart > string.len(s)
  return t
end

-- Convert from table to CSV string
function toCSV (tt)
  local s = ""
  for _,p in ipairs(tt) do  
    s = s .. "," .. escapeCSV(p)
  end
  return string.sub(s, 2)      -- remove first comma
end

-- read CSV file if itemnames is set, use the first line as item name
-- returns table[1..nlines] of tables with items, 
-- and returns table with ordered heading (needed to rewrite)
function readCSV(filename, itemnames)
  itemnames = itemnames or true
  local t = {}
  local heads
  local f = io.open(filename, 'rt')
  for s in f:lines() do 
    if itemnames then
      heads = fromCSV (s)
      for k,v in ipairs(heads) do 
        if v=='' then v = 'Column'..k end
        heads[k] = v
      end
      itemnames = false
    else
      t[#t+1] = {}
      s = fromCSV (s)
      for k,v in ipairs(s) do
        if v~='' then
          if tonumber(v) then
            t[#t][heads[k]] = tonumber(v)
          else
            t[#t][heads[k]] = v
          end
        end
      end
    end
  end
  f:close()
  return t, heads
end

-- write table t as CSV file; if heads passed use heads as first line and
-- each row in t contains names items; if not it contains numbered items
function writeCSV(t, filename, heads)
  local f = io.open(filename, 'wt')
  if heads then
    f:write(toCSV(heads)..'\n')
  end
  for _, v in ipairs(t) do
    if heads then
      local w = {}
      for _, v2 in ipairs(heads) do
        w[#w+1]=v[v2] or ''
      end
      f:write(toCSV(w)..'\n')
    else
      f:write(toCSV(v)..'\n')
    end
  end
  f:close()
end

-------------------------------------
-------------------------------------
-------------------------------------

-------------------------------------
-- vector object (internal use, mostly for toR method)
-- @param contents 
-- access elements as vector[index]
-------------------------------------

vector = {}
vector._class = 'vector'
function vector:new(contents)
  local t = {}
  t.contents = contents or {}
  setmetatable(t, vector)
  return t
end

function vector:__index(key)
  return self.contents[key] or rawget(self, key) or rawget(vector, key)
end

function vector:fromR(t)
  self.contents = t
end

function vector:toR(var)
  var = var or '__t0'
  return string.format('%s <- c(%s)', var, table.concat(self.contents, ','))
end
    
-------------------------------------
-- matrix object; represents 1D..4D array
-- @param contents data (table of tables ..)
-- @param dims     dims array, optional
-- access as matrix[i]; matrix[j][i]; etc
-------------------------------------

matrix = {}
matrix._class = 'matrix'
function matrix:new(contents, dims)
  local t = {}
  t.contents = contents or {}
  if dims==nil then
    t.dims  = {}
    local s = t.contents
    for _=1,4 do
      table.insert(t.dims, 1, #s)
      s = s[1]
      if type(s)~='table' then 
        break;
      end
    end
  else
    t.dims = dims
  end
  setmetatable(t, matrix)
  return t
end

function matrix:__index(key)
  return self.contents[key] or rawget(self, key) or rawget(matrix, key)
end

function matrix:fromR(t)
  local acount=1;
  self.contents = {}
  self.dims = t[1][1][1]
  if #self.dims==1 then
    self.contents = t[2]
  elseif #self.dims==2 then
    for i=1, self.dims[1] do
      self.contents[i] = {}
      for j=1, self.dims[2] do
        self.contents[i][j] = t[2][acount]
        acount = acount+1
      end
    end
  elseif #self.dims==3 then
    for i=1, self.dims[1] do
      self.contents[i] = {}
      for j=1, self.dims[2] do
        self.contents[i][j] = {}
        for k=1, self.dims[3] do
          self.contents[i][j][k] = t[2][acount]
          acount = acount+1
        end
      end
    end
  elseif #self.dims==4 then
    for i=1, self.dims[1] do
      self.contents[i] = {}
      for j=1, self.dims[2] do
        self.contents[i][j] = {}
        for k=1, self.dims[3] do
          self.contents[i][j][k] = {}
          for L=1, self.dims[3] do
            self.contents[i][j][k][L] = t[2][acount]
            acount = acount+1
          end
        end
      end
    end
  else
    error('Matrix fromR() not implemented for dimension = '..#self.dims)
  end    
end

function matrix:toR(var)
  var = var or '__t0'
  if #self.dims==1 then
    return string.format('%s <- c(%s)', var, table.concat(self.contents, ','))
  elseif #self.dims==2 then
    local ds = {}
    for i=1, self.dims[2] do
      ds[i] = table.concat(self.contents[i], ',')
    end
    return string.format('%s <- matrix(c(%s), nrow=%d, ncol=%d)', 
      var, table.concat(ds, ','), self.dims[1], self.dims[2])
  elseif #self.dims==3 then
    local ds = {}
    for j=1, self.dims[3] do
      for i=1, self.dims[2] do
        ds[i+j*self.dims[2]] = table.concat(self.contents[j][i], ',')
      end
    end
    return string.format('%s <- array(c(%s), dim=c(%d, %d, %d))', 
      var, table.concat(ds, ','), self.dims[1], self.dims[2], self.dims[3])
  elseif #self.dims==4 then
    local ds = {}
    for k=1, self.dims[4] do
      for j=1, self.dims[3] do
        for i=1, self.dims[2] do
          ds[i+self.dims[2]*(j+k*self.dims[3])] = table.concat(self.contents[k][j][i], ',')
        end
      end
    end
    return string.format('%s <- array(c(%s), dim=c(%d, %d, %d, %d))', 
      var, table.concat(ds, ','), self.dims[1], self.dims[2], self.dims[3], self.dims[4])
  else
    error('matrix:toR not implemented for matrix of dimension: '..#self.dims)
  end
end

-------------------------------------
-- frame object
-- @param contents table of column tables
-- @param names    names of columns
-- access columns provided as frame.name[i]
-------------------------------------
frame = {}
frame._class = 'frame'
function frame:new(contents, names)
  local t = {}
  t.contents = contents or {}
  t.names    = names or {}
  setmetatable(t, frame)
  return t
end

function frame:__index(key)
  for k,v in ipairs(self.names) do
    if v==key then
      return self.contents[k]
    end
  end
  return rawget(self, key) or rawget(frame, key)
end

function frame:fromR(t)
  local count=1;
  self.names=t[1][1][1];
  local skip=false;
  self.contents = {}
  for i=1, #t[2] do
    if type(t[2][i][1])=='table' then
      if t[2][i][1][2]=='levels' then
        local levels = t[2][i][1][1]
        self.contents[count]={}
        for j=1, #t[2][i+1] do
          self.contents[count][j] = levels[t[2][i+1][j]]
        end
        count = count+1
        skip=true;
      end
    else 
      if not skip then
        self.contents[count] = t[2][i]
        count = count+1
      end
      skip=false;
    end
  end
end

function frame:toR(var)
  var = var or '__t0'
  
  local c  = {}
  local n  = {}
  local ns = {}
  for k, v in ipairs(self.names) do
    ns[k] = '"'..v..'"'
  end
  
  local maxn=0
  for i=1, #self.contents do
    maxn = math.max(maxn, #self.contents[i])
  end
  
  function typeof(t)
    for i=1, maxn do
      if t[i] then
        return type(t[i])
      end
    end
  end
  
  for i=1, #self.contents do
    local ds = {}
    local t = typeof(self.contents[i])
    
    if t=='string' then
      for j=1, maxn do
        local v = self.contents[i][j]
        if v then 
          ds[j] = '"'..v..'"'
        else
          ds[j] = 'NA'
        end
      end
    elseif t=='boolean' or t=='number' then
      for j=1, maxn do
        local v = self.contents[i][j]
        if v==nil or v~=v then
          ds[j] = 'NA'
        else
          ds[j] = tostring(v)
          if type(v)=='boolean' then ds[j]=string.upper(ds[j]) end
        end
      end
    end
    c[i] = string.format('rclient_t%d <- c(%s)', i, table.concat(ds, ','))
    n[i] = string.format('rclient_t%d', i)
  end
  local  str1 = table.concat(c, ';')
  local  str2 = string.format(';%s <- data.frame(%s); ', var, table.concat(n, ','))
  local  str3 = string.format('names(%s) = c(%s); '   , var, table.concat(ns, ','))
  return str1 .. str2 .. str3    
end

-- CSV data access for frame
function frame:readCSV(filename)
  local a
  a, self.names = readCSV(filename, true)
  
  -- transpose CSV table for frame format
  self.contents = {}
  for i=1, #self.names do
    self.contents[i] = {}
    for j=1, #a do
      self.contents[i][j] = a[j][self.names[i]]
    end
  end
  collectgarbage()
end

-- CSV data access for frame
function frame:fromCSV(data, names)
  self.names = names
  
  -- transpose CSV table for frame format
  self.contents = {}
  for i=1, #self.names do
    self.contents[i] = {}
    for j=1, #data do
      self.contents[i][j] = data[j][self.names[i]]
    end
  end
  collectgarbage()
end

function frame:writeCSV(filename)
  local a = {}
  for i=1, #self.contents[1] do
    a[i] = {}
    for j=1, #self.names do
      a[i][self.names[j]] = self.contents[i][j]
    end
  end
  writeCSV(a, filename, self.names)
  collectgarbage()
end

-------------------------------------
-- factor object
-- @param contents item list
-- @param levels   name of classes (optional)
-- access translated items as factor[index]
-------------------------------------

factor = {}
factor._class = 'factor'
function factor:new(contents, levels)
  local t = {}
  t.contents = contents or {}
  -- factorize contents if needed
  if levels==nil then
    local l = {} 
    local i = 1
    for _, v in ipairs(t.contents) do
      if l[v]==nil then
        l[v] = i
        i = i+1
      end
    end
    t.levels = {}
    for k, v in pairs(l) do
      t.levels[v] = k
    end
    for k, v in ipairs(t.contents) do
      t.contents[k] = l[v]
    end
  else
    t.levels = levels
  end
  setmetatable(t, factor)
  return t
end

function factor:__index(key)
  if self.contents[key] then
    return self.levels[self.contents[key]]
  else
    return rawget(self, key) or rawget(factor, key)
  end
end

function factor:fromR(t)
  self.levels   = t[1][1][1]
  self.contents = t[2]
end

-- todo: pass frame without unpacking
function factor:toR(var)
  var = var or '__t0'
  local ds = {}
    
  function typeof(t)
    for _, v in pairs(t) do
      if v==v then
        return type(v)
      end
    end
  end
  
  local t = typeof(self.levels)
  
  if t=='string' then
    for i=1, #self.contents do
      local v = self[i]
      if v then 
        ds[i] = '"'..v..'"'
      else
        ds[i] = 'NA'
      end
    end
  elseif t=='boolean' then
    for i=1, #self.contents do
      local v = self[i]
      if v==nil then
        ds[i] = 'NA'
      else
        ds[i] = string.upper(tostring(v))
      end
    end
  elseif t=='number' then
    for i=1, #self.contents do
      local v = self[i]
      if v~=v then
        ds[i] = 'NA'
      else
        ds[i] = v
      end
    end
  end
  return string.format('%s <- factor(c(%s)); ', var, table.concat(ds, ','))
end

-------------------------------------
-- Print a table's content recursively.
-- @param e       A table
-- @param prefix  A string used to indent sub-tables' contents
-- @param level   internal counter for indenting
-------------------------------------
function printtable (e, prefix, level)
  prefix = prefix or "  "
  level = level or 0
  if type(e) == "table" then
    for k,v in pairs(e) do -- for every element in the table
      if type(v) == "table" then
        print(string.rep(prefix, level) .. '[' .. tostring(k).. "] = {")
        printtable(v, prefix, level+1)       -- recursively repeat the same procedure
        print(string.rep(prefix, level) .. '}')
      else
        if type(v)=='string' then v = "'"..v.."'" end
        if type(k)=='number' then k = "["..k.."]" end 
        print(string.rep(prefix, level) .. tostring(k) .. " = " .. tostring(v))
      end
    end
  else
    print(tostring(e))
  end
end

-------------------------------------
-- Retrieve Rserve metadata
-- @param rsserver  Server name or IP address
-- @param rsport    Port number
-------------------------------------
local server = {} -- R Server metadata

local function getserverdata(rsserver, rsport)
  local s, _, partial = rclient._calltcp(rsserver, rsport, " ", false)
  local res = s or partial
  server = rclient._parseids(string.sub(res , 1 , 32), rsserver, rsport)
end

-------------------------------------
-- Print the Rserve metadata
-------------------------------------

local function printserverdata()
  printtable(server)
end

-------------------------------------
-- Build an QAP1 message
-- @param rexp    R expression (string)
-- @param command R command (number)
-- @param extra   length of extra payload appended to message
-------------------------------------
local function buildstrmsg(rexp, command, extra)
  extra = extra or 0
  local s = rexp .. string.char(0)
  
  while math.fmod(#s, 4)~=0 do
    s = s .. string.char(0)
  end
  
  -- QAP1 message header
  command = command or 3      -- command specifies the request or response type
  local length = (#s + extra + 4)  -- length of the message (bits 0-31)
  local offset = 0            -- offset of the data part --  offset specifies the offset of the data part, 0 directly 
  local length2 = 0           -- length of the message (bits 32-6(24-bit int) 
  
  -- QAP1 data part
  local dptype = 4  -- "DT_STRING"
  local data = {command, length, offset, length2, dptype, #s, s}
  return vstruct.write("u4 u4 u4 u4 u1 u3 s" .. #s, data)
end

-------------------------------------
-- Build an SEXP string message
-- @param array table of numver, string or boolean
-------------------------------------

local function buildArraySEXP(array)
  if type(array[1])=='number' then
    local S = vstruct.write(#array .. "*f8", array)
    return vstruct.write("u1 u3 u1 u3", {10, #array*8+4, 33, #array*8}) .. S
  elseif type(array[1])=='string' then
    local S = table.concat(array, string.char(0))
    local L = string.len(S)
    local E = math.fmod(L+1, 4)
    if E~=0 then E = 4-E end
    S = S .. string.rep(string.char(0), 1+E)
    return vstruct.write("u1 u3 u1 u3", {10, string.len(S)+4, 34, L+1}) .. S
--  elseif type(array[1])=='boolean' then
--    return vstruct.write("u1 u3 u1 u3 ", {10, #array+4, 36, #array}) .. vstruct.write(#array .. "*b1", array)
--  elseif type(array[1])=='integer' then
--    return vstruct.write("u1 u3 u1 u3 ", {10, #array*4+4, 36, #array*4}) .. vstruct.write(#array .. "*i4", array)
  end
end

function rclient.storeArray(var, array)
  local x = buildArraySEXP(array)
  local msgbin = buildstrmsg(var, 32, string.len(x)) .. x
  rclient._calltcp(server.host, server.port, msgbin, true)
end

-------------------------------------
-- Parse the header of a SEXP expression
-- @param str    first 4 bytes of a binary encoded SEXP
-------------------------------------
local function getheader(str)
  if #str < 4 then
    print("ERROR: Invalid header (too short)")
    return nil
  end
  local header = string.sub(str, 1, 4)
  --local type = vstruct.read("[1 | b2 u6]", string.sub(header, 1, 1)) -- type[1] = XT_HAS_ATTR; type[2] = expression type
  local t = vstruct.read("u1", string.sub(header, 1, 1))[1]
  local type = {math.floor(t/64)~=0, math.fmod(t, 64)}
  local len = vstruct.read("u3", string.sub(header, 2, 4))
  return({exptype = type[2], hasatts = type[1], explen = len[1]})
end


-------------------------------------
-- Parse a SEXP expression
-- @param sexp    binary encoded DT_SEXP
-------------------------------------
local function parsesexp(sexp)
  if #sexp < 4 then
    print("WARNING: Invalid SEXP (too short) - " .. #sexp)
    return nil
  end
  local sexpexps = {}
  local sexpcounter = 1 -- number of sexp expressions found
  local token = 1
  repeat
    -- get the header
    local header = getheader(string.sub(sexp, token, token + 3))
--print("+++++++++++++++++++"); print(QAP1_XPRESSIONTYPES[header.exptype]); printtable(header)
    token = token + 4 -- move token after the header
    local sexpend = token + header.explen - 1 -- final char of SEXP's content
    if header.hasatts then
      local attheader = getheader(string.sub(sexp, token, token + 3))
      local att = parsesexp(string.sub(sexp, token, token + attheader.explen + 3)) -- recursion -- get the whole inner SEXP, that is, header + content
      token = token + 4 + attheader.explen -- move token after the inner SEXP
      sexpexps[sexpcounter] = att
      sexpcounter = sexpcounter + 1
    end -- if header.hasatts
    -- get the content
    local content = string.sub(sexp, token, sexpend)
    token = sexpend + 1 -- move token to the first byte of the next sexp header
    -- local fmt = getFormat(header.exptype, #content) -- format for parsing binary data
    local data = ""
    if header.exptype == 0 then                               -- XT_NULL
      data = "XT_NULL"
    elseif header.exptype == 3 or header.exptype == 19 then -- XT_STR, XT_SYMNAME
      data = vstruct.read((#content-1) .. "*s", content)
      local str=data[1]
      data = ''
      if str then 
        for i = 1, #str do
	        if str:byte(i) >= 32 then data = data .. str:sub(i,i) end
        end
      end
	  elseif header.exptype == 16 or header.exptype == 21 or header.exptype == 23
        or header.exptype == 20 or header.exptype == 22 then  -- XT_VECTOR, XT_LIST_TAG, XT_LANG_TAG, XT_LIST_NOTAG, XT_LANG_NOTAG
      data = parsesexp(content)
    elseif header.exptype == 32 then                          -- XT_ARRAY_INT
      local len = #content / 4
      data = vstruct.read(len .. "*u4", content)
    elseif header.exptype == 33 then                          -- XT_ARRAY_DOUBLE
      local len = #content / 8
      data = vstruct.read(len .. "*f8", content)
    elseif header.exptype == 34 then                          -- XT_ARRAY_STR
      data = splitstring(content, string.char(0))
    elseif header.exptype == 36 then                          -- XT_ARRAY_BOOL
      local len = vstruct.read("u4", string.sub(content, 1, 4))[1]
      data = vstruct.read(len .. "*u1", string.sub(content, 5))
      for i=1, #data do 
        if data[i]==0 then 
          data[i]=false
        elseif data[i]==1 then 
          data[i]=true
        else
          data[i]=nil
        end
      end
    elseif header.exptype == 48 then                          -- XT_UNKNOWN
        data = "XT_UNKNOWN"
    else
      print("ERROR: unknown QAP1 expression type:" .. header.exptype)
      return nil
    end
    --print("---------------"); if data ~= nil then; print(#data); printtable(data); end; print("---------------")
    sexpexps[sexpcounter] = data
    sexpcounter = sexpcounter + 1
  until token > #sexp
  return(sexpexps)
end


-------------------------------------
-- Parse the server's id string
-- @param idstring  Server's id string
-------------------------------------
function rclient._parseids(idstring, rsserver, rsport)
  local rsid = string.sub(idstring, 1, 4)
  local rspver = string.sub(idstring, 5, 8)
  local rsp = string.sub(idstring, 9, 12)
  local rsatts = string.sub(idstring, 13)
  local server = {serverid = rsid, protocol = rsp, protversion = rspver,
                  attributes = rsatts, host = rsserver, port = rsport}
  return server
end


-------------------------------------
-- Get the version of this module
-------------------------------------
function rclient.version()
  return "rclient 0.2"
end

-------------------------------------
-- Send a message using TCP
-- @param rsserver  Server name or IP address
-- @param rsport    Port number
-- @param msg       Message to be sent
-------------------------------------
function rclient._calltcp(rsserver, rsport, msg, read)
  if read==nil then read = true end
  local tcp = socket.tcp()
  tcp:settimeout(30, 'b')
  tcp:connect(rsserver, rsport)
  if msg and msg ~= " " then
    tcp:send(msg)
  end
  
  local s, status, partial = tcp:receive(32)
  s = s or partial
  partial = nil
  
  if read and status~='timeout' then
    local s2, _, partial2 = tcp:receive(16)
    s2 = s2 or partial2
    s = s .. s2
    
    local r = '*a'

    if string.len(s2)==16 then
      local qmsgheader = vstruct.read("u4 u4 u4 u4", s2)
      r = qmsgheader[2] + 256 * 256 * 256 * 256 * qmsgheader[4]
    end
  
    if r~=0 then 
      local s3, _, partial3 = tcp:receive(r)
      s = s .. (s3 or partial3)
    end
  end
  tcp:close()
  return s, status, partial
end

-------------------------------------
-- Print Rserve metadata
-- @param rsserver  Server name or IP address
-- @param rsport    Port number
-------------------------------------
function rclient.getserverid(rsserver, rsport)
  if(#server == 0) then
    getserverdata(rsserver, rsport)
  end
  printserverdata()
end

-------------------------------------
-- Connect to R
-- @param rsserver  Optional server name or IP address
-- @param rsport    Optional port number
-------------------------------------
function rclient.connect(rserver, rsport)
  rserver = rserver or "localhost"
  rsport  = rsport  or 6311
  getserverdata(rserver, rsport)
end

-------------------------------------
-- Evaluate R expression
-- @param rexp An R expression
-- @param rsserver  Optional server name or IP address
-- @param rsport    Optional port number
-- @param command   Optional command, default 3 = evaluate
-------------------------------------
function rclient.evaluate(rexp, rsserver, rsport, command)
--  if command==2 then
--    print('-- R > ' .. rexp)
--  else
--    print('-- R < ' .. rexp)
--  end
  
  rsserver = rsserver or server.host
  rsport   = rsport   or tonumber(server.port)
  command  = command or 3
  
  local parameters = {}
  local msgbin = buildstrmsg(rexp, command)
  local s, _, partial = rclient._calltcp(rsserver, rsport, msgbin, true)
  local res = s or partial
  
  -- parse message
  if res==nil then
    return
  end
  
  local idstring = string.sub(res, 1, 32)
  local qmsg     = string.sub(res, 33)
  server = rclient._parseids(string.sub(idstring , 1 , 32), rsserver, rsport) -- updates the server metadata
  
  if string.len(res)<=32 then
    return
  end
  
  -- parse QAP1 message header
  local qmsgheader = vstruct.read("u4 u4 u4 u4", string.sub(qmsg, 1, 16))
      -- qmsgheader[1] = command
      -- qmsgheader[2] = lenght
      -- qmsgheader[3] = offset
      -- qmsgheader[4] = lenght2
 
  -- parse QAP1 data
  local qmsgdata = string.sub(qmsg, 17)
  local token = 1 -- track the byte being parsed
  local pcounter = 1 -- parameter counter
  
  if qmsgheader[2]==0 and qmsgheader[4]==0 then
    return {}
  end
    
  repeat
    -- parse the parameter's head
    local paramheader = vstruct.read("u1 u3", string.sub(qmsgdata, token, token + 3))
        -- paramheader[1] = type
        -- paramheader[2] = length
    -- parse the parameter's data
    token = token + 4 -- move token to the first byte of data
    local parambody = string.sub(qmsgdata, token, token + paramheader[2] - 1)
    if paramheader[1] == 1 then                               -- DT_INT
      parameters[pcounter] = vstruct.read("u4", parambody)    -- TODO: test
    elseif paramheader[1] == 3 then                           -- DT_DOUBLE
      parameters[pcounter] = vstruct.read("f8", parambody)    -- TODO: test
    elseif paramheader[1] == 2 or paramheader[1] == 4 then    -- DT_CHAR or DT_STRING
      parameters[pcounter] = vstruct.read("s", parambody)     -- TODO: test
    elseif paramheader[1] == 10 then                          -- DT_SEXP
      parameters[pcounter] = parsesexp(parambody)
    else
      print("ERROR: parameter type " .. paramheader[1] .. " not implemented")
    end
    token = token + paramheader[2] -- move token to the first byte of the next parameter header
    pcounter = pcounter + 1
  until token > qmsgheader[2]
  
  if #parameters==1 then
    parameters = parameters[1]
  end
  if #parameters==1 then
    parameters = parameters[1]
  end
    
  -- translate raw response into lua wrappers
  if type(parameters[1])=='table' then
    if parameters[1][1] then
      if parameters[1][1][2]=='dim' then
        local m = matrix:new()
        m:fromR(parameters)
        return m
      end
      
      if parameters[1][1][2]=='names' then
        local f = frame:new()
        f:fromR(parameters)
        return f
      end
      
      if parameters[1][1][2]=='levels' then
        local f = factor:new()
        f:fromR(parameters)
        return f
      end
    end
    
    local v = vector:new()
    v:fromR(parameters)
    return v
  end
  
  return parameters
end

function rclient.evaluateVoid(rexp, rsserver, rsport)
  rclient.evaluate(rexp, rsserver, rsport, 2)
end

function flatten(list)
  if type(list) ~= "table" then return {list} end
  local flat_list = {}
  for _, elem in ipairs(list) do
    for _, val in ipairs(flatten(elem)) do
      flat_list[#flat_list + 1] = val
    end
  end
  return flat_list
end

-------------------------------------
-- Implementation of () and [] interface
-------------------------------------

function rclient:_call(rexp)
  return rclient.evaluateVoid(rexp, server.host, server.port)
end

function rclient._fetch(t, key)
  return rclient.evaluate(key, server.host, server.port)
end

function rclient._set(t, key, value)
  -- fast method to store matrix (double/string)
  if value._class=='matrix' then
    rclient.storeArray('rclient_t0', flatten(value.contents))
    return rclient.evaluateVoid(string.format('%s <- array(rclient_t0, dim=c(%s)); rm(rclient_t0)', 
        key, table.concat(value.dims, ',')))
  -- normal method
  elseif value.toR then
    return rclient.evaluateVoid(value:toR(key), server.host, server.port, 2)
  else
    return rclient.evaluateVoid(vector:new(value):toR(key), server.host, server.port, 2)
  end
end

setmetatable(rclient, {__call=rclient._call, __index=rclient._fetch, __newindex=rclient._set})
return rclient
