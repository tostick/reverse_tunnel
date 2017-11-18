#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

#include <readline/readline.h>
#include <readline/history.h>

#include "command.h"
#include "libevent.h"
#include "file.h"

extern int g_file_flag;

/* 
* A structure which contains information on the commands this program
* can understand. 
*/
typedef struct {
    char *name;            /* User printable name of the function. */
    rl_icpfunc_t *func;        /* Function to call to do the job. */
    char *doc;            /* Documentation for this function. */
} COMMAND;


int com_connect(char *para)
{
    send_msg("connect", para);
    
    return 0;
}

int com_list(char *para)
{
    send_msg("ls", para);
    
    return 0;
}
    
int com_cat(char *para)
{
    send_msg("cat", para);
    
    return 0;
}

int com_mv(char *para)
{
    send_msg("mv", para);
    return 0;
}
int com_stat(char *para)
{
    send_msg("stat", para);
    return 0;
}

int com_pwd(char *para)
{
    send_msg("pwd", para);
    return 0;
}

int com_recv(char *para)
{
    char *p_dir = NULL;
    char *p_file = NULL;
    char src_file[1024] = {0};

    if (NULL == para)
    {
        printf("para is null\n");
        printf("recv <dir> <file>\n");
        return 1;
    }

    p_dir = strtok(para, " ");
    p_file = strtok(NULL," ");
        
    /* 判断参数的路径是否存在 */
    if (NULL == p_dir || 0 != access(p_dir, 0x2) )
    {
        printf("%s not exist!\n", p_dir);
        return 1;
    }
    
    
    sprintf(src_file, "%s/%s", p_dir,strrchr(p_file, '/') + 1);
    /* 判断该文件是否存在 */
    if (0 == access(src_file, 0x0) )
    {
        printf("%s exist!\n", src_file);
        return 1;
    }
    
    if (NULL != p_file)
    {
        g_file_flag = 1;
        open_file(src_file);
        send_msg("recv", p_file);
    }
    else
    {
        printf("%s not null!\n", p_file);
        return 1;
    }

    wait_file();
    return 0;
}


int com_rm(char *para)
{
    send_msg("rm", para);
    return 0;
}

#if 0
int com_cd(char *para)
{
    send_msg("cd", para);
      
    return 0;
}
#endif

int com_dmesg(char *para)
{
    send_msg("dmesg", para);
      
    return 0;
}


int com_help(char *para)
{
    printf("do com_help:%s\n", para);
    return 0;
}


int com_quit(char *para)
{
    exit(0);
}

COMMAND commands[] = {
  { "cat", com_cat, "View the contents of FILE" },
//  { "cd", com_cd, "Change to directory DIR" },
  { "connect", com_connect, "connect a client" },
  { "dmesg", com_dmesg, "dmesg info" },
  { "ls", com_list, "Synonym for `list'" },
  { "mv", com_mv, "Rename FILE to NEWNAME" },
  { "rm", com_rm, "Delete FILE" },
  { "stat", com_stat, "Print out statistics on FILE" },
  { "pwd", com_pwd, "Print the current working directory" },
  { "recv", com_recv, "receive file from client" },
  { "quit", com_quit, "Quit using Fileman" },
  { "help", com_help, "Display this text" },
  { (char *)NULL, (rl_icpfunc_t *)NULL, (char *)NULL }
};
  


char* dupstr(char *s)
{
  char *r;

  r = malloc (strlen (s) + 1);
  strcpy(r, s);
  return (r);
}

// clear up white spaces
char* stripwhite (char *string)
{
    register char *s, *t;
    
    for (s = string; whitespace (*s); s++)
        ;
    
    if (*s == 0)
        return (s);

    t = s + strlen (s) - 1;
    while (t > s && whitespace (*t))
        t--;

    *++t = '\0';

    return s;
}


/* 
* Look up NAME as the name of a command, and return a pointer to that
* command. Return a NULL pointer if NAME isn't a command name. 
*/
COMMAND *find_command (char *name)
{
    register int i;
    
    for (i = 0; commands[i].name; i++)
        if (strcmp (name, commands[i].name) == 0)
            return (&commands[i]);

    return ((COMMAND *)NULL);
}

/* Execute a command line. */
int execute_line (char *line)
{
    register int i;
    COMMAND *command;
    char *word;

    /* Isolate the command word. */
    i = 0;
    while (line[i] && whitespace (line[i]))
        i++;
    word = line + i;

    while (line[i] && !whitespace (line[i]))
        i++;

    if (line[i])
        line[i++] = '\0';

    command = find_command (word);

    if (!command)
    {
        fprintf (stderr, "%s: No such command for FileMan.\n", word);
        return (-1);
    }

    /* Get argument to command, if any. */
    while (whitespace (line[i]))
        i++;

    word = line + i;

    /* Call the function. */
    return ((*(command->func)) (word));
}

/* 
* Generator function for command completion. STATE lets us know whether
* to start from scratch; without any state (i.e. STATE == 0), then we
* start at the top of the list. 
*/
char* command_generator (const char *text, int state)
{
    static int list_index, len;
    char *name;

    /* 
    * If this is a new word to complete, initialize now. This includes
    * saving the length of TEXT for efficiency, and initializing the index
    * variable to 0.
    */
    if (!state)
    {
         list_index = 0;
        len = strlen (text);
    }

    /* Return the next name which partially matches from the command list. */
    while (NULL != (name = commands[list_index].name))
    {
         list_index++;
    
         if (strncmp (name, text, len) == 0)
             return (dupstr(name));
    }

    /* If no names matched, then return NULL. */
    return ((char *)NULL);
}

/* 
* Attempt to complete on the contents of TEXT. START and END bound the
* region of rl_line_buffer that contains the word to complete. TEXT is
* the word to complete. We can use the entire contents of rl_line_buffer
* in case we want to do some simple parsing. Return the array of matches,
* or NULL if there aren't any. 
*/
char** fileman_completion (const char *text, int start, int end)
{
    char **matches;

    matches = (char **)NULL;

    /* 
    * If this word is at the start of the line, then it is a command
    * to complete. Otherwise it is the name of a file in the current
    * directory. 
    */
    if (start == 0)
        matches = rl_completion_matches (text, command_generator);

    return (matches);
}


/* 
* Tell the GNU Readline library how to complete. We want to try to complete
* on command names if this is the first word in the line, or on filenames
* if not. 
*/
void initialize_readline ()
{
    /* Allow conditional parsing of the ~/.inputrc file. */
    rl_readline_name = ">";

    /* Tell the completer that we want a crack first. */
    rl_attempted_completion_function = fileman_completion;
}




void init_command( )
{
    char *line, *s;
    
    initialize_readline();    /* Bind our completer. */


    /* Loop reading and executing lines until the user quits. */
    for ( ; ;)
    {
        usleep(1000*100);
        
        line = readline (">: ");

        if (!line)
            break;

        /* 
        * Remove leading and trailing whitespace from the line.
        * Then, if there is anything left, add it to the history list
        * and execute it. 
        */
         s = stripwhite (line);
         if (*s)
         {
             add_history(s);
             execute_line(s);
         }

        free(line);
    }
}



