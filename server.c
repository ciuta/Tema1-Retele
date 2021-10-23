#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdbool.h>
#include <utmp.h>
#include <time.h>
#include <sys/time.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>


#define FIFO_CLIENT_SERVER "clientServerFifo"
#define FIFO_SERVER_CLIENT "serverClientFifo"
#define NMAX 1024

void handle_errors(char * msg)
{
	perror(msg);
	exit(2);
}

bool test_logger(char *name)
{
    FILE *fp;
    char *line = NULL;
    bool FLAG_Valid = false;
    size_t len;
    
    fp = fopen("/home/andrei-calin/rc/tema1/config", "r");

    if(fp == NULL)
    {
        exit(2);
    }

    while((getline(&line, &len, fp)) != -1)
    {
        char *firstWord = strtok(line, " \n");
        if(strcmp(firstWord, name) == 0)
            FLAG_Valid = true;
        printf("This is the line {%s} \n", firstWord);
    }

    fclose(fp);
    if(line)
        free(line);
    return FLAG_Valid;
}

void split_instruction(char *original, char *firstWord, char *secondWord, char *thirdWord)
{
    int index = 0;
    char *token;

	
	index = 0;

    token = strtok(original, " ");
	strcat(firstWord, token);

    while(token != NULL)
    {
        
		if( index == 1 )
			strcat(secondWord, token);
		else if ( index == 2 )
			strcat(thirdWord, token);
        index++;

        token = strtok(NULL, " ");
    }
	firstWord[strlen(firstWord)] = '\0';
	secondWord[strlen(secondWord)] = '\0';
	thirdWord[strlen(thirdWord)] = '\0';
    return;
}
bool get_proc_info(char *pid, char *info)
{
    FILE *fp;
    char *line = NULL;
    size_t len;
    char path[NMAX] = "";
    strcat(path, "/proc/");
    strcat(path, pid);
    strcat(path, "/status");
    //printf("Path: {%s}\n", path);


    fp = fopen(path, "r");
    if(fp == NULL)
    {
        return false;
    }
    while((getline(&line, &len, fp)) != -1)
    {
        char *firstWord = strtok(line, " \n\t");
        if(strcmp(firstWord, "Name:") == 0)
        {
            strcat(info, firstWord);
            if((firstWord = strtok(NULL, " \n\t")) != NULL)
                strcat(info, firstWord);
            strcat(info, "\n");
        }
        else
        if(strcmp(firstWord, "State:") == 0)
        {
            strcat(info, firstWord);
            if((firstWord = strtok(NULL, " \n\t")) != NULL)
                strcat(info, firstWord);
            if((firstWord = strtok(NULL, " \n\t")) != NULL)
                strcat(info, firstWord);
            strcat(info, "\n");
        }
        else
        if(strcmp(firstWord, "Ppid:") == 0)
        {
            strcat(info, firstWord);
            if((firstWord = strtok(NULL, " \n\t")) != NULL)
                strcat(info, firstWord);
            strcat(info, "\n");
        }
        else
        if(strcmp(firstWord, "Uid:") == 0)
        {
            strcat(info, firstWord);
            if((firstWord = strtok(NULL, " \n\t")) != NULL)
                strcat(info, firstWord);
            if((firstWord = strtok(NULL, " \n\t")) != NULL)
                strcat(info, firstWord);
            if((firstWord = strtok(NULL, " \n\t")) != NULL)
                strcat(info, firstWord);
            if((firstWord = strtok(NULL, " \n\t")) != NULL)
                strcat(info, firstWord);
            strcat(info, "\n");
        }
        else
        if(strcmp(firstWord, "VmSize:") == 0)
        {
            strcat(info, firstWord);
            if((firstWord = strtok(NULL, " \n\t")) != NULL)
                strcat(info, firstWord);
            if((firstWord = strtok(NULL, " \n\t")) != NULL)
                strcat(info, firstWord);
            strcat(info, "\n");
        }
    }
    fclose(fp);
    if(line)
        free(line);
    return true;
}

void get_logged_users(char *logged_users)
{
    FILE *fp;
    int logsize = 10;
    struct utmp log[logsize];

    int index = 0;
    fp = fopen("/var/run/utmp", "rb"); 
    
    if(fp == NULL)
    {
        exit(2);
    }
    fread(&log, sizeof(struct utmp), logsize, fp);
    for(index = 0 ; index < logsize ; ++index)
    {
        if(log[index].ut_user[0] >= 'A' && log[index].ut_user[0] <= 'z' )
        {
            time_t second;
            second = log[index].ut_tv.tv_sec;
            char convert[264];

            strcat(logged_users, "\n User: ");
            sprintf(convert, "%s", log->ut_user);
            strcat(logged_users, convert);

            strcat(logged_users, " \n Host: ");
            sprintf(convert, "%s", log->ut_host);
            strcat(logged_users, convert);

            strcat(logged_users, " \n Time: ");
            strcat(logged_users, asctime(localtime(&second)));
            strcat(logged_users, "\n");

            //printf("%s", asctime(localtime(&second)));
        }
    }
    logged_users[strlen(logged_users)] = '\0';
    fclose(fp);
}

int main()
{
	char instruction[NMAX], forClient[NMAX];
	int fifo_c2s, fifo_s2c, fd_c2s, fd_s2c;

	fifo_c2s = mknod(FIFO_CLIENT_SERVER, S_IFIFO | 0666, 0);
	
	if( fifo_c2s < 0 )
	{
		handle_errors("Server:Some problems with mknod, line 11 \0");
	}

	fd_c2s = open(FIFO_CLIENT_SERVER, O_RDONLY);
	
	if( fd_c2s < 0 )
	{
		handle_errors("Server:Some problems with opening fifo, line 33?? \0");
	}
	int num;
	do
	{
		if((num = read(fd_c2s, instruction, NMAX)) == -1)
			handle_errors("Nu e ok la scriere \0");
		else
		{
			//printf("Server22: A venit instructiunea {%s}-{%d} \n", instruction, num);
			if(num == 0)
			{
				return 0;
			}
			instruction[num] = '\0';		
			char firstWord[NMAX] = "";
			char secondWord[NMAX] = "";
			char thirdWord[NMAX] = "";
			split_instruction(instruction, firstWord, secondWord, thirdWord);
			fd_s2c = open(FIFO_SERVER_CLIENT, O_WRONLY);

			if(fd_s2c < 0)
			{
				handle_errors("Server:Some problems with opening fd_s2c, line 33?? \0");
			}

			
			if(strcmp(firstWord, "login") == 0)
			{
				if(test_logger(thirdWord))
				{
					write(fd_s2c, "22 Successfully logged!\0", NMAX);
				}
				else
					write(fd_s2c, "47 You Are Unauthorized To Perform This Request!\0", NMAX);
			}
			else
			if(strcmp(firstWord, "get-proc-info") == 0)
			{
				char info[NMAX] = "";
				if(get_proc_info(thirdWord, info))
				{
					char message_to_be_sent[NMAX] = "";
					sprintf(message_to_be_sent, "%ld", strlen(info));
					strcat(message_to_be_sent, " ");
					strcat(message_to_be_sent, info);
					message_to_be_sent[strlen(message_to_be_sent)] = '\0';
					write(fd_s2c, message_to_be_sent, sizeof(message_to_be_sent));
				}
				else
					write(fd_s2c, "23 No such process exist!\n\0", NMAX);










                int sockp[2], kid;
                if(socketpair(AF_UNIX, SOCK_STREAM, 0, sockp) < 0)
                {
                    handle_errors("Some problems with socket");
                }
                else
                {
                    if((kid = fork()) == -1)
                    {
                        handle_errors("Some problems with fork");
                    }
                    else
                    {
                        if(kid == 0)
                        {

                        }
                        else
                        {
                            
                        }
                    }
                }
			}
			else
			if(strcmp(firstWord, "get-logged-users") == 0)
			{
				char logged_users[NMAX] = "";
   		 		get_logged_users(logged_users);
				printf("%s", logged_users);
				write(fd_s2c, logged_users, sizeof(logged_users));
				
			}
			close(fd_s2c);
		}
		printf("Server: A venit instructiunea {%s} \n", instruction);
		instruction[0] = '\0'; 
	}while(num > 0);
	
	return 0;
}



