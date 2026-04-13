#ifndef COMMON_H
#define COMMON_H

/*
 * ========================================================
 * ARCHIVO: common.h (DEFINICIONES GLOBALES DEL PROYECTO)
 * 
 * Propósito:
 * Este archivo está reservado para almacenar elementos comunes
 * que se reutilizan en MÚLTIPLES archivos del proyecto.
 * 
 * ¿Qué puede ir aquí en el futuro?
 * 
 * 1. CONSTANTES GLOBALES
 *    #define MAX_NOMBRE 100
 *    #define MAX_PRODUCTOS 1000
 *    #define RUTA_BD "data/inventario.sqlite"
 * 
 * 2. MACROS ÚTILES (pequeñas funciones reutilizables)
 *    #define MIN(a, b) ((a) < (b) ? (a) : (b))
 *    #define MAX(a, b) ((a) > (b) ? (a) : (b))
 * 
 * 3. TIPOS ENUMERADOS COMPARTIDOS
 *    typedef enum {
 *        OPERACION_EXITOSA,
 *        ERROR_BD,
 *        PRODUCTO_NO_ENCONTRADO
 *    } CodigoError;
 * 
 * 4. CONFIGURACIONES GLOBALES DEL SISTEMA
 *    #define VERSION_PROGRAMA "1.0"
 *    #define MODO_DEBUG 0
 * 
 * ¿Por qué separar esto en un archivo aparte?
 * Para evitar repetir las mismas definiciones en varios archivos.
 * Si varios .c necesitan la constante MAX_NOMBRE,
 * la escribimos UNA sola vez aquí y todos la incluyen.
 * 
 * ========================================================
 */

#endif
