#include <dirent.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <string.h>
#include <sys/stat.h>

int main()
{
    // system("clear");

    DIR *d;
    struct dirent *dir;
    struct stat st;

    // TODO: leer la ruta del usuario (hacerla dinamica)
    char RUTA_ORIGEN[255];
    strcpy(RUTA_ORIGEN, "./documentos/");
    char RUTA_DESTINO[255];
    strcpy(RUTA_DESTINO, "./backup/");

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

    /*
    Hacemos una segunda iteracion ahora si para guardar las rutas de los
    archivos en un solo arreglo */
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

    printf("Listado de archivos a respaldar:\n");
    for (int j = 0; j < i; j++) {
        printf("Archivo %d: %s\n", j+1, nombreArchivo[j]);
    }

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