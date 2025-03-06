# ESTRUCTURAS DE DATOS - PROYECTO

## Uso

Para correr el programa, se deben ejecutar los siguientes comandos:

Para compilar:
```bash
[ruta-repositorio]$ make
```

Para ejecutar:
```bash
[ruta-repositorio]$ ./exec/run --users [DATASET_USUARIOS] --connections [DATASET_CONEXIONES]
```

Tras la ejecución, se crearán automáticamente los siguientes archivos:
 * Un archivo `log.txt` que contiene algunos detalles de la ejecución de los algoritmos
 * Un archivo `biases.txt` que contiene la asignación de tendencias a cada usuario
 * Un archivo `components.txt` que contiene una descripción de las componentes fuertemente conexas del grafo

## Ejemplo

Si el repositorio fue clonado en la carpeta `/home/usuario/Escritorio` con el nombre `proyecto`, se pueden usar los siguientes comandos para ejecutar el programa sobre el dataset completo.

```bash
/home/usuario/Escritorio/proyecto$ make
/home/usuario/Escritorio/proyecto$ ./exec/run --users "./data/users.csv" --connections "./data/connections.csv"
```

(Más datasets de prueba disponibles en la carpeta `data`, junto con representaciones gráficas en la carpeta `docs`)
