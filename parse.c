/************************************************************************  
 *   parse.c  -  The parsing portion of my small shell                                     *
 *   Syntax:     myshell command1 [< infile] [| command]* [> outfile] [&]       *
 ************************************************************************/

#include <ctype.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "parse.h"

/* parse - parse a new line 
 *
 * Accepts: nothing
 * Returns: parse information structure
 */
#define MAXLINE 81

void init_info(parseInfo *p) {
  int i;

  p->boolInfile=0;
  p->boolOutfile=0;
  p->boolBackground=0;
  p->pipeNum=0;
  p->boolAppend=0;
  p->boolpipe=0;

  for (i=0; i<PIPE_MAX_NUM; i++) {
    p->CommArray[i].command=NULL;
    p->CommArray[i].VarList[0]=NULL;
    p->CommArray[i].VarNum=0;
  }
}

void parse_command(char * command, struct commandType *comm) {

      int i=0;
      int pos=0;
      char word[MAXLINE];

      comm->VarNum=0;
      comm->command=NULL;
      comm->VarList[0]=NULL;
     
      while(isspace(command[i]))
        i++; 
      if (command[i] == '\0') 
        return;
      while (command[i] != '\0') {
          while (command[i] != '\0' && !isspace(command[i])) {
          word[pos++]=command[i++];
        }
        word[pos]='\0';
        comm->VarList[comm->VarNum]=malloc((strlen(word)+1)*sizeof(char));
        strcpy(comm->VarList[comm->VarNum], word);
        comm->VarNum++;
        word[0]='\0';
        pos=0;
        while(isspace(command[i]))
          i++; 
      }
      comm->command=malloc((strlen(comm->VarList[0])+1)*sizeof(char));
      strcpy(comm->command, comm->VarList[0]);
      comm->VarList[comm->VarNum]=NULL;
}

parseInfo *parse (char *cmdline) {
  parseInfo *Result;
  int i=0,j=0;
  int pos;
  int end=0;
  int count;
  char command[MAXLINE];
  int com_pos;

  if (cmdline[i] == '\n' && cmdline[i] == '\0')
    {
      fprintf(stdout,"hey its NULL");
      return NULL;
    }

  Result = malloc(sizeof(parseInfo));
  if (Result == NULL){
    return NULL;
  }
  init_info(Result);
  com_pos=0;
  count=0;
  while (cmdline[j] != '\n' && cmdline[j] != '\0') {
    if(cmdline[j]=='>'){
      count++;j++;
    }
    else{j++;}

  }
  
  while (cmdline[i] != '\n' && cmdline[i] != '\0') {
    /*fprintf(stdout, "ith %c\n",cmdline[i] );
    fprintf(stdout, "1+1 %c\n",cmdline[i+1] );*/
    if (cmdline[i] == '&') {
      Result->boolBackground=1;
      if (cmdline[i+1] != '\n' && cmdline[i+1] != '\0') {
	       fprintf(stderr, "Ignore anything beyond &.\n");
      }
      break;
    }
    else if (cmdline[i] == '<' ) {
      Result->boolInfile=1;
      while (isspace(cmdline[++i]));
        pos=0;
      while (cmdline[i] != '\0' && !isspace(cmdline[i])) {
	    if (pos==FILE_MAX_SIZE) {
	      fprintf(stderr, "Error.The input redirection file name exceeds the size limit 40\n");
	      free_info(Result);
	      return NULL;
	    }
    	 Result->inFile[pos++] = cmdline[i++];
      }
      Result->inFile[pos]='\0';
      end =1;
      while (isspace(cmdline[i])) {
	    if(cmdline[i] == '\n') 
	      break;
	      i++;
      }
    }
    else if(count==1 && cmdline[i]=='>' ){
      Result->boolOutfile=1;
      while (isspace(cmdline[++i]));
        pos=0;
      while (cmdline[i] != '\0' && !isspace(cmdline[i])) {
          if (pos==FILE_MAX_SIZE) {
            fprintf(stderr, "Error.The input redirection file name exceeds the size limit 40\n");
            free_info(Result);
            return NULL;
          }
           Result->outFile[pos++] = cmdline[i++];
      }
      Result->outFile[pos]='\0';
      end =1;
      while (isspace(cmdline[i])) {
        if(cmdline[i] == '\n') 
          break;
          i++;
      }
     
    }
    else if (cmdline[i] == '>' && cmdline[i+1]=='>' && count==2) {
        
        Result->boolAppend=1;
        i++;
        while (isspace(cmdline[++i]));
        pos=0;
        while (cmdline[i] != '\0' && !isspace(cmdline[i])) {
      	if (pos==FILE_MAX_SIZE) {
      	  fprintf(stderr, "Error.The output redirection file name exceeds the size limit 40\n");
      	  free_info(Result);
      	  return NULL;
      	}
      	Result->appendFile[pos++] = cmdline[i++];
            }
            Result->appendFile[pos]='\0';
            /*printf("yes u r correct its coming here :P");*/
            end =1;
            while (isspace(cmdline[i])) {
            	if (cmdline[i] == '\n') 
            	  break;
            i++;
            }
            
    }

    /*else if (cmdline[i] == '>' && cmdline[i+1]=='>') {
        Result->boolAppend=1;
        i = i+1;
        while (isspace(cmdline[++i]));
        pos=0;
        while (cmdline[i] != '\0' && !isspace(cmdline[i])) {
        if (pos==FILE_MAX_SIZE) {
          fprintf(stderr, "Error.The output redirection file name exceeds the size limit 40\n");
          free_info(Result);
          return NULL;
        }
        Result->appendFile[pos++] = cmdline[i++];
            }
            Result->appendFile[pos]='\0';
            end =1;
            while (isspace(cmdline[i])) {
              if (cmdline[i] == '\n') 
                break;
            i++;
            } 
    }*/
    else if (cmdline[i] == '|') {
      Result->boolpipe=1;
      /*command[com_pos]='\0';
      parse_command(command, &Result->CommArray[Result->pipeNum]);
      com_pos=0;
      end =0;
      Result->pipeNum++;*/
      i++;
    }
    else {
      if (end == 1) {
	       fprintf(stderr, "Error.Wrong format of input\n");
	       free_info(Result);
	       return NULL;
      }
      if (com_pos == MAXLINE-1) {
        	fprintf(stderr, "Error. The command length exceeds the limit 80\n");
        	free_info(Result);
        	return NULL;
      }
      command[com_pos++] = cmdline[i++];
    }
  }
  command[com_pos]='\0';
  parse_command(command, &Result->CommArray[Result->pipeNum]);
  /*Result->pipeNum++;*/
  return Result;
}

void print_info (parseInfo *info) {
  int i, j;
  struct commandType *comm;

  if (NULL == info){
    printf("Null info\n");
    return;
  }
  printf("Parse struct:\n\n");
  printf("# of pipes:%d\n", info->pipeNum);
  for (i=0; i<=info->pipeNum;i++) {
    comm=&(info->CommArray[i]);
    if ((NULL == comm) || (NULL == comm->command)){
      printf("Command %d is NULL\n", i);
    } else {
      printf("Command %d is %s.\t", i+1, comm->command);
      for (j=0; j<comm->VarNum; j++) {
	printf("Arg %d: %s ", j, comm->VarList[j]);
      }
      printf("\n");
    }
  }
  printf("\n");

  if (info->boolInfile){
    printf("infile: %s\n", info->inFile);
  }else {
    printf("no input redirection.\n");
  }
  if (info->boolOutfile) {
    printf("outfile: %s\n", info->outFile);
  }else {
    printf("no output redirection.\n");
  }
  if (info->boolBackground){
    printf("Background process.\n");
  } else {
    printf("Foreground process.\n");
  }
}
 
void free_info (parseInfo *info) {
  int i,j;
  struct commandType *comm;

  if (NULL == info) return;
  for (i=0; i<PIPE_MAX_NUM;i++) {
    comm=&(info->CommArray[i]);
    for (j=0; j<comm->VarNum; j++) {
      free(comm->VarList[j]);
    }
    if (NULL != comm->command){
      free(comm->command);
    }
  }
  free(info);
}

