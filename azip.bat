del ..\install\dicomserver1500x.zip
7za.exe -r -xr!az*.bat -xr!au*.bat -xr!obsolete -xr!.git* -xr!doc -xr!docker\prebuild\dg* -xr!win32 -xr!inholland -x!process_inholland.lua -xr!__history -xr!*.py -xr!ohiftop a ..\install\dicomserver1500x.zip *.* *
