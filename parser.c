#include <stdio.h>
#include <string.h>
#include "wet.h"

#define MAX_LINE_SIZE 128

char	command_name[MAX_LINE_SIZE + 2];
char	command_line[MAX_LINE_SIZE + 2];
char*	arguments;
int		missing_argc;
int		argc;

char name       [MAX_LINE_SIZE + 2];
char photoId	[MAX_LINE_SIZE + 2];
char userId	    [MAX_LINE_SIZE + 2];
char info		[MAX_LINE_SIZE + 2];
char word       [MAX_LINE_SIZE + 2];
char k	    	[MAX_LINE_SIZE + 2];
char j          [MAX_LINE_SIZE + 2];

void goto_next_line(FILE* file)
{
	while (!feof(file) && fgetc(file) != '\n');
}

int check_command(char* expected_name) 
{
	return (!strcmp(command_name,expected_name));
}

int check_arguments(int expected_argc) 
{
	return ((missing_argc = expected_argc - argc) <= 0);
}

void illegalCommand() 
{
	printf("Illegal command!\n");
}

void parseInput() 
{
	while (!feof(stdin)) 
	{
        if (fgets(command_line,MAX_LINE_SIZE + 2,stdin) == NULL) break;
                
		if (strlen(command_line) > MAX_LINE_SIZE) 
		{
            printf("Command too long, can not parse\n");
            goto_next_line(stdin);
        }
        else 
		{
            missing_argc = 0;
            sprintf(command_name,"");
            sscanf(command_line,"%s",command_name);
            arguments=command_line + strlen(command_name) + 1;
                 
            if (check_command(COMMAND_ADD_USER))
            {
                argc = sscanf(arguments, "%s", name);
                check_arguments(1) ? addUser(name) : illegalCommand();
            }
            else if (check_command(COMMAND_ADD_USER_MIN))
            {
                argc = sscanf(arguments, "%s", name);
                check_arguments(1) ? addUserMin(name) : illegalCommand();
            }
            else if (check_command(COMMAND_REMOVE_USER))
            {
                argc = sscanf(arguments, "%s", userId);
                check_arguments(1) ? removeUser(userId) : illegalCommand();
            }
            else if(check_command(COMMAND_ADD_PHOTO))
			{
                argc = sscanf(arguments, "%s %s", userId, photoId);
                check_arguments(2) ? addPhoto(userId, photoId) : illegalCommand();
            }
            else if (check_command(COMMAND_TAG_PHOTO))
			{
                argc = sscanf(arguments, "%s %s %s", userId, photoId, info);
                check_arguments(3) ? tagPhoto(userId, photoId, info) : illegalCommand();
			}
            else if (check_command(COMMAND_PHOTOS_TAGS))
            {
                photosTags();
            }
            else if (check_command(COMMAND_SEARCH))
            {
                argc = sscanf(arguments, "%s", word);
                check_arguments(1) ? search(word) : illegalCommand();
            }
            else if (check_command(COMMAND_COMMON_TAGS))
            {
                argc = sscanf(arguments, "%s", k);
                check_arguments(1) ? commonTags(k) : illegalCommand();
            }
            else if (check_command(COMMAND_MOST_COMMON_TAGS))
            {
                argc = sscanf(arguments, "%s", k);
                check_arguments(1) ? mostCommonTags(k) : illegalCommand();
            }
            else if (check_command(COMMAND_SIMILAR_PHOTOS))
            {
                argc = sscanf(arguments, "%s %s", k, j);
                check_arguments(1) ? similarPhotos(k, j) : illegalCommand();
            }
            else if (check_command(COMMAND_ADD_PHOTOS_ON_TAG_ON))
            {
                autoPhotoOnTagOn();
            }
            else if (check_command(COMMAND_ADD_PHOTOS_ON_TAG_OFF))
            {
                autoPhotoOnTagOFF();
            }
            else if (check_command(COMMAND_EXIT)) 
			{
				break;
			}
			else if (missing_argc > 0) 
			{
                printf("Missing %d argument(s) in command %s!\n",missing_argc,command_name);
            } 
			else if (argc > 0) 
			{
                illegalCommand();
            } 
			else 
			{
                illegalCommand();
            }
        }
    }
}
