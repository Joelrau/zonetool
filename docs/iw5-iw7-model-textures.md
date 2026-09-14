# IW5 → IW7 model normal and specular maps

Measured 2026-09-04 from stock IW7 image dumps (mp_paris, mp_breakneck, mp_afghan, cp_zmb),
11,240 IW7 world materials under `zonetool_paths\techsets_*\materials`, and IW5 `.iwi` files from
the MW3 dumps. Channel roles come from decoding BC7 endpoints directly, not from the names.

## The short version

IW7 keeps IW5's material **slot hashes** but changes both the **semantic** and the **image
content**. A texture that is correct for IW5 is wrong for IW7 even in the same slot.

| slot (typeHash) | IW5 semantic | IW7 semantic |
|---|---|---|
| 2695565377 | 2 `TS_COLOR_MAP` | **14 `TS_COLOR_SPECULAR`** |
| 1507003663 | 5 `TS_NORMAL_MAP` | **15 `TS_NORMAL_OCC_GLOSS`** |

## IW5 source layout

| map | semantic | format | packing |
|---|---|---|---|
| `_c` colorMap | 2 | DXT1 / DXT5 | RGB albedo, A alpha-test |
| `_n` normalMap | 5 | **DXT5**, 10175/10189 sampled | one component in RGB (greyscale), one in alpha, Z reconstructed |
| `_s` specularMap | 8 | DXT1 | RGB specular colour |

Verified on the normal maps by decoding DXT5 endpoints: R, G and B means and standard deviations
are identical to a decimal place (e.g. `bear_n` R 168.9/13.9, G 169.1/14.1, B 168.9/13.9), i.e. the
colour block is greyscale, while alpha varies independently (a0 171.6, a1 85.6). Two live channels,
one in the greyscale colour block and one in alpha, with Z reconstructed.

Which of the two is X and which is Y is **not** the conventional "X in alpha": transcoding
`car_engine_nml` and correlating against the linker's own BC5_SNORM output, BC5 red matches the
source's **colour block** at +0.9991 and BC5 green matches the source's **alpha** at +0.9998. So
colour -> first component, alpha -> second.

## IW7 target layout

Stock model textures, by name suffix, across four maps:

| suffix | count | format | semantic |
|---|---|---|---|
| `_cs` | 3042 | BC7_UNORM | 14 `TS_COLOR_SPECULAR` |
| `_ng` | 1415 | BC7_UNORM | 15 `TS_NORMAL_OCC_GLOSS` |
| `_nog` | 1328 | BC7_UNORM | 15 `TS_NORMAL_OCC_GLOSS` |
| `_sg` | 500 | BC3_UNORM | 8 `TS_SPECULAR_MAP` |
| `_n` | 286 | BC7_UNORM | 5 `TS_NORMAL_MAP` |
| `_a` | 716 | BC4_UNORM | 16 `TS_ALPHA_REVEAL_THICKNESS` |
| `_o` | 92 | BC4_UNORM | 9 `TS_SPECULAR_OCCLUSION` |

The packed names record their own construction: `<colour>_c_<spec>_sg_packed_cs`,
`<normal>_nml_<hash>_packed_ng`, `<colour>_c_00000000_packed_a`.

### Channel roles, measured

100% of BC7 blocks in both `_ng` and `_cs` use alpha-capable modes (4–7), so alpha carries data in
both. Decoding mode-5 and mode-6 endpoints separately gives the same answer:

**`_ng` / `_nog` (semantic 15)** — `ground_wires_01_nml_ecfca591_packed_ng`, 4519 blocks:

| channel | mean | std | role |
|---|---|---|---|
| R | 146.3 | 31.2 | **gloss** |
| G | 128.0 | 12.0 | **normal Y** |
| B | 254.6 | 0.5 | **occlusion** (1.0 here — this is an `_ng`, not `_nog`) |
| A | 124.6 | 11.9 | **normal X** |

G and A are a matched pair centred on 128 with the same spread — the two signed normal components.
**IW7 keeps IW5's pairing exactly: the normal lives in green and alpha.** The channels IW5 wasted on
a greyscale duplicate are reused for gloss (R) and occlusion (B).

**`_cs` (semantic 14)** — same model, 4985 blocks:

| channel | mean | std | role |
|---|---|---|---|
| R, G, B | 128–150 | 26–67 | **albedo** |
| A | 10.0 | 0.0–0.3 | **specular F0** — 10/255 = 0.039, the standard dielectric 0.04 |

**`_sg` (semantic 8, BC3)**: the colour block is effectively monochrome (decoded R and B identical,
G differing only by 5- vs 6-bit quantisation) and alpha varies widely (a0 48.4 ± 43.9). RGB =
specular amount, A = gloss.

### The unpacked path also works

IW7 does not require packed BC7. Stock world materials use semantic 5 with a plain `_n`/`_nml` and
semantic 8 with an `_sg` thousands of times (3325 and 1989 uses respectively). H1-sourced assets
already in this tree use `BC5_SNORM` (fmt 84) at semantic 5, with R and G as signed XY — a third
convention again, and one IW7 accepts.

That is the cheap route: keep IW5's separate maps, fix the semantics and formats, skip the packing.

## Format is not the problem

IW5 model textures are `.iwi` files, not zone-embedded, so they never go through
`GenerateIW7GfxImage` (which only handles the map images that carry a `loadDef`). The IW7 pipeline
ingests `.iwi` directly - 6,621 of them across `zonetool\`, `mods\` and `zonetool_assets\` in the
IW7 tree, alongside 12,985 `.iw7Image` and 137 `.dds`. What matters is the **semantic** the material
assigns and the **channel content**, not the container.

## The normal map is already compatible

`convert_semantic` maps IW5 semantic 5 to IW7 `TS_NORMAL_MAP` (5), which is the right slot, and the
packing agrees:

- 10,175 of 10,189 IW5 normal maps sampled are DXT5, with greyscale RGB and independent alpha -
  the two components in green and alpha.
- Every IW7 normal map that could be decoded uses the same pair. `metal_painted_white_01_n`
  (semantic 5, BC7, flat normal) decodes to G 128, A 128; `ground_wires_01..._packed_ng`
  (semantic 15) to G 128 +/- 12, A 125 +/- 12.

So an IW5 normal bound at semantic 5 is read correctly. Do **not** move it to semantic 15: there R
and B are live (gloss and occlusion), and an IW5 normal's R and B are a greyscale duplicate of Y, so
gloss and AO would both be fed the normal's Y channel.

## Gloss is the real gap

IW7 carries a gloss channel in every one of its three model texture forms:

| form | semantic | gloss lives in |
|---|---|---|
| `_ng` / `_nog` | 15 | R |
| `_sg` | 8 | A |
| `_cs` | 14 | (specular F0 in A; gloss comes from the paired `_ng`) |

IW5 has no gloss channel anywhere. Its normal is DXT5 with only two live channels, and its
specular maps are mostly DXT1 - of the specular-suffixed `.iwi` files found, 10 are DXT1 (no alpha
at all), 2 DXT3, 4 DXT5.

That is the visible difference. An IW5 DXT1 specular map bound at IW7 semantic 8 gives the shader a
**constant alpha of 1.0**, i.e. maximum gloss everywhere. Models come out uniformly mirror-like
regardless of surface. The two DXT5 specular maps that do exist carry high, nearly constant alpha
(a0 191.9 +/- 3.6, a1 196.7 +/- 6.6 for `arctic_headgear_b_spc`; 243/248 with zero variance for
`~me_brick_spc`), so even those do not encode a useful gloss field.

## Conversion recipe

All of this is implemented in [IwiImage.cpp](../src/IW5/Converter/IW7/Assets/IwiImage.cpp), called
from the IW7 image dumper for every non-map image. The `.iwi` is read back through the game's own
filesystem (`filesystem_read_big_file`), so `main\iw_*.iwd` archives resolve the same as loose
files, and the semantic comes straight off the IW5 `GfxImage`:

1. Reverse the mip chain - `.iwi` is smallest-first, IW7 is largest-first.
2. Normals: transcode DXT5 to `BC5_SNORM` with red from the colour block and green from alpha. The
   alpha half copies bit for bit (a DXT5 alpha block is already a BC4 block; biasing both endpoints
   by -128 makes it `BC4_SNORM`).
3. Specular: a DXT1 source has no alpha, so promote it to BC3 with a chosen gloss constant. The
   colour half copies verbatim, since a BC3 colour block *is* a BC1 block. DXT5 speculars - which
   is what IW3's combined `~spec-rgb&cos-l-11` images are - already carry gloss and pass through.
4. Don't build packed `_ng` / `_cs` BC7 unless you have real gloss and occlusion data. Without it
   the unpacked semantic 5 + semantic 8 path is cheaper and no worse - stock world materials use it
   3,325 and 1,989 times respectively.

## Cube maps

Measured across the 26 cube images in the IW7 trees:

- `mapType = MAPTYPE_CUBE (5)`, and **`depth` and `numElements` both stay 1** - the six faces are
  implied by the map type, not by an array size.
- `dataLen1` is 6x the full 2D mip chain. Confirmed exactly on `hdr_sky_h1_bog_a_ft` (BC6H 1024,
  1 level, 1048576 x 6 = 6291456), `hdr_sky_h1_mp_shipment_02_ft` (4194304 x 6 = 25165824),
  `sp_bonus_ft` (BC1, 524288 x 6 = 3145728) and `_reflection_probe0` (RGBA16F 128, 8 levels,
  174760 x 6 = 1048560).
- **Face order is D3D subresource order** - face 0 whole chain, then face 1. Measured on
  `mp_credits_s1\_reflection_probe1` (R11G11B10_FLOAT, 8 levels): downsampling level 0 matches
  level 1 at +0.9986 face-major, against +0.9602 and +0.9883 for the two mip-major readings.
- Every stock IW7 sky cube is single-level, so for a skybox only the face order matters.

The converter detects a cube by size rather than by flag - if the payload does not fit the chain but
`available / 6` does. The cubemap bit has to survive several struct casts to reach that code; the
arithmetic does not.

The source-side layout is the one half that is argued rather than measured: no cube `.iwi` exists in
either image tree (all 27,422 scanned), so it comes from what `fileSizeForPicmip` implies -
truncating the end must drop the largest level across all six faces, so the faces sit together
inside each level. `source_is_mip_major` in IwiImage.cpp flips it if a multi-level cube ever comes
out shuffled.

Validated against the linker's own output for the same `.iwi` files: four normal maps convert to an
identical header (DXGI format, payload length, level count) with per-texel agreement of 0.12-1.06
out of 255 and channel correlations of 0.948-0.9998. The alpha-derived channel is bit-exact, which
is better than the linker manages.

Note: stock IW7 model textures are **streamed** (`streamed=1`, `dataLen1=0`, resident mip 1x1), so
their dumped headers carry valid format and semantic but no pixels and no real dimensions. The
resident examples used above come from `zonetool_paths\code_post_gfx\images`, `dump\cp_zmb` and
`dump\mp_afghan`.
