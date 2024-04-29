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

char RUTA_ORIGEN[255]; //De donde se van a tomar los archivos
char RUTA_DESTINO[255]; //A donde los queremos hacer backup

int main(int argc, char *argv[])
{
    // system("clear");

    DIR *d; //Puntero para representar al directorio del sistema de archivos
    struct dirent *dir; //Puntero para representar la entrada a un directorio
    struct stat st; //Estructura de datos que contiene informacion detallada acerca de un archivo

    if(getDirPath(argc, argv) != 0){
        printf("Error al leer alguna de las direcciones\n");
        return 1;
    }
    /*
    Hacemos una primera iteracion para conocer los n
    archivos que hay en la ruta que el usuario pone, esto lo hacemos
    para asignar n localidades de memoria al arreglo */
    int n = 0;
    d = opendir( RUTA_ORIGEN );
    if (d)
    {
        while ((dir = readdir(d)) != NULL)
        {
            char path[255];
            strcpy(path, RUTA_ORIGEN);
            strcat(path, dir->d_name);
            stat(path, &st);
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
        return 1;
    }

    /*
    Hacemos una segunda iteracion ahora si para guardar las rutas de los
    archivos en un solo arreglo*/
    char nombreArchivo[n][255];
    d = opendir(RUTA_ORIGEN);
    int i = 0;
    while ((dir = readdir(d)) != NULL)
    {
        char path[255];
        strcpy(path, RUTA_ORIGEN);
        strcat(path, dir->d_name);
        stat(path, &st);

        // ignoramos las carpetas
        if (!S_ISREG(st.st_mode))
        {
            continue;
        }

        /*
        Si es archivo incrementamos el contador
        y lo guardamos en el arreglo */
        strcpy(nombreArchivo[i], path);
        i++;
    }
    closedir(d);

    /*Salida a consola
    printf("Listado de archivos a respaldar:\n");
    for (int j = 0; j < i; j++) {
        printf("\033[1;36mArchivo %d\033[1;36m: %s\n", j+1, nombreArchivo[j]);
    }
    */
    FILE *archivoListaBackup;
    archivoListaBackup = fopen("listaBackup.txt", "w");
    if(archivoListaBackup == NULL){printf("Imposible abrir archivo.\n"); return 1;}
    fprintf( archivoListaBackup ,"Listado de los %d archivos a respaldar:\n", i);
    for (int j = 0; j < i; j++) {
        fprintf(archivoListaBackup,"Archivo %d: %s\n", j+1, nombreArchivo[j]);
    }
    fclose(archivoListaBackup);

    // Copiamos 
    // for (int j = 0; j < i; j++) {
    //     char path_origen[255];
    //     strcpy("")
    //     strcpy(path_origen, RUTA_ORIGEN);
    //     strcat(path_origen, dir->d_name);
    // }

    // creamos un nuevo proceso hijo
    // pid = fork();

    // switch ( pid )
    // {
    //     case -1:
    //         printf("Error al crear el proceso\n");
    //     break;

    //     // codigo para el hijo
    //     case 0:
    //         printf("%d: Soy el proceso hijo [%d] y mi padre es [%d]\n", getpid(), getppid());

    //         // ponemos a trabajar al hijo
    //         sleep(1);

    //         printf("\thijo [%d]...Termine\n", getpid());
    //         // matamos al hijo para que no haga nietos
    //         exit(0);
    //     break;

    //     // codigo para el padre
    //     default:
    //         printf("Soy el proceso principal [%d]\n", getpid());
    //         /*
    //         Esperamos a que el nuevo proceso hijo termine para subir al fork de nuevo
    //         y crear un nuevo hijo */
    //         waitpid(pid, NULL, 0);
    //     break;
    // }

    return 0;
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
        return 1;
    }
    if(fgets(RUTA_ORIGEN, sizeof(RUTA_ORIGEN), archivo) != NULL){
        if(RUTA_ORIGEN[strlen(RUTA_ORIGEN) - 1] == '\n'){RUTA_ORIGEN[strlen(RUTA_ORIGEN) - 1] = '\0';} //Elimina caracter escape
    }
    if(fgets(RUTA_DESTINO, sizeof(RUTA_DESTINO), archivo) != NULL){
        if(RUTA_DESTINO[strlen(RUTA_DESTINO) - 1] == '\n'){RUTA_DESTINO[strlen(RUTA_DESTINO) - 1] = '\0';} //Elimina caracter escape
    }
    fclose(archivo);
        break;
    case 2:
    system("clear");
    if(argc==3){
        strcpy(RUTA_ORIGEN, argv[1]);
        strcpy(RUTA_DESTINO, argv[2]);
    }
    else{
        printf("No hay argumentos suficientes para hacer el backup\n");
        return 1;
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
    return 1;
        break;
    }

    /*Validación para que el backup sea exitoso*/
    if((RUTA_ORIGEN[strlen(RUTA_ORIGEN)-1]!= '/')||(RUTA_DESTINO[strlen(RUTA_DESTINO)-1]!= '/')){
        printf("Formato de directorio invalido, debe acabar en /\n");
        return 1;
    }
    return 0;
}