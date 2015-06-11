#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <dirent.h>
#include <string.h>
#include <sys/wait.h>

#define TAM_BUFFER_ARCHIVO 500
#define NUM_MAX_ARCHIVOS 20
#define NUM_MAX_DIRECTORIOS 10



/*
	Autor: Georvic Tur ---- Carnet: 12-11402
	
*/


/*
--------------------------------------------------------------------------------
--------------------------------------------------------------------------------
--------------------------------------------------------------------------------
                            FUNCIONES
--------------------------------------------------------------------------------
--------------------------------------------------------------------------------
--------------------------------------------------------------------------------
*/

void abrir_directorio(char *nombre_directorio, char *nombres_archivos[], int *num_archivos){

	/*
	Abre un directorio de nombre nombre_directorio y obtiene los nombres de todos sus
	archivos, además de su cantidad.
	*/

	

	DIR *directorio = opendir(nombre_directorio); //Abre directorio
	struct dirent *entrada_directorio;
	
	//Se recorre el directorio y se guardan los nombres de archivos
	char *nombre_archivo;
	*num_archivos = 0;
	entrada_directorio = readdir(directorio);
	
	while(entrada_directorio != NULL){
		
		//Solo tomo en cuenta a los archivos regulares
		
		if (strcmp(entrada_directorio->d_name,".") == 0){
			entrada_directorio = readdir(directorio);
			continue;
		}
		
		if (entrada_directorio->d_type != DT_REG){
			entrada_directorio = readdir(directorio);
			continue;
		}
		
		nombre_archivo = (char *)malloc(sizeof(char)*256);
		strcpy(nombre_archivo, entrada_directorio->d_name);
		nombres_archivos[*num_archivos] = nombre_archivo;
		*num_archivos = *num_archivos + 1;
		entrada_directorio = readdir(directorio);
	}
	
	closedir(directorio);

}


void abrir_archivo(char *nombre_archivo, char *texto_parcial){

	/*
	Abre un archivo de nombre nombre_archivo y guarda e texto_parcial el texto 
	que alli se encuentra.
	
	La memoria de texto parcial se solicita fuera de esta funcion
	*/
	
	int descriptor = open(nombre_archivo, O_RDONLY);
	read(descriptor, texto_parcial,TAM_BUFFER_ARCHIVO);
	
	close(descriptor);

}


int esta_en(int numero, int arreglo[], int tamano){

	/*
	Retorna 1 si el nuemro esta en el arreglo. De lo contrario, retorna 0
	*/

	int i;
	for(i = 0; i < tamano; i++){
		
		if (numero == arreglo[i]){
			return 1;
		}
	
	}
	
	return 0;

}


void generar_secuencia_aleatorios(int secuencia_numeros_aleatorios[]
									,int numero_archivos_seleccionados
									,int tamano_de_secuencia){
	
	/*
	Genera una secuencia de numeros aleatorios que representan una seleccion
	de los archivos del directorio. 
	*/
	
	
	//Introduzco la semilla
	int semilla = 1;
	srand(semilla);
	
	int numero_aleatorio;
	
	//Genero numeros aleatorios unicos
	int i = 0;
	while(i < numero_archivos_seleccionados){
	
	
		numero_aleatorio = rand(); //Siguiente numero de la secuencia
		numero_aleatorio = numero_aleatorio % tamano_de_secuencia + 1;
		
		if (esta_en(numero_aleatorio, secuencia_numeros_aleatorios, i)){
			continue;
			
		}
		else{
			
			secuencia_numeros_aleatorios[i] = numero_aleatorio;
			i++;
		}
		
	}
	

}

void abrir_archivos(char *nombre_dir //Nombre del directorio
					, int *num_archivos //Numero de archivos de nombre_dir
					, int numero_archivos_seleccionados
					, char **arreglo_textos_parciales) //Se guardan los punteros a textos
	{
	
	/*
	Datos los nombres de los archivos y el directorio, asi como la seleccion
	aleatoria de los mismos, procedo a abrirlos.
	
	POST: 
		num_archivos
		arreglo_textos_parciales
	*/
	
	
	//Abro el directorio y leo los nombres de los archivos
	char *nombres_archivos[20];
	abrir_directorio(nombre_dir, nombres_archivos, num_archivos);
	
	//Obtengo una secuencia de numeros aleatorios
	int secuencia_numeros_aleatorios[20];
	generar_secuencia_aleatorios(secuencia_numeros_aleatorios
								, numero_archivos_seleccionados
								, numero_archivos_seleccionados);
	
	
	//Solicito memoria para el arreglo de textos parciales
	//arreglo_textos_parciales = (char **)malloc(sizeof(char *)*numero_archivos_seleccionados);
	
	//Leo los archivos y guardo cada texto parcial en un arreglo
	char *path_archivo;
	char *texto_parcial;
	int indice;
	int i;
	for(i = 0; i < numero_archivos_seleccionados; i++){
	
		//Obtengo el indice del archivo seleccinado
		//Se supone que secuencia_numeros_aleatorios contiene una secuencia
		// de indices que representan una seleccion aleatoria de los archivos
		// encontrados en el directorio
		indice = secuencia_numeros_aleatorios[i];
		
		//Construyo en path de dicho archivo
		path_archivo = (char *)malloc(sizeof(char)*200);
		//strcat(path_archivo, nombre_dir);
		//strcat(path_archivo, "/");
		//strcat(path_archivo, nombres_archivos[indice]);
		
		sprintf(path_archivo, "%s/%s", nombre_dir, nombres_archivos[indice]);
		
		//Arbo el archivo
		texto_parcial = (char *)malloc(sizeof(char)*TAM_BUFFER_ARCHIVO);
		abrir_archivo(path_archivo, texto_parcial);
		arreglo_textos_parciales[i] = texto_parcial;
		
		free(path_archivo);
		free(nombres_archivos[indice]);
	}
	
	
	}
	
	
	

void enviar_texto_a_padre(int mi_pipe[2], char **arreglo_textos_parciales, int tam_arreglo){

	/*
	Le envia los textos del arreglo al proceso padre a traves del pipe
	*/
	
	int i;
	for(i = 0; i < tam_arreglo; i++){
	
		write(mi_pipe[1], arreglo_textos_parciales[i]
						, TAM_BUFFER_ARCHIVO);
	
	}
	

}


void leer_texto_de_hijos(int mi_pipe[2]
						, char texto_parcial[20][TAM_BUFFER_ARCHIVO]
						, int tam_arreglo)
{

	/*
	El padre usa pipes para leer los textos de cada hijo
	*/
	
	int i;
	for(i = 0; i < tam_arreglo; i++){
	
		if (read(mi_pipe[0], texto_parcial[i], TAM_BUFFER_ARCHIVO) <= 0){
			break;
		}
	
	}
	
	for(i=tam_arreglo;i < 20; i++){
		
		strcpy(texto_parcial[i], "");
	}
	
}


/*
--------------------------------------------------------------------------------
--------------------------------------------------------------------------------
--------------------------------------------------------------------------------
                          PROGRAMA PRINCIPAL
--------------------------------------------------------------------------------
--------------------------------------------------------------------------------
--------------------------------------------------------------------------------
*/

int main(int argc, char *argv[]){
	
	int numero_directorios;
	int numero_archivos;
	char *directorio_de_trabajo; //Sera anexada al principio de los nombres
								//de los directorios
	char *nombre_archivo_salida = (char *)malloc(sizeof(char)*100);
	
	
	
	//Examino los argumentos del ejecutable
	if (strcmp("-d", argv[1]) == 0){
	
		//Si se especifica un directorio de trabajo
		directorio_de_trabajo = (char *)malloc(sizeof(char)*100);
		strcpy(directorio_de_trabajo, argv[2]);
		numero_directorios = atoi(argv[3]);
		numero_archivos = atoi(argv[4]);
		strcpy(nombre_archivo_salida, argv[5]);
		
	}
	else{
	
		//Si no se especifica un directorio de trabajo
		directorio_de_trabajo = (char *)malloc(sizeof(char)*100);
		strcpy(directorio_de_trabajo, "");
		numero_directorios = atoi(argv[2]);
		numero_archivos = atoi(argv[3]);
		strcpy(nombre_archivo_salida, argv[4]);
		
	
	
	}
	
	strcat(directorio_de_trabajo, "/");
	
	
	
	//Si hubo algun error con el input
	
	if ((numero_archivos > 20)&&(numero_directorios > 10)){
	
		printf("El numero de archivos ha de ser menor o igual que 20\n");
		printf("El numero de directorios ha de ser menor o igual que 10\n");
		
		free(nombre_archivo_salida);
		free(directorio_de_trabajo);
		
		exit(1);
	}
	
	if ((numero_archivos > 20)&&(numero_directorios < 10)){
	
		printf("El numero de archivos ha de ser menor o igual que 20\n");
		
		free(nombre_archivo_salida);
		free(directorio_de_trabajo);
		
		exit(1);
	
	}
	
	if ((numero_archivos < 20)&&(numero_directorios > 10)){
	
		printf("El numero de directorios ha de ser menor o igual que 10\n");
		
		free(nombre_archivo_salida);
		free(directorio_de_trabajo);
		
		exit(1);
	
	}
	
	
	
	
	//Genero una secuencia de numeros unicos que representan a directorios
	int arreglo_directorios[10];
	generar_secuencia_aleatorios(arreglo_directorios, numero_directorios, NUM_MAX_DIRECTORIOS);
	
	
	//Path de cada hijo
	char *path_directorio_hijo = (char *)malloc(sizeof(char)*100);
	
	//Creo los pipes
	int arreglo_pipes[numero_directorios][2];
	int i;
	for(i = 0; i < numero_directorios; i++){
	
		pipe(arreglo_pipes[i]);
	}
	
	
	
	//Entero a ASCII
	char *itoa[10] = {"1", "2", "3", "4", "5", "6", "7", "8", "9", "10"};
	
	//Creo un abanico de procesos
	int procesos_hijos_pid[numero_directorios];
	
	//int i;
	for(i = 0; i < numero_directorios; i++){
	
		procesos_hijos_pid[i] = fork();
		
		if (procesos_hijos_pid[i] == 0){
		
			//CODIGO QUE EJECUTAN LOS HIJOS
			
			//Genero el path del directorio del hijo actual
			strcpy(path_directorio_hijo, directorio_de_trabajo);
			strcat(path_directorio_hijo, itoa[arreglo_directorios[i]]);
			
			//Se abren los archivos del directorio correspondiente y se crea
			// el texto parcial
			
			int numero_archivos_encontrados;
			char **arreglo_textos_parciales;
			//Solicito memoria para el arreglo de textos parciales
			arreglo_textos_parciales = (char **)malloc(sizeof(char *)*numero_archivos);
	
			abrir_archivos(path_directorio_hijo
							, &numero_archivos_encontrados
							, numero_archivos
							, arreglo_textos_parciales);
			
			
			//Le comunico al padre el texto parcial del hijo usando pipes
			
			enviar_texto_a_padre(arreglo_pipes[i]
								, arreglo_textos_parciales
								, numero_archivos);
			
			close(arreglo_pipes[i][0]);
			close(arreglo_pipes[i][1]);
			
			
			
			//Libera Memoria
			free(directorio_de_trabajo);
			free(nombre_archivo_salida);
			free(path_directorio_hijo);
			
			
			
			int m;
			for(m = 0; m < numero_archivos; m++){
				free(arreglo_textos_parciales[m]);
			}
			
			
			free(arreglo_textos_parciales);
			
			//El proceso hijo termina
			exit(0);
			
			
		}
		else if(procesos_hijos_pid[i] < 0){
		
			printf("Error proceso hijo\n");
		}
		
	
	}
	
	
	//CODIGO DEL PROCESO PADRE

	
	//Espero que todos los hijos terminen
	while(wait(NULL) > 0);
	
	
	
	//Leo los pipes
	char arreglo_textos_parciales_de_cada_hijo[numero_directorios][20][TAM_BUFFER_ARCHIVO];
	
	for(i = 0; i < numero_directorios; i++){
	
		leer_texto_de_hijos(arreglo_pipes[i]
							, arreglo_textos_parciales_de_cada_hijo[i]
							, numero_archivos);
		
		close(arreglo_pipes[i][0]);
		close(arreglo_pipes[i][1]);
		
	}
	

	
	
	//Debo guardar los textos en un archivo de salida
	
	int descriptor_archivo = open(nombre_archivo_salida, O_WRONLY | O_CREAT | O_APPEND, 0640);
	
	int k;
	int j;
	for(k = 0; k < numero_directorios; k++){
	
		for(j = 0; j < 20; j++){
		
			write(descriptor_archivo
					,arreglo_textos_parciales_de_cada_hijo[k][j]
					, strlen(arreglo_textos_parciales_de_cada_hijo[k][j]));
			
		}
	
	}
	
	close(descriptor_archivo);
	
	
	//Liberar Memoria
	free(directorio_de_trabajo);
	free(nombre_archivo_salida);
	free(path_directorio_hijo);
	
	

	return 0;
}
