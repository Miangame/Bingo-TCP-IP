#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <stdlib.h>
#include <string.h>
#include "funciones.h"


int main ( )
{
  
	/*---------------------------------------------------- 
		Descriptor del socket y buffer de datos                
	-----------------------------------------------------*/
	system("clear");
	int sd, m2, z, m3, z2, num, z3;
	struct sockaddr_in sockname;
	char buffer[250], *cadena1, numero[10];
	socklen_t len_sockname;
    fd_set readfds, auxfds;
    int salida;
    int fin = 0;
    int i, j, silla, mesa;
    char aux2[30], aux3[30];
	
	/* --------------------------------------------------
		Se abre el socket 
	---------------------------------------------------*/
  	sd = socket (AF_INET, SOCK_STREAM, 0);
	if (sd == -1)
	{
		perror("No se puede abrir el socket cliente\n");
    		exit (1);	
	}

	

	/* ------------------------------------------------------------------
		Se rellenan los campos de la estructura con la IP del 
		servidor y el puerto del servicio que solicitamos
	-------------------------------------------------------------------*/
	sockname.sin_family = AF_INET;
	sockname.sin_port = htons(2000);
	sockname.sin_addr.s_addr =  inet_addr("127.0.0.1");

	/* ------------------------------------------------------------------
		Se solicita la conexión con el servidor
	-------------------------------------------------------------------*/
	len_sockname = sizeof(sockname);
	
	if (connect(sd, (struct sockaddr *)&sockname, len_sockname) == -1)
	{
		perror ("Error de conexión");
		exit(1);
	}
    
    //Inicializamos las estructuras
    FD_ZERO(&auxfds);
    FD_ZERO(&readfds);
    
    FD_SET(0,&readfds);
    FD_SET(sd,&readfds);

    
	/* ------------------------------------------------------------------
		Se transmite la información
	-------------------------------------------------------------------*/
	do
	{
        auxfds = readfds;
        salida = select(sd+1,&auxfds,NULL,NULL,NULL);
        
        //Tengo mensaje desde el servidor
        if(FD_ISSET(sd, &auxfds)){
            
            bzero(buffer,sizeof(buffer));
            recv(sd,buffer,sizeof(buffer),0);
            
            printf("\n%s\n",buffer);
            
            if(strcmp(buffer,"Demasiados clientes conectados\n") == 0)
                fin =1;
                
            if(strcmp(buffer,"-ERR. Desconexión del servidor\n") == 0)
                fin =1;

            if(strcmp(buffer,"iniciar-partida\n") == 0)
            	system("clear");

            if(strncmp(buffer, "+Ok. Se le ha asignado la mesa ",31) == 0) 
            {
            	m2=0;
                for(z=31;buffer[z]!='\n';z++)
                {
                    aux2[m2]=buffer[z];
                    m2++;
                }
                aux2[m2]='\0';

                mesa=atoi(aux2);

            }

            if(strncmp(buffer,"+Ok. Se ha sentado en la silla ", 31)==0) 
            {
            	m3=0;
                for(z2=31;buffer[z2]!='\n';z2++)
                {
                    aux3[m3]=buffer[z2];
                    m3++;
                }
                aux3[m3]='\0';

                silla=atoi(aux3);

                leerCarton(bingo[mesa].jugadores[silla].carton_jugador); //aprovecho el bingo, pero la matriz se puede poner en una matriz cualquiera

                imprimeCarton(bingo[mesa].jugadores[silla].carton_jugador);
                

            }


            /*if(strncmp(buffer,"Este es tu carton", 31)==0) 
            {
                
                
                imprimeCarton(bingo[mesa].jugadores[silla].carton_jugador);
                

            }*/
				            
        }
        else
        {
            
            //He introducido información por teclado
            if(FD_ISSET(0,&auxfds)){
                bzero(buffer,sizeof(buffer));
                
                fgets(buffer,sizeof(buffer),stdin);
                
                if(strcmp(buffer,"SALIR\n") == 0){
                        fin = 1;
                
                }
                
                send(sd,buffer,sizeof(buffer),0);
                
            }
            
            
        }
        
        
				
    }while(fin == 0);
		
    close(sd);
		
    return 0;
}





















