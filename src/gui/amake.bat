if exist "C:\Program Files (x86)\Embarcadero\Studio\16.0" (call "C:\Program Files (x86)\Embarcadero\Studio\16.0\bin\rsvars.bat")
if exist "C:\Program Files (x86)\Embarcadero\Studio\18.0" (call "C:\Program Files (x86)\Embarcadero\Studio\18.0\bin\rsvars.bat")
if exist "C:\Program Files (x86)\Embarcadero\Studio\19.0" (call "C:\Program Files (x86)\Embarcadero\Studio\19.0\bin\rsvars.bat")

msbuild \software\dicomserver\gui\ConquestDICOMServer.dproj /p:config=Release /p:platform=Win32
