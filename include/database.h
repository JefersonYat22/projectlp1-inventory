#ifndef DATABASE_H
#define DATABASE_H

/*
 * ========================================================
 * ARCHIVO: database.h (HEADER - INTERFAZ DE ACCESO A DATOS)
 * 
 * Este archivo define las funciones wrapper (envoltorios) que encapsulan
 * la complejidad de SQLite3. Permite que otros archivos (.c) interactúen
 * con la base de datos sin escribir SQL directamente.
 * 
 * IMPORTANTE: DECLARACIÓN vs IMPLEMENTACIÓN
 * 
 * Este archivo (.h) contiene solo DECLARACIONES (prototipos).
 * Una declaración es como un "anuncio" que dice:
 * "Existe una función llamada db_add_product que hace tal cosa".
 * 
 * PERO no contiene la lógica real (el código que ejecuta la función).
 * 
 * La IMPLEMENTACIÓN REAL (el código con la lógica, SQL, etc) está en
 * el archivo database.c (el que acompaña a este .h).
 * 
 * Analogía:
 * - database.h = Menú de un restaurante
 *   (dice qué platos existen: "tenemos Pasta Carbonara")
 * - database.c = Cocina del restaurante
 *   (prepara realmente el plato: toma pasta, quesos, huevos, calienta, etc)
 * 
 * Cuando main.c o cualquier otro archivo necesita usar db_add_product(),
 * primero mira este .h para saber que existe y cómo usarla,
 * y luego el compilador busca la implementación real en database.c.
 * ========================================================
 */

/*
 * DIRECTIVAS #include (INCLUSIONES)
 * Incluye el contenido de otro archivo header en este archivo.
 * Permite reutilizar estructuras, funciones y definiciones de otros .h
 * 
 * - #include "archivo.h"   → Busca el archivo en la carpeta actual
 *                             o en rutas RELATIVAS.
 * - #include <archivo.h>   → Busca el archivo en las librerías
 *                             del SISTEMA (no en el proyecto).
 */

#include "models.h"
#include "sqlite3.h"

/*
 * ========================================================
 * DECLARACIONES DE FUNCIONES WRAPPER (ENVOLTORIOS DE BASE DE DATOS)
 * 
 * Los wrappers son funciones propias que encapsulan las operaciones
 * complejas de SQLite3, permitiendo una interfaz más simple y específica
 * para el problema del inventario.
 * 
 * En lugar de escribir SQL directo en main.c, llamamos a estas funciones
 * que internamente generan y ejecutan el SQL necesario.
 * ========================================================
 */

/*
 * FUNCIÓN: db_init
 * Propósito: Inicializar la base de datos y crear las tablas necesarias.
 * Parámetro: nombre del archivo .sqlite a crear/abrir.
 * Retorno: código de error (0 = éxito).
 */
int db_init(const char* db_name);

/*
 * FUNCIÓN: db_add_category
 * Propósito: Agregar una nueva categoría de productos a la base de datos.
 * Parámetro: nombre de la categoría.
 * Retorno: código de error (0 = éxito).
 */
int db_add_category(const char* nombre);

/*
 * FUNCIÓN: db_add_product
 * Propósito: Agregar un nuevo producto a la tabla de productos.
 * Parámetro: estructura Producto rellena con los datos del producto.
 * Retorno: código de error (0 = éxito).
 */
int db_add_product(Producto p);

/*
 * FUNCIÓN: db_list_products
 * Propósito: Obtener y mostrar todos los productos del inventario.
 * Utilizado para generar reportes.
 * Parámetro: ninguno.
 * Retorno: código de error (0 = éxito).
 */
int db_list_products();

/*
 * FUNCIÓN: db_search_product
 * Propósito: Buscar productos por nombre o categoría.
 * Parámetro: palabra clave para la búsqueda (nombre o categoría).
 * Retorno: código de error (0 = éxito).
 */
int db_search_product(const char* keyword);

#endif
