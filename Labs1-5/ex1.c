 #include <stdio.h> 
 #include <stdlib.h>
 #include <errno.h>
 #include <unistd.h>
 #include <sys/types.h>
 #include <sys/wait.h>

int main()
{
   pid_t pid;
   int status;
   char *Path = "/bin/ls";
   char *args[] = {Path, "-a", "-l", NULL};
   


    if ((pid = fork())<0)
        {
        perror ("fork()");
        exit (1);
        }

    else if (pid)
        {
            if (wait (&status)<0)
                {
                perror("wait()");
                }
            printf("Succes\n");
            exit(0);
        }
    else
    {
      execv(Path, args);

      printf("Eroare\n");
      exit(1);
    }

    
}
