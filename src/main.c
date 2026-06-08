#include <stdlib.h>

int main(int argc, char *argv[])
{
	if (argc <= 1 || argv == nullptr || argv[1] == nullptr)
	{
		(void)argc; // explicitly mark argc as unused in this branch
		(void)argv; // explicitly mark argv as unused in this branch
	}
	else
	{
		// i = 1 to skip argv[0] which holds the program name
		for (int i = 1; i < argc; i++)
		{
			(void)printf("argv[%d]: %s\n", i, argv[i]); // print each argument
		}
	}

	return EXIT_SUCCESS; // return success exit code
}
