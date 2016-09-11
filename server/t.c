#include <stdio.h>

int main()
{
	char current_absolute_path[1000];
	if(realpath("./",current_absolute_path) == NULL)
	{
		printf("error\n");
	}
	else
		printf("%s\n",current_absolute_path);
	return 0;
}
