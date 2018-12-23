if Data.PhotometricInterpretation~='RGB' then
  return
end
 
function scramble(x1, x2, y1, y2)
 local s=1
 if Data.PhotometricInterpretation=='RGB' then s=3 end
  for y=y1, y2 do
    local row = Data:GetRow(y)
    for x=x1*s, x2*s do
      row[x] = row[math.random(x2-x1)]
    end
    Data:SetRow(y, 0, row)
  end
end  

if Data.Modality=='NM' and string.find(Data.SeriesDescription, "SAVE_SCREEN") then
  scramble(1, math.floor(Data.Columns*0.8), 1, 50)
  -- script('destroy')
end
