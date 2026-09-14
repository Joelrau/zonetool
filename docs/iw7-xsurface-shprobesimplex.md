# IW7 XSurface::shProbeSimplexVertData

Reversed 2026-09-04 from `iw7_ship_dump_named.i64` plus the 21,882 model surfaces dumped from
mp_paris, mp_breakneck, mp_afghan and cp_zmb. Everything under **Measured** is counted; everything
under **Inferred** is not.

## What gates what

`SURF_FLAG_SECONDUV` (0x100) does **not** gate a second UV set, despite the name. Across all
21,882 shipped surfaces:

| gate | consequence | agreement |
|---|---|---|
| `SURF_FLAG_LIGHTMAP_COORDS` (0x10) | `lmapUnwrap` present (2 floats/vert) | 310 / 310, and absent in all 21,572 without it |
| `SURF_FLAG_SECONDUV` (0x100) | `shProbeSimplexVertData` present, 8 B/vert | 11,449 / 11,449 |
| `SURF_FLAG_MAYHEM_SELFVIS` (0x400) alone | 24 B/vert | 448 |
| both 0x100 and 0x400 | 32 B/vert | 66 |
| neither | nothing loaded | 9,919 |

0x100 and `SURF_FLAG_MAYHEM_CUSTOM_CHANNELS` (0x200) always co-occur in shipped data - 0x100 never
appears without 0x200 and vice versa.

## The 8-byte record (SHProbeSimplexData1)

**Measured** over 207,123 vertices from mp_paris:

- bytes 0..3: four indices, each in [0, 63]
- bytes 4..7: four weights, and `w0+w1+w2+w3 == 255` for **every single vertex**, 207,123 / 207,123
- an unused slot has weight 0, and its index byte is 0 in 98.9-99.4% of cases
- the index space is per-surface: palette sizes run 1..64 (mode 2), contiguous 0..n-1 in 85% of
  surfaces, and uncorrelated with `rigidVertListCount`

So: per vertex, up to four (index, weight) pairs over a small per-surface palette, weights
normalised to 255.

**The data is model-local, not map-baked.** Of the 191 `.xsb` files shared between mp_paris and
mp_breakneck, all 167 comparable surfaces have byte-identical `shProbeSimplexVertData` (and
byte-identical vertices). Nothing in it references a particular map's probe volume.

## Loader

`Load_XSurface` @ `0x140A17E70` streams the 256-byte struct, then near the end:

```
varSHProbeSimplexData = &varXSurface->shProbeSimplexVertData;
Load_SHProbeSimplexDataUnion();                       // 0x1409F36E0
Load_Stream(0, &surf->shProbeSimplexVertBuffer, 8);
Load_VertexBuffer(&surf->shProbeSimplexVertBuffer,
                  surf->shProbeSimplexVertData.data,
                  GetSHProbeSimplexDataSize(surf) * surf->vertCount,
                  "xsurface sh probe simplex vertices");
Load_Stream(0, &surf->shProbeSimplexVertBufferView, 8);
Load_VertexBufferView(&surf->shProbeSimplexVertBufferView, surf->shProbeSimplexVertBuffer,
                      GetSHProbeSimplexDataSize(surf) * surf->vertCount,
                      "xsurface sh probe simplex vertices view");
```

It becomes a **D3D vertex buffer**, through the same `Load_VertexBuffer` path as `verts0` and
`lmapUnwrap` - not a structured/raw buffer like `blendVerts` or `tensionData`. It is a per-vertex
input stream.

`Load_SHProbeSimplexDataUnion` @ `0x1409F36E0` branches exactly as zonetool's dumper does:

```
if ((flags & 0x500) == 0x500)  Load_SHProbeSimplexData0();   // 32 * vertCount
else if (flags & 0x100)        Load_SHProbeSimplexData1();   //  8 * vertCount
else if (flags & 0x400)        Load_SHProbeSimplexData2();   // 24 * vertCount
```

Each `Load_SHProbeSimplexDataN` handles the usual four pointer states (0 absent, -1 shared-data
push/pop, -2 inline, otherwise block offset via `DB_ConvertOffsetToPointer`), calls
`DB_PatchMem_FixStreamAlignment(3)` for 4-byte alignment, then
`Load_Stream(1, g_streamPos, K * vertCount)`.

`GetSHProbeSimplexDataSize` @ `0x140E4E0C0`:

```
if ((flags & 0x100) == 0) return 24;
if ((flags & 0x400) != 0) return 32;
return 8;
```

Note this disagrees with the load gate: with neither 0x100 nor 0x400 set it reports 24 while the
union loads nothing, so the vertex buffer is created from a null source. Harmless, but it means the
size function alone is not a reliable presence test.

## What the material type actually selects

`XSurfaceGetModelVertDecl` @ `0x140E4E190` - the input layout, which is what `MTL_TYPE_MODEL_*`
has to agree with:

```
if (!(flags & 0x80))            return (flags & 0x800) ? 5 : ((flags & 8) ? 2 : 1);
if (flags & 0x200)              return (flags & 0x400) ? 11 : 8;   // MAYHEM_CUSTOM_CHANNELS
if (flags & 0x100)              return (flags & 0x400) ? 10 : 7;   // SECONDUV
                                return (flags & 0x400) ?  9 : 6;   // MAYHEM_SELFVIS
```

0x200 outranks 0x100, and since shipped data always pairs them, decls 7 and 10 never occur in a
retail map. A material declaring one layout drawn on a surface that produces another gets the wrong
input layout - the material type and the surface flags are two halves of one contract.

## Runtime architecture (names and strings, not yet decompiled end to end)

IW7 resolves the light grid for dynamic geometry on the GPU, asynchronously:

- buffers `"light grid sampling requests"`, `"light grid sampling history"`,
  `"lightgrid sample constant buffer"`; local type `GpuLightGridRequestRecord`
- compute shaders `cs_sh_sample_lightgrid.hlsl` and `cs_del_sh_sample_lightgrid{,_sh}.hlsl`, the
  latter in `_1 _2 _4 _8` variants (also `_lowres`, `_reflection_volumes`) - the numbers match
  sample counts per entity
- the GfxWorld GPU buffers are named `probesData`, `probesPositions`, `probeTets`,
  `probeTetNeighbors`, `probeTetVisibility`, `probeVoxelStartTet`, `gpuVisibleProbes` - exactly the
  arrays the converter generates
- `smodelShProbeParams` buffers are created by `sub_140E058B0` (0x2000 + three of 0x800)
- `R_AllocStaticModelLighting` @ `0x140E154B0`, `R_ResetModelLighting` @ `0x140E159F0`,
  `R_LightGridCacheFlush` @ `0x140E1BDD0`

**Inferred**, consistent with all of the above but not proven: the palette is the set of light grid
sample points the engine resolves for that draw, and the per-vertex indices/weights blend those
resolved SH results across the mesh. That is why the data can be model-local - the positions are
resolved at runtime, only the blend topology is baked.

## Dvars worth knowing

Registered in `R_RegisterDvars`:

- `r_useCameraPositionForViewModelLightGridSampling` - "Use camera position to sample lightgrid for viewmodel"
- `r_lightGridDefaultColor` - "Default ambient light for failed lightgrid lookups"
- `r_lightGridSampleAsync`, `r_lightGridTempSmoothingFactor`, `r_lightGridSHBands`,
  `r_lightGridIntensity`, `r_lightGridContrast`, `r_lightGridEnableTweaks`, `r_lightGridDebugPosLocked`
- `r_lightGridDebug` - "GPU light grid debug modes", an enum whose value names are, in memory order:
  Show GPU lightgrid points / Show static probes / Show dynamically sampled probes /
  Show lighting at camera position / Show camera position tet / Show camera position tet probes
  visibility / Show tetrahedrons visibility / Show GPU lightgrid sun light visibility / Show static
  probes sun light visibility / Show dynamically sampled probe sun light visibility

## Consequences for the IW5 -> IW7 converter

- `src/IW5/Converter/IW7/Assets/XSurface.cpp` never sets `SURF_FLAG_SECONDUV` and never allocates
  `shProbeSimplexVertData`, so converted models take vertex decl 6 and get one implicit sample.
  That is self-consistent; it is not a bug, just the lowest-fidelity path.
- Line 78 forces `SURF_FLAG_SELF_VISIBILITY` on every surface and line 116 writes a constant
  `{0, 0, 1, 0}` to every vertex: bent normal zero, primary visibility fully open, **secondary
  visibility zero**. Every converted model tells the shader nothing occludes anything.
- `SURF_FLAG_LIGHTMAP_COORDS` (0x10) is likewise never set and `lmapUnwrap` never allocated.

See [iw7-lightgrid-ground-truth.md](iw7-lightgrid-ground-truth.md) for the probe volume itself.
