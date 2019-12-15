loadfile ("lua/api-client.lua")()
-- Usage :
-- api_call(
--   method,
--   url,
--   params
-- )
--
-- Where:
-- method is GET, POST, PUT etc
-- url is your api endpoint
-- params is a strign which contains all data you want to pass in the format:
--  "key1=value1;key2=value2"
--

result = api_call(
  "GET",
  --"http://127.0.0.1/wb/db.php/slides/1",
  "http://127.0.0.1/cgi-bin/newweb/dgate.exe?mode=start",
  "apikey=ABCD"
)

print(result)
