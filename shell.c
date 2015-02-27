#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <strings.h>
#include <readline/readline.h>
#include <readline/history.h>
#include "parse.h"   /*include declarations for parse-related structs*/

enum BUILTIN_COMMANDS { NO_SUCH_BUILTIN=0, EXIT,JOBS,CD,PWD,LS,RMDIR,MKDIR,RM};
 
char *buildPrompt()
{
  return  "[SS-05]-[ss-35]>>";
}
 
int isBuiltInCommand(char * cmd){
  
  if ( strncmp(cmd, "exit", strlen("exit")) == 0){
    return EXIT;
  }
  if ( strncmp(cmd, "cd" , strlen("cd")) == 0 ){
    return CD;
  }
  if ( strncmp(cmd, "pwd" , strlen("pwd")) == 0 ){
    return PWD;
  }
  if ( strncmp(cmd, "ls" , strlen("ls")) == 0 ){
    return LS;
  }
  if ( strncmp(cmd, "rmdir" , strlen("rmdir")) == 0 ){
    return RMDIR;
  }
  if ( strncmp(cmd, "mkdir" , strlen("mkdir")) == 0 ){
    return MKDIR;
  }
  if ( strncmp(cmd, "rm" , strlen("rm")) == 0 ){
    return RM;
  }
 
  return NO_SUCH_BUILTIN;
}

/*
argc refers to the number of command line arguments passed in, which includes the actual name of the program, as invoked by the user.  
argv contains the actual arguments, starting with index 1. Index 0 is the program name.
So, if you ran your program like this:
./program hello world
Then:
argc would be 3.
argv[0] would be "./program".
argv[1] would be "hello".
argv[2] would be "world".
*/
int main (int argc, char **argv)
{

  
  char * cmdLine;
  parseInfo *info; /*info stores all the information returned by parser.*/
  struct commandType *com; /*com stores command name and Arg list for one command.*/
  int infi,outf,backg,append,pipe_var;
  char inputfile[FILE_MAX_SIZE];
  char outputfile[FILE_MAX_SIZE];
   
  fprintf(stdout, "-------This is 1st OS_LAB project by Shrestha Garg and Divya Nagar UNIX version------\n");
  fprintf(stdout, "###############---Welcome to our own Unix Shell press EXIT to exit.---############### \n");



  while(1){

    /*insert your code to print prompt here*/
   
/*
The function readline () prints a prompt and then reads and returns a single line of text from the user. 
The line readline returns is allocated with malloc (); you should free () the line when you are done with it.
If readline encounters an EOF while reading the line, and the line is empty at that point, then (char *)NULL is returned. 
Otherwise, the line is ended just as if a newline had been typed.
*/
  


    cmdLine = readline(buildPrompt());


    
    if (cmdLine == NULL) {
      fprintf(stderr, "Unable to read command. \n");
      continue;
    }


    /*insert your code about history and !x !-x here*/

    /*calls the parser*/
    info = parse(cmdLine);
    if (info == NULL){
      free(cmdLine);
      continue;
    }
    /*prints the info struct*/
    /*print_info(info);*/

    /*com contains the info. of the command before the first "|"*/
    com=&info->CommArray[0];
    infi = info->boolOutfile;
    outf = info->boolInfile;
    backg = info->boolBackground;
    append = info->boolAppend;
    pipe_var = info->boolpipe;
   	
   	

    /*fprintf(stdout, "infi: %d outf: %d backg: %d\n",infi,outf,backg);*/
    if ((com == NULL)  || (com->command == NULL)) {
      free_info(info);
      free(cmdLine);
      continue;
    }
    /*com->command tells the command name of com*/

    /*----------CHDIR-------------------------*/

     if(isBuiltInCommand(com->command) == CD && infi==0 && outf==0 && backg==0 && append ==0 && pipe_var==0){

        if(com->VarList[1]==NULL){
          printf("please write a file name also.");
          printf("\n");
        }
        else {
          char *directory = com->VarList[1];
          int is_possible;
          is_possible = chdir(directory);
        }       
    }

    /*----------------END CHDIR------------------*/

    
    /*----------------PWD------------------------*/
    else if(isBuiltInCommand(com->command)==PWD && infi==0 && outf==0 && backg==0  && append ==0 && pipe_var==0){
        long size;
        char *buf;
        char *ptr;

        size = pathconf(".", _PC_PATH_MAX);  /* give some size equal to the size of array which will be required.*/
        if ((buf = (char *)malloc((size_t)size)) != NULL){
            ptr = getcwd(buf,(size_t)size);
            printf("%s", ptr);
            printf("\n");
        }
        free(buf);
    }
    /*----------------END PWD--------------------*/

    /*-----------------LS -l---------------------*/
    else if(isBuiltInCommand(com->command)==LS && infi==0 && outf==0 && backg==0  && append ==0 && pipe_var==0){
        pid_t child;
        int status;
        pid_t c;
        char *args[3];
        args[0] = "ls"; args[1] = "-l"; args[2] = NULL;
        child = fork();
        if(child == 0){
           /* printf("Child: PID of Child = %ld\n", (long) getpid());*/
            execvp(args[0], args); 

            fprintf(stderr, "Child process could not do execvp.\n");
            exit(1);
        } 
        else{
          if (child == (pid_t)(-1)) {
              fprintf(stderr, "Fork failed.\n"); exit(1);
          }
          else {
              c = wait(&status);       /* Wait for child to complete. */
          }

        }
    }

    /*-----------------END LS--------------------*/

    /*-----------------RMDIR---------------------*/
    else if(isBuiltInCommand(com->command) == RMDIR && infi==0 && outf==0 && backg==0  && append ==0 && pipe_var==0){    
        int stat;
        if(com->VarList[1]==NULL){
            printf("Please specify some directory name.");
        }
        else{
            stat = rmdir(com->VarList[1]);
            if(stat==-1){
              printf("There is somehing wrong with your inputs.");
            }
            else{
              printf("Removed the directory: %s\n",com->VarList[1]);
              printf("\n");
            }
        }
    }

    /*-----------------EXIT RMDIR----------------*/

    /*-----------------MKDIR---------------------*/
    else if(isBuiltInCommand(com->command) == MKDIR && infi==0 && outf==0 && backg==0  && append ==0 && pipe_var==0){    
        int stat;
        if(com->VarList[1]==NULL){
            printf("Please specify some directory name.");
        }
        else{
            stat = mkdir(com->VarList[1]);
            if(stat==-1){
              printf("There is somehing wrong with your inputs.");
            }
            else{
              printf("Created the directory: %s\n",com->VarList[1]);
              printf("\n");
            }
        }
    }

    /*-----------------EXIT MKDIR----------------*/

    /*-----------------Remove File---------------------*/
    else if(isBuiltInCommand(com->command) == RM && infi==0 && outf==0 && backg==0  && append ==0 && pipe_var==0){    
        int stat;
        if(com->VarList[1]==NULL){
            printf("Please specify some file name.");
        }
        else{
            stat = remove(com->VarList[1]);
            if(stat==-1){
              printf("There is somehing wrong with your inputs.");
            }
            else{
              printf("Removed the file: %s\n",com->VarList[1]);
              printf("\n");
            }
        }
    }

    /*-----------------End Remove File----------------*/


    /*-------------------EXIT------------            */
    else if (isBuiltInCommand(com->command) == EXIT && infi==0 && outf==0 && backg==0  && append ==0 && pipe_var==0) {
      exit(1);
    }

    


    /*  ----------------------   APPEND START-----------------------      */
    else if(infi==0 && outf==0 && backg==0  && append ==1 && pipe_var==0){
    	pid_t child;
        int status;
        pid_t c;
        char *args[2];
      
        args[0] = com->command; args[1] = NULL; 
        
        child = fork();
        if(child == 0){
        	if(info->appendFile==NULL){
        		printf("Please specify the file name.");
        	}
        	else{
	            printf("Child: PID of Child =%d %ld\n", child,(long) getpid());
	            
	            freopen(info->appendFile, "a+", stdout);
	            
	            execvp(args[0], args); 
	            

	            fprintf(stderr, "Child process could not do execvp.\n");
	            exit(1);
	        }
        } 
       else{
        	
          if (child == (pid_t)(-1)) {
              fprintf(stderr, "Fork failed.\n"); exit(1);
          }
          else {
              c = wait(&status);     
          }

        }
    }


    /*---------------------------INPUT FILE I?O START--------------------------*/


    else if(infi==1 && outf==0 && backg==0  && append ==0 && pipe_var==0){
    	pid_t child;
        int status;
        pid_t c;
        char *args[2];
      
        args[0] = com->command; args[1] = NULL; 
        
        child = fork();
        if(child == 0){
        	if(info->outFile==NULL){
        		printf("Please specify the file name.");
        	}
        	else{
	            printf("Child: PID of Child =%d %ld\n", child,(long) getpid());
	            
	            freopen(info->outFile, "w+", stdout);
	            
	            execvp(args[0], args); 
	            

	            fprintf(stderr, "Child process could not do execvp.\n");
	            exit(1);
	        }
        }
     
       else{
        	
          if (child == (pid_t)(-1)) {
              fprintf(stderr, "Fork failed.\n"); exit(1);
          }
          else {
              c = wait(&status);     
          }

        }
    }
    /*------------------------------OUTPUT I?O REDIRECTION START-----------*/
        else if(infi==0 && outf==1 && backg==0  && append ==0 && pipe_var==0){
    	pid_t child;
        int status;
        pid_t c;
        char *args[2];
      
        args[0] = com->command; args[1] = NULL; 
        
        child = fork();
        if(child == 0){
        	if(info->inFile==NULL){
        		printf("Please specify the file name.");
        	}
        	else{
	            printf("Child: PID of Child =%d %ld\n", child,(long) getpid());
	            
	            freopen(info->inFile, "r+", stdin);
	            
	            execvp(args[0], args); 
	            

	            fprintf(stderr, "Child process could not do execvp.\n");
	            exit(1);
	        }
        }
     
       else{
        	
          if (child == (pid_t)(-1)) {
              fprintf(stderr, "Fork failed.\n"); exit(1);
          }
          else {
              c = wait(&status);     
          }

        }
    }


   /*---------------PATH EXECUTION START-------------------*/

    else if(infi==0 && outf==0  && append ==0 && pipe_var==0){	
 			
       		
       			pid_t child;       		
       			
       			int status;
       			pid_t c;
       			child = fork();
       			
		        if(child == 0){
		        	 
			        execvp(com->VarList[0], com->VarList); 

		            fprintf(stderr, "Child process could not do execvp.\n");
		            exit(1);
		        } 
		        else{
		          if (child == (pid_t)(-1)) {
		              fprintf(stderr, "Fork failed.\n"); exit(1);
		          }
		          else {
		          		if(backg==0){
		          			printf("forground proceses\n");
		          			waitpid(child,0,0);
		          		}

		          		if(backg==1){
		          			printf("backgrund...\n");
		          			backg=0;
		          		}

		          		
		                     /* Wait for child to complete. */
		          }

		        }
    }

       else if(pipe_var==1){
	        	int i;
	        	int args[2];
	        	pid_t child,child2;
	        	int status;
	        	pid_t c;
	        	child = fork();
	        	/*args[1]--> writing descriptor, args[0]--> reading descriptors
	        	1---> stdout and 0-> stdin
	        	*/
	    
	        	if(child==(pid_t)0){
	        		pipe(args);
	        		dup2(args[1],1);
	        		close(args[0]);
	        		execlp(com->VarList[0],com->VarList[0],NULL);

	        		fprintf(stderr, "Child process could not do execvp.\n");
		            exit(1);
	        	}
	        	else{
	        		if (child == (pid_t)(-1)) {
		              	fprintf(stderr, "Fork failed.\n"); exit(1);
		          	}
		          	else{
			          	waitpid(child,&status,0);
			          	child2 =fork();
			          	if(child2==0){
			          		close(args[1]);
			          		dup2(args[0],0);
			          		execlp(com->VarList[1],com->VarList[1],NULL);

			          	}
		        		else{
		        			waitpid(child2,&status,0);
	        			}
	        	}
	        }
        } 

    else{
    	printf("command not found.\n");

    }

    /*insert your code here.*/

    free_info(info);
    free(cmdLine);
  }/* while(1) */
    	}

  





