/*************************************
 * wyshell.c part2
 * Author: Ian Moon
 * Date: 5 May 2022
 *
 * This file is the set up the shell
 * using functions from the wyscanner
 * to make a working shell
 *
 * Collaborated with Zach Chrimmel on
 * this project
 ************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>
#include "wyscanner.h"

struct word
{
  struct word *next, *prev;
  char *string;
};

typedef struct word Word;

struct node
{
  struct node *node, *prev;
  char *command;
  Word *arg_list;
  int input, output, error;
  char *in_file, *out_file, *err_file;
};

typedef struct node Node;

int rtn = 0;
char *prtn;
char buff[4096];

void addToList(char *input, Node *list)
{
  Word *tmp;
  tmp = list->arg_list;
  if (tmp == NULL)
  {
    tmp = calloc(1, sizeof(Word));
    tmp->string = strdup(input);
    list->arg_list = tmp;
  }
  else
  {
    while (tmp->next != NULL)
      tmp = tmp->next;
    tmp->next = calloc(1, sizeof(Word));
    tmp->next->prev = tmp;
    tmp->next->string = strdup(input);
  }
  list->arg_list = tmp;
}

void com_free(Node *list)
{
  list->command = NULL;
}

void word_free(Node *list)
{
  list->arg_list = NULL;
}

int main()
{
  Node *Head = NULL, *current = NULL;
  Word *commands = NULL;
  // The process loop
  while (1)
  {
    // flags to trigger specific events
    int flag = 0;
    int amOut = 0;
    int amIn = 0;
    int eol = 0;
    int amp = 0;
    char *args[100] = {lexeme, NULL};
    int itt = 0;

    printf("$> ");
    // reads in from standard in
    prtn = fgets(buff, 4096, stdin);
    // if ^d is entered it exits
    if (prtn == NULL)
    {
      printf("\n");
      return 0;
    }
    rtn = parse_line(buff);
    // printf('\n');

    while (rtn != EOL)
    {
      switch (rtn)
      {
      case WORD:
        if (Head == NULL)
        {
          Head = calloc(1, sizeof(Node));
          current = Head;
        }

        if (current->command == NULL || flag == 1)
        {
          flag = 0;
          current->command = strdup(lexeme);
        }
        else
        {
          /*
          args[itt] = lexeme;
          args[itt + 2] = args[itt + 1];
          itt++;
          */
          if (eol == 1)
          {
            break;
          }
          addToList(lexeme, current);
        }
        break;
      case REDIR_OUT:
        // if read out is used more than once
        if (amOut == 1)
        {
          printf("Ambiguous output redirection\n");
          eol = 1;
          break;
        }
        // I know this is redundant but it helps my brain
        else
        {
          amOut = 1;
          break;
        }
      case REDIR_IN:
        if (amOut == 1 || amIn == 1)
        {
          printf("Ambiguous output redirection\n");
          eol = 1;
          break;
        }
        // see line 136
        else
        {
          amIn = 1;
          break;
        }
      case PIPE:
        if (fork() == 0)
        {
          int stat_code = execvp(current->command, args);
          if (stat_code == -1)
          {
            printf("Terminated Incorrectly\n");
          }
          exit(0);
        }
        if (amp != 1)
        {
          wait(NULL);
        }
        printf("\n");
        flag = 1;
        break;
      case SEMICOLON:
        if (fork() == 0)
        {
          int stat_code = execvp(current->command, args);
          if (stat_code == -1)
          {
            printf("Terminated Incorrectly\n");
          }
          else
          {
            exit(0);
          }
        }
        if (amp != 1)
        {
          wait(NULL);
        }
          printf("\n");
        flag = 1;
        break;
      case ERROR_CHAR:
        break;
      case QUOTE_ERROR:
        break;
      case SYSTEM_ERROR:
        return 0;
      case AMP:
        amp = 1;
      default:
        break;
      }
      /*
      if (amp != 1)
      {
        wait(NULL);
      }
      */
      rtn = parse_line(NULL);
      if (eol == 1)
      {
        break;
      }
    }
    if (flag == 0)
    {
      int a = 0;
      char* argues[100] = {lexeme, NULL};
      while(current->arg_list->next != NULL)
      {
        argues[a] = current->arg_list->string;
        a++;
      }
      args[a] = NULL;
      if (fork() == 0)
      {
        if (execvp(current->command, argues) == -1)
        {
          printf("Terminated incorrectly\n");
        }
        else
        {
          exit(0);
        }
      }
    }
    if(amp != 1)
    {
      wait(NULL);
    }
    current = Head;
    Node *tmp;
    com_free(current);
    word_free(current);
    tmp = current;
    com_free(tmp);
    amp = 0;
  }
}
