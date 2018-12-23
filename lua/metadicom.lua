require('niftyutil')

Data:Read('c:\\data\\t.dcm')
readdicom('c:\\data\\t.dcm')
--print(1)

a = DicomObject:new()
a:Read([[c:\users\marcel\DeskTop\1.dcm]])

function printdicom(a, level)
  level = level or 1
  local names,types,groups,elements = a:ListItems()
  names = split(names, '|')
  types = split(types, '|')
  groups = split(groups, '|')
  elements = split(elements, '|')
  for k,v in ipairs(names) do
    if elements[k]~="0" then
      if v=='' then v=string.format('%04x,%04x',0+groups[k],0+elements[k]) end
      print (string.rep('>', level-1)..' '..k..' '..(types[k] or 'UN')..' '..v..' '..tostring(a[v]))
      if types[k]=='SQ' then
        print(string.rep('>', level)..'-----------')
        for i=0, #a[v]-1 do
          printdicom(a[v][i], level+1)
          print(string.rep('>', level)..'-----------')
        end
      end
    end
  end
end

function comparedicom(a, b)
  local names,types,groups,elements = a:ListItems()
  names = split(names, '|')
  types = split(types, '|')
  groups = split(groups, '|')
  elements = split(elements, '|')
  for k,v in ipairs(names) do
    if elements[k]~=0 then
      if types[k]=='SQ' then
        for i=0, #a[v]-1 do
          if comparedicom(a[v][i], b[v][i])==false then
            return false
          end
        end
      else
        c = a:GetVR(groups[k], elements[k])
        d = b:GetVR(groups[k], elements[k])
        if type(c)=='table' and type(d)=='table' then
          if #c~=#d then
            return false
          end
          for k,v in ipairs(c) do
            if c[k]~=d[k] then
              return false
            end
          end
        elseif c ~= d then
          return false
        end
      end
    end
  end
  return true
end

function copydicom(a, b)
  b = b or DicomObject:new()
  local names,types,groups,elements = a:ListItems()
  names = split(names, '|')
  types = split(types, '|')
  groups = split(groups, '|')
  elements = split(elements, '|')
  for k,v in ipairs(names) do
    if elements[k]~=0 then
      if types[k]=='SQ' then
        local ar = newdicomarray()
        local j=0
        for i=0, #a[v]-1 do
          ar[i] = {}
          copydicom(a[v][i], ar[j])
          j=j+1
        end
        b:SetVR(groups[k], elements[k], ar)
      else
        b:SetVR(groups[k], elements[k], a:GetVR(groups[k],  elements[k]))
      end
    end
  end
  return b
end

function deleteitemfromarray(a, name, n)
  local ar=newdicomarray()
  local j=0
  for i=0,#a[name]-1 do
    if i~=n then
      ar[j] = {}
      copydicom(a[name][i], ar[j])
      j=j+1
    end
  end
  local g, e = dictionary(name)
  a:SetVR(g, e, ar);
end

-- printdicom(a)
--printdicom(copydicom(a))
b = copydicom(a)
c = a:Copy()
print(comparedicom(b,c))
print(b.ROIContourSequence[1].ContourSequence)
--deleteitemfromarray(b.ROIContourSequence[1], "ContourSequence", 0)
--b.ROIContourSequence[1]:DeleteFromSequence("ContourSequence", 0)
b.ROIContourSequence:Delete(0)
print(b.ROIContourSequence[1].ContourSequence)
print(comparedicom(a,b))
printdicom(b)
  