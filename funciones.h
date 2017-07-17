#ifndef FUNCIONES_H
#define FUNCIONES_H

#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <arpa/inet.h>

#define MSG_SIZE 250
#define MAX_CLIENTS 40
#define NUMPARTIDAS 2

//Defines que utilizo en el carton
#define nFil 3
#define nCol 9
#define nEle 27

typedef struct usuario
{
        char nick[30];
        char pass[30];
}Usuario;

typedef struct cliente
 {
    char nombre[30];
    int estado;
    /*
        LOS ESTADOS POSIBLES SON:
               -1 - USUARIO INACTIVO EN EL SISTEMA 
                1 - USUARIO CONECTADO AL SISTEMA
                2 - USUARIO VALIDADO EN EL SISTEMA
                3 - ESPERANDO PARTIDA
                4 - JUGANDO
    */
    int numSock;
    int numero_partida; 
    int carton_jugador[3][9];

 }Cliente;



typedef struct partida
 {
    int status;
     /*
    LOS ESTADOS POSIBLES SON:
            0 - PARTIDA SIN EMPEZAR
            1 - PARTIDA ACTIVA
            2 - PARTIDA COMPLETA, COMIENZA EL JUEGO
    */
    int numJugadores;
    Cliente jugadores[4];
 	  int sorteo[90];
 	  int partida_estado;
 	/*
		0 - AUN NO SE HA CANTADO NADA EN LA PARTIDA
		1 - SE HA CANTADO 1 LINEA
		2 - SE HAN CANTADO 2 LINEAS
		3 - SE HA CANTADO BINGO
 	*/
		        
 }Partida;





int buscarUsuario(char cadena[])
{       
        Usuario A;
        int encon=0;
        FILE * fich;
        if(fich=fopen("usuarios.txt","r"))
        {
                while(fscanf(fich,"%s %s",A.nick,A.pass)==2)
                {
                        if(strcmp(cadena,A.nick)==0)
                                encon = 1;
                }
                fclose(fich);
                
        }
        else
        {
                printf("ERROR al abrir el fichero.\n");
        }

        return encon;    
}

int buscarPassword(char cadena[],char cadena1[])
{       
        Usuario A;
        int encon=0;
        FILE * fich;
        if(fich=fopen("usuarios.txt","r"))
        {
                while(fscanf(fich,"%s %s",A.nick,A.pass)==2)
                {
                        if((strcmp(cadena1,A.pass)==0) && (strcmp(cadena,A.nick)==0))
                                encon = 1;
                }
                fclose(fich);
                
        }
        else
        {
                printf("ERROR al abrir el fichero.\n");
        }

        return encon;    
}

int registrarUsuario(char cadena[], char cadena1[])
{
        FILE *f;
        Usuario A;
        if(f=fopen("usuarios.txt","a"))
        {
                fprintf(f,"%s %s\n",cadena,cadena1);
                fclose(f);
                return 1;
        }
}

int asignarPartida(Partida bingo[], Cliente quiereJugar)
{
    int partida;

    for(partida=0; partida<NUMPARTIDAS; partida++)
    {
        printf("%d\n", partida);
        if(bingo[partida].numJugadores<4) //4 porque son 4 jugadores por partida
        {
            return partida;
        }
    }

    return -1;
}

//A PARTIR DE AQUI VIENEN LAS FUNCINES CORRESPODIENTES AL CARTON DEL BINGO

void rellenaCarton(int m[][nCol]){
  //Para rellenar el carton de números aleatorios hay que tener en cuenta varias normas:
  //      POR COLUMNAS
  //      1.- primera columna va de 1 a 9 (9 posibles números)
  //      2.- de la segunda a la octava columna va de 10 a 19 x número de columna  (10 posibles números)
  //      3.- la última columna va de 80 al 90 (11 posibles números)
  //Por tanto se rellena un vector de 27 números con 3 nums del 1 al 9, 3 del 10 al 19, 3 del 20 al 29 .... 3 del 80 al 90.
  int i, j, k, z, duplicado, num;
  srand(time(NULL));

  for (j = 0, z = 0; j < nCol; j++){
    for (i = 0; i < nFil; i++, z++){
      duplicado = 0;
      k = 0;
      if(z < 3)
        num = 1 + rand() % 9;
      if(z >= 3 && i <6)
        num = 10 + rand() % 10;
      if(z >= 6 && i <9)
        num = 20 + rand() % 10;
      if(z >= 9 && i <12)
        num = 30 + rand() % 10;
      if(z >= 12 && i <15)
        num = 40 + rand() % 10;
      if(z >= 15 && i <18)
        num = 50 + rand() % 10;
      if(z >= 18 && i <21)
        num = 60 + rand() % 10;
      if(z >= 21 && i <24)
        num = 70 + rand() % 10;
      if(z >= 24 && i <27)
        num = 80 + rand() % 11;
      while(k < i){
        if(m[k][j] == num){
          duplicado = 1;
          break;
        }
        k++;
      }

      if (duplicado == 1){
        i--;
        z--;
      }
      else
        m[i][j] = num;
    }
  }

}

void ordenaCarton(int m[][nCol]){
//ordena por columnas
  int i, j, mayor;

  for(j = 0; j < nCol; j++){
    for(i = 0; i < nFil-1; i++){
      if(m[i][j] > m[i+1][j]){
        mayor = m[i][j];
        m[i][j] = m[i+1][j];
        m[i+1][j] = mayor;
      }
    }
  }
  for(j = 0; j < nCol; j++){
    for(i = 0; i < nFil-1; i++){
      if(m[i][j] > m[i+1][j]){
        mayor = m[i][j];
        m[i][j] = m[i+1][j];
        m[i+1][j] = mayor;
      }
    }
  }
}

void quitaNumsCarton(int m[][nCol]){
  //Para poner huecos en blanco hay que tener en cuenta varias normas:
  //      POR COLUMNAS
  //      1.- en todas las columnas debe haber al menos un hueco en blanco.
  //      2.- en cada columna no puede haber más de dos huecos en blanco.
  //      3.- el número de huecos en blanco debe ser 12
  //      POR FILAS
  //      4.- los huecos por fila tienen que ser 4
  int i, j, quitados, blancoCol, blancoFil;
  int blancosPorColumnas[nCol] = {0,0,0,0,0,0,0,0,0};
  int blancosPorFilas[nFil] = {0,0,0};
  quitados = 0;

  for(j=0; j < nCol; j++){ //insertamos 1 blanco aleatorio por columna.
    blancoFil = rand()%3;
      if(blancosPorFilas[blancoFil] < 4){
        m[blancoFil][j] = -1;
        blancosPorFilas[blancoFil]++;
        blancosPorColumnas[j]++;
        quitados++;
    }
  }

  while(quitados < 12){ //insertamos blancos aleatorios comprobando que no haya más de 2 por columna ni más de 4 por fila ni repetidos.
    blancoFil = rand()%3;
    blancoCol = rand()%9;
    if(m[blancoFil][blancoCol] != -1 && blancosPorColumnas[blancoCol] < 2 && blancosPorFilas[blancoFil] < 4 ){
      m[blancoFil][blancoCol] = -1;
      blancosPorFilas[blancoFil]++;
      blancosPorColumnas[blancoCol]++;
      quitados++;
    }
  }

}

void imprimeCarton(int m[][nCol]){
  int i, j;
  puts("+--------------------------------------------+");
  for(i = 0; i < nFil; i++){
    for(j = 0; j < nCol; j++){
      if(m[i][j] == -1)
        printf("|    ");
      else
      printf("| %-2d ", m[i][j]);
    }
    puts("|");
  }
  puts("+--------------------------------------------+");
}

void generaCarton(int m[][nCol]){
  rellenaCarton(m);
  ordenaCarton(m);
  quitaNumsCarton(m);
}





int pasarCarton(int m[3][9])
{
    FILE *f;
    int i, j;

    if(f=fopen("carton.txt","w"))
   	{
   		for (i=0; i< nFil; i++)
   		{
   			for (j=0; j< nCol; j++)
   			{
   				fprintf(f,"%d ",m[i][j]);
   			}
   			fprintf(f,"\n");
   		}
    	
    	fclose(f);
    	return 1;
    }
}

void leerCarton(int m[][9])
{
        FILE * fich=fopen("carton.txt","r");
        int i=0,col1,col2,col3,col4,col5,col6,col7,col8,col9;

        

        if(fich)
        {
        	fseek (fich,0,SEEK_SET);
                while(fscanf(fich,"%d %d %d %d %d %d %d %d %d",&col1,&col2,&col3,&col4,&col5,&col6,&col7,&col8,&col9)==9)
                {
                    m[i][0]=col1;
                    m[i][1]=col2;
                    m[i][2]=col3;
                    m[i][3]=col4;
                    m[i][4]=col5;
                    m[i][5]=col6;
                    m[i][6]=col7;
                    m[i][7]=col8;
                    m[i][8]=col9;
                    i++;
                }
                fclose(fich); 
        }
        else
        {
                printf("ERROR al abrir el fichero.\n");
        }

}

/*
  Comprueba que una línea del carton tenga 5 números
*/
int comprobarLinea(int carton [][9], int sorteo[90])
{
  int i, j, k;
  int linea=0;
  int cont=0;

  for(i=0;i<3;i++)
  {
    for(j=0; j<9; j++)
    {
      for(k=0; k<91; k++)
      { 
        if(carton[i][j] == sorteo[k] )
        {
          cont++;  
        }   
      }
    }
  }


  if(cont==5)
  {
    linea=1;
  }

return linea; 
}


/*
  Comprueba que estén todos los números del cartón
*/
int comprobarBingo(int carton [][9], int sorteo[90])
{
  int i, j, k;
  int bingo=0;
  int cont=0;

  for(i=0;i<3;i++)
  {
    for(j=0; j<9; j++)
    {
      for(k=0; k<91; k++)
      { 
        if(carton[i][j] == sorteo[k])
        {
          cont++;  
        }   
      }
    }
  }


  if(cont==15)
  {
    bingo=1;
  }

return bingo; 
}

Cliente clientesSistema[MAX_CLIENTS];

Partida bingo[10];

#endif

