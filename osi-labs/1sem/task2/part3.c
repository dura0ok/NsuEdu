#include <sys/ptrace.h>
#include <sys/user.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

//System call -1 made by child <-- это не сисколл, это debug signal
//System call 1 made by child <--- вход в сисколл write
//Hi1
//System call 1 made by child <--- выход
//System call 1 made by child <--- вход в следующий сисколл write
//Hi2
//System call 1 made by child <--- выход
//System call 231 made by child <- вход в сисколл exit_group


int main(){
    
    int  status = 0, pid, r;
    struct user_regs_struct uregs;
    if ((pid=fork())==0) {
        printf("pid = %d\n", getpid());


        /*
         сделает родителя текущего процесса отладчиком для него. 
         «A process probably shouldn't make this request if its parent isn't expecting 
         to trace it.» 
         Если у текущего процесса уже был отладчик, то вызов не удастся.
        */

        // type, parent pid, offset, data
        // traceme offset, data not used
        if(ptrace(PTRACE_TRACEME, 0, 0, 0) < 0){
            printf("Something went wrong");
            exit(1);
        }

         // execl("/bin/ls", "ls", NULL);
        // int 3 - debug signal
        //The __volatile__ modifier on an __asm__ block forces the compiler's optimizer to execute the code as-is.
        write(1, "hi1\n", 4);
        write(1, "hi2\n", 4);
        asm volatile ("int3");
        


       
       // write(1, "Hi\n", 3);
       //  execl("/bin/echo", "echo", "asdasd", NULL);
        
    
	    
        // kill(getpid(), SIGINT);
        // r = getpid();
        // printf("%d\n", r);

    } else {
             /*
           The wait() system call suspends execution of the calling thread
       until one of its children terminates.  The call wait(&wstatus) is
       equivalent to: waitpid(-1, &wstatus, 0);

       The value of pid can be:

       < -1   meaning wait for any child process whose process group ID
              is equal to the absolute value of pid.

       -1     meaning wait for any child process.

       0      meaning wait for any child process whose process group ID
              is equal to that of the calling process at the time of the
              call to waitpid().

       > 0    meaning wait for the child whose process ID is equal to
              the value of pid.

       The value of options is an OR of zero or more of the following
       constants:
    

       */
      
        /*
         *
           Вызову передаются четыре аргумента, где request -- определяет что
           необходимо сделать. pid -- это ID трассируемого процесса. addr -- это
           смещение в пользовательском пространстве трассируемого процесса,
           откуда будет прочитано слово данных и возвращено в качестве результата
           работы вызова.

           Родительский процесс может породить дочерний процесс и выполнять его
           трассировку посредством вызова ptrace с аргументом request, имеющим
           значение PTRACE_TRACEME. Процесс-трассировщик может выполнять
           трассировку уже существующего процесса, используя значение
           PTRACE_ATTACH для аргумента request. Значения, которые может принимать
           аргумент request, обсуждаются ниже.
         */

        //prtrace(PTRACE_SETOPTIONS, pid, NULL, PTRACE_O_TRACESYSGOOD)

        // PTRACE_SYSCALL — продолжить выполнение 
        //программы до входа или выхода из системного вызова
        ptrace(PTRACE_SYSCALL, pid, 0, 0);
        wait(&status);
        ptrace(PTRACE_GETREGS, pid, 0, &uregs);
            //this should print 20, syscall number of getpid
            /*
            Следует отметить, что регистр eax в момент остановки заменен, и поэтому необходимо использовать 
            сохраненный state.orig_eax:
            */
        printf("syscall nr: %lld\n", uregs.orig_rax);
            // 64 is syscall number of getppid
            //uregs.orig_rax = 64; // восстанавливаем регистр
            //ptrace(PTRACE_SETREGS, pid, 0, &uregs);
        ptrace(PTRACE_CONT, pid, 0, 0);

   
    
        wait(&status);

        // child terminated normally
        if(WIFEXITED(status)){
            printf("child over\n");
        }
    }
}

