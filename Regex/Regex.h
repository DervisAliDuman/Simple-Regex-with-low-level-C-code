#include <fcntl.h>
#include <stdio.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include<dirent.h>
#include<signal.h>

int findREGEX(char* regex);
int compareInsensitive(int inSensitive,char *ch1, char *ch2);

enum REGEX {inCaseSensitive= 1, lineStart = 2, lineEnd = 4}; 

typedef enum STATE{
    START,
    WAIT,
    S0,
    S1,
    S2,
    RESET,
    REPLACE
}FSM_STATES;

typedef struct FSM{
  char *str1;
  int length;
  int cursor;
  int repeat;
  char comparewith;
  char *squareBracet;
  FSM_STATES state;

}FSM_STATE_DATA;


char* FSM_start(char* manipulate, int enumReturned, char* str1, char* str2) {
    char squareBracet[2] = {'a','b'};
    FSM_STATE_DATA machine = {str1,strlen(str1),0,0,' ',squareBracet };
    char *iter = manipulate;

    int alloc = 0;

    while(*iter != 0){
        iter++;
        alloc++;
    }
    iter = manipulate;

    char *completed  = (char*)malloc(alloc*4*sizeof(char));
    char *temp2  = (char*)malloc(2*sizeof(char));
    char *temp  = (char*)malloc((strlen(manipulate)*2)*sizeof(char));
    
    free(temp);

    int inSensitive = 0,lineS = 0,lineE = 0;

    if(enumReturned == 1 || enumReturned == 3 || enumReturned == 5)
        inSensitive = 1;
    if(enumReturned == 2 || enumReturned == 3)
        lineS= 1;
    if(enumReturned == 4 || enumReturned == 5)
        lineE= 1;
    
    //printf("\nStruct is:  %s %d %d %d %c %c %c \n",machine.str1,machine.length,machine.cursor, machine.repeat, machine.comparewith, machine.squareBracet[0], machine.squareBracet[1] );
    
    int i = 0,k=0,m=0;
    char ch[20];
    int successComparison = 0;
    temp2[0] = '$';

    machine.state = S0;

    while (*iter != 0) {
        
        if(machine.state == RESET ){
            iter = iter -m;
            while(m> 0){
                completed[i] = *iter;
                iter++;
                m--;
                i++;
            }

            completed[i] = *iter;
            i++;
            iter++;
            m=0;
            successComparison =0;
            machine.cursor = 0;
            machine.state = S0;
        }

        if(machine.cursor < machine.length && str1[machine.cursor] == '*'){
            iter--;
            //printf("\ncompare : %c  with  %c \n", *iter,machine.str1[machine.cursor-1]);  
            ch[0] = machine.str1[machine.cursor-1];
            if(compareInsensitive(inSensitive,ch,iter) == 1){
                machine.comparewith = *iter;
                machine.state = S2;
                machine.cursor+=1;
            
            }else{
                machine.state = S0;
                machine.cursor+=1;
            }
            iter++;
        }else if(machine.cursor < machine.length && str1[machine.cursor] == '['){
            machine.cursor++;
            char sq[20];
            int l = 0;
            while(str1[machine.cursor] != ']'){
                sq[l] = str1[machine.cursor];
                machine.cursor++;
                l++; 
            }
            machine.squareBracet = sq;

            if(machine.str1[machine.cursor+1] == '*'){
                machine.cursor++;
                machine.state = S0;
                //printf("\ncompare : %c  with  %c and %c\n", *iter,machine.squareBracet[0],machine.squareBracet[1]);  
                while(l>0){
                    ch[0] = sq[l-1];
                    if(compareInsensitive(inSensitive,ch,iter) == 1){
                        machine.comparewith = ch[0];
                        machine.state = S2;
                         machine.cursor+=1;
                        break;
                    }
                    l--;
                }
                if(machine.state != S2){
                    machine.state = S0;
                    machine.cursor+=1;
                }
            }else{ 
                //printf("\ncompare : %c  with  %c and %c\n", *iter,machine.squareBracet[0],machine.squareBracet[1]);                   
                while(l>0){
                    ch[0] = sq[l-1];
                    if(compareInsensitive(inSensitive,ch,iter) == 1){
                        machine.comparewith = ch[0];
                        machine.state = S1;
                         machine.cursor+=1;
                        break;
                    }
                    l--;
                }
                if(machine.state != S1){
                    machine.state = RESET;
                }

                
            }
        }else if(machine.state != RESET){
            machine.state = S0;
        }



        if(machine.state == S1){
            
            ch[0] = machine.comparewith;
            if( *iter != 0 && machine.cursor < machine.length && compareInsensitive(inSensitive,ch,iter) == 1 ){ 
                machine.cursor++;
                iter++;
                m++;
                ch[0] = machine.str1[machine.cursor];
                successComparison++;
                machine.state = START;
            }

        }

        if(machine.state == S2){
            ch[0] = machine.comparewith;
            while( *iter != 0 && machine.cursor < machine.length && compareInsensitive(inSensitive,ch,iter) == 1 && str1[machine.cursor] != '*' && str1[machine.cursor] != '['){ 
                iter++;
                m++;
                successComparison++;
            }
        }

        if(machine.state == S0){
            ch[0] = machine.str1[machine.cursor];
            
            while( *iter != 0 && machine.cursor < machine.length && compareInsensitive(inSensitive,ch,iter) == 1 && str1[machine.cursor] != '*' && str1[machine.cursor] != '['){ 
                machine.cursor++;
                iter++;
                m++;
                ch[0] = machine.str1[machine.cursor];
                successComparison++;
            }
            if(str1[machine.cursor+1] == '*' ){
                machine.cursor++;
            }
            if(machine.cursor < machine.length && (str1[machine.cursor] != '*' && str1[machine.cursor] != '[') && compareInsensitive(inSensitive,ch,iter) == 0 ){
                machine.state = RESET;
            }
            if(*(iter )== 0 && machine.cursor < machine.length){
                machine.state = RESET;
            }
            
        }

        if(machine.cursor >= machine.length && machine.state != RESET){
            if(lineS == 0 && lineE == 0){
                k = 0;
                while(k < strlen(str2)){
                    completed[i] = str2[k];
                    i++; k++;
                }                                                       
                m=0;
                successComparison =0;
                machine.cursor = 0;
                machine.state = S0;
            }

            if(lineS == 1 && i==0){
                k = 0;
                while(k < strlen(str2)){
                    completed[i] = str2[k];
                    i++; k++;
                }
                //machine.state = RESET;                                                           
                m=0;
                successComparison =0;
                machine.cursor = 0;
                machine.state = S0;
            }
            if(*(iter+1) == 0 && lineE == 1 ){
                k = 0;
                while(k < strlen(str2)){
                    completed[i] = str2[k];
                    i++; k++;
                }
                //machine.state = RESET;                                                           
                m=0;
                successComparison =0;
                machine.cursor = 0;
                machine.state = S0;
            }
            if(lineS == 1 && i != 0){
                machine.state = RESET;
            }
            if(lineE == 1 && *(iter+1) != 0 ){
                machine.state = RESET;
            }

            
        }

        if(machine.state == RESET ){
            iter = iter -m;
            while(m> 0){
                completed[i] = *iter;
                iter++;
                m--;
                i++;
            }

            completed[i] = *iter;
            i++;
            iter++;
            m=0;
            successComparison =0;
            machine.cursor = 0;
            machine.state = S0;
        }



    }

    completed[i] = '\0';
    free(manipulate);
    //free(temp);
    return completed;
}

void mywrite(char* path ,char* completed,int length){
    
    mode_t mode = S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH ;

    int fd = open( path , O_RDWR | O_TRUNC, mode  );
    if(fd == -1){
        perror("\n***********************************\nFILE INPUT OUTPUT ERROR\n***********************************\n");
        exit(1);
    }

    struct flock lock;

    if(*completed == '\n'){
        completed++;
    }
    memset(&lock,0,sizeof(lock));
    lock.l_type = F_WRLCK;
    if(fcntl(fd,F_SETLKW,&lock) == -1){
        perror("Error: Filelock didn't locked\n");
        _exit(EXIT_FAILURE);
    }
    
    write(fd,completed,strlen(completed)*sizeof(char));

    
    lock.l_type = F_UNLCK;
    if(fcntl(fd,F_SETLKW,&lock) == -1){
        perror("Error: Filelock didn't unlocked\n");
        _exit(EXIT_FAILURE);
    }
    close (fd);


    
}


char* Operation(char* path,char *buffer,int length, char* task,char* regex){
    int enumReturned = 0;
	enumReturned = findREGEX(task);
    char* ptr1 = strtok(task, "/;\"';^$");
    char* str1 = (char*)malloc((strlen(regex))*sizeof(char));
    char* str2 = (char*)malloc((strlen(regex))*sizeof(char));
    int i = 0;
	while(ptr1 != NULL && i<2){
		//printf("REGEX:'%s'\n", ptr1);
        if(i == 0) strcpy(str1,ptr1);
        else if(i == 1) strcpy(str2,ptr1);
		ptr1 = strtok(NULL, "/;\"';^$");
        i++;
	}
    //printf("STR1:'%s'\n", str1);
    //printf("STR2:'%s'\n", str2);

    char *temp  = (char*)malloc((strlen(buffer)+20)*sizeof(char));
    char *completed  = (char*)malloc((strlen(buffer)*2)*sizeof(char));
    int k = 0,in = 0,m = 0,j=0;;
    completed[0] = '\0';
    while(buffer[m] != '\0'){
        k = 0;
        in = 0;
        while(buffer[m]!= '\0' && buffer[m] != '\n'){
            temp[k] = buffer[m];
            k++;
            m++;
            in = 1;
        }
        temp[k] = '\0';
        
        temp = FSM_start(temp,enumReturned,str1,str2);   //HERE MY FSM
        
        j=0;
        while(completed[j]!= '\0')
            j++;
        
        if(strlen(temp) > 1){
            completed[j++] = '\n';
        }
        
        completed[j] = '\0';

        k = 0;
        while(temp[k] != '\0'){
            completed[j] = temp[k]; 
            j++;
            k++;
        }
        completed[j] = '\0';

        if(in == 0 || buffer[m] == '\n')
            m++;
    }

    
    printf("\nEND : %s \n", completed );
    mywrite(path,completed,length);

    //free(completed);
    free(temp);
    free(str1);
    free(str2);

    return completed;
}

int findREGEX(char* regex){
    
    int sum = 0,i = 0;
    
    for (i = 0; regex[i] != '\0'; i++) {
        if (regex[i] == '^'){
            sum+=lineStart;
        } else if ('$' == regex[i]){
                sum+=lineEnd;
        } else if('i'== regex[i] && regex[i+1] == '\0'){
            sum+= inCaseSensitive;
        }
    }
    
    return sum;
}


void myRegex(char* path,char* buffer, int length, char* regex){

    char *temp  = (char*)malloc(strlen(regex)*sizeof(char));
    int m = 0,i = 0;
    int k = 0;
    int in = 0;
    char * newtemp ;
    char *completed = (char*)calloc(4*strlen(buffer)*sizeof(char),4*strlen(buffer)*sizeof(char));

    for(i = 0; i< length; i++){
        completed[i] = buffer[i];
    }
    completed[i] = '\0';

    while(regex[m]!= '\0'){
        
        k = 0;
        in = 0;
        while(regex[m]!= '\0' && regex[m] != ';'){
            temp[k] = regex[m];
            k++;
            m++;
            in = 1;
        }
        temp[k] = '\0';
        //printf("\n OPERATION WILL MADE: %s " , temp);
        newtemp = Operation(path,completed,strlen(completed),temp,regex);
        for(i = 0; i< length; i++){
            completed[i] = newtemp[i];
        }
        completed[i] = '\0';
        if(in == 0 || regex[m] == ';')
            m++;
    }
    
    free(temp);
    free(completed);
}



void lockAndRead(int fd,char*path, int size, char* regex){
    struct flock lock;

    ssize_t length = 0;
    size_t i = 0;
    char *buffer = (char*)calloc(size*2,sizeof(char));
    memset(&lock,0,sizeof(lock));
    lock.l_type = F_WRLCK;
    if(fcntl(fd,F_SETLKW,&lock) == -1){
        perror("Error: Filelock didn't locked\n");
        _exit(EXIT_FAILURE);
    }
    length = read(fd, buffer ,size*2*sizeof(char));

    printf ("\nLine number is %ld -> \n%s (%ld chars)\n", i++, buffer, length);  

    lock.l_type = F_UNLCK;
    if(fcntl(fd,F_SETLKW,&lock) == -1){
        perror("Error: Filelock didn't unlocked\n");
        _exit(EXIT_FAILURE);
    }
    close (fd);

    if(length <= 0){
        perror("\n***************************************************\nEmpty File or Reading error occured.\n***************************************************\n");
        exit(1);
    }

    myRegex(path,buffer, length, regex);

    free(buffer);
}

void reader(char * path, char* regex){

    struct stat st; 
    int size = 0;
    if (stat(path, &st) == 0){
        size = st.st_size;
    }
    else{
        exit(1);  
    }
    if(size == 0){
        perror("File is empty!! \n");
        exit(1);
    }

    int fd = open( path , O_RDWR);

    if(fd == -1){
        perror("Error: File cannot be opened.\n");
        exit(1);
    }

    lockAndRead(fd,path, size,regex);

    close(fd);
}



int lowerChar(char ch){
    if(ch >= 'A' && ch <= 'Z') return (ch+32);
    else return ch;
}

int compareInsensitive(int inSensitive,char *ch1, char *ch2){
    if(*ch1 == *ch2) return 1;
    if(inSensitive == 1 && lowerChar(*ch1) == lowerChar(*ch2)) return 1;
    return 0;
}


void inputCheck(int argc){
    if(argc != 3){
        printf("\n**************************************************\nInproper input, you must use like:  \n./hw1 '/^Window[sz]*/Linux/i;/close[dD]$/open/' b.txt \n**************************************************\n\n");
        _exit(1);
    }
}

void signalHandler(int signal) {
   printf("\n\n************************************************\nSignal Caught number: %d. CTRL+C INTERRUPT Terminating \n************************************************\n\n", signal);
   exit(1);
}

