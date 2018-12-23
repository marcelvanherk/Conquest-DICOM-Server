-- function readCSV(filename, itemnames)
-- read CSV file if itemnames is set, use the first line as item name
-- returns table[1..nlines] of tables with items, 
-- and returns table with ordered heading (needed to rewrite)

-- function writeCSV(t, filename, heads)
-- write table t as CSV file; if heads passed use heads as first line and
-- each row in t contains names items; if not it contains numbered items

-- mvh 20151126 Fix for tables with empty header cells
-- mvh 20151130 Fix for tables with booleans

function escapeCSV (s)
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
      table.insert(t, (string.gsub(f, '""', '"')))
      fieldstart = string.find(s, ',', i) + 1
    else                -- unquoted; find next comma
      local nexti = string.find(s, ',', fieldstart)
      table.insert(t, string.sub(s, fieldstart, nexti-1) or '')
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
  local t = {}, heads
  f = io.open(filename, 'rt')
  for s in f:lines(s) do 
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
          t[#t][heads[k]] = v
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
