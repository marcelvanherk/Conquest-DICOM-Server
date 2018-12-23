    inputFile = 'list1.txt'
    srcAet = 'ABC'
    destAet = 'XYZ'
   
    for line in io.lines(inputFile) do
       ptId = line

       cmd = newdicomobject()
       cmd.QueryRetrieveLevel = 'PATIENT'
       cmd.PatientID = ptId
             
       dicommove(srcAet, destAet, cmd);
    end
