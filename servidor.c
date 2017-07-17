#include "funciones.h"

/*
 * El servidor ofrece el servicio de un chat
 */

int main ( )
{
  
	/*---------------------------------------------------- 
		Descriptor del socket y buffer de datos                
	-----------------------------------------------------*/
	system("clear");

    int sd, new_sd, numPartida=0,x, cont, cont2, cont3, numeroPartida, m=0,m2=0, existe=0, m3=0, numClientes = 0, p, salida, i, j, k, recibidos, t, c, r, z, q, _i, _j, i2, i3, q2, numero, it, q3;

	struct sockaddr_in sockname, from;
	char buffer[MSG_SIZE], aux[30] = "", aux2[30],aux3[30],aux4[30], nombre[30],contrasena[30],cadena[30],*cadena1, *cadena2, *cadena3;
	socklen_t from_len;
    fd_set readfds, auxfds;
    int arrayClientes[MAX_CLIENTS];
    char identificador[MSG_SIZE],COLOR[20];
   	int CARTON[3][9];
    

    //INICIALIZO LAS PARTIDAS



    for(p=0;p<NUMPARTIDAS;p++)
    {
        bingo[p].status=0;
        bingo[p].numJugadores=0;
        bingo[p].partida_estado=0;
    }



	/* --------------------------------------------------
		Se abre el socket 
	---------------------------------------------------*/
  	sd = socket (AF_INET, SOCK_STREAM, 0);
	if (sd == -1)
	{
		perror("No se puede abrir el socket cliente\n");
    		exit (1);	
	}


	sockname.sin_family = AF_INET;
	sockname.sin_port = htons(2000);
	sockname.sin_addr.s_addr =  INADDR_ANY;

	if (bind (sd, (struct sockaddr *) &sockname, sizeof (sockname)) == -1)
	{
		perror("Error en la operación bind");
		exit(1);
	}
	

   	/*---------------------------------------------------------------------
		Del las peticiones que vamos a aceptar sólo necesitamos el 
		tamaño de su estructura, el resto de información (familia, puerto, 
		ip), nos la proporcionará el método que recibe las peticiones.
   	----------------------------------------------------------------------*/
	from_len = sizeof (from);


	if(listen(sd,1) == -1){
		perror("Error en la operación de listen");
		exit(1);
	}
    
    //Inicializar los conjuntos fd_set
    FD_ZERO(&readfds);
    FD_ZERO(&auxfds);
    FD_SET(sd,&readfds);
    FD_SET(0,&readfds);

    struct timeval timeout;      
    timeout.tv_sec = 1;
    timeout.tv_usec = 0;


	/*-----------------------------------------------------------------------
		El servidor acepta una petición
	------------------------------------------------------------------------ */
	while(1)
	{

        //Aquí hago el correspondiente reparto de numeros a las partidas iniciadas. Desde 95 hasta 120 a las partidas le voy dando numeros
        for (it=0;it<10;it++)
        {

        	if (bingo[it].status==2)
        	{
                do
                {
                    numero=(rand()%90)+1;
                                        		
                }while(bingo[it].sorteo[numero]!=-1);

                bingo[it].sorteo[numero]=1;

                                        	
                for ( q2=0; q2<4; q2++ )//CAMBIAR A 4
                {
                    bzero(buffer,sizeof(buffer));
                    //sprintf(buffer,"Número %d para la mesa %d\n", numero, it);  No hace falta
                    sprintf(buffer, "Número %d",numero);
                    send(bingo[it].jugadores[q2].numSock, buffer,strlen(buffer),0);
                    bzero(buffer,sizeof(buffer));
            	}
            }
        }   
        sleep(1);


        //Esperamos recibir mensajes de los clientes (nuevas conexiones o mensajes de los clientes ya estados)
     
        auxfds = readfds;

        salida = select(FD_SETSIZE,&auxfds,NULL,NULL,&timeout); //con el timeout, cada segundo va mandando un numero a las partidas empezadas. Si no, habria que estar dandole al intro para sacar numeros

        if(salida > 0)
        {
            for(i=0; i<FD_SETSIZE; i++)
            {
                
                //Buscamos el socket por el que se ha establecido la comunicación
                if(FD_ISSET(i, &auxfds)) 
                {
                    
                    //Si es un cliente nuevo
                    if( i == sd)
                    {

                        
                        if((new_sd = accept(sd, (struct sockaddr *)&from, &from_len)) == -1)
                        {
                            perror("-Err. Error aceptando peticiones");
                        }
                        else
                        {
                            if(numClientes < MAX_CLIENTS)
                            {
                                
                                FD_SET(new_sd,&readfds);
                            
                                
                            
                                sprintf(buffer, "+OK. Usuario estado\n");
                                clientesSistema[numClientes].nombre[0]='\0';
                                clientesSistema[numClientes].estado=-1;//Igualo a cero para controlar que el usuario esta o no en el sistema
                                clientesSistema[numClientes].numSock=new_sd;//Asigno el numero de socket que tiene el cliente
                                numClientes++;

                                printf("El usuario %d se ha conectado.\n",clientesSistema[numClientes].numSock);

                                bzero(buffer,sizeof(buffer));
                                sprintf(buffer, "\n**********************************************************************\n\t\t   BIENVENIDO AL JUEGO DEL BINGO\n**********************************************************************\n");
                                send(clientesSistema[numClientes-1].numSock,buffer,strlen(buffer),0);

                                bzero(buffer,sizeof(buffer));
                                sprintf(buffer, "Para comenzar a jugar introduzca alguno de los siguientes comandos:");
                                send(clientesSistema[numClientes-1].numSock,buffer,strlen(buffer),0);

                                bzero(buffer,sizeof(buffer));
                                sprintf(buffer, "\n\t-REGISTRO -u <nombre> -p <contraseña>\n\t-USUARIO <nombre>\n\t-PASSWORD <contraseña>\n\t-INICIAR-PARTIDA\n\t-SALIR\n\n**********************************************************************\n");
                                send(clientesSistema[numClientes-1].numSock,buffer,strlen(buffer),0);


                                for(j=0; j<(numClientes-1);j++)
                                {
                                
                                    bzero(buffer,sizeof(buffer));
                                    sprintf(buffer, "Nuevo Cliente estado: %d\n",new_sd);
                                    send(clientesSistema[j].numSock,buffer,strlen(buffer),0);
                                }
                            }
                            else
                            {
                                bzero(buffer,sizeof(buffer));
                                strcpy(buffer,"Demasiados clientes estados\n");
                                send(new_sd,buffer,strlen(buffer),0);
                                close(new_sd);
                            }
                            
                        }
                        
                        
                    }

                    else if (i == 0)
                    {
                        //Se ha introducido información de teclado
                        bzero(buffer, sizeof(buffer));
                        fgets(buffer, sizeof(buffer),stdin);
                        
                        //Controlar si se ha introducido "SALIR", cerrando todos los sockets y finalmente saliendo del servidor. (implementar)
                        //Mensajes que se quieran mandar a los clientes (implementar)
                        
                        if(strcmp(buffer,"SALIR\n") ==0)
                        {
                        	
                        	for(k=0;k<numClientes;k++)
                        	{
                        		bzero(buffer,sizeof(buffer));
                                sprintf(buffer,"-ERR. Desconexión del servidor\n");
                                send(clientesSistema[k].numSock, buffer,strlen(buffer),0);
                                	
                                close(k);
                                FD_CLR(k,&readfds);
                                	
                        	}
                        	return 0;
                        	
                        }                                                    
                    }

                    else
                    {
                        bzero(buffer,sizeof(buffer));
                        
                        recibidos = recv(i,buffer,sizeof(buffer),0);
                        
                        if(recibidos > 0)
                        {
                            //Controlar que un usuario quiera salir
                            if(strcmp(buffer,"SALIR\n") == 0)
                            {
                                
                                close(i);
                                FD_CLR(i,&readfds);
                               
                                //Re-estructurar el array de clientes
                                for (j = 0; j < numClientes - 1; j++)
                                    if (arrayClientes[j] == i)
                                        break;
                                for (; i < numClientes - 1; i++)
                                    (arrayClientes[i] = arrayClientes[i+1]);

                                if(clientesSistema[i-4].estado>2)
                                {
                                    for(t=0;t<5;t++)
                                    {
                                        if(clientesSistema[i-4].numero_partida==clientesSistema[t].numero_partida)
                                        {
                                            clientesSistema[t].estado=2;
                                        }

                                    }
                                }

                                clientesSistema[j].nombre[0]='\0';
                                clientesSistema[j].estado=-1;
                                clientesSistema[j].numSock=-1;

                                numClientes--;
                                
                                bzero(buffer,sizeof(buffer));
                                //sprintf(buffer,"Desconexión del cliente: %d\n",i);
                                
                            }

                            //Controlar el nombre de usuario introducido por el cliente
                            else if(strncmp(buffer,"USUARIO ",8) == 0)
                            {
                                printf("%s\n", aux);
                                //Cojo el nombre escrito tras USUARIO
                                    m=0;
                                    for(c=8;buffer[c]!='\n';c++)
                                    {
                                        aux[m]=buffer[c];
                                        m++;
                                    }

                                    //Busco en primer lugar si el usuario esta ACEPTADO o VALIDADO en el sistema
                                    existe=0;
                                    for(r=0; r<numClientes;r++)
                                    {
                                        if(strcmp(clientesSistema[r].nombre,aux)==0 || clientesSistema[r].estado==1)
                                        {
                                            existe=1;
                                            break;
                                        }
                                    }

                                    //Sino esta ni ACEPTADO ni VALIDADO busco si existe en el fichero de usuarios registrados
                                    if(existe==0)
                                    {
                                        printf("%s\n", aux);
                                        if(buscarUsuario(aux))
                                        {
                                            clientesSistema[i-4].estado=1;
                                            strcpy(clientesSistema[i-4].nombre,aux);
                                            bzero(buffer,sizeof(buffer));
                                            sprintf(buffer,"+OK. Usuario aceptado\n");
                                            send(clientesSistema[i-4].numSock, buffer,strlen(buffer),0);
                                        }
                                        else
                                        {
                                            bzero(buffer,sizeof(buffer));
                                            sprintf(buffer,"-ERR. Usuario no existe \n");
                                            send(clientesSistema[i-4].numSock, buffer,strlen(buffer),0);
                                        }
                                        
                                    }
                                    //En caso de que ya este VALIDADO mostramos mensaje 
                                    if (existe>=1)
                                    {
                                        bzero(buffer,sizeof(buffer));
                                        sprintf(buffer,"-ERR. Ya estas validado\n");
                                        send(clientesSistema[i-4].numSock, buffer,strlen(buffer),0);
                                    }
                                    bzero(buffer,sizeof(buffer));

                            }

                            else if(strncmp(buffer,"PASSWORD ",9) == 0)
                            {
                                //Copiamos la contraseña que introducimos detras del comando PASSWORD
                                m2=0;
                                for(z=9;buffer[z]!='\n';z++)
                                {
                                    aux2[m2]=buffer[z];
                                    m2++;
                                }
                                aux2[m2]='\0';

                                //Comprobamos que la contraseña sea valida
                                if(buscarPassword(clientesSistema[i-4].nombre,aux2) && clientesSistema[i-4].estado==1)
                                {
                                    bzero(buffer,sizeof(buffer));
                                    sprintf(buffer,"+OK. Contraseña válida.\n");
                                    send(clientesSistema[i-4].numSock, buffer,strlen(buffer),0);
                                    clientesSistema[i-4].estado=2;
                                    printf("Usuario %s se ha conectado\n", clientesSistema[i-4].nombre);
                                }
                                else
                                {
                                    bzero(buffer,sizeof(buffer));
                                    sprintf(buffer,"-ERR. Contraseña incorrecta!\n");
                                    send(clientesSistema[i-4].numSock, buffer,strlen(buffer),0);
                                }

                                bzero(buffer,sizeof(buffer));
                            }
                            else if(strncmp(buffer,"REGISTRO ", 9)==0)
                            {
                                 m3=0;
                                    for(c=9;buffer[c]!='\n';c++)
                                    {
                                        aux4[m3]=buffer[c];
                                        m3++;
                                    }
                                        
                                    
                                    aux4[m3]='\0';
                                    
                                    cadena1=strtok(aux4," ");
                                    cadena2=strtok(NULL," ");
                                    strcpy(nombre, cadena2);
                                    cadena2=strtok(NULL," ");
                                    cadena3=strtok(NULL," ");
                                    strcpy(contrasena, cadena3);

                                    //Si no existe en la base de datos, lo registro
                                    if(buscarUsuario(nombre)==0)
                                    {
                                        bzero(buffer,sizeof(buffer));                       

                                        registrarUsuario(nombre, contrasena);
                                        
                                        sprintf(buffer,"\nEl usuario %s se ha registrado correctamente\n", nombre);
                                        send(i, buffer,strlen(buffer),0);
                                        
                                    }
                                    else
                                    {
                                        bzero(buffer,sizeof(buffer));
                                        sprintf(buffer,"\nEl usuario %s ya se encuentra registrado\n", nombre);
                                        send(i, buffer,strlen(buffer),0);
                                    
                                    }
                            }

                            //Funcion que asigna a un jugador una partida
                            else if(strcmp(buffer,"INICIAR-PARTIDA\n")==0)
                            {
                                bzero(buffer,sizeof(buffer));
                                sprintf(buffer,"iniciar-partida\n");
                                send(clientesSistema[i-4].numSock, buffer,strlen(buffer),0);
                                bzero(buffer,sizeof(buffer));
                                

                                //Comprobamos que el usuario esta validado
                                if(clientesSistema[i-4].estado==2)
                                {
                                    clientesSistema[i-4].estado=3;

                                    numeroPartida=asignarPartida(bingo,clientesSistema[i-4]);
                                    
                                    if (numeroPartida!=-1) //Esta funcion asigna una partida en la que falten jugadores para empezar partida
                                    {
                                        bingo[numeroPartida].jugadores[bingo[numeroPartida].numJugadores]=clientesSistema[i-4];//meto el jugador en esa mesa
                                        bingo[numeroPartida].numJugadores++;
                                        clientesSistema[i-4].numero_partida=numeroPartida; //No se usa para nada, se puede quitar
                                        bzero(buffer,sizeof(buffer));
                                        sprintf(buffer,"+Ok. Petición Recibida. Quedamos a la espera de más jugadores.\n");
                                        send(clientesSistema[i-4].numSock, buffer,strlen(buffer),0);
                                        bzero(buffer,sizeof(buffer));

                                        //Le enseñamos el carton al jugador mientras espera mas jugadores

                                        generaCarton(bingo[numeroPartida].jugadores[bingo[numeroPartida].numJugadores-1].carton_jugador);

                                        if (!pasarCarton(bingo[numeroPartida].jugadores[bingo[numeroPartida].numJugadores-1].carton_jugador))
                                        {
                                        	printf("ERROR AL ESCRIBIR EN EL FICHERO EL CARTON\n");
                                        }

                                        //imprimeCarton(bingo[numeroPartida].jugadores[bingo[numeroPartida].numJugadores-1].carton_jugador); Tampoco sirve

                                        bzero(buffer,sizeof(buffer));
                                        sprintf(buffer,"+Ok. Se le ha asignado la mesa %d\n",numeroPartida);
                                        send(bingo[numeroPartida].jugadores[bingo[numeroPartida].numJugadores-1].numSock, buffer,strlen(buffer),0);
                                        bzero(buffer,sizeof(buffer));

                                       	sleep(1); //porque no le da tiempo al cliente a guardar mesa y silla. Si le ponemos el sleep primero mete la mesa y luego la silla.

                                        bzero(buffer,sizeof(buffer));
                                        sprintf(buffer,"+Ok. Se ha sentado en la silla %d\n",bingo[numeroPartida].numJugadores-1);
                                        send(bingo[numeroPartida].jugadores[bingo[numeroPartida].numJugadores-1].numSock, buffer,strlen(buffer),0);
                                        bzero(buffer,sizeof(buffer));

                                        
                                        //Comprobar si es el cuarto jugador
                                        if (bingo[numeroPartida].numJugadores==4)//CAMBIAR A 4
                                        {
                                        	bingo[numeroPartida].status=2;

                                            for ( q=0; q<4; q++ )//CAMBIAR A 4
                                            {
                                                bzero(buffer,sizeof(buffer));
                                                sprintf(buffer,"+Ok. Va a comenzar la partida\n");
                                                send(bingo[numeroPartida].jugadores[q].numSock, buffer,strlen(buffer),0);
                                                bzero(buffer,sizeof(buffer));
                                            }

                                            printf("Partida %d iniciada por los jugadores jugadores: \n - %s\n - %s\n - %s\n -  %s\n", numeroPartida,bingo[numeroPartida].jugadores[0].nombre, bingo[numeroPartida].jugadores[1].nombre,bingo[numeroPartida].jugadores[2].nombre,bingo[numeroPartida].jugadores[3].nombre);

                                            for (i3=0; i3<90; i3++)
                                            {
                                            	bingo[numeroPartida].sorteo[i3]=-1;
                                            }
                                            
                                        }
                                    }
                                    
                                    else//Si todas las mesas estan llenas entraría en este else 
                                    {
                                        bzero(buffer,sizeof(buffer));
                                        sprintf(buffer,"-ERR. Todas las mesas estan llenas. Intentelo de nuevo mas tarde.\n");
                                        send(clientesSistema[i-4].numSock, buffer,strlen(buffer),0);
                                        bzero(buffer,sizeof(buffer));
                                    }
                                }
                                else
	                            {
	                                bzero(buffer,sizeof(buffer));
	                                sprintf(buffer,"-ERR. Debe estar validado\n");
	                                send(clientesSistema[i-4].numSock, buffer,strlen(buffer),0);
	                                bzero(buffer,sizeof(buffer));
	                            }
                            }                            
                            
                            else if(strcmp(buffer,"UNA-LINEA\n")==0)
                            {
	                           	x=comprobarLinea(bingo[numeroPartida].jugadores[bingo[numeroPartida].numJugadores-1].carton_jugador, bingo[numeroPartida].sorteo);     
                                if(x==1)
                                {
                                    if(cont<1)
                                    {
                                        for ( q=0; q<4; q++ )
                                        {
                                            bzero(buffer,sizeof(buffer));
                                            sprintf(buffer,"+Ok. Jugador %s ha cantado una línea\n", bingo[numeroPartida].jugadores[clientesSistema[i-4].numSock].nombre);
                                            send(bingo[numeroPartida].jugadores[q].numSock, buffer,strlen(buffer),0);
                                            bzero(buffer,sizeof(buffer));
                                            cont ++;
                                        }
                                    }
                                    else
                                    {
                                        bzero(buffer,sizeof(buffer));
                                        sprintf(buffer,"\n-Err. El comando UNA-LINEA no procede\n");
                                        send(i, buffer,strlen(buffer),0);
                                    }
                                }
                                else
                                {
                                    bzero(buffer,sizeof(buffer));
                                    sprintf(buffer,"\n-Err. El comando UNA-LINEA no procede\n");
                                    send(i, buffer,strlen(buffer),0);
                                }
                            }

                            else if (strcmp(buffer, "DOS-LINEA\n")==0)
                            {
                            	x=comprobarLinea(bingo[numeroPartida].jugadores[bingo[numeroPartida].numJugadores-1].carton_jugador, bingo[numeroPartida].sorteo);     
                                if(x==1)
                                {
                                    if(cont2<1)
                                    {
                                        for ( q=0; q<4; q++ )
                                        {
                                            bzero(buffer,sizeof(buffer));
                                            sprintf(buffer,"+Ok. Jugador %s ha cantado DOS-LINEA\n", bingo[numeroPartida].jugadores[clientesSistema[i-4].numSock].nombre);
                                            send(bingo[numeroPartida].jugadores[q].numSock, buffer,strlen(buffer),0);
                                            bzero(buffer,sizeof(buffer));
                                            cont2++;
                                        }
                                    }
                                    else
                                    {
                                        bzero(buffer,sizeof(buffer));
                                        sprintf(buffer,"\n-Err. El comando DOS-LINEA no procede\n");
                                        send(i, buffer,strlen(buffer),0);
                                    }
                                }
                                else
                                {
                                    bzero(buffer,sizeof(buffer));
                                    sprintf(buffer,"\n-Err. El comando DOS-LINEA no procede\n");
                                    send(i, buffer,strlen(buffer),0);
                                } 
                            }

                            else if (strcmp(buffer, "BINGO\n")==0)
                            {
                            	x=comprobarBingo(bingo[numeroPartida].jugadores[bingo[numeroPartida].numJugadores-1].carton_jugador, bingo[numeroPartida].sorteo);     
                                if(x==1)
                                {
                                    if(cont3<1)
                                    {
                                        for ( q=0; q<4; q++ )
                                        {
                                            bzero(buffer,sizeof(buffer));
                                            sprintf(buffer,"+Ok. Jugador %s ha cantado BINGO\n\n+Ok. Partida finalizada\n", bingo[numeroPartida].jugadores[clientesSistema[i-4].numSock].nombre);
                                            send(bingo[numeroPartida].jugadores[q].numSock, buffer,strlen(buffer),0);
                                            bzero(buffer,sizeof(buffer));
                                            clientesSistema[q].estado=2;
                                            bingo[numeroPartida].status=0;
                                            cont3 ++;
                                        }
                                    }
                                    else
                                    {
                                        bzero(buffer,sizeof(buffer));
                                        sprintf(buffer,"\n-Err. El comando BINGO no procede\n");
                                        send(i, buffer,strlen(buffer),0);
                                    }
                                }
                                else
                                {
                                    bzero(buffer,sizeof(buffer));
                                    sprintf(buffer,"\n-Err. El comando BINGO no procede\n");
                                    send(i, buffer,strlen(buffer),0);
                                }
                            }

                            else if(strcmp(buffer,"SALIR-PARTIDA")==0)
                            {
                            	if(bingo[numeroPartida].numJugadores>0)
                                {
                                    bzero(buffer,sizeof(buffer));
                                    sprintf(buffer,"\n+Ok. Ha salido de la partida\n");
                                    send(i, buffer,strlen(buffer),0);

                                    for ( q=0; q<4; q++ )
                                    {
                                        bzero(buffer,sizeof(buffer));
                                        sprintf(buffer,"+Ok. Usuario %s ha abandonado la partida\n", bingo[numeroPartida].jugadores[clientesSistema[i-4].numSock].nombre);
                                        send(bingo[numeroPartida].jugadores[q].numSock, buffer,strlen(buffer),0);
                                        bzero(buffer,sizeof(buffer));
                                    }

                                    clientesSistema[i-4].estado=2;
                                    bingo[numeroPartida].numJugadores--;
                                    
                                }
                                else
                                {
                                    bingo[numeroPartida].status=0;
                                }
                            }


                                
                            else
                            {
                                sprintf(identificador,"%d: %s",i-4,buffer);
                                bzero(buffer,sizeof(buffer));
                                sprintf(buffer,"-ERR. Comando incorrecto\n");
                                send(clientesSistema[i-4].numSock, buffer,strlen(buffer),0);
                                strcpy(buffer,identificador);

                            }

                        }

                    }

                }

            }

	    }

	}

	close(sd);
	
}

