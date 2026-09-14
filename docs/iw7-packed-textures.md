# Building IW7's packed model textures

How to construct `_cs`, `_ng`/`_nog`, `_sg` and `_a` from IW3/IW5 sources. Measured 2026-09-05
against 14,630 IW7 materials under `zonetool_paths\techsets_*\materials`, the resident packed
images in `zonetool_paths\code_post_gfx\images`, and the converted IW3 textures in
`dump\mp_test_h1\images`.

## Correction: the packed forms are not optional

[iw5-iw7-model-textures.md](iw5-iw7-model-textures.md) said the unpacked semantic 5 + semantic 8
path was a viable substitute, because stock materials use those semantics thousands of times. That
was the wrong reading. Counting whole materials rather than texture entries:

| semantic set | materials |
|---|---|
| (14, 15, 16) | 4290 |
| (2, 5, 14, 15, 16) | 953 |
| (2, 5, 8, 14, 15, 16) | 860 |
| (2, 5, 14, 15) | 801 |
| (2, 5, 8, 9, 14, 15, 16) | 748 |

Of 11,313 materials carrying real textures, **only 247 use semantic 5 or 8 without 14/15, and every
one is a `tools_*` debug shader** (`$additive`, `$line`, …). Semantics 2, 5, 8 and 9 never form a
base layer — they are the *additional blend layers* of a multi-layer world material
(`w_l_sm_blend_b0c0s0…_b1c1s1…`). The base layer is always `_cs` (14) + `_ng` (15), plus `_a` (16)
where the material reveals or blends.

## The slot hashes do not change

This is what makes the material rewrite cheap:

| slot typeHash | IW5 use | IW7 use |
|---|---|---|
| 2695565377 | semantic 2, `colorMap` | semantic **14**, `_packed_cs` |
| 1507003663 | semantic 5, `normalMap` | semantic **15**, `_packed_ng` |
| 1253113093 | — | semantic **16**, `_packed_a` |

So a converted material keeps its existing texture entries and only changes each entry's `semantic`
and `image`. The separate specular entry is consumed into the other two and dropped.

## Channel recipe

Confirmed against [GameImageUtil](https://github.com/Scobalula/GameImageUtil), which is the
reference for reading these formats. Its `CoDNOGProcessor` documents the `_ng`/`_nog` layout
exactly as measured here, and settles the one thing the data could not:

| target | semantic | format | R | G | B | A |
|---|---|---|---|---|---|---|
| `_packed_cs` | 14 | BC7 | albedo / spec colour | " | " | **metalness + reflectance** |
| `_packed_ng` | 15 | BC7 | gloss | **normal X** | occlusion | **normal Y** |
| `_packed_nog` | 15 | BC7 | gloss | normal X | baked AO | normal Y |
| `_sg` | 8 | BC3 | spec | spec | spec | gloss |
| `_packed_a` | 16 | BC4 | alpha / reveal | | | |

### The normal is hemi-octahedral, not raw XY

This is the part that would have been silently wrong. G and A are not the tangent-space X and Y —
they are a hemi-octahedron encoding, rotated 45 degrees so the diamond `|x| + |y| <= 1` fills the
whole unit square. GameImageUtil's decode:

```
nv  = (G * 2 - 1, A * 2 - 1)
xy  = (nv.x + nv.y, nv.x - nv.y) * 0.5
n   = normalize(xy.x, xy.y, 1 - |xy.x| - |xy.y|)
```

Inverting it for the encode, given a unit tangent-space normal with `n.z >= 0`:

```
L = |n.x| + |n.y| + n.z
p = (n.x, n.y) / L
G = (p.x + p.y) * 0.5 + 0.5
A = (p.x - p.y) * 0.5 + 0.5
```

A flat normal gives `L = 1`, `p = 0`, so `G = A = 0.5` - which is exactly the 128 both channels
centre on in the measured data. The 45 degree rotation is why their spread is wider than a raw XY
encoding would give.

Sources: the Activision 2017 "Rendering of Infinite Warfare" course notes, JCGT 3(2), and Vlachos'
GDC 2015 VR rendering talk, all cited in GameImageUtil.

### `_cs` is a metalness encode, not albedo plus F0

Also wrong in the earlier draft. The alpha is not a specular F0 value - it is a combined
metalness/reflectance control, and the RGB is *shared* between albedo and specular colour.
GameImageUtil's `CoDFusedCSProcessor` unpacks it as:

```
insulatorSpecRange = 0.1
m      = clamp(A - 0.1, 0, 1) * (1 / 0.9)          // metalness
d      = clamp(1 - m, 0, 1)                        // diffuse weight
r      = min(A, 0.1)                               // dielectric reflectance
albedo = RGB * d
spec   = max(r + m * RGB, 0.21)                    // per channel
```

Below `A = 0.1` the surface is a dielectric: albedo is the RGB unchanged and the specular collapses
to a flat 0.21, which is the sRGB form of the usual ~0.04 linear. Above it, albedo fades out and the
specular takes on the RGB colour - the standard metalness workflow, packed into one channel.

That is consistent with the one resident stock `_cs` measured here, whose alpha is a constant
10/255 = 0.039, comfortably inside the dielectric range.

**Building one from IW5 is therefore lossy in a specific way.** IW5 has no metalness channel, so the
safe conversion is dielectric throughout: `RGB = colorMap`, `A = 10/255`. The IW5 specular *colour*
is then discarded - only its alpha survives, as gloss in `_ng`.R. Deriving metalness from a bright,
saturated specular over a dark albedo is possible as a heuristic, but it is an invention, not a
conversion.

### Source normals: X is in alpha

GameImageUtil's `CoDXYNormalMapProcessor` (MW/WaW/MW2/MW3/BO1) reads `nX = pixel.W` and
`nY = pixel.Y` - X from alpha, Y from the greyscale colour block. Its `BC5XYNormalMapProcessor`
reads `nX = pixel.X`, `nY = pixel.Y`. So a correct DXT5 -> BC5 conversion is red from alpha, green
from the colour block.

**The IW7 linker gets this backwards.** Correlating its `car_engine_nml` output against the source,
its BC5 red matches the source's *colour block* at +0.9991 and its BC5 green matches the source's
*alpha* at +0.9998 - X and Y transposed, which mirrors the normal about the diagonal and reverses
which way surface detail appears to catch the light. IwiImage.cpp originally reproduced this because
the linker's output was the only reference available; it now does the correct thing, behind
`match_linker_normal_swap` for anyone who needs byte-compatibility with the linker instead.

## Gloss really is there

IW3's material compiler already merges a gloss map into the specular's alpha, and the image name
records it: `~<spec>-rgb&<cos>-l-11`, i.e. spec in RGB and the *cosine-power* map folded into
luminance-as-alpha. Measured on the converted IW3 speculars:

| image | alpha (gloss) | distinct alpha values |
|---|---|---|
| `~pine02_spec-rgb&pine02_cos-l-11` | 83.4 ± 50.2 | 253 |
| `~plasticcrate_spec-r30g30b30&…` | 170.3 ± 50.7 | 256 |
| `~paint_can_spc-rgb&paint_can_cos-l-11` | 117.6 ± 46.8 | 219 |
| `~junktire_spc-rgb&junktire_cos-l-11` | 198.0 ± 9.1 | 150 |
| `~pail_spec-rgb&$white-l-11` | 243.0 ± 0.0 | 2 |

The last row is the failure mode to expect: where the artist supplied no cos map the compiler
merged `$white`, so gloss is a flat 243. Those are the materials that will still look uniformly
shiny, and no conversion can recover what was never authored.

Note the specular's own RGB is close to monochrome in stock `_sg` (decoded R and B identical, G
differing only by 5- vs 6-bit quantisation), so little is lost by not carrying its colour through a
dielectric `_cs`.

## BC7 encoder

Only **mode 6** is needed: one subset, RGBA, endpoints 7.7.7.7 plus one p-bit each, 4-bit indices.
Bit layout is the mode marker (six zero bits then a 1), then R0 R1 G0 G1 B0 B1 A0 A1 at 7 bits each,
then P0, P1, then 63 index bits — texel 0 gets 3 bits because its high bit is implicit, the rest 4.
An endpoint channel is `(v << 1) | p`, and the anchor's implicit high bit means the endpoints must
be swapped (and every index inverted) whenever index 0 lands above 7.

Fitting a block:

1. Endpoints from the **principal axis** of the block's 4×4 covariance, projected to min/max.
   Bounding-box endpoints are noticeably worse — they cost roughly 2× the error on the normal
   channels.
2. Try all four p-bit combinations; each p-bit is shared across its endpoint's four channels.
3. Pick indices by nearest palette entry, then **least-squares refit** the endpoints against those
   indices and iterate. Four passes is enough.

Round-trip error measured on real assembled `_ng` data — actual converted normals paired with
actual gloss:

| source | blocks | mean | p95 | max |
|---|---|---|---|---|
| `junktire_nml` 512² | 3000 | **0.28** | 2 | 32 |
| `potted_plant01_nml` 256×512 | 3000 | **0.36** | 1 | 19 |
| `paint_can_nml` 128² | 1024 | **1.02** | 6 | 44 |

That is well inside the quantisation the DXT sources already carry, so the packing costs
essentially nothing. (Synthetic blocks of independent per-texel noise measure 5–44, but no real
texture looks like that — a 4×4 block of a real normal map is spatially coherent, which is exactly
what a single-line fit wants.)

## Resolved

The earlier open question - which of `_ng`'s G and A holds which tangent-space component - is
answered by `CoDNOGProcessor`: G is X, A is Y. It was never separable from the data, since both are
a matched pair centred on 128, and the correct answer turned out not to be a straight XY pair at all.

## Naming

Stock records the construction in the name, always as

```
<primary source name>_<secondary token>_packed_<channels>
```

The secondary token is spelled two different ways, and which one it is depends on the **target**,
not on the technique. Measured over the 17,686 distinct packed image names referenced by
`zonetool_paths\techsets_*\materials`:

| target | count | secondary token |
|---|---|---|
| `_packed_cs` | 8381 | the specular's **name**, e.g. `camo_97_c_camo_97_sg_packed_cs` |
| `_packed_nog` | 3756 | 8 hex digits |
| `_packed_ng` | 3104 | 8 hex digits |
| `_packed_a` | 2170 | 8 hex digits |
| `_packed_r` / `_ar` / `_at` / `_n` / `_no` | 274 | 8 hex digits |

Not one `_packed_cs` ends in a hex token, and not one of the others ends in a name.

### The hash

**djb2 seeded with zero** — `h = 0; for each byte: h = h * 33 + c`, over the raw image name,
printed as 8 lowercase hex digits. `00000000` when there is no secondary source (2170 `_packed_a`,
3 `_packed_r`; no stock `_packed_ng` or `_nog` is ever built without a gloss source).

Note this is *not* the material slot `typeHash` — that is a different CoD string hash. Do not reuse
one for the other.

Evidence: 3096 of the 3104 stock `_packed_ng` hashes resolve back to a real image name in the
corpus. Pairing each material's `_packed_cs` (which names its specular outright) against its
`_packed_ng`, 737 of 775 reproduce exactly:

```
camo_97_c_camo_97_sg_packed_cs          <->  camo_97_n_8d864148_packed_ng
djb2_0("camo_97_sg")                     =   0x8d864148

iw7_drywall_painted_white_a_iw7_drywall_painted_white_c_sg_packed_cs
                                        <->  iw7_drywall_painted_white_n_cd821074_packed_ng
djb2_0("iw7_drywall_painted_white_c_sg") =   0xcd821074

cp_disco_graffiti_02_c_cp_disco_graffiti_02_sg_packed_cs
                                        <->  cp_disco_graffiti_02_n_58b43ea9_packed_ng
```

The 38 that do not reproduce are materials whose `_cs` and `_ng` came from *different* source sets
(a decal blended over an unrelated base), not a second rule. `_packed_nog` never reproduces from a
single name because it hashes two secondaries, gloss and baked AO; IW5 has no AO source, so that
target is never emitted by the converter.

### The unpacked counterparts

Not every slot is packed, and the unpacked ones have their own convention. Mapping every stock
texture entry's semantic against its image-name suffix:

| semantic | dominant naming | n |
|---|---|---|
| 2 | `_c` 7599, `_col` 675 | 11651 |
| 5 | `$identitynormalmap` 4896, `_n` 3731, `_nml` 542 | 9449 |
| **8** | **`_sg` 5208 (97.4%)**, `$white` 72 | 5346 |
| 9 | `_o` 2972 | 3000 |
| 14 | `_packed_cs` — **100%** | 24379 |
| 15 | `_packed_ng` 13005, `_packed_nog` 11284 | 24379 |
| 16 | `_packed_a` 6837, `_packed_r` 4601, `_packed_ar` 1894 | 13332 |

So semantic 8 is `_sg` and semantic 14 has no unpacked form at all — there is no bare `_cs` in
stock, only `_packed_cs`.

`_sg` needs no new encoder: `IwiImage.cpp` already produces the right pixels for both source
formats. A DXT5 specular *is* the `_sg` layout, because IW3's compiler merged the cos map into its
alpha, and a DXT1 specular is transcoded up to BC3 with an alpha block added. What was missing was
the name — a converted material pointed its specular slot at a raw IW5 name.

IW7 names its unpacked maps off one shared base (`camo_97_c`, `camo_97_n`, `camo_97_sg`), so
`iw7_map_base` recovers that base before appending `_sg`:

```
~pine02_spec-rgb&pine02_cos-l-11  ->  pine02_sg
~pail_spec-rgb&$white-l-11        ->  pail_sg
camo_97_spc / _spec / _s / _sg    ->  camo_97_sg     (idempotent)
```

### What the converter emits

`Material.cpp` builds the pair for every material it dumps, keyed and cached on the source pair
(plus the fastfile, since the dump directory moves between zones). `p0` selects the *technique* and
decides whether the separate specular entry survives — it does not decide whether the images
exist. The one extrapolation: stock has no example of a `_packed_cs` built without a specular, so
that case borrows `00000000`, the token every other target uses for a missing secondary.
