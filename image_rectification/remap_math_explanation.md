# Remap Math Explanation (Stereo Rectification)

## Problem statement

For every output pixel in the rectified image, we need to find the matching source location in the original distorted image.

- Output pixel: `(u, v)` in the rectified image
- Source sample location: `(u_src, v_src)` in the original image

The remap tables store this inverse mapping:

- `map1(v, u) = u_src`
- `map2(v, u) = v_src`

This is why the loop runs over destination pixels and computes source coordinates.

## Step-by-step through the code block

### 1) Rectified pixel to normalized rectified ray

```cpp
const float x_rect = (u - cxp) / fxp;
const float y_rect = (v - cyp) / fyp;
```

- `fxp, fyp, cxp, cyp` are rectified intrinsics from `P_rect`.
- This converts pixel units to normalized camera coordinates.
- `(x_rect, y_rect, 1)` is a ray direction in the rectified camera frame.

Effects of changing values:

- Increase `fxp` or `fyp`: normalized offsets get smaller (weaker spread from center).
- Decrease `fxp` or `fyp`: normalized offsets get larger (stronger spread from center).
- Increase `cxp`: all `x_rect` shift left numerically.
- Increase `cyp`: all `y_rect` shift up numerically.

### 2) Undo the rectification rotation

```cpp
cv::Mat ray_rect = (cv::Mat_<float>(3, 1) << x_rect, y_rect, 1.0f);
cv::Mat ray_orig = R_inv * ray_rect;
```

- `R_inv` rotates the ray from rectified frame back to original camera frame.
- If `R_inv` changes, the map field rotates/tilts accordingly.

### 3) Perspective normalization

```cpp
const float inv_z = 1.0f / ray_orig.at<float>(2, 0);
const float x = ray_orig.at<float>(0, 0) * inv_z;
const float y = ray_orig.at<float>(1, 0) * inv_z;
```

- Converts homogeneous ray `(X, Y, Z)` to normalized point `(x, y) = (X/Z, Y/Z)`.

### 4) Radial distortion

```cpp
const float r2 = x * x + y * y;
const float r4 = r2 * r2;
const float r6 = r4 * r2;
const float radial = 1.0f + k1 * r2 + k2 * r4 + k3 * r6;
```

- `r2` is distance from optical center in normalized coordinates.
- `k1, k2, k3` control radial distortion.

Typical behavior:

- More negative dominant radial terms: barrel-like pull inward.
- More positive dominant radial terms: pincushion-like push outward.
- Larger `|k3|` mostly affects farther-from-center regions.

### 5) Tangential + radial combined distortion

```cpp
const float x_dist = x * radial + 2.0f * p1 * x * y + p2 * (r2 + 2.0f * x * x);
const float y_dist = y * radial + p1 * (r2 + 2.0f * y * y) + 2.0f * p2 * x * y;
```

- `p1, p2` are tangential distortion parameters (decentering/misalignment effects).
- They create asymmetric warps (not purely circular).

### 6) Convert back to source image pixels

```cpp
map1.at<float>(v, u) = fx * x_dist + cx;
map2.at<float>(v, u) = fy * y_dist + cy;
```

- `fx, fy, cx, cy` are original camera intrinsics.
- This projects distorted normalized coordinates into original pixel coordinates.
- These are the source lookup coordinates used later during interpolation.

Effects of changing values:

- Increase `fx` or `fy`: larger pixel displacement per normalized unit.
- Increase `cx` or `cy`: uniform shift of the entire source lookup field.

## How this builds a remap table

For each destination pixel `(u, v)`:

1. Convert `(u, v)` to a rectified normalized ray.
2. Rotate ray back to original camera frame.
3. Normalize by depth to get `(x, y)`.
4. Apply distortion model to get `(x_dist, y_dist)`.
5. Project into original pixel coordinates `(u_src, v_src)`.
6. Store `(u_src, v_src)` in `map1/map2` at index `(v, u)`.

After all pixels are processed, `map1` and `map2` are complete inverse-warp lookup tables.

## One numeric example

Assume:

- Rectified intrinsics: `fxp=fyp=700`, `cxp=640`, `cyp=360`
- Original intrinsics: `fx=fy=710`, `cx=638`, `cy=359`
- Distortion: `k1=-0.20`, `k2=0.03`, `k3=0`, `p1=0.001`, `p2=-0.001`
- Destination pixel: `(u, v) = (700, 380)`
- For this toy example, `R_inv = I`

Compute:

- `x_rect = (700-640)/700 = 0.085714`
- `y_rect = (380-360)/700 = 0.028571`
- `x = 0.085714`, `y = 0.028571`
- `r2 = 0.008163`, `r4 = 0.0000666`, `r6 = 0.000000543`
- `radial = 1 + k1*r2 + k2*r4 + k3*r6 = 0.998369`
- `x_dist ≈ 0.085556`
- `y_dist ≈ 0.028530`
- `u_src = 710*0.085556 + 638 ≈ 698.745`
- `v_src = 710*0.028530 + 359 ≈ 379.256`

So:

- `map1(380,700) = 698.745`
- `map2(380,700) = 379.256`

Interpretation: rectified output pixel `(700,380)` samples from around `(698.75,379.26)` in the original image.
