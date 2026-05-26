@echo off
REM ========================================================
REM SCRIPT DE LOTES (BATCH) - Compilación del Proyecto
REM 
REM Un Script de Lotes es un archivo que AGRUPA varios comandos
REM y los EJECUTA AUTOMATICAMENTE, uno tras otro.
REM
REM Sin este archivo: tendrías que escribir varios comandos manualmente.
REM Con este archivo: haces doble clic y todo se compila automáticamente.
REM ========================================================

REM PASO 1: Deshabilita la visualización de comandos
REM El "@echo off" evita que se muestre cada línea de código mientras se ejecuta.
REM Esto hace que la terminal sea más legible y limpia.


REM PASO 2: Imprime un mensaje inicial en la terminal
echo --- COMPILANDO PROYECTO C ---

REM PASO 3: Verifica si existe la carpeta 'build/'
REM Si no existe, la crea automáticamente.
REM Esta carpeta es donde irá el archivo ejecutable (.exe) generado por el compilador.
if not exist build mkdir build

REM ========================================================
REM PASO 4: COMANDO PRINCIPAL - Invocar al compilador
REM
REM Este comando toma todos los archivos de código fuente (.c),
REM los procesa con el compilador gcc,
REM y genera el archivo ejecutable final (inventario.exe) en la carpeta build/.
REM ========================================================
gcc src/main.c src/database.c src/sqlite3.c -Iinclude -o build/inventario.exe -DWIN32 -D_WIN32_WINNT=0x0600 -lkernel32

REM PASO 5: Evalúa el resultado de la compilación
REM La variable %errorlevel% es igual a 0 si la compilación fue exitosa sin errores.
REM Si es diferente de 0, significa que hubo algún error durante el proceso.
if %errorlevel% equ 0 (
    echo.
    echo [+] Compilacion exitosa. Ejecutable generado en build/inventario.exe
) else (
    echo.
    echo [-] Error en la compilacion.
)

REM PASO 6: Pausa la ejecución del script
REM Esto detiene la ventana de terminal, permitiendo que visualices los mensajes
REM antes de que la ventana se cierre automáticamente.
pause
