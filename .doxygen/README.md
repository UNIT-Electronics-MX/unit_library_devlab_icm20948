# Doxygen Configuration

This directory contains all Doxygen configuration files for generating the DevLab_ICM20948 project documentation.

## Structure

```
.doxygen/
├── Doxyfile                  # Main Doxygen configuration file
├── README.md                 # This documentation
└── custom/
    ├── custom.css            # Custom styles (DevLab theme)
    └── README.md             # Style customization guide
```

## Usage

### Generate documentation locally

From the project root:

```bash
cd .doxygen
doxygen Doxyfile
```

Documentation will be generated in `docs/html/` and `docs/latex/`.

### View documentation

Open `docs/html/index.html` in your browser.

## Current Configuration

### Input Directories
- `../src/` - Driver source code
- `../examples/` - Usage examples

### Output Directories
- `../docs/html/` - HTML documentation
- `../docs/latex/` - LaTeX documentation (tags only)

### Style
- **Custom theme**: DevLab (light blue, black, white)
- **CSS**: `custom/custom.css`
- **Mode**: Light mode
- **Tree view**: Enabled for improved navigation

## Customization

To customize styles, edit `custom/custom.css`. 

See the [customization guide](custom/README.md) for more details.

## GitHub Actions

The workflow `.github/workflows/doxygen.yml` automatically generates documentation when:
- Push to `main`
- Tag created with `v*` pattern
- Manual execution (workflow_dispatch)

Changes are automatically published to GitHub Pages and generated files are committed.

## Useful Commands

### Generate base files for customization

```bash
cd .doxygen
doxygen -w html custom/header.html custom/footer.html custom/stylesheet.css
```

### Verify configuration

```bash
cd .doxygen
doxygen -g - > /dev/null  # Shows warnings only
```

### Update Doxyfile to new version

```bash
cd .doxygen
doxygen -u Doxyfile  # Updates file with new options
```

## Troubleshooting

### Styles not applying

1. Verify that `custom/custom.css` exists
2. Check path in Doxyfile: `HTML_EXTRA_STYLESHEET = custom/custom.css`
3. Regenerate documentation after cleaning: `rm -rf ../docs/html ../docs/latex`

### Files not found

All paths in Doxyfile are relative to `.doxygen/`:
- `INPUT = ../src ../examples`
- `OUTPUT_DIRECTORY = ../docs`
- `HTML_EXTRA_STYLESHEET = custom/custom.css`

### Warning log not appearing

The file is generated at `.doxygen/doxygen_warnings.log` (relative to the directory where doxygen runs).

## References

- [Doxygen Manual](https://www.doxygen.nl/manual/)
- [Doxygen Configuration](https://www.doxygen.nl/manual/config.html)
- [CSS Customization](https://www.doxygen.nl/manual/customize.html)
