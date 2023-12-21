// You need to install 'xdotool' to make this work!

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

unsigned long window = 0;
unsigned long check = 0;
unsigned long browse = 0;

FILE *input = NULL;
int bytes = 0;
bool loop = true;

char command[65536];
char name[1024];

unsigned char counter = 0;

char path[16][256];

void clearcommand()
{
	for (int i=0; i<65536; i++) command[i] = 0;
	
	return;
};

int waitcommand()
{
	loop = true;

	while (loop)
	{
		system("sleep 1");

		clearcommand();
		sprintf(command, "xdotool windowfocus %lu ; xdotool type --window %lu \"var myint=setInterval(function(){if(document.readyState=='complete'){document.title='BbTestReady%x';clearInterval(myint);}},1000);\"", window, window, counter);
		system(command);

		clearcommand();
		sprintf(command, "xdotool windowfocus %lu ; xdotool key --window %lu Return", window, window);
		system(command);

		system("sleep 1");

		clearcommand();
		sprintf(command, "xdotool search --name \"BbTestReady%x\" > TempWindow.txt", counter);
		system(command);
	
		input = NULL;
		input = fopen("TempWindow.txt", "rt");
		if (!input)
		{
			printf("Error in 'waitcommand()'\n");
			return 0;
		}
	
		check = 0;
			
		bytes = fscanf(input, "%lu", &check);
		
		if (bytes > 0 && check == window)
		{
			loop = false;
		}
		else if (bytes > 0)
		{
			while (bytes > 0)
			{
				bytes = fscanf(input, "%lu", &check);
				
				if (bytes > 0 && check == window)
				{
					loop = false;
				}
			}
		}
	
		fclose(input);
	}

	counter++;
	
	return 1;
};	

void anchorcommand(const char *n)
{
	clearcommand();
	sprintf(command, "xdotool windowfocus %lu ; xdotool type --window %lu \"for(var i=0;i<document.getElementsByTagName('a').length;i++){if(document.getElementsByTagName('a')[i].innerHTML.search('%s')>=0){document.getElementsByTagName('a')[i].click();}}\"", window, window, n);
	system(command);

	clearcommand();
	sprintf(command, "xdotool windowfocus %lu ; xdotool key --window %lu Return", window, window);
	system(command);

	return;
};

void inputcommand(const char *n, const char *t)
{
	clearcommand();
	sprintf(command, "xdotool windowfocus %lu ; xdotool type --window %lu \"document.getElementById('%s').value='%s';\"", window, window, n, t);
	system(command);

	clearcommand();
	sprintf(command, "xdotool windowfocus %lu ; xdotool key --window %lu Return", window, window);
	system(command);

	return;
};

void submitcommand(const char *n)
{
	clearcommand();
	sprintf(command, "xdotool windowfocus %lu ; xdotool type --window %lu \"document.getElementById('%s').click();\"", window, window, n);
	system(command);

	clearcommand();
	sprintf(command, "xdotool windowfocus %lu ; xdotool key --window %lu Return", window, window);
	system(command);

	return;
};


int main(const int argc, const char **argv)
{
	if (argc < 5)
	{
		printf("You MUST use Chromium for this, due to permissions, etc...\n");
		printf("Also, you must use a 'new tab' and delete the old Ultra tab before start.\n");
		printf("Go to the Blackboard course, then go to Course Tools -> Tests, Surveys, and Pools\n");
		printf("Make sure that when you press F12 that it will pull up the console, and not something else.\n");
		printf("Do not have the console up when starting this script, it will do it for you.\n");
		printf("From there, it should automatically upload all of the pools for you!\n");
		printf("Do NOT touch the computer while it is uploading, it will probably take an hour...\n");

		printf("Also, the folder with all of the pools MUST be located at: ~/Desktop/BbTestGen/Pools\n");
		
		printf("Arguments: <File Prefix> <Bb Pool Prefix> <Question Start> <Question End>\n");
		printf("Example: ./BbTestUpload.o \"Quiz-1\" \"QuizV3-1\" 1 15\n");
	
		return 0;
	}

	for (int i=0; i<16; i++)
	{
		for (int j=0; j<256; j++)
		{
			path[i][j] = 0;
		}
	}

	sprintf(path[0], "Desktop");
	sprintf(path[1], "BbTestGen");
	sprintf(path[2], "Pools");

	system("xdotool search --name \"Tests, Surveys, and Pools\" > TempWindow.txt");

	input = fopen("TempWindow.txt", "rt");
	if (!input)
	{
		printf("Error reading TempWindow.txt\n");
		return 0;
	}
	
	window = 0;

	bytes = fscanf(input, "%lu", &window);

	if (bytes <= 0 || window == 0)
	{
		printf("Error finding window with 'Tests, Surveys, and Pools'\n");
		return 0;
	}

	while (bytes > 0)
	{
		bytes = fscanf(input, "%lu", &check);
		
		if (bytes > 0) window = check;
	}

	fclose(input);

	clearcommand();
	sprintf(command, "xdotool windowfocus %lu ; xdotool key --window %lu F12", window, window);
	system(command);

	system("sleep 5");

	if (!waitcommand()) return 0;

	anchorcommand("Pools");	

	int start_questions = atoi(argv[3]);

	int total_questions = atoi(argv[4]);

	//for (int style=style_start; style>=1; style--)
	//{
		//for (int lesson=lesson_start; lesson<=6; lesson++)
		//{
			for (int question=start_questions; question<=total_questions; question++)
			{
				if (!waitcommand()) return 0;
	
				anchorcommand("Build Pool");
	
				if (!waitcommand()) return 0;
	
				for (int j=0; j<1024; j++) name[j] = 0;
				sprintf(name, "%s-%02d", argv[2], question);
				inputcommand("assessment_name_input", name);
				submitcommand("bottom_Submit");
	
				if (!waitcommand()) return 0;
	
				anchorcommand("Upload Questions");
		
				if (!waitcommand()) return 0;
	
				submitcommand("importFile_localBrowse");
			
				system("sleep 3"); // just in case
		
				clearcommand();
				sprintf(command, "xdotool search --name \"Open File\" > TempWindow.txt");
				system(command);
			
				input = NULL;
	
				input = fopen("TempWindow.txt", "rt");
				if (!input)
				{
					printf("Error reading TempWindow.txt");
					return 0;
				}
	
				browse = 0;
			
				bytes = fscanf(input, "%lu", &browse);
	
				if (bytes <= 0 || browse == 0)
				{
					printf("Error finding window with 'Open File'\n");
					return 0;
				}
		
				while (bytes > 0)
				{
					bytes = fscanf(input, "%lu", &check);
					
					if (bytes > 0) browse = check;
				}
		
				fclose(input);
	
				for (int j=0; j<16; j++)
				{
					if (path[j][0] != 0)
					{
						clearcommand();
						sprintf(command, "xdotool windowfocus %lu ; xdotool key --window %lu Ctrl+F ; sleep 1", browse, browse);
						system(command);
		
						clearcommand();
						sprintf(command, "xdotool type --window %lu \"%s\" ; sleep 1 ; xdotool key --window %lu Return ; sleep 1", browse, path[j], browse);
						system(command);
					}
				}
		
				clearcommand();
				sprintf(command, "xdotool windowfocus %lu ; xdotool key --window %lu Ctrl+F ; sleep 1", browse, browse);
				system(command);
		
				clearcommand();
				sprintf(command, "xdotool type --window %lu \"%s-%02d.txt\" ; sleep 1 ; xdotool key --window %lu Return", browse, argv[1], question, browse);
				system(command);
		
				system("sleep 1");
			
				submitcommand("bottom_Submit");
	
				if (!waitcommand()) return 0;
		
				clearcommand();
				sprintf(command, "xdotool windowfocus %lu ; xdotool type --window %lu \"document.getElementsByClassName('backLink')[0].getElementsByTagName('a')[0].click();\"", window, window);
				system(command);
		
				clearcommand();
				sprintf(command, "xdotool windowfocus %lu ; xdotool key --window %lu Return", window, window);
				system(command);	
			}
		//}	
	//}	
	
	system("sleep 5");

	clearcommand();
	sprintf(command, "xdotool windowfocus %lu ; xdotool type --window %lu \"console.log('Done!');\"", window, window);
	system(command);

	clearcommand();
	sprintf(command, "xdotool windowfocus %lu ; xdotool key --window %lu Return", window, window);
	system(command);

	printf("Done!\n");		
	
	return 1;
}
