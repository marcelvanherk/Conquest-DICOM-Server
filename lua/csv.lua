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
-- mvh 20170524 Note: readCSV(file, false) does not return unstructured table
-- mvh 20180105 Fix reading unstructured CSV file
-- mvh+amw 20180209 Fix reading structured CSV file

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
  if itemnames==nil then itemnames=true end
  local t = {}
  local heads
  local f = io.open(filename, 'rt')
  for s in f:lines() do 
    if itemnames==true then
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
		    v = tonumber(v)
		  end
          if heads then
            t[#t][heads[k]] = v
          else
            t[#t][k] = v
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
  for k, v in ipairs(t) do
    if heads then
      local w = {}
      for k2,v2 in ipairs(heads) do
        w[#w+1]=v[v2] or ''
      end
      f:write(toCSV(w)..'\n')
    else
      f:write(toCSV(v)..'\n')
    end
  end
  f:close()
end
