#ifndef MODELS_H
#define MODELS_H

/*
 * ========================================================
 * DIRECTIVAS DE PREPROCESADOR (Header Guard / Guardián de Inclusión)
 * 
 * #ifndef MODELS_H
 * Significa: "Si NO está definido MODELS_H, ejecuta lo siguiente".
 * 
 * #define MODELS_H
 * Crea un símbolo llamado MODELS_H en la memoria del compilador.
 * MODELS_H es solo un nombre (por convención, el nombre del archivo en mayúsculas).
 * 
 * #endif (al final)
 * Termina el bloque que comenzó con #ifndef.
 * 
 * ¿Para qué sirve esto?
 * Es un mecanismo de protección llamado "include guard". Evita errores cuando
 * este archivo se incluye múltiples veces en la cadena de inclusiones.
 * 
 * ¿CUÁNDO SUCEDE ESTO?
 * Ejemplo práctico:
 * - database.h incluye models.h (porque necesita usar la estructura Producto)
 * - main.c incluye database.h
 * - main.c TAMBIÉN intenta incluir models.h directamente por su cuenta
 * 
 * Sin include guard:
 *   main.c incluye database.h
 *     └─ database.h incluye models.h (Producto se define por 1ª vez)
 *   main.c incluye models.h de nuevo
 *     └─ Intenta definir Producto de nuevo (2ª vez) → ERROR: "Redefinición de tipo"
 * 
 * Con include guard:
 *   main.c incluye database.h
 *     └─ database.h incluye models.h
 *        └─ #ifndef MODELS_H (no existe) → define MODELS_H
 *        └─ Define Producto ✓
 *   main.c incluye models.h de nuevo
 *     └─ #ifndef MODELS_H (ya existe) → salta todo hasta #endif ✓
 * 
 * ========================================================
 */

/*
 * ========================================================
 * ARCHIVO: models.h (ARCHIVO HEADER / ENCABEZADO)
 * Un archivo header (.h) es un archivo que define la "interfaz" o "contrato"
 * de cómo se usan las cosas en el proyecto. No contiene implementación,
 * solo declaraciones (tipos de datos, funciones disponibles, etc).
 * 
 * Los archivos .c (implementación) incluyen estos .h mediante #include
 * para saber qué estructuras y funciones existen y cómo usarlas.
 * Esto se adjunta a los archivos .c para decirles
 * "estas son las estructuras que puedes usar".
 * 
 * En este archivo específico:
 * Definimos las estructuras de datos (tipos personalizados) que representan
 * las entidades principales del sistema de inventario.
 * 
 * Estas estructuras se corresponden directamente con las tablas
 * de la base de datos SQLite, permitiendo que el código C trabaje
 * con datos estructurados en lugar de valores sueltos.
 * ========================================================
 */

/*
 * ESTRUCTURA: Categoria
 * 
 * Representa una categoría de productos en el inventario.
 * Esta estructura se mapea a la tabla 'categorias' de la base de datos.
 * 
 * Campos:
 * - id: Identificador único de la categoría (clave primaria en BD).
 * - nombre: Texto que describe la categoría (máximo 50 caracteres).
 */
typedef struct {
    int id;
    char nombre[50];
} Categoria;

/*
 * ESTRUCTURA: Producto
 * 
 * Representa un producto dentro del inventario de la tienda.
 * Contiene toda la información relevante de un artículo vendible.
 * Esta estructura se mapea a la tabla 'productos' de la base de datos.
 * 
 * Campos:
 * - id: Identificador único del producto (clave primaria en BD).
 * - nombre: Descripción del producto (máximo 100 caracteres).
 * - id_categoria: Referencia a la categoría a la que pertenece este producto
 *                 (clave foránea que enlaza con la tabla categorias).
 * - precio: Costo unitario del producto en formato decimal.
 * - stock: Cantidad disponible del producto en el almacén.
 */
typedef struct {
    int id;
    char nombre[100];
    int id_categoria;
    float precio;
    int stock;
} Producto;

/*
 * ESTRUCTURA: Venta
 * 
 * Registra una transacción de venta en el sistema.
 * Se utiliza para mantener un historial de las operaciones realizadas
 * y facilitar la generación de reportes.
 * Esta estructura se mapea a la tabla 'ventas' de la base de datos.
 * 
 * Campos:
 * - id: Identificador único de la transacción (clave primaria en BD).
 * - id_producto: Referencia al producto que fue vendido
 *                (clave foránea que enlaza con la tabla productos).
 * - cantidad: Número de unidades que se vendieron.
 * - fecha: Marca de tiempo de la venta (texto en formato YYYY-MM-DD HH:MM).
 */
typedef struct {
    int id;
    int id_producto;
    int cantidad;
    char fecha[20];
} Venta;

#endif
