# Doxygen Customization - DevLab Style

This directory contains customization files for Doxygen-generated documentation.

## Project Structure

```
.doxygen/
├── Doxyfile           # Main Doxygen configuration
└── custom/
    ├── custom.css     # Custom styles (DevLab theme)
    └── README.md      # This documentation
```

## Included Files

### `custom.css`
Custom stylesheet with DevLab theme:
- **Main colors**: Light Blue (#4A9EFF), Black (#1A1A1A), White
- **Professional design** with DevLab identity
- **Responsive** for mobile devices
- **Enhanced tables** with blue headers
- **Code blocks** with distinctive blue border

## How to Customize

### 1. Change Colors
Edit CSS variables in `custom.css`:

```css
:root {
    --devlab-blue: #4A9EFF;          /* Primary blue color */
    --devlab-dark-blue: #2E7FD9;     /* Dark blue */
    --devlab-black: #1A1A1A;         /* Black for navbar */
    --devlab-white: #FFFFFF;          /* White */
}
```

### 2. Add Your Logo

1. Save your logo in this directory (e.g., `logo.png`)
2. Edit `Doxyfile` in `.doxygen/`:
   ```
   PROJECT_LOGO = custom/logo.png
   ```
3. Recommended dimensions: **200x55 pixels** (maximum)

### 3. Customize Header/Footer

From the `.doxygen/` directory, generate base files:
```bash
cd .doxygen
doxygen -w html custom/header.html custom/footer.html custom/stylesheet.css
```

Then edit them and configure in `Doxyfile`:
```
HTML_HEADER = custom/header.html
HTML_FOOTER = custom/footer.html
```

## Current Doxyfile Configuration

The following options are configured:

```
HTML_EXTRA_STYLESHEET = custom/custom.css
HTML_COLORSTYLE = LIGHT
GENERATE_TREEVIEW = YES
HTML_DYNAMIC_MENUS = YES
PROJECT_NAME = "DevLab_ICM20948"
PROJECT_BRIEF = "ICM-20948 Sensor Driver"
OUTPUT_DIRECTORY = ../docs
INPUT = ../src ../examples
```

## Customization Examples

### Change to darker blue
```css
:root {
    --devlab-blue: #2E7FD9;
    --devlab-dark-blue: #1E5FB9;
    --devlab-light-blue: #4A9EFF;
}
```

### Change to cyan blue
```css
:root {
    --devlab-blue: #00BCD4;
    --devlab-dark-blue: #0097A7;
    --devlab-light-blue: #26C6DA;
}
```

## References

- [Doxygen Documentation](https://www.doxygen.nl/manual/)
- [DevLab Website](https://uelectronics.com)
- [CSS Variables](https://developer.mozilla.org/en-US/docs/Web/CSS/Using_CSS_custom_properties)

## Notes

- CSS changes will be applied automatically when regenerating documentation
- To test locally: `cd .doxygen && doxygen Doxyfile` and open `docs/html/index.html`
- GitHub Actions workflow applies these styles automatically
