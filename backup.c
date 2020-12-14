/*
	Materia: Sistemas Operativos
	Examen Parcial 3.2

	Alumnos:
				Carrillo Hernández Rogelio Ignacio
				Cruz Matías Yuridia Elizabeth

	Profesora:  Ing. Patricia Del Valle Morales

*/

#include <string.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <memory.h>
#include <strings.h>

#define SIZE 512
 
int main( int argc, char **argv )
{
	pid_t pid;
	int a[2], b[2],c[2],d[2],e[2],f[2],g[2],h[2],bucle=1;
	char buffer[SIZE],buffer1[SIZE],buffer2[SIZE],buffer3[SIZE],buffer4[SIZE];

	 if (pipe(a) < 0 || pipe(b) < 0 || pipe(c) < 0 || pipe(d) < 0 || pipe(e) < 0 || pipe(g) < 0 || pipe(h) < 0 || pipe(f) < 0) { 
	 	perror("\nError al crear el pipe");
		exit(1);
	 }

	system ("clear");
    pid = fork();
    switch(pid){
		case -1: // Error
			printf("No se ha podido crear un hijo \n");
			exit(-1);
		break;
		case 0:
			close(a[0]);//manda mensaje a padre de que recibio lainstruccion
			close(b[1]);//lee el mensaje del padre
			close(c[1]);//lee el tamaño del lineas del archivo
			close(d[1]);//lee ./Lista_Archivos el directorio origen
			close(e[1]);//lee ./respaldo directorio destino
			close(f[1]);//arreglo que recibe linea del archivo txt
			close(g[0]);//manda mensaje a padre de que termino
			close(h[0]);//manda el numeotr de archivos al padre
			char*str1,*str2,*str3,*str4,*s;
			int k;
			printf("\nHIJO(pid=%i) esperando mensaje de mi padre...\n", getpid());
			read(b[0], buffer, 256);
			printf("\nHIJO(pid=%i), Instruccion de mi padre: %s\n", getpid(), buffer);
			write(a[1],"Claro padre, a trabajar",SIZE);
			
			read(c[0], buffer1, 256);
			buffer1[strlen(buffer1)-1]='\0';//se elimina el salto de linea
			s=buffer1;
			k=atoi(buffer1);//Servira para imprimir los archivos que faltan
			
			str1="cp ";
			read(d[0], buffer2,256);
			str2=buffer2;
			read(e[0], buffer3,256);
			str3=buffer3;
			while(bucle){//bucle es el ciclo que nos ayuda a identificar
				k--;
				bzero(buffer4,SIZE);
				if(read(f[0], buffer4, SIZE)>0){//La funcion read devuelve los bytes de la palabra por lo que se asegura que esa palabra exista con >0
					printf("\nHijo(pid=%i), respaldando el archivo %s \t pendientes %i / %s",getpid(),buffer4,k,s);
					
					char*command=(char*)malloc(strlen(str1)+strlen(str2)+strlen(buffer4)+strlen(str3));
					strcpy(command, str1);
					strcat(command, str2);
					strcat(command, buffer4);
					strcat(command, str3);
					bzero(buffer4,SIZE);
					
					system(command);
					if(k==0){
						write(g[1]," Listo Termine",SIZE);
						write(h[1],s,SIZE);
						bucle=0;
					}
				}
				else{
					printf("\n No se puede realizar el respaldo\n");
				}
				bzero(buffer4,SIZE);
			}
			close(a[1]);
			close(b[0]);
			close(c[0]);
			close(d[0]);
			close(e[0]);
			close(f[0]);
			close(g[1]);
			close(h[1]);
			exit(0);
			break;
		default: 
			close(a[1]);
			close(b[0]);
			close(c[0]);
			close(d[0]);
			close(e[0]);
			close(f[0]);
			close(g[1]);
			close(h[1]);

			system("rm ./reporte.txt");
			system("rm ./list.txt");
			system("rm ./info_num_total.txt");

			FILE *fichero_list, *fichero_info_num_total,*fichero_reporte;
			char list[1024], info_num_total[8], reporte[8] ;
			int rep, rep1;
			
            printf("\nPADRE(pid=%i): borrando respaldo viejo...",getpid());
            system("rm -rfv respaldo");   
            system("mkdir respaldo");
            printf("\nPADRE(pid=%i): generando Lista de Archivos a Respaldar",getpid());

            system("ls Lista_Archivos > list.txt");//genera el archivo txt
			system("ls ./Lista_Archivos | wc -l > info_num_total.txt");// genera un archivo con el nuemto toral de arhivos
			
			write(b[1],"Hola hijo, realiza el respaldo de archivos", SIZE); 
			read(a[0], buffer, SIZE);
			printf("\nPADRE(pid=%i), lee mensaje de mi hijo: %s\n", getpid(), buffer);
			//NUMERO TOTAL DE ARCHIVOS

			fichero_info_num_total = fopen("info_num_total.txt", "r");	
			while(fgets(info_num_total, 8, (FILE*) fichero_info_num_total)) {
				printf("\n\tTotal de archivos a respaldar : %s    \n", info_num_total);
				rep1=atoi(info_num_total);//variable donde se almacenan lo sustraido del archivo
				write(c[1],info_num_total,SIZE);//comunica cada uno de los nombres de los archivos añ hijo
			}
			
			write(d[1],"./Lista_Archivos/",SIZE);
			write(e[1]," ./respaldo",SIZE);
			
			//abre archivo de lista
			fichero_list = fopen("list.txt", "r");
			//write(f[0][1],&prue ,SIZE);
			while(fgets(list, 1024, (FILE*) fichero_list)) {
				list[strlen(list)-1]='\0';
				write(f[1],list,SIZE);
			}
            //write mandar el archivo txt 
			
			read(g[0],buffer2,SIZE);
			printf("\n\nPADRE(pid=%i), lee mensaje de mi hijo: %s\n\n", getpid(), buffer2);
			
			read(h[0], buffer3, SIZE);
			printf("PADRE(pid=%i), recibe el TOTAL %s archivos respaldados\n", getpid(), buffer3);
			
			//comprobando respaldo
			//Se cera otro archiv donde se cuenta los archivos de carpeta de respaldo y se guardan el el, 
			//posteriormente se extrae y se compara con la del archivo dende se cuentan los archivos de la carpeta Lista_Archivo
			system("ls ./respaldo | wc -l > reporte.txt");
			fichero_reporte = fopen("reporte.txt", "r");
			while(fgets(reporte, 8, (FILE*) fichero_reporte)) {
				rep=atoi(reporte);//variable donde se almacenan lo sustraido del archivo
			}
			if(rep == rep1){
				printf("\n---------------------------------------------\n");
				system("ls -l respaldo");
				printf("\n---------------------------------------------\n");
			}
			else{
				printf("\n---------------------------------------------\n");
				printf("\n\tNo se realizo correctamente el respaldo\n");
				printf("\n---------------------------------------------\n");
			}
			fclose(fichero_list);
			fclose(fichero_reporte);
			fclose(fichero_info_num_total);
			close(a[0]);
			close(b[1]);
			close(c[1]);
			close(d[1]);
			close(e[1]);
			close(f[1]);
			close(g[0]);
			close(h[0]);
		break;
	}
	printf("\nTermino el proceso padre...\n\n");
	return 0;
}