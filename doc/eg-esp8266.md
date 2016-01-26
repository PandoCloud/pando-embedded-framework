###PANDO框架移植至ESP8266平台
#1, 从github下载tisan_demo工程文件包,目录如下 
---app(框架植入的位置) 

---bin(编译生成的固件) 

---include(乐鑫库文件) 

---ld    (编译文件) 

---lib 

---tools (编译工具)  


#2, 将pando_embedded_framework放在app的目录下 
 
	---app 
	  
	  	--pando 
	  	  
	  		--example 
	  		
	  			--esp8266 
	  			
	  				--peripheral 
	  				
	  				 --user 
	  				 
	  				 --util 
	  				 
	  				 makefile
	  				
   					  
	---bin  
	 
	---include   
	
	---ld  
	
	---lib   
	
	---tools 
	 
   	
#3, 将 目录 app-pando-example-esp8266下的 peripheral，user，util，makefile 复制粘贴到app的根目录下，与pando同级，如下图所示  
	---app   
			--pando（网关）  
			 
					--example 
					 
							--esp8266  
							
									--peripheral  
									
									--user  
									
									--util  
									
									makefile  
									 
					--framework 
					 
							--gateway  
							
							--lib  
							
							--platform 
								 
							--protocol  
							
							--subdevice   
							
							makefile  
							
							pando_framework  
							
	   	 --peripheral（硬件驱动）  
	   	 
	  	 --user		（程序入口）  
	  	  
	   	 --util		（工具函数）  
	   	 
	    	 makefile  
	    	
	---bin  
	
	---include  
	
	---ld  
	
	---lib  
	
	---tools   
	

至此pando框架在esp8266平台移植成功。


	    
		
		
		
									


			
