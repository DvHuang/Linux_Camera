target:
	use this  program you could receive data from the board and displat them 
	on the framebuffer.
version discription:
    (version 1.2)could write framebuffer correctlly ,then the next
    one(version 1.3) will make the code Algorithm,we need separate rgb

	through last version,we found that the framebufer was not char ones
    the version 1.2 was put a white zone
	the version 1.3 was put a green line 
    so next version we need to make clear what would we write in framebuffer
    char;;  
	long char??  
	int???
	version 1.4 yes it's ok we finally make it correctly 
	         the last problem about how long should we put in 
	         fb_addr,the length depends on our definiton 
	         (int *)fb_addr,then we can put int data to fb_addr
	         the forover stubbon is char.

	         in this version we also write some "printf" function 
	         watch the codebuff ,it's so comples so next day 
	         tommorrow it will be work

	
