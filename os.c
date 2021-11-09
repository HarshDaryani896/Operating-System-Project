int main()
{
	pid_t pid1;
	pid1 = fork(); 
	if (pid1 < 0) 
	{ 
		fprintf(stderr, "Fork Failed");
		return 1;
	}
	else if (pid1 == 0) 
	{
		//C1
	}
	else 
	{ 
		pid_t pid2;
		pid2 = fork();
		if (pid2 < 0) 
		{ 
			fprintf(stderr, "Fork Failed");
			return 1;
		}
		else if (pid2 == 0) 
		{
			//C2
		}
		else 
		{ 
			pid_t pid3;
			pid3 = fork();
			if (pid3 < 0) 
			{ 
				fprintf(stderr, "Fork Failed");
				return 1;
			}
			else if (pid == 0) 
			{
				//C3
			}
			else 
			{ 
			//M
			}
		}
	}
	return 0;
}
