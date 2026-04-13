# Sistema de Gestión de Inventario (P05) - ProyectoLP

Aplicación de consola para gestión de inventario de una tienda, utilizando la API C de SQLite3 como motor de persistencia, con capacidades de reporte y búsqueda.

## Tecnologías

- **Lenguaje:** C
- **Base de Datos:** SQLite3
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
  ├── compile.bat
  ├── README.md
  ├── include/
  │   ├── models.h
  │   ├── database.h
  │   ├── common.h
  │   └── sqlite3.h
  ├── src/
  │   ├── main.c
  │   ├── database.c
  │   └── sqlite3.c
  ├── build/
  │   └── inventario.exe (generado)
  └── data/
      └── inventario.sqlite (generado)
```

## Documentación

Cada archivo de código fuente contiene documentación detallada. Ver comentarios en:
- `compile.bat` - Script de compilación
- `include/models.h` - Estructuras de datos
- `include/database.h` - Interfaz de acceso a datos
- `src/database.c` - Implementación con callbacks de SQLite3
- `src/main.c` - Programa principal
