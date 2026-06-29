#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "sqlite3.h"
#include "database.h"

static int db_list_callback(void* data, int argc, char** argv, char** col_name);
static int db_search_callback(void* data, int argc, char** argv, char** col_name);
static int db_category_list_callback(void* data, int argc, char** argv, char** col_name);
static int db_sales_list_callback(void* data, int argc, char** argv, char** col_name);
// una funcion static solo es visible dentro del archivo donde se define, no puede ser llamada desde otros archivos, es como una funcion privada de ese archivo.

static sqlite3 *db = NULL; // puntero db para que sqlite lo rellene con el manejador de la base de datos abierta.
// sqlite3 es un tipo de dato opaco, lo que significa que su estructura interna no es visible para el programador, solo se puede manipular a través de funciones proporcionadas 
// por la biblioteca sqlite3. Esto ayuda a mantener la integridad de los datos y evita que el programador acceda directamente a la memoria interna de la base de datos, lo que podría causar errores o corrupción de datos.

int db_init(const char* db_name) {  //sqlite3_open abre la base de datos(.sqlite)
    int rc = sqlite3_open(db_name, &db); // return code, se usa para saber si la operacion fue exitosa o hubo error
    if (rc != SQLITE_OK) { //constante de sqlite3.h que indica que la operacion fue exitosa
        fprintf(stderr, "No se pudo abrir la base de datos: %s\n", sqlite3_errmsg(db));
        // fprintf es como printf, pero permite elegir a qué flujo enviar el texto, en este caso impprime en flujo de errores, y sqlite3_errmsg(db) devuelve un mensaje de error legible para humanos basado en el código de error almacenado en el objeto db.
        return rc;
    }

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
        "  FOREIGN KEY (id_categoria) REFERENCES categorias(id)" // una categoría tiene muchos productos
        ");"
        
        "CREATE TABLE IF NOT EXISTS ventas ("
        "  id INTEGER PRIMARY KEY AUTOINCREMENT,"
        "  id_producto INTEGER NOT NULL,"
        "  cantidad INTEGER NOT NULL,"
        "  fecha TIMESTAMP DEFAULT CURRENT_TIMESTAMP,"
        "  FOREIGN KEY (id_producto) REFERENCES productos(id)"  // se pueden hacer muchas ventas de un producto
        ");";

    char *err_msg = 0;
    rc = sqlite3_exec(db, sql, 0, 0, &err_msg); // ejecuta una operación, le mandamos el manejador de la base de datos, el sql a ejecutar, un callback (en este caso no lo usamos, por eso es 0), un puntero para datos del callback (tampoco lo usamos), y un puntero para almacenar mensajes de error si ocurren.
    
    if (rc != SQLITE_OK) {
        fprintf(stderr, "Error al crear tabla: %s\n", err_msg);
        sqlite3_free(err_msg); // libera memoria cuando ya no lo usa
    }
    
    return rc;
}

int db_add_category(const char* nombre) {
    char sql[256];
    sprintf(sql, "INSERT INTO categorias (nombre) VALUES ('%s');", nombre); // escribe texto en un string (a sql)
    // solo llenamos la columna nombre porque id es autoincremental, se genera solo y no puede ser nulo, y no hay otras columnas en la tabla categorias.
    char *err_msg = 0;
    int rc = sqlite3_exec(db, sql, 0, 0, &err_msg);
    
    if (rc != SQLITE_OK) {
        fprintf(stderr, "Error al insertar categoria: %s\n", err_msg);
        sqlite3_free(err_msg);
    }
    
    return rc;
}

int db_list_categories() {
    const char *sql =
        "SELECT id, nombre FROM categorias ORDER BY nombre ASC;";  // trae las columnas id y nombre de la tabla

    char *err_msg = 0;
    printf("\n========== CATEGORIAS =========="
        "\n");
    int rc = sqlite3_exec(db, sql, db_category_list_callback, NULL, &err_msg);
    // en este caso le mandamos un callback al exec, ya que necesitamos imprimir los resultados de la consulta.
    // cosa que no es necesaria para casos como insertar, actualizar o eliminar, donde solo nos interesa saber si la operación fue exitosa o no, y no necesitamos procesar resultados de una consulta.
    // por eso en esas operaciones el callback es 0.
    // exec recibe un puntero a función como argumento.

    if (rc != SQLITE_OK) {
        fprintf(stderr, "Error al listar categorias: %s\n", err_msg);
        sqlite3_free(err_msg);
    }

    printf("===============================\n\n");
    return rc;
}

int db_update_category(int id, const char* nombre) {
    char sql[256];
    sprintf(sql, "UPDATE categorias SET nombre='%s' WHERE id=%d;", nombre, id);
    // SET especifica qué columnas se van a actualizar y WHERE es para saber qué fila, es la condición para encontrar la fila a actualizar, en este caso el id, que es único para cada categoría, por eso se usa como identificador para actualizar la categoría correcta.


    char *err_msg = 0;
    int rc = sqlite3_exec(db, sql, 0, 0, &err_msg);

    if (rc != SQLITE_OK) {
        fprintf(stderr, "Error al actualizar categoria: %s\n", err_msg);
        sqlite3_free(err_msg);
    }

    return rc;
}

int db_delete_category(int id, int delete_products) {
    char sql[512];

    if (delete_products) {
        sprintf(sql,
                "BEGIN; " //sirven para que operaciones se ejecuten juntas
                "DELETE FROM productos WHERE id_categoria=%d; "
                "DELETE FROM categorias WHERE id=%d; "
                "COMMIT;",
                id, id);
    } else {
        sprintf(sql,
                "BEGIN; "
                "UPDATE productos SET id_categoria=NULL WHERE id_categoria=%d; "
                "DELETE FROM categorias WHERE id=%d; "
                "COMMIT;",
                id, id);
    }
    // Con BEGIN...COMMIT es todo o nada, se ejecutan ambas o ninguna, porque en este caso no nos sirve que solo se ejecute una de las instrucciones
    // en el caso de la constante sql inicial, no usamos BEGIN, porque son independientes, si falla una, las otras ya se ejecutaron, pero no causa inconsistencia.

    char *err_msg = 0;
    int rc = sqlite3_exec(db, sql, 0, 0, &err_msg);

    if (rc != SQLITE_OK) {
        fprintf(stderr, "Error al eliminar categoria: %s\n", err_msg);
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
        fprintf(stderr, "Error al insertar producto: %s\n", err_msg);
        sqlite3_free(err_msg);
    }
    
    return rc;
}

int db_list_products() {
    const char *sql = 
    /*
    El orden de texto SQL es fijo por la sintaxis, normalmente va:
    SELECT ... FROM ... [JOIN ...] [WHERE ...] [GROUP BY ...] [ORDER BY ...];
    pero aunque se escriba así, el motor lo evalúa en otro orden lógico, primero from, luego where...
    */
        "SELECT p.id, p.nombre, p.precio, p.stock, c.nombre as categoria "
        "FROM productos p " // por eso acá recién se define a p como la tabla productos.
        "LEFT JOIN categorias c ON p.id_categoria = c.id "
        "ORDER BY c.nombre ASC, p.nombre ASC;";
    /*  
        Primero lee el FROM, luego el LEFT JOIN, por eso al llegar a SELECT es que ya tiene definidas ambas tablas
        y puede elegir columnas de ambas, ya que sabe que hará un left join, y as categoria sirve para mostrarlo de 
        esa forma en los resultados
    */
    char *err_msg = 0;
    printf("\n========== INVENTARIO DE PRODUCTOS ==========\n");
    int rc = sqlite3_exec(db, sql, db_list_callback, NULL, &err_msg);
    
    if (rc != SQLITE_OK) {
        fprintf(stderr, "Error al listar productos: %s\n", err_msg);
        sqlite3_free(err_msg);
    }
    
    printf("===========================================\n\n");
    return rc;
}

int db_update_product(Producto p) {
    char sql[256];
    sprintf(sql,
            "UPDATE productos SET nombre='%s', id_categoria=%d, precio=%.2f, stock=%d WHERE id=%d;",
            p.nombre, p.id_categoria, p.precio, p.stock, p.id);

    char *err_msg = 0;
    int rc = sqlite3_exec(db, sql, 0, 0, &err_msg);

    if (rc != SQLITE_OK) {
        fprintf(stderr, "Error al actualizar: %s\n", err_msg);
        sqlite3_free(err_msg);
    }

    return rc;
}

int db_delete_product(int id) {
    char sql[128];
    sprintf(sql, "DELETE FROM productos WHERE id=%d;", id);

    char *err_msg = 0;
    int rc = sqlite3_exec(db, sql, 0, 0, &err_msg);

    if (rc != SQLITE_OK) {
        fprintf(stderr, "Error al eliminar: %s\n", err_msg);
        sqlite3_free(err_msg);
    }

    return rc;
}

int db_get_product_by_id(int id, Producto* out_product) {
    sqlite3_stmt *stmt = NULL;  // instruccion SQL ya "compilada", las instrucciones SQL en realidad tienen que ser pasadas a binarios, eso hacía el exec
    const char *sql = "SELECT id, nombre, id_categoria, precio, stock FROM productos WHERE id=?;";
    int rc = sqlite3_prepare_v2(db, sql, -1, &stmt, NULL); // manejador de la base de datos abierta, sql, tamaño maximo en bytes del texto sql (-1 porque no hay limite, que lea hasta \0), stmt puntero a un puntero para llenar el stmt con la instruccion preparada, y un puntero para el resto del sql que no se usó (no lo usamos, por eso es NULL) 

    if (rc != SQLITE_OK) {
        return rc;
    }
    // vincula un número entero en el espacio vacío de la consulta ya compilada
    sqlite3_bind_int(stmt, 1, id); // 1. consulta ya compilada donde está el hueco(?), 2. posición del hueco (como es el primer hueco, acá empieza de 1), 3. valor a poner en el hueco (el id del producto que queremos buscar)
    rc = sqlite3_step(stmt); // ejecuta la instrucción hasta encontrar la primera fila de resultados.

    // row = fila, encontrada

    if (rc == SQLITE_ROW) {
        const unsigned char *nombre = sqlite3_column_text(stmt, 1); // la función no copia el texto, devuelve un puntero que señala a la memoria interna de sqlite donde está guardado el nombre
        // orden de la instrucción, desde cero.
        out_product->id = sqlite3_column_int(stmt, 0);
        out_product->id_categoria = sqlite3_column_int(stmt, 2);
        out_product->precio = (float)sqlite3_column_double(stmt, 3);
        out_product->stock = sqlite3_column_int(stmt, 4);
        if (nombre) { // devolvió nombre válido?
            strncpy(out_product->nombre, (const char*)nombre, sizeof(out_product->nombre));
            out_product->nombre[sizeof(out_product->nombre) - 1] = '\0';
        } else {
            out_product->nombre[0] = '\0';
        }
        sqlite3_finalize(stmt); // destruye en memoria a la declaración preparada sql.
        return SQLITE_OK;
    }

    sqlite3_finalize(stmt);
    return SQLITE_ERROR;
}

int db_register_sale(int id_producto, int cantidad, float* total, const char* fecha) {
    Producto p;
    char sql[512];

    if (cantidad <= 0) {
        return SQLITE_ERROR;
    }

    if (db_get_product_by_id(id_producto, &p) != SQLITE_OK) {
        return SQLITE_ERROR;
    }

    if (p.stock < cantidad) {
        return SQLITE_ERROR;
    }

    sprintf(sql,
            "BEGIN; "
            "UPDATE productos SET stock = stock - %d WHERE id = %d; "
            "INSERT INTO ventas (id_producto, cantidad, fecha) VALUES (%d, %d, '%s'); "
            "COMMIT;",
            cantidad, id_producto, id_producto, cantidad, fecha);

    char *err_msg = 0;
    int rc = sqlite3_exec(db, sql, 0, 0, &err_msg);

    if (rc != SQLITE_OK) {
        fprintf(stderr, "Error al registrar venta: %s\n", err_msg);
        sqlite3_free(err_msg);
        return rc;
    }

    if (total) {
        *total = p.precio * (float)cantidad;
    }

    return rc;
}
static int db_list_callback(void* data, int argc, char** argv, char** col_name) {
    static int primera_llamada = 1;
    if (primera_llamada) {
        printf("%-5s %-30s %-10s %-8s %-15s\n", "ID", "Nombre", "Precio", "Stock", "Categoría");
        printf("%-5s %-30s %-10s %-8s %-15s\n", "--", "------", "------", "-----", "---------");
        primera_llamada = 0;
    } // por cada fila de resultados crea un arreglo argv con los valores de las columnas, y un arreglo col_name con los nombres de las columnas, y argc es la cantidad de columnas, entonces se puede iterar sobre esos arreglos para imprimir los resultados, en este caso se imprimen directamente usando printf, pero también se podrían almacenar en una estructura o procesar de otra forma.
    printf("%-5s %-30s %-10s %-8s %-15s\n", 
        (argv[0] ? argv[0] : "NULL"),
        (argv[1] ? argv[1] : "NULL"),
        (argv[2] ? argv[2] : "NULL"),
        (argv[3] ? argv[3] : "NULL"),
        (argv[4] ? argv[4] : "SIN CAT")
    );
    
    return 0;
}

int db_search_product(const char* keyword) {
    char sql[512];
    sprintf(sql, 
        "SELECT p.id, p.nombre, p.precio, p.stock, c.nombre as categoria "
        "FROM productos p " // tabla izquierda, mostrará todas las filas. 
        "LEFT JOIN categorias c ON p.id_categoria = c.id "
        "WHERE p.nombre LIKE '%%%s%%' OR c.nombre LIKE '%%%s%%';",
        keyword, keyword);
    
    char *err_msg = 0;
    printf("\n===== RESULTADOS DE BÚSQUEDA ('%s') =====\n", keyword);
    int rc = sqlite3_exec(db, sql, db_search_callback, NULL, &err_msg);
    
    if (rc != SQLITE_OK) {
        fprintf(stderr, "Error en la búsqueda: %s\n", err_msg);
        sqlite3_free(err_msg);
    }
    
    printf("==========================================\n\n");
    return rc;
}
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

static int db_category_list_callback(void* data, int argc, char** argv, char** col_name) {
    static int primera_llamada = 1;
    if (primera_llamada) {
        printf("%-5s %-30s\n", "ID", "Nombre");
        printf("%-5s %-30s\n", "--", "------");
        primera_llamada = 0;
    }

    printf("%-5s %-30s\n",
            (argv[0] ? argv[0] : "NULL"),
            (argv[1] ? argv[1] : "NULL"));

    return 0;
}

static int db_sales_list_callback(void* data, int argc, char** argv, char** col_name) {
    static int primera_llamada = 1;
    if (primera_llamada) {
        printf("%-5s %-30s %-10s %-10s %-20s\n", "ID", "Producto", "Cantidad", "Subtotal", "Fecha");
        printf("%-5s %-30s %-10s %-10s %-20s\n", "--", "---------", "--------", "---------", "----");
        primera_llamada = 0;
    }

    float precio = (argv[3] ? (float)atof(argv[3]) : 0.0f);
    int cantidad = (argv[2] ? atoi(argv[2]) : 0);
    float subtotal = precio * cantidad;

    printf("%-5s %-30s %-10s %-10.2f %-20s\n",
            (argv[0] ? argv[0] : "NULL"),
            (argv[1] ? argv[1] : "NULL"),
            (argv[2] ? argv[2] : "NULL"),
            subtotal,
            (argv[4] ? argv[4] : "SIN FECHA"));

    return 0;
}

int db_list_sales() {
    const char *sql =
        "SELECT v.id, p.nombre, v.cantidad, p.precio, v.fecha "
        "FROM ventas v "
        "LEFT JOIN productos p ON v.id_producto = p.id "
        "ORDER BY v.fecha DESC;";

    char *err_msg = 0;
    printf("\n========== REPORTE DE VENTAS ==========\n");
    int rc = sqlite3_exec(db, sql, db_sales_list_callback, NULL, &err_msg);

    if (rc != SQLITE_OK) {
        fprintf(stderr, "Error al listar ventas: %s\n", err_msg);
        sqlite3_free(err_msg);
    }

    printf("=======================================\n\n");
    return rc;
}

int db_list_low_stock(int umbral) {
    sqlite3_stmt *stmt = NULL;
    const char *sql =
        "SELECT p.id, p.nombre, p.precio, p.stock, c.nombre as categoria "
        "FROM productos p "
        "LEFT JOIN categorias c ON p.id_categoria = c.id "
        "WHERE p.stock <= ? "
        "ORDER BY p.stock ASC;";

    int rc = sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);
    if (rc != SQLITE_OK) {
        fprintf(stderr, "Error al preparar consulta de stock bajo: %s\n", sqlite3_errmsg(db));
        return rc;
    }

    sqlite3_bind_int(stmt, 1, umbral);

    printf("\n========== ALERTA: STOCK BAJO (umbral <= %d) ==========\n", umbral);
    printf("%-5s %-30s %-10s %-8s %-15s\n", "ID", "Nombre", "Precio", "Stock", "Categoría");
    printf("%-5s %-30s %-10s %-8s %-15s\n", "--", "------", "------", "-----", "---------");

    int count = 0;
    while ((rc = sqlite3_step(stmt)) == SQLITE_ROW) {
        int id = sqlite3_column_int(stmt, 0);
        const char *nombre = (const char*)sqlite3_column_text(stmt, 1);
        double precio = sqlite3_column_double(stmt, 2);
        int stock = sqlite3_column_int(stmt, 3);
        const char *categoria = (const char*)sqlite3_column_text(stmt, 4);

        printf("%-5d %-30s %-10.2f %-8d %-15s\n",
               id,
               nombre ? nombre : "NULL",
               precio,
               stock,
               categoria ? categoria : "SIN CAT");
        count++;
    }

    if (count == 0) {
        printf("No hay productos con stock bajo.\n");
    } else {
        printf("\nTotal de productos con stock bajo: %d\n", count);
    }

    printf("=====================================================\n\n");
    sqlite3_finalize(stmt);
    return SQLITE_OK;
}

int db_export_sales_csv(const char* filename) {
    sqlite3_stmt *stmt = NULL;
    const char *sql =
        "SELECT v.id, p.nombre, v.cantidad, p.precio, v.fecha "
        "FROM ventas v "
        "LEFT JOIN productos p ON v.id_producto = p.id "
        "ORDER BY v.fecha DESC;";

    int rc = sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);
    if (rc != SQLITE_OK) {
        fprintf(stderr, "Error al preparar consulta de ventas: %s\n", sqlite3_errmsg(db));
        return rc;
    }

    FILE *archivo = fopen(filename, "w");
    if (!archivo) {
        fprintf(stderr, "Error al abrir archivo: %s\n", filename);
        sqlite3_finalize(stmt);
        return SQLITE_ERROR;
    }

    // Encabezado del CSV
    fprintf(archivo, "ID,Producto,Cantidad,Precio Unitario,Subtotal,Fecha\n");

    int count = 0;
    float total_general = 0.0f;

    while ((rc = sqlite3_step(stmt)) == SQLITE_ROW) {
        int id = sqlite3_column_int(stmt, 0);
        const char *nombre = (const char*)sqlite3_column_text(stmt, 1);
        int cantidad = sqlite3_column_int(stmt, 2);
        float precio = (float)sqlite3_column_double(stmt, 3);
        const char *fecha = (const char*)sqlite3_column_text(stmt, 4);
        float subtotal = precio * (float)cantidad;

        fprintf(archivo, "%d,%s,%d,%.2f,%.2f,%s\n",
                id,
                nombre ? nombre : "NULL",
                cantidad,
                precio,
                subtotal,
                fecha ? fecha : "SIN FECHA");

        total_general += subtotal;
        count++;
    }

    // Fila final con el total general
    fprintf(archivo, "\n,,,,%.2f,TOTAL GENERAL\n", total_general);

    fclose(archivo);
    sqlite3_finalize(stmt);

    printf("[+] Reporte exportado a '%s' (%d ventas, total: %.2f)\n", filename, count, total_general);
    return SQLITE_OK;
}
