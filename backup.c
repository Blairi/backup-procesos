#include <dirent.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <string.h>
#include <sys/stat.h>

/* Integrantes :
            ROBERTO ABURTO LOPEZ
            AXEL FERNANO MONTIEL 
            ANDRIK URIEL REYES ROQUE
            IAN ALEXIS SOLIS HERNANDEZ */

int getDirPath(int, char *[]);
int countFiles(char *);
char **getFilesPath(char *, int);
void removeDirectory(char *);
void copyFile(char *, char *);

#define BUFFER_SIZE 1024
char RUTA_ORIGEN[255]; //De donde se van a tomar los archivos
char RUTA_DESTINO[255]; //A donde los queremos hacer backup

int main(int argc, char *argv[])
{

    if( getDirPath(argc, argv) != 0 ){
        printf("Error al leer alguna de las direcciones\n");
        return -1;
    }

    DIR *dir = opendir( RUTA_DESTINO );

    // comprobando si existe la carpeta destino
    if (dir)
        // si existe la eliminamos
        removeDirectory( RUTA_DESTINO );

    // si no existe la creamos
    mkdir( RUTA_DESTINO, 0777 );

    // Contamos los archivos en la carpeta origen    
    int n = countFiles( RUTA_ORIGEN );
    // // Obtenemos las rutas de los archivos
    char **rutasArchivos = getFilesPath( RUTA_ORIGEN, n );
    
    int fd[2];
    pipe( fd );
    pid_t pid = fork();
    char buff[BUFFER_SIZE];

    switch ( pid )
    {
        case -1:
            printf("Error al crear el proceso\n");
            exit(EXIT_FAILURE);
        break;

        // codigo para el hijo
        case 0:
            for (int i = 0; i < n; i++)
            {
                // obtener nombre del archivo
                char *nombreArchivo;
                char *ultimaDiagonal = strrchr(rutasArchivos[i], '/');
                nombreArchivo = ultimaDiagonal + 1;

                printf("(Padre --> %s)\n", nombreArchivo);
                printf("\tHijo[pid=%d], respaldando el archivo: %s\t pendientes: %d/%d\n", getpid(), nombreArchivo, n-i-1, n);

                copyFile( rutasArchivos[i], RUTA_DESTINO );
            }
            // Cerrar lectura
            close(fd[0]);
            // escribir mensaje al padre
            write(fd[1], "Adios padre, termine el respaldo...\n", 37);
            // Cerrar escritura
            close(fd[1]);
            exit(0);
        break;

        // codigo para el padre
        default:
            printf("PADRE [%d]: Hola hijo, realiza el respaldo de %d archivos\n\n", getpid(), n);

            // Cerrando escritura
            close(fd[1]);
            // esperando que el hijo termine de respaldar
            if (read(fd[0], buff, BUFFER_SIZE))
            {
                printf("\nPadre[pid=%d] mensaje de mi hijo: %s\n", getpid(), buff);
            }
            
            printf("Padre[pid=%d] comprobando respaldo:\n", getpid());

            char comandoComprobacion[BUFFER_SIZE];
            strcpy(comandoComprobacion, "ls -l ");
            strcat(comandoComprobacion, RUTA_DESTINO);
            system(comandoComprobacion);


        break;
    }

    printf("\n%d ARCHIVOS RESPALDADOS CON EXITO\n", n);
    printf("Proceso padre[pid=%d] terminado\n", getpid());

    return 0;
}


void copyFile(char *pathFile, char *dest)
{
    char *nombreArchivo;

    // obtener nombre del archivo
    char *ultimaDiagonal = strrchr(pathFile, '/');

    // Si se encuentra el carácter '/'
    if (ultimaDiagonal != NULL) {
        // Avanza un carácter para obtener el nombre del archivo
        nombreArchivo = ultimaDiagonal + 1;
    } else {
        // Si no se encuentra el carácter '/', la ruta contiene solo el nombre del archivo
        nombreArchivo = pathFile;
    }

    // asignamos memoria
    char *archivoRespaldado;
    archivoRespaldado = (char *)malloc(strlen(dest) + 1 + 255);

    strcpy(archivoRespaldado, dest);
    strcat(archivoRespaldado, nombreArchivo);

    // Abre el archivo de origen en modo lectura 
    FILE *fuente = fopen(pathFile, "rb");
    FILE *copia = fopen(archivoRespaldado, "wb");

    if (fuente && copia) {    
        char ch;
    
        // Lee y escribe cada byte del archivo
        while (fread(&ch, sizeof(char), 1, fuente) == 1) { 
            fwrite(&ch, sizeof(char), 1, copia);
        }

        fclose(fuente); 
        fclose(copia);
    }

}


void removeDirectory(char * path)
{
    printf("PADRE [pid=%d]: borrando respaldo viejo...\n", getpid());
    DIR *d; //Puntero para representar al directorio del sistema de archivos
    struct dirent *dir; //Puntero para representar la entrada a un directorio
    struct stat st; //Estructura de datos que contiene informacion detallada acerca de un archivo
    char ruta[255];
    d = opendir( path );
    if ( d )
    {
        while ( (dir = readdir(d)) != NULL )
        {
            // ignorar las rutas relativas
            if (!strcmp(dir->d_name, ".") || !strcmp(dir->d_name, ".."))
            {
                continue;
            }

            strcpy(ruta, path);
            strcat(ruta, dir->d_name);
            stat(ruta, &st);

            // si es un subdirectorio
            if ( !S_ISREG(st.st_mode) )
            {
                strcat(ruta, "/");

                // llamada recursiva para los subdirectorios
                removeDirectory( ruta );

                rmdir( ruta );
                printf("dir removed '%s'\n", ruta);
            }
            else
            {
                unlink( ruta );
                printf("file removed '%s'\n", ruta);
            }
        }
        closedir(d);
        
        // finalmente borramos la carpeta principal
        rmdir( path );
        printf("dir removed '%s'\n", path);
    }
}


/*
    Hacemos una segunda iteracion ahora si para guardar las rutas de los
    archivos en un solo arreglo */
char **getFilesPath(char *path, int size)
{
    DIR *d; //Puntero para representar al directorio del sistema de archivos
    struct dirent *dir; //Puntero para representar la entrada a un directorio
    struct stat st; //Estructura de datos que contiene informacion detallada acerca de un archivo

    char **rutasArchivos = (char **)malloc(size * sizeof(char *));

    d = opendir(path);
    int i = 0;
    while ( (dir = readdir(d)) != NULL )
    {
        char ruta[255];
        strcpy(ruta, path);
        strcat(ruta, dir->d_name);
        stat(ruta, &st);

        // ignoramos las carpetas
        if ( !S_ISREG(st.st_mode) )
        {
            continue;
        }

        // Asigna memoria para cada ruta de archivo y copia la ruta
        rutasArchivos[i] = (char *)malloc(strlen(ruta) + 1);

        /*
        Si es archivo lo guardamos en el arreglo
        e incrementamos el contador */
        strcpy( rutasArchivos[i], ruta );
        i++;
    }
    closedir(d);

    return rutasArchivos;
}


/*
    Hacemos una primera iteracion para conocer los n
    archivos que hay en la ruta que el usuario pone, esto lo hacemos
    para asignar n localidades de memoria al arreglo */
int countFiles(char *path)
{
    DIR *d; //Puntero para representar al directorio del sistema de archivos
    struct dirent *dir; //Puntero para representar la entrada a un directorio
    struct stat st; //Estructura de datos que contiene informacion detallada acerca de un archivo

    int n = 0;

    d = opendir( path );
    if ( d )
    {
        while ( (dir = readdir(d)) != NULL )
        {
            char ruta[255];
            strcpy(ruta, path);
            strcat(ruta, dir->d_name);
            stat(ruta, &st);
            // ignoramos las carpetas
            if (!S_ISREG(st.st_mode))
            {
                continue;
            }

            // si es archivo incrementamos el contador
            n++;
        }
        closedir(d);
    }
    else
    {
        printf("\033[1;31mCarpeta de origen no existente, imposible de abrir\033[1;31m\n");
        exit(EXIT_FAILURE);
        return -1;
    }

    return n;
}


int getDirPath(int argc, char *argv[]){
    printf("Seleccione una opcion para hacer backup: \n");
    printf("1. Archivo .txt en esta misma carpeta\n");
    printf("2. Argumento de entrada al programa, argumento 1: Origen, argumento 2: Destino\n");
    printf("3. Por teclado\n");
    printf("Cualquier otra opcion terminara INMEDIATAMENTE el programa\n");
    int opcion = 0;
    scanf("%d", &opcion);

    /*Switch para elegir una opción de backup, por un archivo, 
    argumentos del programa, consola*/
    switch (opcion)
    {
    case 1:
    system("clear");
    FILE *archivo; //Apuntador al archivo que se abrira
    char nombre_archivo[100]; //Nombre del archivo a abrir
    printf("\033[1;33mEl archivo debe contener dos lineas, 1: Direccion a guardar, 2: Direccion destino\n");
    printf("Nombre del archivo: "); 
    scanf("%s", nombre_archivo);
    archivo = fopen(nombre_archivo, "r"); //Se intenta abrir el archivo
    if (archivo == NULL) {
        printf("No se pudo abrir el archivo.\n");
        return -1;
    }
    if(fgets(RUTA_ORIGEN, sizeof(RUTA_ORIGEN), archivo) != NULL){
        if(RUTA_ORIGEN[strlen(RUTA_ORIGEN) - 1] == '\n'){RUTA_ORIGEN[strlen(RUTA_ORIGEN) - 1] = '\0';} //Elimina caracter escape
    }
    if(fgets(RUTA_DESTINO, sizeof(RUTA_DESTINO), archivo) != NULL){
        if(RUTA_DESTINO[strlen(RUTA_DESTINO) - 1] == '\n'){RUTA_DESTINO[strlen(RUTA_DESTINO) - 1] = '\0';} //Elimina caracter escape
    }
        break;
    case 2:
    system("clear");
    if(argc==3){
        strcpy(RUTA_ORIGEN, argv[1]);
        strcpy(RUTA_DESTINO, argv[2]);
    }
    else{
        printf("No hay argumentos suficientes para hacer el backup\n");
        return -1;
    }
        break;
    case 3:
    system("clear");
    printf("Directorio actual:\n");
    system("pwd");
    printf("Ingresar la ruta de la carpeta de la que quiera hacer un backup,\n");
    printf("indique un \'.\' al principio para referir a directorios dentro del actual: ");
    scanf("%s", RUTA_ORIGEN);
    printf("Ingresar la ruta de la carpeta de en la que quiera guardar los archivos,\n");
    printf("indique un \'.\' al principio para referir a directorios dentro del actual: ");
    scanf("%s", RUTA_DESTINO);
        break;
    default:
    return -1;
        break;
    }

    /*Validación para que el backup sea exitoso*/
    if((RUTA_ORIGEN[strlen(RUTA_ORIGEN)-1]!= '/')||(RUTA_DESTINO[strlen(RUTA_DESTINO)-1]!= '/')){
        printf("Formato de directorio invalido, debe acabar en /\n");
        return -1;
    }
    return 0;
}