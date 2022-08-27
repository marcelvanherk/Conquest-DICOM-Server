  -- list all predefined web client variables
  -- for development and debugging web scripts
  -- 20181227 mvh Created
  
  HTML('Content-type: text/html\n\n');
  print(
  [[
  <HEAD><TITLE>Version ]].. (version or '-')..[[ WEB interface</TITLE></HEAD>
  <BODY BGCOLOR='FFDFCF'>
  <H3>Conquest ]].. (version or '-') ..[[ WEB interface</H3>
  ]])


HTML("query_string=%s<br>"    ,query_string)
HTML("server_name=%s<br>"     ,server_name)    
HTML("script_name=%s<br>"     ,script_name)     
HTML("path_translated=%s<br>" ,path_translated)
HTML("port=%s<br>"            ,port)
HTML("address=%s<br>"         ,address)
HTML("webcodebase=%s<br>"     ,webcodebase)     
HTML("webscriptadress=%s<br>" ,webscriptadress) 
HTML("webscriptaddress=%s<br>",webscriptaddress)
HTML("extra=%s<br>"           ,extra)           
HTML("version=%s<br>"         ,version)         
HTML("mode=%s<br>"            ,mode)            
HTML("uploadedfile=%s<br>"    ,uploadedfile)    
HTML("<br>")
HTML("series=%s<br>"          ,series)          
HTML("series2=%s<br>"         ,series2)         
HTML("slice=%s<br>"           ,slice)           
HTML("slice2=%s<br>"          ,slice2)          
HTML("study=%s<br>"           ,study)          
HTML("study2=%s<br>"          ,study2)          
HTML("patid=%s<br>"           ,patid)          
HTML("patid2=%s<br>"          ,patid2)          
HTML("seruid=%s<br>"          ,seruid)          
HTML("studyuid=%s<br>"        ,studyuid)        
HTML("sopuid=%s<br>"          ,sopuid)          
HTML("<br>")
HTML("size=%s<br>"            ,size)            
HTML("dsize=%s<br>"           ,dsize)           
HTML("compress=%s<br>"        ,compress)        
HTML("iconsize=%s<br>"        ,iconsize)        
HTML("graphic=%s<br>"         ,graphic)        
HTML("viewer=%s<br>"          ,viewer)          
HTML("lw=%s<br>"              ,lw)              
HTML("query=%s<br>"           ,query)           
HTML("db=%s<br>"              ,db)              
HTML("source=%s<br>"          ,source)          
HTML("dest=%s<br>"            ,dest)            
HTML("patientidmatch=%s<br>"  ,patientidmatch)  
HTML("patientnamematch=%s<br>",patientnamematch)
HTML("studydatematch=%s<br>"  ,studydatematch)  
HTML("startdatematch=%s<br>"  ,startdatematch)  

HTML([[  </BODY>
  ]])
