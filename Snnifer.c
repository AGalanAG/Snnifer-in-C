//Alan Alberto Gonzalez Garcia
#include<netinet/in.h>
#include<errno.h>
#include<netdb.h>
#include<stdio.h> 
#include<stdlib.h>    
#include<string.h>    
#include <sys/ioctl.h>
#include <net/if.h>
#include <netinet/in.h>
#include <linux/if_ether.h>  
#include<netinet/ip.h>    
#include<netinet/if_ether.h>  
#include<net/ethernet.h>  
#include<sys/socket.h>
#include<arpa/inet.h>
#include<sys/ioctl.h>
#include<sys/time.h>
#include<sys/types.h>
#include <semaphore.h>
#include <pthread.h>
#include<unistd.h>
void print_inicio();

#define NUM_HILOS 2
int I = 0;

sem_t sem1;

    FILE *logfile;
    struct sockaddr_in source,dest;
    struct ethhdr *eth;
    struct ifreq ethreq;
    int arp=0,ipv4=0,ipv6=0,ieee=0,total=0,et=0;
    int var=0,numPaquetes;
    char red[20];
 char proto[20];
   
    int saddr_size , data_size;
    struct sockaddr saddr;
    char buffer[65536] ;


void *codigo_del_hilo (void *id){
    
          sem_wait(&sem1);
                  int sock_raw = socket( AF_PACKET , SOCK_RAW , htons(ETH_P_ALL)) ;

                if(sock_raw < 0)
                {
                    
                    perror("Error con el socket");
                    
                }

            strncpy(ethreq.ifr_name,red,IFNAMSIZ);

                ioctl(sock_raw,SIOCGIFFLAGS,&ethreq);
             ethreq.ifr_flags |= IFF_PROMISC;
                ioctl(sock_raw,SIOCSIFFLAGS,&ethreq);
  
        saddr_size = sizeof saddr;
        
        data_size = recvfrom(sock_raw , buffer , 65536 , 0 , &saddr , (socklen_t*)&saddr_size);
        if(data_size <0 )
        {
            printf("Error en la recepcion de paquetes\n");

        }
        
   
    
    close(sock_raw);
     sem_post(&sem1);
     pthread_exit(id); 
}

void *codigo_del_hilo2 (void *id){
     int i;
          sem_wait(&sem1);

        
        struct ethhdr *eth = (struct ethhdr *)buffer;
     if(htons(eth->h_proto)>0x0600){
        switch(htons(eth->h_proto)){
            case 0x0800:
            strcpy(proto,"IPv4");
            ipv4++;
                break;
                case 0x86dd:
                strcpy(proto,"IPv6");
                ipv6++;
                    break;
                    case 0x0806:
                    strcpy(proto,"ARPA");
                    arp++;
                        break;
                            default:
                                strcpy(proto,"Otro");
                            break;
                    }

    fprintf(logfile , "\n");
    fprintf(logfile , "Trama Ethernet\n");
    fprintf(logfile , "   |-Numero de trama : %d \n",var+1);
    fprintf(logfile , "   |-Direccion Mac destino : %.2X-%.2X-%.2X-%.2X-%.2X-%.2X \n", eth->h_dest[0] , eth->h_dest[1] , eth->h_dest[2] , eth->h_dest[3] , eth->h_dest[4] , eth->h_dest[5] );
    fprintf(logfile , "   |-Direccion Mac fuente : %.2X-%.2X-%.2X-%.2X-%.2X-%.2X \n", eth->h_source[0] , eth->h_source[1] , eth->h_source[2] , eth->h_source[3] , eth->h_source[4] , eth->h_source[5] );
    fprintf(logfile , "   |-Protocolo            : %s  ( %x ) \n",proto,htons(eth->h_proto));
    fprintf(logfile , "   |-Longitud de la trama: %d \n",data_size);
    fprintf(logfile , "   |-Longitud de la carga util: %d \n",data_size-12);
    int n =(unsigned short)eth->h_dest[5];
                    int i=0,j,a[1000];
                    
                    while(n>0){
                        a[i]=n%2;
                        n /= 2;
                        i++;
                    }

                    for (j = i-1; j >= 0; --j)
                    {
                    //printf("%d",a[j]);
                    //printf("\n");
                    }
                    if(a[i]==0){
                    fprintf(logfile , "   |-: Unidifusion \n");
                    }else{fprintf(logfile , "   |-: Multidifusion \n");}
       
        et++;
        }else{
            printf("La trama no puede se analizada...\n");
            ieee++;
        }
        total++;
  
     sem_post(&sem1);
     pthread_exit(id); 
}
 

int main()
{
    logfile=fopen("Analisis.txt","w");
    if(logfile==NULL) 
    {
        printf("No se puede crear el archivo");
    }
     

    printf("Iniciando...\n");

        printf("Ingresa el nombre de la tarjeta de red:\n");
        scanf("%s",&*red);
        //printf("%s\n",red );
        printf("Ingresa el numero de paquetes:\n");
        scanf("%d",&numPaquetes);
        printf("Escaneando los paquetes...\n");

print_inicio();
//---------------------------------------------------------------------------------------------------
        while(var<numPaquetes){
 int h ;
    pthread_t hilos[NUM_HILOS];
    int id[NUM_HILOS] ={1,2};
    int error,error2,error3,error4;
    int *salida;

    sem_init(&sem1,0,1);
   
   
        error = pthread_create(&hilos[0], NULL, codigo_del_hilo, &id[h]);
        if(error){
           printf("error\n");
           exit(-1);
        }
   
        error2 = pthread_join(hilos[0], (void **) &salida);
        if(error)
           printf("error\n");
         else
       // printf("Hilo %d terminado \n", *salida);

            


             error3 = pthread_create(&hilos[1], NULL, codigo_del_hilo2, &id[h]);
        if(error){
           printf("error\n");
           exit(-1);
        }
   
        error4 = pthread_join(hilos[1], (void **) &salida);
        if(error)
           printf("error\n");
         else
        //printf("Hilo %d terminado \n", *salida);
        
        var++;
}
rewind(logfile);
print_inicio();

    //wlp3s0
    char enviar[100];
    char f[20];
    strcpy(enviar,"sudo /sbin/ifconfig ");
    strcpy(f," -promisc");
        strcat(enviar,red);
        strcat(enviar,f);
     //   printf("\n%s\n",enviar );
    system(enviar);

    printf("\nSe termino de recivir los paquetes.\n");
    return 0;
}


void print_inicio(){
            fprintf(logfile , "             ||ANALISI DE LOS DATOS||\n\n\n");
            fprintf(logfile , "      |-Paquetes Totales : %d \n",total);
            fprintf(logfile , "      |-Paquetes de IEE 802.3 : %d\n",ieee);
            fprintf(logfile , "      |-Paquetes de Ethernet II : %d\n\n",et);
            fprintf(logfile , "             ||PROTOCOLOS||\n\n\n");
             fprintf(logfile , "      |-Paquetes de IPv4:%d\n",ipv4);
             fprintf(logfile , "      |-Paquetes de IPv6:%d\n",ipv6);
             fprintf(logfile , "      |-Paquetes de ARP:%d\n\n\n\n",arp);

}