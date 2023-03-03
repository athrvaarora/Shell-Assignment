/*
  Name: Athrva Arora  
  ID: 1001935988
*/
// The MIT License (MIT)
// 
// Copyright (c) 2016 Trevor Bakker 
// 
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
// 
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
// 
// 
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.
#define _GNU_SOURCE

//Required Header files
#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <signal.h>


#define WHITESPACE " \t\n" /*                                                  
                              We want to split our command line up into tokens  
                              so we need to define what delimits our tokens.    
                              In this case  white space                         
                              will separate the tokens on our command line.    
                            */

#define MAX_COMMAND_SIZE 255 // The maximum command-line size

#define MAX_NUM_ARGUMENTS 10 // Mav shell only supports ten arguments

#define MAX_NUMBER_OF_COMMANDS 15 // The maximum history commands to display

char command_history[MAX_NUMBER_OF_COMMANDS][MAX_COMMAND_SIZE];

int pid_history[MAX_NUMBER_OF_COMMANDS];



void print(int counter, char* Command);

int main()
{
 
  char *cmd_str = (char *)malloc(MAX_COMMAND_SIZE);
  int command_counter = 0;
  int pids_counter = 0;
  int stopper = 1;

  while (1)
  {
    // stopper is an integer variable to stop taking inputs
    // whenever needed like a break statement.
    if (stopper)
    {  
      // Print out the msh prompt  
      printf("msh> ");

      // Read the command from the commandline.  The
      // maximum command that will be read is MAX_COMMAND_SIZE
      // This while command will wait here until the user
      // inputs something since fgets returns NULL when there
      // is no input
      while (!fgets(cmd_str, MAX_COMMAND_SIZE, stdin));
    }

    // 2-D Character array to store tokens of the inpus
    char *token[MAX_NUM_ARGUMENTS];
    // Counter for number of tokens made from the input
    int token_count = 0;
    // Pointer to point to the token
    // parsed by strsep
    char *arg_ptr;
    char *working_str = strdup(cmd_str);
    // we are going to move the working_string pointer so
    // keep track of its original value so we can deallocate
    // the correct amount at the end
    char *head_ptr= working_str;
    stopper = 1;
    // Tokenize the input strings with whitespace used as the delimiter
    while (((arg_ptr = strsep(&working_str, WHITESPACE)) != NULL) &&
           (token_count < MAX_NUM_ARGUMENTS))
    {
      token[token_count] = strndup(arg_ptr, MAX_COMMAND_SIZE);
      if (strlen(token[token_count]) == 0)
      {
        token[token_count] = NULL;
      }
      token_count++;
    }
    // Shell Functionality handling 

    //*Handling for adding commands to history array
    //first checking if there is a user input which is not the !number command 
    // checking for null if user pressed enter
    // so a different if statement will be executed 
    if (token[0] != NULL && cmd_str[0] != '!')
    {
      //checking if number of inputs are more than 15
      if (command_counter < MAX_NUMBER_OF_COMMANDS)
      {
        //adding input command to history array if number of commands in array
        // are less than 15
        strcpy(command_history[command_counter], cmd_str);
        //incrementing the number of commands already there in the history array
        command_counter++;
      }
      // if number of commands in the history array are already 15
      else
      {
        // move all the elements of the history array to (index-1) postion
        // removing the 1st element in history array and 
        // adding an element at 15th position
        for (int i = 1; i <MAX_NUMBER_OF_COMMANDS; i++)
        {
          for (int j = 1; j < MAX_COMMAND_SIZE; j++)
          {
            command_history[i-1][j-1] = command_history[i][j-1];
          }
        }
        // adding the new command at the end of the array at index 15
        strcpy(command_history[MAX_NUMBER_OF_COMMANDS - 1], cmd_str);
      }
    }
    // if there is a blank input or user pressed enter key 
    // keep on prompting msh>
    if (token[0] == NULL)
    {
      // continue asking user for input until exit
      continue;
    }
    // execute the code for !n functionality
    else if (cmd_str[0] == '!')
    {
      // convert the history element number entered(char*)
      // to an integer using atoi()  
      int index = atoi(&cmd_str[1]);
      // checking if number entered in more then number of elements
      // in history array
      if (index > command_counter)
      {
        printf("Command not found in history.\n");
      }
      //copying over the command at index (index -1) to command string 
      // and executing the command at that particular index in the history 
      // array
      strcpy(cmd_str, command_history[index - 1]);
      // stop asking for inputs once we have a command copied over in command string 
      // and execute the following command 
      stopper = 0;
      continue;
    }
    // Handling If the command string token is showpids
    else if (strcmp("showpids", token[0]) == 0)
    {
      // Add pid of -1 for this process (from slack chat)
      // incrementing the number if pids in the pid_history array
      pid_history[pids_counter] = -1;
      pids_counter++;
      // printing the pid's of the atmost 15 process ID's int he format specified
      print(pids_counter,token[0]);
    }
    //Handling If the command string token is cd (parameter)
    else if (strcmp("cd", token[0]) == 0)
    {
      // checking if the directory exist or not otherwise print 
      // Directory not found
      // executing cd in linux
      if (chdir(token[1]) == -1)
      {
        printf("%s: Directory not found.\n", token[1]);
      }
      //adding the process to the process history array and 
      //incrementing the number of pids in pid_history array 
      // of last 15 processes 
      pid_history[pids_counter] = -1;
      pids_counter++;
    }
    // Handling if the command entered is history
    else if (strcmp("history", token[0]) == 0 && token[1]==NULL)
    {
      // Add pid of -1 for this process (from slack chat)
      // incrementing the number of pids in the pid_history array
      pid_history[pids_counter] = -1;
      pids_counter++;
      // printing the last 15 commands entered in the terminal in the format specified
      print(command_counter,token[0]);
    }
    // handling for histor -p command 
    else if(strcmp("history", token[0]) == 0 && strcmp("-p",token[1]) ==0)
    {
      // Add pid of -1 for this process (from slack chat)
      // incrementing the number of pids in the pid_history array
      pid_history[pids_counter] = -1;
      pids_counter++;
      // if token[1] = "-p" printing the process ID's of commands and Commands
      // in the history together int the format specified
      for (int i = 0; i <pids_counter; i++)
      {
        printf("[%d]: [Pid: %d] %s", i, pid_history[i],command_history[i]);
      }
    }
    // handling if user input quit or exit as the command entered
    else if ((strcmp("quit", token[0]) == 0) || (strcmp("exit", token[0]) == 0))
    {
      // exiting the shell if the user command string is "quit" or "exit"
      exit(0);
    }
    else
    {
      // crating a process using fork()
      // it will assign the child's process ID to the parent
      // and child prcess ID equal to 0
      pid_t pid = fork(); 
      //if fork() is unable to create a process we will exit the shell with failure 
      // with the messege of fork failed
      // Reference form github repo - Code Samples (Operating Systems)
      if (pid == -1)
      {
        perror("Fork Failed.");
        exit(EXIT_FAILURE);
      }
      // when we are in child process
      else if (pid == 0)
      {
        // executing the shell commands entered and search for the path orders
        // Current working directory
        //  /usr/local/bin
        //  /usr/bin
        //  /bin Ref - (Readme Document)/ (Lecture Recording )
        int return_value = execvp(token[0], token);
        // handling if the command entered is not a valid linux command and there 
        // is no handling for it outside this else if block
        if (return_value == -1)
        {
          printf("%s: Command not found.\n", token[0]);
        }
        //fflush(NULL);
        // exiting the child process before the parent process
        exit(EXIT_SUCCESS);
      }
      else
      {
        //checking if number of pid's in pid history arrays are more than 15
        if (pids_counter < MAX_NUMBER_OF_COMMANDS)
        {
          //adding pids of input command to pid history array if number of elements in PID's
          //array are less than 15
          pid_history[pids_counter] = pid;
          //incrementing the number of pids already there in the pid_history array
          pids_counter++;
        }
        else
        {
          // if there are alrady 15 elements in pid history array
          // move all the elements of the pid history array to (index-1) postion
          // removing the 1st element in pid history array and 
          // adding an element at 15th position
          int i = 0;
          for (i = 0; i < pids_counter - 1; i++)
          {
            pid_history[i] = pid_history[i + 1];
          }
          // adding the pid of the new command at the end of the array at index 15
          pid_history[MAX_NUMBER_OF_COMMANDS - 1] = pid;
        }

        int status;
        // parent process will wait and only terminate only once child process terminates
        // or finish executing
        waitpid(pid, &status, 0);
      }
    }

    free(head_ptr);
  }
  return 0;
}
// printing the command history and pid_history for showpids
// and history command 
// passing the number of commands/ processes already in the array
// and the string which is command to print for 
void print(int count, char token[])
{
  // if command entered is history then print the last 15 commands in the command history
  // array
  if(strcmp("history",token)==0)
  {
    for (int i = 0; i < count; i++)
    {
      //using number of commands counter in main which is also passed throught the function print 
      //the last 15 commands in the command history array in the format specified.
      printf("[%d]: %s", i, command_history[i]);
    }
    
  }
  // if command entered is showpids print the process ID's of the last 15 processes in the pid's
  // array
  else if(strcmp("showpids",token)==0)
  {
    // using the pid counter in main which is passed throught the print function to print
    // the process id's in the format specified 
    for (int i = 0; i < count; i++)
    {
      printf("[%d]: %d\n", i, pid_history[i]);
    }
  }
}
