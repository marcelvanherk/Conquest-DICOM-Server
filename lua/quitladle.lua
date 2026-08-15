-- mvh 20180204: send request to stop Ladle webserver
-- mvh 20220912: changed address of quit command
-- mvh 20260812: use ladleport
local socket = require("socket")
client = socket.connect(ip or "127.0.0.1", Global.ladleport or 8086)
if client then
  client:send("GET /app/newweb/dgate.exe?mode=quit HTTP/1.1\r\n\r\n")
  client:close()
end
