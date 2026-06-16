# Personalización de Doxygen - Estilo DevLab

Este directorio contiene los archivos de personalización para la documentación generada con Doxygen.

## Estructura del proyecto

```
.doxygen/
├── Doxyfile           # Configuración principal de Doxygen
└── custom/
    ├── custom.css     # Estilos personalizados (tema DevLab)
    └── README.md      # Esta documentación
```

## Archivos incluidos

### `custom.css`
Hoja de estilos personalizada con el tema de DevLab:
- **Colores principales**: Azul claro (#4A9EFF), Negro (#1A1A1A), Blanco
- **Diseño profesional** con identidad DevLab
- **Responsive** para dispositivos móviles
- **Tablas mejoradas** con encabezados en azul
- **Bloques de código** con borde azul distintivo

## Cómo personalizar

### 1. Cambiar colores
Edita las variables CSS en `custom.css`:

```css
:root {
    --devlab-blue: #4A9EFF;          /* Color principal azul */
    --devlab-dark-blue: #2E7FD9;     /* Azul oscuro */
    --devlab-black: #1A1A1A;         /* Negro para navbar */
    --devlab-white: #FFFFFF;          /* Blanco */
}
```

### 2. Agregar tu logo

1. Guarda tu logo en este directorio (ej: `logo.png`)
2. Edita el `Doxyfile` en `.doxygen/`:
   ```
   PROJECT_LOGO = custom/logo.png
   ```
3. Dimensiones recomendadas: **200x55 píxeles** (máximo)

### 3. Personalizar encabezado/pie de página

Desde el directorio `.doxygen/`, genera los archivos base:
```bash
cd .doxygen
doxygen -w html custom/header.html custom/footer.html custom/stylesheet.css
```

Luego edítalos y configura en `Doxyfile`:
```
HTML_HEADER = custom/header.html
HTML_FOOTER = custom/footer.html
```

## Configuración actual en Doxyfile

Las siguientes opciones están configuradas:

```
HTML_EXTRA_STYLESHEET = custom/custom.css
HTML_COLORSTYLE = LIGHT
GENERATE_TREEVIEW = YES
HTML_DYNAMIC_MENUS = YES
PROJECT_NAME = "DevLab_ICM20948"
PROJECT_BRIEF = "Driver para sensor ICM-20948"
OUTPUT_DIRECTORY = ../docs
INPUT = ../src ../examples
```

## Ejemplos de personalización

### Cambiar a azul más oscuro
```css
:root {
    --devlab-blue: #2E7FD9;
    --devlab-dark-blue: #1E5FB9;
    --devlab-light-blue: #4A9EFF;
}
```

### Cambiar a azul cian
```css
:root {
    --devlab-blue: #00BCD4;
    --devlab-dark-blue: #0097A7;
    --devlab-light-blue: #26C6DA;
}
```

## Referencias

- [Doxygen Documentation](https://www.doxygen.nl/manual/)
- [DevLab Website](https://uelectronics.com)
- [CSS Variables](https://developer.mozilla.org/en-US/docs/Web/CSS/Using_CSS_custom_properties)

## Notas

- Los cambios en CSS se aplicarán automáticamente al regenerar la documentación
- Para probar localmente: `cd .doxygen && doxygen Doxyfile` y abre `docs/html/index.html`
- El workflow de GitHub Actions aplica estos estilos automáticamente
