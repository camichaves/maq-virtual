#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <ctype.h>

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>

#include <sys/mman.h>
#include <semaphore.h>



int Contador[21];
const char* PalabrasReservadas[3]; //cuantas??
char buff[1500];
char buffauxiliar[1500];


//funcion cererar contador
void CerearContador(){
        for(int i=0; i<21;i++){
                Contador[i]=0;
        }
}
//fin func      

//Funcion lista palabras reservadas
void Reservadas(){
        PalabrasReservadas[0]="THESE";
        PalabrasReservadas[1]="ADVENTURE";
        PalabrasReservadas[2]="WITHOUT";
}


//Funcion Buscar pal reservada en hijo2
int Buscar(char* palab){


        char palabra[25];
        strcpy(palabra,palab);
        int j=0;
        for(int i=0; i<strlen(palabra); i++){
                palabra[i]=toupper(palabra[i]);
        }
        int nn=1;
        while(nn!=0 && j<3){
                nn=strcmp(palabra,PalabrasReservadas[j]);
                j++;
        }
        if(nn==0) return j-1;
        else return -1;
}


//Funcion Imprimir Palabra Reservada
int Resto(char* palabra, int indpal, int tam2){


        int tam=strlen(palabra);
        int ban=0;

        int i=0,j=0, aux;

        while(ban==0 && i<tam2){
                if(buffauxiliar[i]==palabra[j]){
                        aux=i;
                        while(buffauxiliar[aux]==palabra[j] && j<tam){
                                j++;
                                aux++;
                        }
                        if(tam==j) ban=1; else{ i++; j=0; }
                }
                else i++;


 }
       for(int g=0; g<i; g++){

                printf("%c",buffauxiliar[g]);
        }

        char buffaux[1500];
        strcpy(buffaux,buffauxiliar);

        memset(buffauxiliar,'\0',tam2);
        for(int g=aux;g<tam2;g++){
                buffauxiliar[g-aux]=buffaux[g];
        }
        int rta=(tam2-aux);

        printf("%s",PalabrasReservadas[indpal]);

        return rta;
}

//Funcion hijo 1 contador
int Hijo1(int *leido, char *ptr , sem_t *sema, sem_t *sem2){

        
  int tamanio;
        char * lineas[256];
        char d2[]="|123456789 - 0'¿°!«#$%&/()=—?¡[]_:;,.-{+}´ \n";

        int j;
        CerearContador();        
        while(*leido>0){
                for(int i= 0; i<*leido;i++){
		        buff[i]=ptr[i];
	        }
                j=0;
                lineas[j]=strtok(buff,d2);
                while(lineas[j]!= NULL){
                        tamanio= strlen(lineas[j]);
                        Contador[tamanio-1]++;
                        j++;
                        lineas[j]=strtok(NULL,d2);

                }
                sem_post(sem2);
                sem_wait(sema);
        }
        // Contadores listos
        for(int g=0;g<21;g++){
        printf("\n Cantidad de palabras de %d caracteres:%d",g+1,Contador[g]);
            }
        printf("\n");

        sem_post(sem2);
      

        return 0;
}



//Funcion Hijo 2 conversor a mayus
int Hijo2(int leido, char *buff){

        Reservadas();
        int ban=0;
        char d2[]="|123456789 - 0'¿°!«#$%&/()=—?¡[]_:;,.-{+}´ \n\t";
        char * lineas[256];
        int j,k=0;
        int indpal;
        int h=0;
        int tambuff;
     
               
        tambuff=leido;
        strncpy(buffauxiliar,buff,leido);
        j=0;
        lineas[j]=strtok(buff,d2);
        while(lineas[j]!=NULL){
                indpal=Buscar(lineas[j]);
                h++;
                if(indpal!=-1){
                        tambuff=Resto(lineas[j],indpal,tambuff);//imprime all hasta pal
                        j++;
                }else {
                        j++;
                }

                lineas[j]=strtok(NULL,d2);
        }

                printf("%s",buffauxiliar);

                memset(buffauxiliar,'\0',tambuff);
        

       
        return 0;

}





int main(int argc, char * const argv[]){

        int flagi=0;
        char buffer[2200];
        char *program_name;
        char* opcion_i = NULL;
        int c;
        char *ptr;
        ptr = mmap(NULL,10,PROT_READ|PROT_WRITE,MAP_ANONYMOUS|MAP_SHARED,-1, 0);
        int *leido;
        //leido = mmap(NULL,10,PROT_READ|PROT_WRITE,MAP_ANONYMOUS|MAP_SHARED,-1, 0);
        sem_t *sema, *sem2, *sema1, *sema2;
       
	sema = (sem_t *) ptr + sizeof sema;
	sem2 = sema + sizeof sem2;
	sema1= sem2 + sizeof sema1;
	sema2= sema1 + sizeof sema2;
        leido= (int *)sema2+ sizeof leido;
	sem_init(sema,1,0);
	sem_init(sem2,1,0);
	sem_init(sema1,1,0);
	sem_init(sema2,1,0);


        /* bucle para loopear por las opciones pasadas */
        while ((c=getopt(argc, argv, "i:"))>=0) {
                switch (c) {
                        case 'i':
                                opcion_i=optarg;
                                flagi++;
                                break;
                        default:
                                fprintf(stderr, "opcion invalida: %c\n", c);
                                        return 1;
        }
                }


    int procnum;
    int pid;

    for(procnum=1;procnum<3;procnum++){
        pid=fork();
        if(pid==0) break;
        else if(pid==-1) { perror("ERROR con fork"); break; }
    }

if(procnum==1){
        sem_wait(sema); //sem rojo
        Hijo1(leido, ptr,sema,sem2);          
        return 0;
}

if(procnum==2){
        do{
        sem_wait(sema1);
         for(int i= 0; i<*leido;i++){
		buff[i]=ptr[i];
	}
        Hijo2(*leido, buff);
        sem_post(sema2);
        }while(*leido!=0); 
return 0;
}


        if(flagi!=0){
                int arch= open(opcion_i, O_RDWR , 0666);

               while((*leido=read(arch,ptr,120))>0){
                     
                        //printf("-PADRE: ENTRO WHILE \n");
                       //write(STDOUT_FILENO,ptr,*leido);
                        sem_post(sema1);
			sem_post(sema);

			sem_wait(sema2);
                       sem_wait(sem2);
                        //write
                        //write
                }
                sem_post(sema);
                 close(arch);
        }else{
                while((*leido=read(STDIN_FILENO,buffer,sizeof buffer))>0){
                         // write
                         //write

                }
        }

               
                wait(NULL);
                return 0;
                           
}
