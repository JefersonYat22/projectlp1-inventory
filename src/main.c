/*
 * ========================================================
 * ARCHIVO: main.c (ENTRADA PRINCIPAL DEL PROGRAMA)
 * 
 * Este es el punto de entrada del programa.
 * Contiene la función main() que se ejecuta primero cuando
 * el usuario corre el archivo ejecutable (inventario.exe).
 * ========================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include "database.h"
#include "models.h"

/*
 * ACERCA DE LOS #include "..." SIN RUTA RELATIVA
 * 
 * Notas que este archivo (src/main.c) usa:
 *   #include "database.h"
 *   #include "models.h"
 * 
 * Pero database.h y models.h están en include/, no en src/.
 * ¿Cómo funciona sin escribir "../include/database.h"?
 * 
 * RESPUESTA: El compilador tiene una lista de búsqueda de carpetas.
 * 
 * En compile.bat ves:
 *   gcc ... -Iinclude -o ...
 * 
 * El flag "-I" (sub-literal 'I' mayúscula) significa:
 * "Agrega la carpeta 'include/' a la búsqueda de headers".
 * 
 * Así cuando el compilador ve #include "database.h", busca en:
 * 1. La carpeta actual (src/) → No lo encuentra
 * 2. Las carpetas agregadas con -I (include/) → LO ENCUENTRA ✓
 * 3. Las carpetas del sistema (librerías del SO)
 * 
 * VENTAJA:
 * El código fuente no necesita conocer la estructura de carpetas.
 * Si cambias la organización, solo modificas compile.bat, no todo el código.
 * ========================================================
 */

int main() {
    /*
     * PASO 1: Imprime información del programa
     */
    printf("--- SISTEMA DE INVENTARIO (SQLite3 + C) ---\n");
    printf("SQLite3 Version: %s\n\n", sqlite3_libversion());

    /*
     * PASO 2: Inicializa la base de datos
     * db_init() es una función wrapper declarada en database.h
     * Implementada en database.c
     * 
     * Retorna SQLITE_OK (valor 0) si todo va bien
     * Retorna otro código de error si algo falla
     */
    if (db_init("data/inventario.sqlite") != SQLITE_OK) {
        return 1;  // Termina el programa si falla la BD
    }
    printf("[+] Base de datos conectada (Modo Relacional).\n");

    /*
     * PASO 3: Demuestra el funcionamiento básico
     * Crea una categoría y un producto vinculado
     */
    db_add_category("Electronica");
    
    /*
     * Crea una estructura Producto con datos de ejemplo
     * Usa la estructura definida en models.h
     */
    Producto ej = {0, "Laptop Gaming", 1, 1200.50, 10};
    
    /*
     * Intenta insertar el producto y reporta el resultado
     */
    if (db_add_product(ej) == SQLITE_OK) {
        printf("[+] Producto '%s' (Categoria ID: %d) guardado correctamente.\n", ej.nombre, ej.id_categoria);
    }

    return 0;
}
