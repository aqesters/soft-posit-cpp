# Julia Set Zoom Comparison

This example demonstrates the precision advantages of posit arithmetic compared to traditional floating-point when visualizing Julia set fractals at high zoom levels.

## Overview

The Julia set is a mathematical fractal that explores complex dynamics. Deep zooming into fractals requires high precision to maintain detail accuracy. This example compares:

1. **Double precision floating-point**: Standard IEEE 754 64-bit floating-point arithmetic
2. **Posit32 arithmetic**: 32-bit posit numbers which offer better precision distribution

At extreme zoom levels (1000x or higher), traditional floating-point numbers begin to lose precision due to their fixed-size exponent and mantissa fields. Posits, however, maintain better precision across a wider dynamic range, resulting in more accurate calculations and visual detail preservation.

## Visual Comparison

This example generates visual comparisons at various zoom levels. The differences become most apparent in the following scenarios:

- At extreme zoom levels (1000x or higher)
- Near the boundaries of the set where precision is critical
- In regions with fine, intricate details

## Sample Results

When running this example, you'll generate PPM image files showing:

- `julia_float_zoom_*.ppm` - Images generated using standard double precision
- `julia_posit_zoom_*.ppm` - Images generated using posit32 arithmetic

The differences will be striking at high zoom factors:

- Float images may show blockiness, artifacts, or loss of detail
- Posit images maintain smooth transitions and preserve fine structures

## Building and Running

### Build Instructions

```bash
mkdir -p build
cd build
cmake ..
make
```

### Running the Example

From the build directory:

```bash
cd julia_set_zoom
./julia_set_comparison
```

This will generate multiple comparison images at different zoom levels and points of interest.

### Manual Exploration

You can also manually explore specific regions:

```bash
./julia_set_posit [zoom] [centerX] [centerY] [cReal] [cImag]
./julia_set_float [zoom] [centerX] [centerY] [cReal] [cImag]
```

Parameters:
- `zoom`: Zoom level (higher values = deeper zoom)
- `centerX`, `centerY`: Coordinates of the zoom center point
- `cReal`, `cImag`: Parameters for the Julia set (c = cReal + cImag*i)

Example:
```bash
./julia_set_posit 1000 -0.753 0.12 -0.8 0.156
./julia_set_float 1000 -0.753 0.12 -0.8 0.156
```

## Viewing the Images

The example produces PPM image files:

1. Many image viewers support PPM format directly
2. Use ImageMagick to convert: `convert image.ppm image.png`
3. Use online PPM viewers

## Why This Matters

This example demonstrates one of the key advantages of posit arithmetic:

1. **Tapered Precision**: Posits allocate precision where it's most needed
2. **Wider Dynamic Range**: Posits can represent a larger range of values
3. **Graceful Degradation**: Posits maintain meaningful precision even at extremes

These properties make posits particularly well-suited for scientific computing, simulations, and visualization where high precision across a wide dynamic range is essential. 