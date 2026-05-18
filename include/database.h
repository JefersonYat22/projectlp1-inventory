#ifndef DATABASE_H
#define DATABASE_H

#include "models.h"
#include "sqlite3.h"

// Inicializa la base de datos y crea tablas.
int db_init(const char* db_name);

// Inserta una categoria.
int db_add_category(const char* nombre);

// Lista categorias ordenadas alfabeticamente.
int db_list_categories();

// Actualiza el nombre de una categoria por ID.
int db_update_category(int id, const char* nombre);

// Elimina una categoria y opcionalmente sus productos.
int db_delete_category(int id, int delete_products);

// Inserta un producto.
int db_add_product(Producto p);

// Actualiza un producto por ID.
int db_update_product(Producto p);

// Elimina un producto por ID.
int db_delete_product(int id);

// Obtiene un producto por ID.
int db_get_product_by_id(int id, Producto* out_product);

// Registra una venta y descuenta stock.
int db_register_sale(int id_producto, int cantidad, float* total);

// Lista productos ordenados alfabeticamente.
int db_list_products();

// Busca productos por nombre o categoria.
int db_search_product(const char* keyword);

#endif
