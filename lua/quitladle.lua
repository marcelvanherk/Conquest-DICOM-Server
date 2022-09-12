-- mvh 20180204: send request to stop Ladle webserver
-- mvh 20220912: changed address of quit command
local socket = require("socket")
client = socket.connect(ip or "127.0.0.1", port or 8086)
if client then
  client:send("GET /app/newweb/dgate.exe?mode=quit HTTP/1.1\r\n\r\n")
  client:close()
end
