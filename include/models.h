#ifndef MODELS_H
#define MODELS_H

// Categoria de productos.
typedef struct {
    int id;
    char nombre[50];
} Categoria;

// Producto del inventario.
typedef struct {
    int id;
    char nombre[100];
    int id_categoria;
    float precio;
    int stock;
} Producto;

// Venta registrada.
typedef struct {
    int id;
    int id_producto;
    int cantidad;
    char fecha[20];
} Venta;

#endif
