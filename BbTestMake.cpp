// You need to install 'xdotool' to make this work!

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

unsigned long window = 0;
unsigned long check = 0;
unsigned long browse = 0;
unsigned long selection = 0;

FILE *input = NULL;
int bytes = 0;
bool loop = true;

char command[65536];
char name[1024];

unsigned char counter = 0;


void alarm()
{
/*
	while (true)
	{
		system("play Laser.mp3");	
		
		unsigned long temp = 0;

		while (temp < 10000000)
		{
			temp++;
		}
	}
*/

	return;
};


void clearcommand()
{
	for (int i=0; i<65536; i++) command[i] = 0;
	
	return;
};

int waitcommand(unsigned long w)
{
	loop = true;

	while (loop)
	{
		system("sleep 1");

		clearcommand();
		sprintf(command, "xdotool windowfocus %lu ; xdotool type --window %lu \"var myint=setInterval(function(){if(document.readyState=='complete'){document.title='BbTestReady%x';clearInterval(myint);}},1000);\"", w, w, counter);
		system(command);

		clearcommand();
		sprintf(command, "xdotool windowfocus %lu ; xdotool key --window %lu Return", w, w);
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
		
			alarm();
			
			return 0;
		}
	
		check = 0;
			
		bytes = fscanf(input, "%lu", &check);
		
		if (bytes > 0 && check == w)
		{
			loop = false;
		}
		else if (bytes > 0)
		{
			while (bytes > 0)
			{
				bytes = fscanf(input, "%lu", &check);
				
				if (bytes > 0 && check == w)
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

void checkcommand(const char *n, const int v)
{
	clearcommand();
	sprintf(command, "xdotool windowfocus %lu ; xdotool type --window %lu \"for(var i=0;i<document.getElementsByClassName('columnPalette')[0].getElementsByTagName('input').length;i++){if(document.getElementsByClassName('columnPalette')[0].getElementsByTagName('label')[i].innerHTML.search('%s%02d')>=0){document.getElementsByClassName('columnPalette')[0].getElementsByTagName('input')[i].click();}}\" ; xdotool key --window %lu Return", selection, selection, n, v, selection);
	system(command);

	//system("sleep 1");

	//clearcommand();
	//sprintf(command, "xdotool windowfocus %lu ; xdotool key --window %lu Return", selection, selection);
	//system(command);

	system("sleep 2");

	clearcommand();
	sprintf(command, "xdotool windowfocus %lu ; xdotool type --window %lu \"document.getElementsByClassName('columnPalette')[1].getElementsByTagName('input')[0].click();\" ; xdotool key --window %lu Return", selection, selection, selection);
	system(command);

	//system("sleep 1");

	//clearcommand();
	//sprintf(command, "xdotool windowfocus %lu ; xdotool key --window %lu Return", selection, selection);
	//system(command);

	system("sleep 2");

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
	if (argc < 4)
	{
		printf("You MUST use Chromium for this, due to permissions, etc...\n");
		printf("Also, you must use a 'new tab' and delete the old Ultra tab before start.\n");		
		printf("Go to the Blackboard course, then go to whatever folder you want the assessment.\n");
		printf("Click on Assessments -> Test -> Create, type a name, and have it ready to create questions.\n");
		printf("You can also edit an existing test that is blank, either way.\n");
		printf("Make sure that when you press F12 that it will pull up the console, and not something else.\n");
		printf("Do not have the console up when starting this script, it will do it for you.\n");
		printf("From there, it should automatically upload all of the pools for you!\n");
		printf("Do NOT touch the computer while it is uploading, it will probably take some hours...\n");
	
		printf("Arguments: <Bb Pool Prefix> <Question Start> <Question End>\n");
		printf("Example: ./BbTestMake.o \"QuizV3-1\" 1 15\n");

		return 0;
	}

	system("xdotool search --name \"Test Canvas\" > TempWindow.txt");

	input = fopen("TempWindow.txt", "rt");
	if (!input)
	{
		printf("Error reading TempWindow.txt\n");
	
		alarm();

		return 0;
	}
	
	window = 0;

	bytes = fscanf(input, "%lu", &window);

	if (bytes <= 0 || window == 0)
	{
		printf("Error finding window with 'Test Canvas'\n");

		alarm();

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


	char name[256];

	int start_questions = atoi(argv[2]);
	int total_questions = atoi(argv[3]);
	
	for (int i=start_questions; i<=total_questions; i++)
	{
		if (!waitcommand(window)) return 0;

		anchorcommand("Create Random Block");	
		
		system("sleep 1"); // just in case
	
		clearcommand();
		sprintf(command, "xdotool search --name \"Create Random Block\" > TempWindow.txt");
		system(command);
		
		input = NULL;

		input = fopen("TempWindow.txt", "rt");
		if (!input)
		{
			printf("Error reading TempWindow.txt");

			alarm();

			return 0;
		}

		browse = 0;
		
		bytes = fscanf(input, "%lu", &browse);

		if (bytes <= 0 || browse == 0)
		{
			printf("Error finding window with 'Create Random Block'\n");

			alarm();

			return 0;
		}

		while (bytes > 0)
		{
			bytes = fscanf(input, "%lu", &check);
			
			if (bytes > 0) browse = check;
		}
	
		fclose(input);

		system("sleep 1");

		clearcommand();
		sprintf(command, "xdotool windowfocus %lu ; xdotool key --window %lu F12", browse, browse);
		system(command);

		system("sleep 5");

		clearcommand();
		sprintf(command, "xdotool search --name \"DevTools\" > TempWindow.txt");
		system(command);
		
		input = NULL;

		input = fopen("TempWindow.txt", "rt");
		if (!input)
		{
			printf("Error reading TempWindow.txt");

			alarm();

			return 0;
		}

		selection = 0;
		
		bytes = fscanf(input, "%lu", &selection);

		if (bytes <= 0 || selection == 0)
		{
			printf("Error finding window with 'DevTools'\n");

			alarm();
	
			return 0;
		}

		while (bytes > 0)
		{
			bytes = fscanf(input, "%lu", &check);
			
			if (bytes > 0) selection = check;
		}
	
		fclose(input);

		//if (!waitcommand(selection)) return 0;

		for (int i=0; i<256; i++) name[i] = 0;
		sprintf(name, "%s-", argv[1]);
		checkcommand(name, i);

		clearcommand();
		sprintf(command, "xdotool windowfocus %lu ; xdotool type --window %lu \"document.getElementsByClassName('submit')[0].click();\"", selection, selection);
		system(command);

		clearcommand();
		sprintf(command, "xdotool windowfocus %lu ; xdotool key --window %lu Return", selection, selection);
		system(command);

		system("sleep 1");
	}

	system("sleep 5");

	clearcommand();
	sprintf(command, "xdotool windowfocus %lu ; xdotool type --window %lu \"console.log('Done!');\"", window, window);
	system(command);

	clearcommand();
	sprintf(command, "xdotool windowfocus %lu ; xdotool key --window %lu Return", window, window);
	system(command);

	printf("Done!\n");

	
	alarm();
	
	return 1;
}
