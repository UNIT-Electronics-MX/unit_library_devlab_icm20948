# Configuración de Doxygen

Este directorio contiene toda la configuración de Doxygen para generar la documentación del proyecto DevLab_ICM20948.

## Estructura

```
.doxygen/
├── Doxyfile                  # Archivo de configuración principal de Doxygen
├── README.md                 # Esta documentación
└── custom/
    ├── custom.css            # Estilos personalizados (tema Texas Instruments)
    └── README.md             # Guía de personalización de estilos
```

## Uso

### Generar documentación localmente

Desde la raíz del proyecto:

```bash
cd .doxygen
doxygen Doxyfile
```

La documentación se generará en `docs/html/` y `docs/latex/`.

### Ver la documentación

Abre `docs/html/index.html` en tu navegador.

## Configuración actual

### Directorios de entrada
- `../src/` - Código fuente del driver
- `../examples/` - Ejemplos de uso

### Directorios de salida
- `../docs/html/` - Documentación HTML
- `../docs/latex/` - Documentación LaTeX (solo en tags)

### Estilo
- **Tema personalizado**: DevLab (azul claro, negro, blanco)
- **CSS**: `custom/custom.css`
- **Modo**: Modo claro (LIGHT)
- **Tree view**: Activado para navegación mejorada

## Personalización

Para personalizar los estilos, edita `custom/custom.css`. 

Ver la [guía de personalización](custom/README.md) para más detalles.

## GitHub Actions

El workflow `.github/workflows/doxygen.yml` genera automáticamente la documentación cuando:
- Se hace push a `main`
- Se crea un tag `v*`
- Se ejecuta manualmente (workflow_dispatch)

Los cambios se publican automáticamente en GitHub Pages y se hace commit de los archivos generados.

## Comandos útiles

### Generar archivos base para personalización

```bash
cd .doxygen
doxygen -w html custom/header.html custom/footer.html custom/stylesheet.css
```

### Verificar la configuración

```bash
cd .doxygen
doxygen -g - > /dev/null  # Muestra solo warnings
```

### Actualizar Doxyfile a nueva versión

```bash
cd .doxygen
doxygen -u Doxyfile  # Actualiza el archivo con nuevas opciones
```

## Solución de problemas

### Los estilos no se aplican

1. Verifica que `custom/custom.css` existe
2. Verifica la ruta en Doxyfile: `HTML_EXTRA_STYLESHEET = custom/custom.css`
3. Regenera la documentación limpiando primero: `rm -rf ../docs/html ../docs/latex`

### Archivos no encontrados

Todas las rutas en Doxyfile son relativas a `.doxygen/`:
- `INPUT = ../src ../examples`
- `OUTPUT_DIRECTORY = ../docs`
- `HTML_EXTRA_STYLESHEET = custom/custom.css`

### El warning log no aparece

El archivo se genera en `.doxygen/doxygen_warnings.log` (relativo al directorio donde se ejecuta doxygen).

## Referencias

- [Doxygen Manual](https://www.doxygen.nl/manual/)
- [Doxygen Configuration](https://www.doxygen.nl/manual/config.html)
- [CSS Customization](https://www.doxygen.nl/manual/customize.html)
