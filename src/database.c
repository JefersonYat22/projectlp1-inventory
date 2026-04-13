#include <stdio.h>
#include <string.h>
#include "sqlite3.h"
#include "database.h"

/*
 * ========================================================
 * ARCHIVO: database.c (IMPLEMENTACIÓN - LÓGICA DE BASE DE DATOS)
 * 
 * Este archivo implementa todas las funciones que declara database.h.
 * Aquí está la "cocina" donde sucede la magia de SQLite3.
 * 
 * CONCEPTO FUNDAMENTAL: CALLBACKS (Funciones de Retorno Llamada)
 * 
 * Un callback es una función que TÚ das como parámetro a OTRA función,
 * diciendo: "Cuando termines de hacer tu trabajo, LLAMA a MI función
 * para que YO procese los resultados".
 * 
 * Es como decir a tu amigo: "Ve al supermercado y cuando regreses,
 * LLÁMAME para que vea qué compraste".
 * 
 * En SQLite:
 * - TÚ llamas a sqlite3_exec(db, sql, MI_CALLBACK, ...);
 * - sqlite3_exec() ejecuta el SQL y obtiene los resultados.
 * - Por CADA FILA de resultado, sqlite3_exec() llama a MI_CALLBACK.
 * - MI_CALLBACK recibe los datos de esa fila y los procesa.
 * 
 * EJEMPLO DE FLUJO:
 * 
 * 1. Tú llamas: db_list_products()
 * 2. db_list_products() arma el SQL: SELECT ... FROM productos
 * 3. db_list_products() llama: sqlite3_exec(db, sql, db_list_callback, ...)
 * 4. sqlite3_exec() ejecuta el SELECT y obtiene 3 productos
 * 5. Para el PRIMER producto, sqlite3_exec() LLAMA a db_list_callback()
 *    pasando los datos (id=1, nombre="Laptop", precio=1200, ...)
 * 6. db_list_callback() imprime en pantalla: 1 | Laptop | 1200 | ...
 * 7. Para el SEGUNDO producto, sqlite3_exec() LLAMA de nuevo a
 *    db_list_callback() con los datos del segundo producto
 * 8. db_list_callback() imprime: 2 | Teclado | 50 | ...
 * 9. TERCERA fila, CUARTA fila, etc. El callback se ejecuta para CADA fila.
 * 
 * PARÁMETROS DEL CALLBACK (Para callbacks de SELECT):
 * - void* data: parámetro adicional que pasaste a sqlite3_exec()
 *              (usualmente NULL si no lo necesitas)
 * - int argc: número de columnas en esta fila
 *            (en nuestro case: 5 columnas -> id, nombre, precio, stock, categoria)
 * - char** argv: array con los VALORES de cada columna como strings
 *               argv[0] = "1"
 *               argv[1] = "Laptop Gaming"
 *               argv[2] = "1200.5"
 *               etc...
 * - char** col_name: array con los NOMBRES de cada columna (no usamos)
 * 
 * RETORNO DEL CALLBACK:
 * - Retorna 0: "Continúa procesando más filas"
 * - Retorna no-cero: "Detén la consulta ya"
 * 
 * ========================================================
 */

/* Prototipos de funciones callback (se definen después) */
static int db_list_callback(void* data, int argc, char** argv, char** col_name);
static int db_search_callback(void* data, int argc, char** argv, char** col_name);

static sqlite3 *db = NULL;

int db_init(const char* db_name) {
    int rc = sqlite3_open(db_name, &db);
    if (rc != SQLITE_OK) {
        fprintf(stderr, "No se pudo abrir la base de datos: %s\n", sqlite3_errmsg(db));
        return rc;
    }

    // SQL DETALLADO DE PERSONA B (Arquitecto de Datos)
    const char *sql = 
        "CREATE TABLE IF NOT EXISTS categorias ("
        "  id INTEGER PRIMARY KEY AUTOINCREMENT,"
        "  nombre TEXT NOT NULL UNIQUE"
        ");"
        
        "CREATE TABLE IF NOT EXISTS productos ("
        "  id INTEGER PRIMARY KEY AUTOINCREMENT,"
        "  nombre TEXT NOT NULL,"
        "  id_categoria INTEGER,"
        "  precio REAL NOT NULL,"
        "  stock INTEGER NOT NULL DEFAULT 0,"
        "  FOREIGN KEY (id_categoria) REFERENCES categorias(id)"
        ");"
        
        "CREATE TABLE IF NOT EXISTS ventas ("
        "  id INTEGER PRIMARY KEY AUTOINCREMENT,"
        "  id_producto INTEGER NOT NULL,"
        "  cantidad INTEGER NOT NULL,"
        "  fecha TIMESTAMP DEFAULT CURRENT_TIMESTAMP,"
        "  FOREIGN KEY (id_producto) REFERENCES productos(id)"
        ");";

    char *err_msg = 0;
    rc = sqlite3_exec(db, sql, 0, 0, &err_msg);
    
    if (rc != SQLITE_OK) {
        fprintf(stderr, "Error al crear tabla: %s\n", err_msg);
        sqlite3_free(err_msg);
    }
    
    return rc;
}

int db_add_category(const char* nombre) {
    char sql[256];
    sprintf(sql, "INSERT INTO categorias (nombre) VALUES ('%s');", nombre);
    
    char *err_msg = 0;
    int rc = sqlite3_exec(db, sql, 0, 0, &err_msg);
    
    if (rc != SQLITE_OK) {
        // Ignoramos el error si ya existe por el UNIQUE, pero liberamos el string
        sqlite3_free(err_msg);
    }
    
    return rc;
}

int db_add_product(Producto p) {
    char sql[256];
    sprintf(sql, "INSERT INTO productos (nombre, id_categoria, precio, stock) VALUES ('%s', %d, %.2f, %d);",
            p.nombre, p.id_categoria, p.precio, p.stock);

    char *err_msg = 0;
    int rc = sqlite3_exec(db, sql, 0, 0, &err_msg);
    
    if (rc != SQLITE_OK) {
        fprintf(stderr, "Error al insertar: %s\n", err_msg);
        sqlite3_free(err_msg);
    }
    
    return rc;
}

int db_list_products() {
    /*
     * IMPLEMENTACIÓN: Obtiene y muestra todos los productos del inventario
     * 
     * Usa un callback para procesar cada fila retornada por SELECT
     * El callback se ejecuta una vez por cada producto en la base de datos
     */
    
    /* SQL que obtiene todos los productos y su categoría asociada */
    const char *sql = 
        "SELECT p.id, p.nombre, p.precio, p.stock, c.nombre as categoria "
        "FROM productos p "
        "LEFT JOIN categorias c ON p.id_categoria = c.id;";
    
    char *err_msg = 0;
    printf("\n========== INVENTARIO DE PRODUCTOS ==========\n");
    
    /*
     * sqlite3_exec() ejecuta el SQL y llama al callback por cada fila
     * Parámetros:
     * - db: conexión a la BD
     * - sql: comando SELECT
     * - callback: función que procesa cada fila
     * - NULL: datos adicionales para el callback (no usamos)
     * - &err_msg: donde guardar el mensaje de error
     */
    int rc = sqlite3_exec(db, sql, db_list_callback, NULL, &err_msg);
    
    if (rc != SQLITE_OK) {
        fprintf(stderr, "Error al listar productos: %s\n", err_msg);
        sqlite3_free(err_msg);
    }
    
    printf("===========================================\n\n");
    return rc;
}

/*
 * ========================================================
 * FUNCIÓN CALLBACK: db_list_callback
 * 
 * ¿Quién la llama?
 * Esta función la llama sqlite3_exec() automáticamente.
 * Una vez por CADA FILA de resultado del SELECT.
 * 
 * ¿Qué hace?
 * Imprime una fila de la tabla de productos en formato bonito
 * (como una tabla en la pantalla).
 * 
 * Parámetros que recibe:
 * - void* data: información adicional (NULL en nuestro caso)
 * - int argc: cuántos campos tiene esta fila (5 en nuestro SELECT)
 * - char** argv: array con los VALORES de esta fila
 *               argv[0] = ID del producto (string)
 *               argv[1] = Nombre del producto (string)
 *               argv[2] = Precio (string)
 *               argv[3] = Stock (string)
 *               argv[4] = Categoría (string)
 * - char** col_name: nombres de las columnas (no usamos aquí)
 * 
 * Retorno:
 * Retorna 0 para decirle a sqlite3_exec(): "Continúa con la siguiente fila"
 * ========================================================
 */
static int db_list_callback(void* data, int argc, char** argv, char** col_name) {
    /*
     * VARIABLE ESTÁTICA: primera_llamada
     * 
     * "static" significa que solo se inicializa UNA SOLA VEZ,
     * aunque la función se ejecute múltiples veces.
     * 
     * Usamos esto para imprimir los encabezados de la tabla
     * solo la PRIMERA VEZ que el callback se ejecuta.
     * Las siguientes veces, primera_llamada = 0, así que no entra
     * en el if y no repite los encabezados.
     */
    static int primera_llamada = 1;
    if (primera_llamada) {
        /*
         * Imprime encabezados de la tabla
         * %-5s = texto alineado a izquierda en 5 caracteres
         * %-30s = texto alineado a izquierda en 30 caracteres
         * etc.
         * Esto crea una tabla alineada y legible
         */
        printf("%-5s %-30s %-10s %-8s %-15s\n", "ID", "Nombre", "Precio", "Stock", "Categoría");
        printf("%-5s %-30s %-10s %-8s %-15s\n", "--", "------", "------", "-----", "---------");
        primera_llamada = 0;  /* La próxima vez, no imprime encabezados */
    }
    
    /*
     * Imprime los valores de esta fila
     * 
     * (argv[0] ? argv[0] : "NULL")
     * significa: "Si argv[0] existe, úsalo. Si es NULL, imprime 'NULL'"
     * Esto evita imprimir basura si un campo está vacío.
     */
    printf("%-5s %-30s %-10s %-8s %-15s\n", 
           (argv[0] ? argv[0] : "NULL"),      /* ID */
           (argv[1] ? argv[1] : "NULL"),      /* Nombre */
           (argv[2] ? argv[2] : "NULL"),      /* Precio */
           (argv[3] ? argv[3] : "NULL"),      /* Stock */
           (argv[4] ? argv[4] : "SIN CAT")    /* Categoría */
    );
    
    return 0;  /* Le dice a sqlite3_exec(): "continúa con la siguiente fila" */
}

int db_search_product(const char* keyword) {
    /*
     * IMPLEMENTACIÓN: db_search_product
     * 
     * Propósito:
     * Busca productos cuyo nombre O categoría contenga la palabra clave.
     * 
     * ¿Cómo funciona la búsqueda?
     * Usa SQL LIKE con % (porcentaje) como comodín.
     * 
     * LIKE '%%%s%%' significa:
     * - % = "cualquier caracteres antes"
     * - %s = la palabra clave que buscas
     * - % = "cualquier caracteres después"
     * 
     * Ejemplo:
     * - Buscas "laptop"
     * - Encuentra "Laptop Gaming", "Gaming Laptop", "LAPTOP", "laptop pro", etc
     * 
     * Se busca en DOS campos:
     * - p.nombre: el nombre del producto
     * - c.nombre: el nombre de la categoría
     * 
     * Usa OR, así si coincide en cualquiera de los dos, lo encuentra.
     */
    
    char sql[512];
    sprintf(sql, 
        "SELECT p.id, p.nombre, p.precio, p.stock, c.nombre as categoria "
        "FROM productos p "
        "LEFT JOIN categorias c ON p.id_categoria = c.id "
        "WHERE p.nombre LIKE '%%%s%%' OR c.nombre LIKE '%%%s%%';",
        keyword, keyword);
    
    char *err_msg = 0;
    printf("\n===== RESULTADOS DE BÚSQUEDA ('%s') =====\n", keyword);
    
    /*
     * Ejecuta el SELECT con el callback db_search_callback
     * El callback se ejecuta por cada producto que coincida con la búsqueda.
     */
    int rc = sqlite3_exec(db, sql, db_search_callback, NULL, &err_msg);
    
    if (rc != SQLITE_OK) {
        fprintf(stderr, "Error en la búsqueda: %s\n", err_msg);
        sqlite3_free(err_msg);
    }
    
    printf("==========================================\n\n");
    return rc;
}

/*
 * ========================================================
 * FUNCIÓN CALLBACK: db_search_callback
 * 
 * ¿Quién la llama?
 * Esta función la llama sqlite3_exec() una vez por CADA PRODUCTO
 * que coincida con la búsqueda en db_search_product().
 * 
 * ¿Qué diferencia tiene de db_list_callback?
 * Básicamente es la MISMA COSA. Ambas imprimen filas en formato tabla.
 * La única diferencia es que esta se usa para búsquedas,
 * mientras que db_list_callback se usa para listar TODO.
 * 
 * Por eso casi el código es idéntico.
 * 
 * Parámetros:
 * - void* data: información adicional (NULL)
 * - int argc: número de columnas (5)
 * - char** argv: array con los valores de esta fila
 * - char** col_name: nombres de columnas (no usamos)
 * 
 * ========================================================
 */
static int db_search_callback(void* data, int argc, char** argv, char** col_name) {
    static int primera_llamada = 1;
    if (primera_llamada) {
        printf("%-5s %-30s %-10s %-8s %-15s\n", "ID", "Nombre", "Precio", "Stock", "Categoría");
        printf("%-5s %-30s %-10s %-8s %-15s\n", "--", "------", "------", "-----", "---------");
        primera_llamada = 0;
    }
    
    printf("%-5s %-30s %-10s %-8s %-15s\n", 
            (argv[0] ? argv[0] : "NULL"),
            (argv[1] ? argv[1] : "NULL"),
            (argv[2] ? argv[2] : "NULL"),
            (argv[3] ? argv[3] : "NULL"),
            (argv[4] ? argv[4] : "SIN CAT")
    );
    
    return 0;
}
