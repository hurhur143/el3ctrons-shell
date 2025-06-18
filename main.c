//
// NOTES:
//
// I should optimize the code or refactor it so it doesn't look really
// messy. (Im not a professional)
//
// Source Code and Project made since June 16th, 2025.
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

// -------- //
// commands //
// -------- //
#define CMD_CLS "cls"
#define CMD_EXIT "exit"
#define CMD_ECHO "echo"
#define CMD_TYPE "type"

#define CMD_HELP "-h"
#define CMD_HELPLONG "--help"
#define CMD_ABOUT "-a"
#define CMD_ABOUTLONG "--about"
#define CMD_CHANGELOG "-c"
#define CMD_CHANGELOGLONG "--changelog"

// command types
char* builtin_cmds[] = { CMD_CLS, CMD_EXIT, CMD_ECHO, CMD_TYPE, CMD_HELP,
	CMD_HELPLONG, CMD_ABOUT, CMD_ABOUTLONG, CMD_CHANGELOG, CMD_CHANGELOGLONG };

size_t total_cmds = (sizeof(builtin_cmds) / sizeof(builtin_cmds[0]));

// ----------- //
// buffer size //
// ----------- //
#define BUFFER_SIZE 1024
#define TEXT_SIZE 4096

// ----- //
// files //
// ----- //
#define HELPFILE "../help.txt"
#define ABOUTFILE "../about.txt"
#define CHANGELOG "../changelog.txt"

#define DELIM " "

// using cmd line like arguments
int argc = 0;
char* argv[BUFFER_SIZE];
char* full_arg;

// command forward declarations
void shell_exit(char* status);
void shell_echo(char* arg[]);
void shell_type(char* cmd);
void shell_runexternal(char* cmd);

void shell_readfile(char* path);

int main()
{
	while (1) {
		// flush stdout (after every printf)
		setbuf(stdout, NULL);

		printf("$ ");

		// get user input
		char buffer[BUFFER_SIZE];
		memset(buffer, 0, BUFFER_SIZE);
		fgets(buffer, BUFFER_SIZE, stdin);

		*strchr(buffer, '\n') = '\0';
		
		full_arg = buffer;

		// tokenize buffer into cmd line arguments (argc, argv)
		char* token = strtok(buffer, DELIM);
		for (argc = 0; token != NULL; argc++)
		{
			argv[argc] = token;
			token = strtok(NULL, DELIM);
		}

		// check commands
		if (!strcmp(argv[0], CMD_EXIT))
		{
			shell_exit(argv[1]); // exits shell
		}
		else if (!strcmp(argv[0], CMD_CLS))
		{
			system("clear"); // clears shell
		}
		else if (!strcmp(argv[0], CMD_ECHO))
		{
			shell_echo(argv); // echos like a command prompt
		}
		else if (!strcmp(argv[0], CMD_TYPE))
		{
			shell_type(argv[1]); // checks for the command type
		}
		else if (!strcmp(argv[0], CMD_HELP) || !strcmp(argv[0], CMD_HELPLONG))
		{
			shell_readfile(HELPFILE); // opens up help center
		}
		else if (!strcmp(argv[0], CMD_ABOUT) || !strcmp(argv[0], CMD_ABOUTLONG))
		{
			shell_readfile(ABOUTFILE); // opens up about page
		}
		else if (!strcmp(argv[0], CMD_CHANGELOG) || !strcmp(argv[0], CMD_CHANGELOGLONG))
		{
			shell_readfile(CHANGELOG); // opens up project changelog
		}
		else
		{
			shell_runexternal(argv[0]); // executes external commands
		}
	}
	
	return 0;
}

void shell_exit(char* status)
{
	unsigned long exit_code = 0;

	if (status != NULL)
	{
		exit_code = strtoul(status, 0, 10);
	}

	if (exit_code > 255 || exit_code < 0)
	{
		printf("exit: exit code must be valid or between 0-255\n");
		return;
	}

	exit(exit_code);
}

void shell_echo(char* arg[])
{
	for (int i = 1; i < argc; i++)
	{
		if (arg[i] == NULL) break;

		printf("%s ", arg[i]);
	}
	printf("\n");
}

void shell_type(char* cmd)
{
	int isBuiltin = 0;
	int isExecutable = 0;

	char* PATH = strdup(getenv("PATH"));

	#if (_WIN32 || _WIN64)
	const char* seperator = ";";
	#else
	const char* seperator = ":";
	#endif

	// check for built in commands
	for (int i = 0; i < total_cmds; i++)
	{
		if (!strcmp(cmd, builtin_cmds[i]))
		{
			printf("%s is a shell builtin\n", cmd);
			free(PATH);
			return;
		}
	}

	// check for external commands (executables)
	char* dir = strtok(PATH, seperator);
	while (dir != NULL)
	{
		char fullpath[BUFFER_SIZE];

		#if (_WIN32 || _WIN64)
		snprintf(fullpath, sizeof(fullpath), "%s\\%s.exe", dir, cmd);
		#else
		snprintf(fullpath, sizeof(fullpath), "%s/%s", dir, cmd);
		#endif

		if (!access(fullpath, X_OK))
		{
			printf("%s is %s\n", cmd, fullpath);
			free(PATH);
			return;
		}

		dir = strtok(NULL, seperator);
	}

	printf("%s: not found\n", cmd);
	free(PATH);
}

void shell_runexternal(char* cmd)
{
	int valid_cmd = 0;

	char* PATH = strdup(getenv("PATH"));

	#if (_WIN32 || _WIN64)
	const char* seperator = ";";
	#else
	const char* seperator = ":";
	#endif

	char* dir = strtok(PATH, seperator);
	while (dir != NULL)
	{
		char fullpath[BUFFER_SIZE];

		#if (_WIN32 || _WIN64)
		snprintf(fullpath, sizeof(fullpath), "%s\\%s.exe", dir, cmd);
		#else
		snprintf(fullpath, sizeof(fullpath), "%s/%s", dir, cmd);
		#endif

		if (!access(fullpath, X_OK)) 
		{
			valid_cmd = 1;
			break;
		}

		dir = strtok(NULL, seperator);
	}

	// check if cmd is valid
	if (valid_cmd)
	{
		system(full_arg); // execute said command
	}
	else
	{
		printf("%s: command not found\n", argv[0]);
	}

	free(PATH);
}

void shell_readfile(char* path)
{
	FILE* file = fopen(path, "r");

	char buffer[TEXT_SIZE];
	while(fgets(buffer, sizeof(buffer), file))
	{
		printf("%s", buffer);
	}

	fclose(file);
}
