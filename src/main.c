#include <stdlib.h>

int main(int argc, char *argv[])
{
	if (argc <= 1 || argv == nullptr || argv[1] == nullptr)
	{
		(void)argc;
		(void)argv;
	}
	else
	{
		for (int i = 1; i < argc; i++)
		{
			(void)printf("argv[%d]: %s\n", i, argv[i]);
		}
	}

	return EXIT_SUCCESS;
}
