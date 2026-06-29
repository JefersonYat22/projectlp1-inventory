#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "database.h"
#include "models.h"

static void read_line(char *buffer, size_t size) {
    if (fgets(buffer, (int)size, stdin) == NULL) {
        buffer[0] = '\0';
        return;
    }
    buffer[strcspn(buffer, "\r\n")] = '\0';  //devuelve el indice del primer \r o \n y lo reemplaza por \0
}

static int read_int_prompt(const char *prompt) {
    char input[64];
    printf("%s", prompt);
    read_line(input, sizeof(input));
    return atoi(input);
}

static float read_float_prompt(const char *prompt) {
    char input[64];
    printf("%s", prompt);
    read_line(input, sizeof(input));
    return (float)strtod(input, NULL);
}

static void read_text_prompt(const char *prompt, char *buffer, size_t size) {
    printf("%s", prompt);
    read_line(buffer, size);
}

static void format_current_time(char *buffer, size_t size) {
    time_t now = time(NULL);
    struct tm local_time;
#ifdef _MSC_VER
    if (localtime_s(&local_time, &now) == 0) {
        strftime(buffer, size, "%Y-%m-%d %H:%M:%S", &local_time);
    } else {
        buffer[0] = '\0';
    }
#else
    struct tm *tmp = localtime(&now);
    if (tmp) {
        local_time = *tmp;
        strftime(buffer, size, "%Y-%m-%d %H:%M:%S", &local_time);
    } else {
        buffer[0] = '\0';
    }
#endif
}

static void menu_productos() {
    while (1) {
        char input[64];
        int opcion = -1;

        db_list_products();
        printf("--- ADMINISTRAR PRODUCTOS ---\n");
        printf("1) Agregar producto\n");
        printf("2) Editar producto\n");
        printf("3) Eliminar producto\n");
        printf("4) Buscar producto\n");
        printf("0) Volver\n");
        printf("Seleccione una opcion: ");

        read_line(input, sizeof(input));
        opcion = atoi(input);

        if (opcion == 1) {
            Producto nuevo;
            char nombre[100];

            read_text_prompt("Nombre del producto: ", nombre, sizeof(nombre));
            nuevo.id = 0;
            strncpy(nuevo.nombre, nombre, sizeof(nuevo.nombre));
            nuevo.nombre[sizeof(nuevo.nombre) - 1] = '\0';
            nuevo.id_categoria = read_int_prompt("ID de categoria: ");
            nuevo.precio = read_float_prompt("Precio: ");
            nuevo.stock = read_int_prompt("Stock inicial: ");

            if (db_add_product(nuevo) == SQLITE_OK) {
                printf("[+] Producto agregado.\n");
            }
        } else if (opcion == 2) {
            Producto editado;
            char nombre[100];

            editado.id = read_int_prompt("ID del producto a actualizar: ");
            read_text_prompt("Nuevo nombre: ", nombre, sizeof(nombre));
            strncpy(editado.nombre, nombre, sizeof(editado.nombre));
            editado.nombre[sizeof(editado.nombre) - 1] = '\0';
            editado.id_categoria = read_int_prompt("Nuevo ID de categoria: ");
            editado.precio = read_float_prompt("Nuevo precio: ");
            editado.stock = read_int_prompt("Nuevo stock: ");

            if (db_update_product(editado) == SQLITE_OK) {
                printf("[+] Producto actualizado.\n");
            }
        } else if (opcion == 3) {
            int id = 0;
            char confirm[8];

            id = read_int_prompt("ID del producto a eliminar: ");
            read_text_prompt("Confirmar eliminacion (s/n): ", confirm, sizeof(confirm));
            if (confirm[0] == 's' || confirm[0] == 'S') {
                if (db_delete_product(id) == SQLITE_OK) {
                    printf("[+] Producto eliminado.\n");
                }
            } else {
                printf("Eliminacion cancelada.\n");
            }
        } else if (opcion == 4) {
            char keyword[128];
            read_text_prompt("Ingrese palabra clave: ", keyword, sizeof(keyword));
            if (keyword[0] != '\0') {
                db_search_product(keyword);
            } else {
                printf("Busqueda cancelada.\n");
            }
        } else if (opcion == 0) {
            break;
        } else {
            printf("Opcion invalida.\n");
        }
    }
}

static void menu_categorias() {
    while (1) {
        char input[64];
        int opcion = -1;

        db_list_categories();
        printf("--- ADMINISTRAR CATEGORIAS ---\n");
        printf("1) Agregar categoria\n");
        printf("2) Editar categoria\n");
        printf("3) Eliminar categoria\n");
        printf("0) Volver\n");
        printf("Seleccione una opcion: ");

        read_line(input, sizeof(input));
        opcion = atoi(input);

        if (opcion == 1) {
            char nombre[50];
            read_text_prompt("Nombre de categoria: ", nombre, sizeof(nombre));
            if (nombre[0] != '\0') {
                if (db_add_category(nombre) == SQLITE_OK) {
                    printf("[+] Categoria agregada.\n");
                } else {
                    printf("No se pudo agregar la categoria.\n");
                }
            }
        } else if (opcion == 2) {
            int id = 0;
            char nombre[50];

            id = read_int_prompt("ID de categoria a actualizar: ");
            read_text_prompt("Nuevo nombre: ", nombre, sizeof(nombre));
            if (nombre[0] != '\0') {
                if (db_update_category(id, nombre) == SQLITE_OK) {
                    printf("[+] Categoria actualizada.\n");
                } else {
                    printf("No se pudo actualizar la categoria.\n");
                }
            }
        } else if (opcion == 3) {
            int id = 0;
            int modo = 0;
            char confirm[8];

            id = read_int_prompt("ID de categoria a eliminar: ");
            printf("1) Mantener productos sin categoria\n");
            printf("2) Eliminar productos de la categoria\n");
            modo = read_int_prompt("Elige una opcion: ");
            read_text_prompt("Confirmar eliminacion (s/n): ", confirm, sizeof(confirm));

            if (confirm[0] == 's' || confirm[0] == 'S') {
                int delete_products = (modo == 2);
                if (db_delete_category(id, delete_products) == SQLITE_OK) {
                    printf("[+] Categoria eliminada.\n");
                } else {
                    printf("No se pudo eliminar la categoria.\n");
                }
            } else {
                printf("Eliminacion cancelada.\n");
            }
        } else if (opcion == 0) {
            break;
        } else {
            printf("Opcion invalida.\n");
        }
    }
}

static void menu_reportes() {
    while (1) {
        char input[64];
        int opcion = -1;

        printf("--- REPORTES ---\n");
        printf("1) Reporte de ventas\n");
        printf("2) Stock bajo\n");
        printf("3) Exportar ventas a CSV\n");
        printf("0) Volver\n");
        printf("Seleccione una opcion: ");

        read_line(input, sizeof(input));
        opcion = atoi(input);

        if (opcion == 1) {
            db_list_sales();
        } else if (opcion == 2) {
            int umbral = read_int_prompt("Ingrese umbral de stock bajo: ");
            db_list_low_stock(umbral);
        } else if (opcion == 3) {
            db_export_sales_csv("data/reporte_ventas.csv");
        } else if (opcion == 0) {
            break;
        } else {
            printf("Opcion invalida.\n");
        }
    }
}

int main() {
    printf("--- SISTEMA DE INVENTARIO (SQLite3 + C) ---\n");
    printf("SQLite3 Version: %s\n\n", sqlite3_libversion());

    if (db_init("data/inventario.sqlite") != SQLITE_OK) {
        return 1;
    }
    printf("[+] Base de datos conectada.\n");

    while (1) {
        char input[64];
        int opcion = -1;

        printf("\n--- MENU PRINCIPAL ---\n");
        printf("1) Administrar productos\n");
        printf("2) Administrar categorias\n");
        printf("3) Registrar venta\n");
        printf("4) Reportes\n");
        printf("0) Salir\n");
        printf("Seleccione una opcion: ");

        read_line(input, sizeof(input));
        opcion = atoi(input);

        if (opcion == 1) {
            menu_productos();
        } else if (opcion == 2) {
            menu_categorias();
        } else if (opcion == 3) {
            int id_producto = 0;
            int cantidad = 0;
            float total = 0.0f;
            char fecha[32];

            db_list_products();
            id_producto = read_int_prompt("ID del producto: ");
            cantidad = read_int_prompt("Cantidad: ");
            format_current_time(fecha, sizeof(fecha));

            if (db_register_sale(id_producto, cantidad, &total, fecha) == SQLITE_OK) {
                printf("[+] Venta registrada. Total: %.2f\n", total);
                if (fecha[0] != '\0') {
                    printf("Fecha: %s\n", fecha);
                }
            } else {
                printf("No se pudo registrar la venta.\n");
            }
        } else if (opcion == 4) {
            menu_reportes();
        } else if (opcion == 0) {
            printf("Saliendo...\n");
            break;
        } else {
            printf("Opcion invalida.\n");
        }
    }

    return 0;
}
