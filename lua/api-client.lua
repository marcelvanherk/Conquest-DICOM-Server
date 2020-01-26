-- -*- coding: utf-8 -*-
--
-- HTTP API Client.
--
-- Copyright 2014 Alexander Tsirel
-- http://noma4i.com
--
-- This code is released under a Apache License Version 2.0:
-- https://www.apache.org/licenses/LICENSE-2.0.txt


http = require("socket.http")
--https = require("ssl.https")
json = (loadfile "lua/JSON.lua")()
--loadfile ("lua/Utils.lua")()

function api_call(...)
  local args = {...}
  local reqbody = args[3] or ""

  local respbody = {}

  http.request {
  	method = args[1],
  	url = args[2],
  	source = ltn12.source.string(reqbody),
  	headers = {
  		["Content-Type"] = "application/x-www-form-urlencoded",
  		["Content-Length"] = #reqbody
  	},
  	sink = ltn12.sink.table(respbody)
  }

  return json:decode(table.concat(respbody))
end

function api_callraw(...)
  local args = {...}
  local reqbody = args[3] or ""

  local respbody = {}

  http.request {
  	method = args[1],
  	url = args[2],
  	source = ltn12.source.string(reqbody),
  	headers = {
  		["Content-Type"] = "application/x-www-form-urlencoded",
  		["Content-Length"] = #reqbody
  	},
  	sink = ltn12.sink.table(respbody)
  }

  return table.concat(respbody)
end
