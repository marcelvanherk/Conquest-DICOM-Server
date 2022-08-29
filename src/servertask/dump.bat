cd %~dp0
servertask "--lua:x=DicomObject:new()x:Read([[%1]]);x:Dump([[%1.txt]]);os.execute[[notepad %1.txt]];os.remove[[%1.txt]]"
