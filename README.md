# Sistema de Gestión de Inventario con SQLite3 en C (P05)

Aplicación de consola para gestión de inventario de una tienda, utilizando la API C de SQLite3 como motor de persistencia, con capacidades de reporte, búsqueda y exportación de datos.

## Integrantes

| Código | Apellidos y Nombres | Rol en el Proyecto |
|--------|---------------------|--------------------|
| 20250156B | Beltrán Mendoza, Jean Paul Tomás | Desarrollador Backend / Lógica de negocio |
| 20231122I | Coronel Tello, Álvaro Jesús | Desarrollador de Base de Datos / Capa DAO |
| 20250088G | Mendoza Oscanoa, Percy Saúl | Documentación / Pruebas y QA |
| 20244565A | Yataco Huasasquiche, Jeferson Snyder | Líder de Proyecto / Integración y Repositorio |

## Descripción

Sistema de inventario de consola desarrollado en C que permite a una tienda gestionar sus productos, categorías y ventas de forma eficiente. Utiliza SQLite3 como base de datos embebida, lo que elimina la necesidad de instalar un servidor de base de datos externo.

## Beneficios de la Aplicación

- **Portabilidad:** Al usar SQLite3 embebido, el sistema funciona en cualquier computadora sin necesidad de instalar software adicional de base de datos.
- **Persistencia de datos:** Toda la información se almacena de forma permanente en un archivo `.sqlite`, evitando pérdida de datos al cerrar la aplicación.
- **Control de inventario en tiempo real:** Permite monitorear el stock de productos y recibir alertas configurables cuando el stock está bajo.
- **Exportación de reportes:** Genera archivos CSV con el historial de ventas, facilitando el análisis de datos en herramientas como Excel.
- **Integridad de datos:** Usa transacciones SQLite (BEGIN/COMMIT) para garantizar que operaciones críticas como ventas se ejecuten de forma atómica.
- **Búsqueda flexible:** Permite buscar productos por nombre o categoría usando consultas LIKE en SQL.

## Funcionalidades

- CRUD completo de productos (ID, nombre, categoría, precio, stock)
- CRUD de categorías
- Registro de ventas con descuento automático de stock
- Búsqueda por nombre o categoría con LIKE en SQL
- Alertas de stock bajo (configurables por umbral)
- Reporte de ventas exportado a CSV con `fprintf()`
- Transacciones SQLite para actualizaciones atómicas

## Tecnologías

- **Lenguaje:** C
- **Base de Datos:** SQLite3 (API C embebida)
- **Compilador:** GCC (MinGW-w64)

## Instalación y Ejecución

### 1. Compilar

```powershell
.\compile.bat
```

Genera el ejecutable en `build/inventario.exe`.

### 2. Ejecutar

```powershell
.\build\inventario.exe
```

Crea/actualiza la base de datos en `data/inventario.sqlite`.

### 3. Resetear la BD (si es necesario)

```powershell
Remove-Item data/inventario.sqlite
.\compile.bat
.\build\inventario.exe
```

Borra la BD anterior y crea una nueva con la estructura actualizada.

## Estructura del Proyecto

```
/projectlp1-inventory
  ├── compile.bat          # Script de compilación
  ├── README.md
  ├── include/
  │   ├── models.h         # Estructuras de datos (Producto)
  │   ├── database.h       # Interfaz de acceso a datos (capa DAO)
  │   ├── common.h         # Definiciones comunes
  │   └── sqlite3.h        # Header de SQLite3
  ├── src/
  │   ├── main.c           # Programa principal y menús
  │   ├── database.c       # Implementación de la capa DAO
  │   └── sqlite3.c        # Motor SQLite3 embebido
  ├── build/
  │   └── inventario.exe   (generado)
  └── data/
      ├── inventario.sqlite (generado)
      └── reporte_ventas.csv (generado al exportar)
```

## Arquitectura - Patrón DAO

El proyecto implementa el patrón **Data Access Object (DAO)** en C mediante funciones wrapper que encapsulan las llamadas a la API de SQLite3:

- **`database.h`** define la interfaz pública (declaraciones de funciones).
- **`database.c`** contiene la implementación, usando `sqlite3_exec()` con callbacks para consultas y `sqlite3_prepare_v2()` con statements preparados para operaciones parametrizadas.
- **`main.c`** solo interactúa con las funciones de `database.h`, sin conocer los detalles de SQLite3.

Esto separa la lógica de presentación (menús) de la lógica de acceso a datos, facilitando el mantenimiento y las pruebas.

## Documentación

Cada archivo de código fuente contiene documentación detallada en forma de comentarios. Ver:
- `compile.bat` - Script de compilación documentado paso a paso
- `include/models.h` - Estructuras de datos
- `include/database.h` - Interfaz de acceso a datos
- `src/database.c` - Implementación con callbacks de SQLite3
- `src/main.c` - Programa principal con menús interactivos
