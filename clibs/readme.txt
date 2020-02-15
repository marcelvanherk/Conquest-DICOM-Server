Place lua external libraries here such as IUP and CD for use within the DICOM server.
Only work correctly with a dgate.exe that uses an external lua5.1.dll (64 bits for now)
iuplua51.dll and iup.dll are required to run console.wlua and notepad.wlua

For 64 bits windows use:

cd-5.4.1_Win64_dll8_lib.zip
iup-3.5_Win64_dll8_lib.zip
lua-5.1.4_Win64_dll10_lib.zip
iup, iuplua51, socket/core as provided

For 32 bits windows use:

cd-5.4.1_Win32_dll8_lib.zip
iup-3.5_Win32_dll8_lib.zip
lua-5.1.4_Win32_dll10_lib.zip
socket/core.dll not required (built-in)
