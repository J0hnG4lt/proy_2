Autor: Georvic Tur 
Carnet: 12-11402


COMPILACIÓN
	
	Sólo hay que acceder con el terminal al directorio donde está libreria.c 
	y makefile. Luego, se debe correr la orden "make".
	

EJECUCIÓN
	
	Se debe tener un directorio con carpetas que contengan los archivos de texto.
	
	Si dicho directorio es en realidad un subdirectorio del directorio de 
	trabajo, se ha de correr el programa así:
		
		hacer_libro -d [nombre_subdirectorio] número_de_directorios número_archivos nombre_archivo_salida
	
	Si el directorio de trabajo contiene las carpetas con los archivos de texto,
	entonces se ha de invocar el programa de esta manera:
		
		hacer_libro número_de_directorios número_archivos nombre_archivo_salida
	
	
	El número de directorios ha de ser menor o igual que 10.
	Así mismo, el número de archivos debe ser menor o igual que 20.
	

RETORNO
	
	El archivo de salida contiene una composición aleatoria de los textos 
	que se encuetran en los subdirectorios.

ESTRUCTURA GENERAL DEL PROGRAMA
	
	      ----------------encontrar_directorios_presentes(...)
	      |
	      |
	      |---------------generar_secuencia_aleatorios(...)
	      |
	      |                                          ------------------abrir_directorio(...)
		  |                                          |
	PROCESO_PADRE---------PROCESOS_HIJOS-------abrir_archivos(...)-----generar_secuencia_aleatorios(...)
	      |                     |                    |
	      |                     |                    ------------------abrir_archivo(...)
	      |                     |
	      |                     ---------------enviar_texto_a_padre(...)
	      |
	      ----------------leer_texto_de_hijos(...)


EXPLICACIÓN
	
	El programa principal genera una secuencia de números aleatorios usando
	la subrutina generar_secuencia_aleatorios y los argumentos pasados
	por el terminal. Dichos números representan a los directorios seleccionados.
	
	Posteriormente, por cada directorio seleccionado se crea un proceso hijo.
	Cada proceso hijo inspecciona su directorio llamando a la rutina
	abrir_directorio dentro del procedimiento abrir_archivos. Adicionalmente, 
	el hijo genera una secuencia de números aleatorios con
	generar_secuencia_aleatorios. Dicha secuencia representa los archivos
	seleccionados. Luego el proceso hijo abre cada archivo seleccionado con abrir_archivo.
	Así, el proceso hijo concluye la llamada a abrir_archivos guardando el
	contenido de cada archivo en un arreglo.
	
	Una vez leídos los archivos, los procesos hijos envían sus contenidos
	usando enviar_texto_a_padre.
	
	Mientras los hijos se ejecutan, el padre se bloquea esperando que éstos
	terminen. Una vez que lo hacen, el programa principal llama a
	leer_texto_de_hijos y luego combina los textos de los cada hijo
	en un arreglo que se guarda en un archivo.
