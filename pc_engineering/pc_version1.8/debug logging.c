
version discription:
(version 1.2)could write framebuffer correctlly ,then the next
(version 1.3) will make the code Algorithm,we need separate rgb

	through last version,we found that the framebufer was not char ones
version 1.2 was put a white zone
version 1.3 was put a green line 
    so next version we need to make clear what would we write in framebuffer
    char;;  
	long char??  
	int???
version 1.4 yes it/*'s ok we finally make it correctly 
	         the last problem about how long should we put in 
	         fb_addr,the length depends on our definiton 
	         (int *)fb_addr,then we can put int data to fb_addr
	         the forover stubbon is char.

	         in this version we also write some "printf" function 
	         watch the codebuff ,it's so comples so next day 
	         tommorrow it will be work*/
version 1.5 Up to now, was finally able to show correct information in the correct 
 			location on the screen(framebuffer), it went through several major mistakes,
 			
 			first one is on the understanding of a pointer type, 
 			it is up to us to define, what I say, 
 			and libraries usually know only the char type, 
 			so it needs our calculations.

	 		second one is  How to locate the wrong place,
	 		knows exactly which statement is wrong, 
	 		we always considered encoding problems at first, 
	 		then modify the encoding function,
	 		modify the send and receive functions, 
	 		it does not mean that this assumption wrong,
	 		there is an assumption that you should do is go to judge this right,
	 		rather than to solve the hypothesis like a files with no head.
version 1.6 
version 1.7 The difference between the two versions lies in coding,
            at last we  used the structures to make the logic clearer
version 1.8 in the last version ,the picture was too red ,the problem maybe the camera 
            format was rgb555,so 

	
