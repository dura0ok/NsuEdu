#include <unistd.h>                                                         
#include <fcntl.h>                                                          
#include <sys/types.h>                                                      

#define CHAR_SIZE 2048                                                      
#define BUFF_SIZE 1                                                         
#define PERMS 0666                                                          

int main(int argc, char* argv[]){                                           
    if (argc < 3) {                                                         
        printf ("ERROR: not enough arguments\n");                           
        return 1;                                                           
    }                                                                       
    int src_file = 0; int dest_file = 0;                                    
    int n = -1;                                                             
    if((src_file=open(argv[1],O_RDONLY)) == -1) return 1;                   
    if((dest_file = creat(argv[2], PERMS)) == -1) return 1;                 
    printf ("The filesize is: %d\n", lseek(src_file, (off_t) 0, SEEK_END)); 
    char buffer[BUFF_SIZE];                                                 
    lseek (src_file,n--,SEEK_END);                                          
    do {                                                                    
        read(src_file,buffer,BUFF_SIZE);                                    
        printf("%02x\n", *buffer);                                          
        if (*buffer == '\n')                                                
            n--;                                                            
        write(dest_file,buffer,BUFF_SIZE);                                  
    }while (lseek (src_file,n--,SEEK_END) > -1);                            
    printf("\n");                                                           
    return 0;                                                               

}   
