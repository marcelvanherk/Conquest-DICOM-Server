-- drop in replacement for dgate built-in 'slice' mode for pure lua webservers
-- mvh 20180124
-- mvh 20180202 delete output file and gracefully fail if not created
-- mvh 20180203 Added local to f
local graphic = CGI('graphic', 'jpg')
local lw = CGI('lw', '0\0\0')
local size = CGI('size', '512')
local slice = CGI('slice', '')
local tempname = 'a.out' -- string.gsub(slice, '.-:', '')..'.jpg'
newdicomobject():Script('rm '..tempname)
servercommand(string.format("convert_to_%s:%s,%s,%s,%s", graphic, slice, size, tempname, lw))
HTML('')
local f=io.open(tempname, 'rb')
if f then
  write(f:read('*all'))
  f:close()
end
