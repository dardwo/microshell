#include <sys/wait.h> //wait()
#include <unistd.h> // fork(), exec(), pid_t
#include <stdlib.h> //malloc()
#include <stdio.h> //printf(), perror(), exit(), free()
#include <string.h> //strcmp(),strtok()
#include <errno.h> //errno
#include <sys/types.h>
#include <fcntl.h>
#include <sys/stat.h> //stat 

#define PATH_MAX 256
#define WIELKOSC_BUFORA 256

#define COLOR_RESET "\x1b[0m"
#define RED "\x1b[31m"
#define MAGENTA "\x1b[35m"
#define CYAN "\x1b[36m"

void wyswietl_znak_zachety(){
    char nazwa[PATH_MAX];
    getcwd(nazwa, PATH_MAX);
    printf(CYAN);
    printf("[%s]", nazwa);
    printf(COLOR_RESET);
}

void polecenie_help(){
    printf(MAGENTA);
    printf("Autor programu: Daria Dworzyńska.\n");
    printf("Dostępne polecenia: \n");
    printf("help - wpisz help jeśli chcesz uzyskać pomoc\n");
    printf("exit - powoduje zakończenie działania powłoki\n");
    printf("cd - (skrót ang. change dirctory) - zmiana aktualnego katalogu roboczego\n");
    printf("clear - czyści terminal\n");
    printf("cp - (cp plik1.c plik2.c) - kopiowanie zawartości jednego pliku do drugiego\n");
    printf("grep - (grep słowo nazwa_pliku.c) - skanuje pliki w poszukiwaniu określonych wzorców\n");
    printf("head - (head plik.c) - wyświetli pierwsze 10 linijek pliku\n");
    printf("head -n - (head -n k plik.c) - wyświetli pierwsze k linijek tekstu (gdzie k jest liczbą int)\n");
    printf(COLOR_RESET);
}



int main(){
        char poprzedni_katalog[WIELKOSC_BUFORA];
        getcwd(poprzedni_katalog, sizeof(poprzedni_katalog));

        while(1){

        wyswietl_znak_zachety();

        int tmp = sizeof(char*) *30;
        char **argumenty = malloc(tmp);

        char komenda[WIELKOSC_BUFORA];
        fgets(komenda, WIELKOSC_BUFORA, stdin);

        char *nie_wczytuj = " \n\t";
        char *wynik;
        int i=0;
        wynik = strtok(komenda, nie_wczytuj);
        while(wynik != NULL){
            argumenty[i]=wynik;
            i++;
            wynik=strtok(NULL, nie_wczytuj);
        }
        argumenty[i] = NULL;

        pid_t pid;
        
        if(argumenty[0]==NULL)
        {
            continue;
        }
        
        else if(strcmp(argumenty[0], "cd")==0){
            if(i<3)
            {
                if(argumenty[1]==NULL){
                    getcwd(poprzedni_katalog, sizeof(poprzedni_katalog));
                    chdir(getenv("HOME"));
                }
                else if(strcmp(argumenty[1],"~")==0){
                    getcwd(poprzedni_katalog, sizeof(poprzedni_katalog));
                    chdir(getenv("HOME"));
                }
                else if(strcmp(argumenty[1],"-")==0){
                    chdir(poprzedni_katalog);
                }
                else if(strcmp(argumenty[1],".")==0){
                    getcwd(poprzedni_katalog, sizeof(poprzedni_katalog));
                    chdir(".");
                }
                else if(strcmp(argumenty[1],"..")==0){
                    getcwd(poprzedni_katalog, sizeof(poprzedni_katalog));
                    chdir("..");
                }
                else{
                    getcwd(poprzedni_katalog, sizeof(poprzedni_katalog));
                    chdir(argumenty[1]);
                }
            }
            else{
                printf("Za dużo argumentów\n");
            }
        }

        else if(strcmp(argumenty[0], "help")==0){
            polecenie_help();
        }

        else if(strcmp(argumenty[0], "exit") == 0){
            printf("Zakończenie działania programu.\n");
            exit(0);
        }
        
        else if(strcmp(argumenty[0], "cp") == 0){

            if(i != 3){
                printf("Niepoprawne dane, zapoznaj się z instrukcją użytkowania\n");
                continue;
            }

            int f1, f2, num;
            f1 = open(argumenty[1], O_RDONLY);

            if(f1 == -1){
                printf("Niepoprawne dane, zapoznaj się z instrukcją użytkowania\n");
                continue;
            }

            f2 = creat(argumenty[2], 0644);
            char buffer[WIELKOSC_BUFORA];

            while((num = read(f1, &buffer, WIELKOSC_BUFORA))>0){
                write(f2, &buffer, num);
            }
                
            close(f1);
            close(f2);
        }

        else if(strcmp(argumenty[0], "grep") == 0){

            struct stat stt;
            if(i==3){
                if(stat(argumenty[2], &stt) == 0){
                    FILE *f;
                    f = fopen(argumenty[2], "r");
                    char line[100];
                    if(f == NULL){
                        printf("Nie udało się otworzyć pliku\n");
                    }
                    else{
                        while(fgets(line, sizeof(line), f)){
                            if(strstr(line, argumenty[1])){
                                printf("%s",line);
                            }
                        }
                    }

                    fclose(f);
                }
                else{
                    printf("%s: nie jest plikiem\n", argumenty[2]);
                }
            }
            else{
                printf("Zapoznaj się z instrukcją działania polecenia grep\n");
            }
        }

        else if(strcmp(argumenty[0], "head") == 0){

            if(i == 2){
                FILE *f;
                f = fopen(argumenty[1], "r");
                int n=10;
                char line[100];
                if(f != NULL){
                    while(fgets(line, sizeof(line), f)){
                        if(n==0){
                            break;
                        }
                        printf("%s", line);
                        n--;
                    }
                }
                else{
                    printf("Nie udało się otworzyć pliku\n");
                }

                fclose(f);
            }

            //jeśli podamy konkretną ilość linijek np. -head -n 5 nazwapliku.c
            else if(i == 4){
                FILE *f;
                f = fopen(argumenty[3], "r");
                char line[100];
                if(f != NULL){
                    if(strcmp(argumenty[1], "-n") == 0){
                        int k;
                        sscanf(argumenty[2], "%d", &k);
                        while(fgets(line, sizeof(line), f)){
                            if(k==0){
                                break;
                            }
                            printf("%s", line);
                            k--;
                        }
                    }
                }
                else{
                    printf("Nie udało się otworzyć pliku\n");
                }
                fclose(f);
            }
                
            else{
                printf("Zapoznaj się z działaniem polecenia head\n");
            }
        }

        else if(strcmp(argumenty[0], "clear") == 0){
            printf("\033[2J\033[1;1H");
        }

        else{
            pid = fork();

            if(pid == -1){
                printf("Unable to create pipe\n");
                exit(EXIT_FAILURE);
            }
            if(pid == 0){
                execvp(argumenty[0], argumenty);
                printf(RED);
                printf("Error number: %d\n", errno);
                printf(COLOR_RESET);
                exit(EXIT_FAILURE);
            }
            else{
                wait(NULL);
            }
        }
        free(argumenty);
        }


    return EXIT_SUCCESS;
}
