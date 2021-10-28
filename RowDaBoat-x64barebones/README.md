## Autores
<hr>

- [Alberto Abancens - 62581](https://github.com/tataabancens/)

- [Gonzalo Rossin - 60135](https://github.com/GonzaloRossin/)
 
- [Uriel Mihura - 59039](https://github.com/uri-99/)

<hr>

# Manual de Usuario
Sistema operativo basado en Barebonesx64 por RowDaBoat. Primero se debe tener instalado qemu-system para visualizar nuestro SO y Docker (con el contenedor `agodio/itba-so:1.0` otorgrado por la cátedra) para el compilado del mismo. Luego se debe compilar desde Docker y ejecutarlo:
1. Solamente la primera vez que se accede se debe hacer ```cd Toolchain``` y luego ```make all```.
2. Luego para compilar el resto del proyecto (primero volviendo al directorio anterior) se puede hacer de dos formas distintas; cada una determinará el Memory Manager que se utilizará:
- `make all MM=BUDDY_MM ` (para utilizar el buddy system).
- `make all MM=FREELIST_MM ` (para utilizar free list).
- De no funcionar la asignación de la variable MM por consola, se puede cambiar la variable `MM` por la deseada dentro del `makefile` de `kernel`. 
- Importante: es necesario hacer `make clean` entre la compilación con un sistema y el otro.
3. Finalmente solo queda ejecutar el sistema, ya sea con el comando ```run.sh``` desde Linux, o ```./run.bat``` desde Windows.

## Shell
Intéprete de comandos del usuario, el sistema inicia al mismo por defecto. 
Dispone de las siguientes funcionalidades:
- **help**: Despliega en pantalla las funciones disponibles para el usuario. 
- **get_time**: Despliega en pantalla información sobre el día y hora actual del sistema.
- **inforeg**: Despliega en pantalla un snapshot de los registros al momento que es llamado.
- **test_divisionby0**: Verifica el funcionamiento correcto de la excepción de tipo operación de código inválido.
- **test_invalidop**: Verifica el funcionamiento correcto de la excepción de tipo división por cero.
- **printmem**: Despliega en pantalla un volcado de memoria de 32 bytes a partir de una dirección de memoria válida recibida como argumento.
- **clean**: Limpia la pantalla.
- **test_mem**: Testeo de memoria.
- ***ps:*** Despliega el estado de los procesos.
- ***kill:*** Elimina el proceso cuyo pid es el recibido como argumento de entrada.
- ***nice:*** Cambia la prioridad del proceso cuyo pid es el recibido como argumento de entrada.
- ***block:*** Bloquea el proceso cuyo pid es el recibido como argumento de entrada.
- ***FALTA loop:*** Crea un proceso el cual itera sobre si mismo.
- ***mem:*** Imprime el estado de la memoria del memory manager por bloques.
-----------------
faltan estos:
- ***test_processes:*** Prueba la creacion de procesos con el scheduler (archivo dado por la cátedra).
- ***test_prio:*** Prueba la prioridad del scheduler (archivo dado por la cátedra).
- ***test_sync:*** Prueba la sincronizacion de semaforos (archivo dado por la cátedra).
- ***test_no_sync:*** Prueba la sincronizacion sin el uso de semaforos (archivo dado por la cátedra).
- ***test_mm:*** Prueba el manejo de memoria del memory manager (archivo dado por la cátedra).
- ***pipe:*** Despliega información sobre de los pipes activos.
- ***sem:*** Despliega información sobre de los semaforos activos.
- ***wc:*** Cuenta las lineas recibidas como argumento de entrada.
- ***filter:*** Filtra las vocales recibidas como argumento de entrada.
- ***cat:*** Imprime en pantalla los caracteres recibidos como argumento de entrada.
- ***phylo:*** Dilema de los filosofos comiendo, inicia con 5 filosofos, más indiaciones son dadas al ejecutarlo.
--------------------