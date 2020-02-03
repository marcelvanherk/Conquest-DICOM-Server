-- sample application adapted to have a persistent history; Marcel van Herk 20120104
-- added ctrl-D and ctrl-S and command by number; Marcel van Herk 20120125
-- Allow single instance only; Marcel van Herk 20120128
-- Just type expression to print it as in ilua; Marcel van Herk 20120202
-- Avoid this option prints nil after e.g. print(1); Marcel van Herk 20120226
-- Save history before line executed 20161204

require ("iuplua")
require ("persistence")

if console==nil then
	console = {}

	console.prompt = iup.text{expand="Horizontal", dragdrop = "Yes"}
	console.output = iup.text{expand="Yes",
			  readonly="Yes",
			  bgcolor="232 232 232",
			  font = "Courier, 11",
			  appendnewline = "No",
			  multiline = "Yes"}

	console.prompt.tip = "Enter - show variable or execute Lua command\n"..
			     "Esc - clears the command\n"..
			     "Ctrl+Del - clears the output\n"..
			     "Ctrl+O - selects a file and execute it\n"..
			     "Ctrl+D - clears command from history\n"..
			     "Ctrl+S - search command(s) in history\n"..
			     "Number - execute command from history\n"..
			     "Up Arrow - shows the previous command in history\n"..
			     "Down Arrow - shows the next command in history\n"..
			     "Drop files here to execute them"

	console.orig_output = io.output
	console.orig_write = io.write
	console.orig_print = print

	console.cmd_list = persistence.load('history.lua');

	function io.output(filename)
	  console.orig_output(filename)
	  if (filename) then
	    io.write = console.orig_write
	  else
	    io.write = console.new_write
	  end
	end

	function console.new_write(...)
	  -- Try to simulate the same behavior of the standard io.write
	  local arg = {...}
	  local str -- allow to print a nil value
	  for i,v in ipairs(arg) do
	    if (str) then
	      str = str .. tostring(v)
	    else
	      str = tostring(v)
	    end
	  end
	  console.print2output(str, true)
	end
	io.write = console.new_write

	function print(...)
	  -- Try to simulate the same behavior of the standard print
	  local arg = {...}
	  local str -- allow to print a nil value
	  for i,v in ipairs(arg) do
	    if (i > 1) then
	      str = str .. "\t"  -- only add Tab for more than 1 parameters
	    end
	    if (str) then
	      str = str .. tostring(v)
	    else
	      str = tostring(v)
	    end
	  end
	  console.print2output(str)
	end

	function nprint(...)
	  local arg = {...}
	  if arg[1]~=nil then
	    print(...)
	  end
	end

	function console.print2output(s, no_newline)
	  if (no_newline) then
	    console.output.append = tostring(s)
	    console.no_newline = no_newline
	  else
	    if (console.no_newline) then
	      -- if io.write was called, then a print is called, must add a new line before
	      console.output.append = "\n" .. tostring(s) .. "\n"
	      console.no_newline = nil
	    else
	      console.output.append = tostring(s) .. "\n"
	    end
	  end
	end

	function console.print_command(cmd)
	  console.add_command(cmd)
	  console.print2output("> " .. cmd)
	end

	function  console.add_command(cmd)
	  console.cmd_index = nil
	  if (not console.cmd_list) then
	    console.cmd_list = {}
	  end
	  local n = #(console.cmd_list)
	  console.cmd_list[n+1] = cmd
	end

	function  console.prev_command()
	  if (not console.cmd_list) then
	    return
	  end
	  if (not console.cmd_index) then
	    console.cmd_index = #(console.cmd_list)
	  else
	    console.cmd_index = console.cmd_index - 1
	    if (console.cmd_index == 0) then
	      console.cmd_index = 1
	    end
	  end
	  console.prompt.value = console.cmd_list[console.cmd_index]
	end

	function  console.next_command()
	  if (not console.cmd_list) then
	    return
	  end
	  if (not console.cmd_index) then
	    return
	  else
	    console.cmd_index = console.cmd_index + 1
	    local n = #(console.cmd_list)
	    if (console.cmd_index == n+1) then
	      console.cmd_index = n
	    end
	  end
	  console.prompt.value = console.cmd_list[console.cmd_index]
	end

	function console.do_file(filename)
	  local cmd = 'dofile(' .. string.format('%q', filename) .. ')'
	  console.print_command(cmd)
	  dofile(filename)
	end

	function console.do_string(cmd)
	  assert(loadstring(cmd))()
	end

	function console.open_file()
	  local fd=iup.filedlg{dialogtype="OPEN", title="Load File",
			       nochangedir="NO", directory=console.last_directory,
			       filter="*.*", filterinfo="All Files", allownew="NO"}
	  fd:popup(iup.CENTER, iup.CENTER)
	  local status = fd.status
	  local filename = fd.value
	  console.last_directory = fd.directory -- save the previous directory
	  fd:destroy()

	  if (status == "-1") or (status == "1") then
	    if (status == "1") then
	      error ("Cannot load file: "..filename)
	    end
	  else
	    console.do_file(filename)
	  end
	end

	function console.prompt:dropfiles_cb(filename)
	  -- will execute all dropped files, can drop more than one at once
	  -- works in Windows and in Linux
	  console.do_file(filename)
	end

	function console.prompt:k_any(key)
	  if (key == iup.K_CR) then  -- Enter executes the string
	    if tonumber(self.value) then
	      console.print2output("> " .. console.cmd_list[tonumber(self.value)])
	      assert(loadstring(console.cmd_list[tonumber(self.value)]))()
	    else
              local p,err = loadstring('nprint('..self.value..')','local')
	      if err then
	        console.print_command(self.value)
   	        persistence.store('history.lua', console.cmd_list);
   	        console.do_string(self.value)
	      else
	        console.print_command(self.value)
   	        persistence.store('history.lua', console.cmd_list);
   	        console.do_string('nprint('..self.value..')')
	      end
	    end
	    self.value = ""
	  end
	  if (key == iup.K_ESC) then  -- Esc clears console.prompt
	    self.value = ""
	  end
	  if (key == iup.K_cO) then  -- Ctrl+O selects a file and execute it
	    console.open_file()
	  end
	  -- if (key == iup.K_cX) then  -- Ctrl+X exits the console (disabled, crashes WM)
	  -- console.dialog:close_cb()
	  -- end
	  if (key == iup.K_cDEL) then  -- Ctrl+Del clears console.output
	    console.output.value = ""
	  end
	  if (key == iup.K_cD) then  -- Ctrl+D clears console.output
	    table.remove(console.cmd_list, console.cmd_index);
	    self.value = ""
	  end
	  if (key == iup.K_cS) then  -- Ctrl+S search command
	    local found, k, v;
	    for k,v in ipairs(console.cmd_list) do
		  if string.match(v, self.value) then
		print(k, v);
		    console.cmd_index = k;
			found = true;
		  end;
		end
		if found then
	      console.prev_command()
	      console.next_command()
		else
	      print('-- not found -- ');
		end
	  end
	  if (key == iup.K_UP) then  -- Up Arrow - shows the previous command in history
	    console.prev_command()
	  end
	  if (key == iup.K_DOWN) then  -- Down Arrow - shows the next command in history
	    console.next_command()
	  end
	end

	console.dialog = iup.dialog
	{
	  iup.vbox
	  {
	    iup.frame
	    {
	      iup.hbox -- use it to inherit margins
	      {
		console.prompt,
	      },
	      title = "Command:",
	    },
	    iup.frame
	    {
	      iup.hbox -- use it to inherit margins
	      {
		console.output
	      },
	      title = "Output:",
	    },
	    margin = "5x5",
	    gap = "5",
	  },
	  title="Lua Console with history",
	  size="500x360", -- initial size
	  icon=0, -- use the Lua icon from the executable in Windows
	}

	function console.dialog:close_cb()
	  print = console.orig_print  -- restore print and io.write
	  io.write = console.orig_write
	  iup.ExitLoop()  -- should be removed if used inside a bigger application
	  persistence.store('history.lua', console.cmd_list);
	  console.dialog:destroy()
	  console=nil
	  return iup.IGNORE
	end

	function console.version_info()
	  print(_VERSION, _COPYRIGHT) -- _COPYRIGHT does not exists by default, but it could...

	  print("IUP " .. iup._VERSION .. "  " .. iup._COPYRIGHT)
	  print("  System: " .. iup.GetGlobal("SYSTEM"))
	  print("  System Version: " .. iup.GetGlobal("SYSTEMVERSION"))

	  local mot = iup.GetGlobal("MOTIFVERSION")
	  if (mot) then print("  Motif Version: ", mot) end

	  local gtk = iup.GetGlobal("GTKVERSION")
	  if (gtk) then print("  GTK Version: ", gtk) end
	end

	console.dialog:show()
	console.dialog.size = nil -- reset initial size, allow resize to smaller values
	iup.SetFocus(console.prompt)

	console.version_info()

	if (iup.MainLoopLevel() == 0) then
	  iup.MainLoop()
	end
else
	iup.SetFocus(console.prompt)
end
