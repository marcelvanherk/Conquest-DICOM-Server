-- mvh 20180113 Added worldmatchmode and conquestmode

local G = ...
local id7 = G.ID("wmatchmenu.conquestmode")
local menuid

local function action(a)
  local id = G.ID(a)
  local commandEvent = wx.wxCommandEvent(wx.wxEVT_COMMAND_MENU_SELECTED, id)
  wx.wxPostEvent(ide:GetMainFrame(), commandEvent)
end

return {
  name = "Conquest starter plugin",
  description = "Select conquest interpreter",
  author = "Marcel van Herk",

  onRegister = function(self)
    local menu = ide:GetMenuBar():GetMenu(ide:GetMenuBar():FindMenu(TR("&Project")))

    menuid = menu:AppendSeparator()
    menuid = menu:Append(id7, "Conquest mode\tAlt-C")
    ide:GetMainFrame():Connect(id7, wx.wxEVT_COMMAND_MENU_SELECTED,
      function (event)
        action("debug.interpreter.conquest_auto")
      end)
  end,

  onUnRegister = function(self)
    local menu = ide:GetMenuBar():GetMenu(ide:GetMenuBar():FindMenu(TR("&Project")))
    ide:GetMainFrame():Disconnect(id, wx.wxID_ANY, wx.wxID_ANY)
    if menuid then menu:Destroy(menuid) end
  end,
}
