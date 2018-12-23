local t = dbquery('UIDMODS','DISTINCT Stage, OldUID, NewUID', "MODType='PatientID'")
print(#t)
for i=1,#t do
  print(t[i])
end
