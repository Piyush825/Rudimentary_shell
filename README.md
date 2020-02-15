
****************************************************************
> This repository contains code for a rudimentary shell program.
> The shell program is written assuming a client-server model.

* **General Instructions** :
    * `make`
    * `./thread_fork` must be running continuously.`
    * `./file_client` Each client would need to run the client binary to access the rudementary shell.` 

****************************************************************
*WORKING AND SUPPORTED COMMANDS :* 

After the succesfull connection of client with server, server prompts for user name (which is a rudimentory form of authentication). It can be easilt extended to password based authentication.

After succesfull authentication, client by default is in its user_name directory. For eg. if user logins with u1 (as user name), by default it would be in u1 directory.

(Currently there are 5 clients in the system which can be extended to any number n)

1. Client can execute ONLY the following commands
   a. ls <Directory_Path (absolute/relative)> 
         output: files and directories along with their owner and users
   
   b. cd <Directory_Path (absolute/relative)> 
         output: Message displayed to client, "Directoty changed" and in actual also directory gets changed.
		 Mandatory Condition: User initiating the cd command must either be the owner of directory or be a part of the group of the directory.
   
   c. create_dir <Directory_Path (absolute/relative)> 
         Output: Message displayed to client, "Directoty Created" and in actual also directory gets created into the file system.
		 Mandatory Condition: The directory in which the user wants to create the new directory, it must either be the owner of the directory or must be 
		 enlisted in group of the directory.
	
	d. fput (is implemented in two variants)
         Variant 1: (if the file is created for the first time)
		 fput <File_Name> <user name> <group name>
		 Output: Message displayed to client, "File Created" and in actual also File gets created into the file system with specified user and group name.
		 Mandatory Condition: (i)User initiating the fput command must be the owner of directory. (ii) User must do cd to the desired directory for file creation
		 Variant 2: (if file is already created)
		 fput <File_Name> <content to be written into the file>
		 Output: Message displayed to client, "Content Written".
		 Mandatory Condition: User initiating the fput command must be the owner of file. (ii) User must do cd to the desired directory 
		 (containing the file on which write operation has to be done)
		  
	e. fget < File_Path (absolute/relative)> 
	    Output: Content written in the file are displayed to client.
        Mandatory Condition: User initiating the fget command must be the owner of file or be a part of the group of the file. (ii) User must do cd 
		to the desired directory (containing the file on whhich read operation has to be done)
		

2. Any communication message between client and server can be max 256 bytes. 

3. Each directory contains three metadata files.
    a) dir_info: contains the list of files present in the directory along with their owner and user names.		
    b) allowed_users: conatins the list of the users who are allowed to acces the content of the directory (including owner of the directory).
    c) owner: contains the owner and group name of the directory
	
4. /simple_home directory contains the group files, viz., if there are three groups g1, g2 and g3: there would be three files with same names as of groups respectively.
    Each file contains the list of users which are part of the group.

