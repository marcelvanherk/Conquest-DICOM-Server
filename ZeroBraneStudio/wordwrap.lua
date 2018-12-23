local G = ...
local id = G.ID("wordwrapmenu.wordwrapmenu")
local menuid
return {
  name = "Wordwrap plugin",
  description = "Adds word wrap option to the menu.",
  author = "Paul Kulchenko",

  onRegister = function(self)
    local menu = ide:GetMenuBar():GetMenu(ide:GetMenuBar():FindMenu(TR("&Edit")))
    menuid = menu:AppendCheckItem(id, "WordWrap\tAlt-W")
    ide:GetMainFrame():Connect(id, wx.wxEVT_COMMAND_MENU_SELECTED,
      function (event)
        local wrap = event:IsChecked() and wxstc.wxSTC_WRAP_WORD or wxstc.wxSTC_WRAP_NONE
        ide:GetEditor():SetWrapMode(wrap) end)
    ide:GetMainFrame():Connect(id, wx.wxEVT_UPDATE_UI,
      function (event)
        event:Check(ide:GetEditor():GetWrapMode() ~= wxstc.wxSTC_WRAP_NONE) end)
  end,

  onUnRegister = function(self)
    local menu = ide:GetMenuBar():GetMenu(ide:GetMenuBar():FindMenu(TR("&Edit")))
    ide:GetMainFrame():Disconnect(id, wx.wxID_ANY, wx.wxID_ANY)
    if menuid then menu:Destroy(menuid) end
  end,
}
