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

debug 1.
    同一个文件中的不同函数，之间的变量是静态局部的应该，这个静态局部是对文件内部可见的，单个文件
    可以看做是静态局部域，而extern可以看作全局域
    The different functions in the same file, 
    between the variable is a static local, 
    static local are visible inside of the file,
    a single file Local domain can be seen as static and 
    extern can be thought of as global domain
debug 2.
	