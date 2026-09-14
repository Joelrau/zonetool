
/* ===================== Load_GfxLightGridProbeData @ 1409F0DC0 ===================== */
void __fastcall Load_GfxLightGridProbeData(char a1)
{
  GfxLightGridProbeData *v1; // rdi
  __int64 v2; // rbx
  __int64 v3; // rbx
  void **p_gpuVisibleProbesBuffer; // rbx
  __int64 v5; // rdi
  void **p_gpuVisibleProbesView; // rbx
  __int64 v7; // rdi
  void **p_gpuVisibleProbesRWView; // rbx
  __int64 v9; // rdi
  GfxLightGridProbeData *v10; // rdi
  __int64 v11; // rbx
  void **p_probesBuffer; // rbx
  __int64 v13; // rdi
  void **p_probesView; // rbx
  __int64 v15; // rdi
  void **p_probesRWView; // rbx
  __int64 v17; // rdi
  GfxLightGridProbeData *v18; // rdi
  __int64 v19; // rbx
  void **p_probePositionsBuffer; // rbx
  __int64 v21; // rdi
  void **p_probePositionsView; // rbx
  __int64 v23; // rdi
  GfxLightGridProbeData *v24; // rdi
  __int64 v25; // rbx
  __int64 v26; // rbx
  void **p_tetrahedronBuffer; // rbx
  __int64 v28; // rdi
  void **p_tetrahedronView; // rbx
  __int64 v30; // rdi
  GfxLightGridProbeData *v31; // rdi
  __int64 v32; // rbx
  void **p_tetrahedronNeighborsBuffer; // rbx
  __int64 v34; // rdi
  void **p_tetrahedronNeighborsView; // rbx
  __int64 v36; // rdi
  GfxLightGridProbeData *v37; // rdi
  __int64 v38; // rbx
  void **p_tetrahedronVisibilityBuffer; // rbx
  __int64 v40; // rdi
  void **p_tetrahedronVisibilityView; // rbx
  __int64 v42; // rdi
  GfxLightGridProbeData *v43; // rdi
  __int64 v44; // rbx
  void **p_voxelStartTetrahedronBuffer; // rbx
  __int64 v46; // rdi
  void **p_voxelStartTetrahedronView; // rbx
  __int64 v48; // rdi

  Load_Stream(a1, varGfxLightGridProbeData, 240u);
  v1 = varGfxLightGridProbeData;
  if ( varGfxLightGridProbeData->gpuVisibleProbePositions )
  {
    v2 = qword_1453E2FD0;
    DB_PatchMem_FixStreamAlignment(3);
    v1->gpuVisibleProbePositions = (GfxGpuLightGridProbePosition *)g_streamPos;
    qword_1453E2FD0 = (__int64)g_streamPos;
    Load_Stream(1, g_streamPos, 12LL * varGfxLightGridProbeData->gpuVisibleProbesCount);
    v1 = varGfxLightGridProbeData;
    qword_1453E2FD0 = v2;
  }
  if ( v1->gpuVisibleProbesData )
  {
    v3 = qword_1453E4790;
    DB_PatchMem_FixStreamAlignment(63);
    v1->gpuVisibleProbesData = (GfxProbeData *)g_streamPos;
    qword_1453E4790 = (__int64)g_streamPos;
    Load_Stream(1, g_streamPos, (unsigned __int64)(varGfxLightGridProbeData->gpuVisibleProbesCount + 0x2000) << 6);
    v1 = varGfxLightGridProbeData;
    qword_1453E4790 = v3;
  }
  p_gpuVisibleProbesBuffer = &v1->gpuVisibleProbesBuffer;
  v5 = varGfxBuffer;
  varGfxBuffer = (__int64)p_gpuVisibleProbesBuffer;
  Load_Stream(0, p_gpuVisibleProbesBuffer, 8u);
  sub_140DD2010(
    (bdVoteRank *)p_gpuVisibleProbesBuffer,
    (__int64)varGfxLightGridProbeData->gpuVisibleProbesData,
    (varGfxLightGridProbeData->gpuVisibleProbesCount + 0x2000) << 6,
    (__int64)"gpuVisibleProbes");
  p_gpuVisibleProbesView = &varGfxLightGridProbeData->gpuVisibleProbesView;
  varGfxBuffer = v5;
  v7 = varGfxShaderView;
  varGfxShaderView = (__int64)&varGfxLightGridProbeData->gpuVisibleProbesView;
  Load_Stream(0, &varGfxLightGridProbeData->gpuVisibleProbesView, 8u);
  Load_RawDataBufferView(
    (__int64)p_gpuVisibleProbesView,
    (__int64)varGfxLightGridProbeData->gpuVisibleProbesBuffer,
    (__int64)varGfxLightGridProbeData->gpuVisibleProbesData,
    (varGfxLightGridProbeData->gpuVisibleProbesCount + 0x2000) << 6);
  p_gpuVisibleProbesRWView = &varGfxLightGridProbeData->gpuVisibleProbesRWView;
  varGfxShaderView = v7;
  v9 = qword_1453E30D0;
  qword_1453E30D0 = (__int64)&varGfxLightGridProbeData->gpuVisibleProbesRWView;
  Load_Stream(0, &varGfxLightGridProbeData->gpuVisibleProbesRWView, 8u);
  Load_RawDataBufferRWView(
    (bdVoteRank *)p_gpuVisibleProbesRWView,
    (__int64)varGfxLightGridProbeData->gpuVisibleProbesBuffer,
    (__int64)varGfxLightGridProbeData->gpuVisibleProbesData,
    (varGfxLightGridProbeData->gpuVisibleProbesCount + 0x2000) << 6);
  qword_1453E30D0 = v9;
  v10 = varGfxLightGridProbeData;
  if ( varGfxLightGridProbeData->probes )
  {
    v11 = qword_1453E4790;
    DB_PatchMem_FixStreamAlignment(63);
    v10->probes = (GfxProbeData *)g_streamPos;
    qword_1453E4790 = (__int64)g_streamPos;
    Load_Stream(1, g_streamPos, (unsigned __int64)varGfxLightGridProbeData->probeCount << 6);
    v10 = varGfxLightGridProbeData;
    qword_1453E4790 = v11;
  }
  p_probesBuffer = &v10->probesBuffer;
  v13 = varGfxBuffer;
  varGfxBuffer = (__int64)p_probesBuffer;
  Load_Stream(0, p_probesBuffer, 8u);
  Load_RawDataBuffer(
    (__int64)p_probesBuffer,
    (__int64)varGfxLightGridProbeData->probes,
    varGfxLightGridProbeData->probeCount << 6,
    (__int64)"probesData");
  p_probesView = &varGfxLightGridProbeData->probesView;
  varGfxBuffer = v13;
  v15 = varGfxShaderView;
  varGfxShaderView = (__int64)&varGfxLightGridProbeData->probesView;
  Load_Stream(0, &varGfxLightGridProbeData->probesView, 8u);
  Load_RawDataBufferView(
    (__int64)p_probesView,
    (__int64)varGfxLightGridProbeData->probesBuffer,
    (__int64)varGfxLightGridProbeData->probes,
    varGfxLightGridProbeData->probeCount << 6);
  p_probesRWView = &varGfxLightGridProbeData->probesRWView;
  varGfxShaderView = v15;
  v17 = qword_1453E30D0;
  qword_1453E30D0 = (__int64)&varGfxLightGridProbeData->probesRWView;
  Load_Stream(0, &varGfxLightGridProbeData->probesRWView, 8u);
  Load_RawDataBufferRWView((bdVoteRank *)p_probesRWView, (__int64)varGfxLightGridProbeData->probesBuffer, 0, 0);
  qword_1453E30D0 = v17;
  v18 = varGfxLightGridProbeData;
  if ( varGfxLightGridProbeData->probePositions )
  {
    v19 = qword_1453E2FD0;
    DB_PatchMem_FixStreamAlignment(3);
    v18->probePositions = (GfxGpuLightGridProbePosition *)g_streamPos;
    qword_1453E2FD0 = (__int64)g_streamPos;
    Load_Stream(1, g_streamPos, 12LL * varGfxLightGridProbeData->probeCount);
    v18 = varGfxLightGridProbeData;
    qword_1453E2FD0 = v19;
  }
  p_probePositionsBuffer = &v18->probePositionsBuffer;
  v21 = varGfxBuffer;
  varGfxBuffer = (__int64)p_probePositionsBuffer;
  Load_Stream(0, p_probePositionsBuffer, 8u);
  Load_RawDataBuffer(
    (__int64)p_probePositionsBuffer,
    (__int64)varGfxLightGridProbeData->probePositions,
    12 * varGfxLightGridProbeData->probeCount,
    (__int64)"probesPositions");
  p_probePositionsView = &varGfxLightGridProbeData->probePositionsView;
  varGfxBuffer = v21;
  v23 = varGfxShaderView;
  varGfxShaderView = (__int64)&varGfxLightGridProbeData->probePositionsView;
  Load_Stream(0, &varGfxLightGridProbeData->probePositionsView, 8u);
  Load_RawDataBufferView(
    (__int64)p_probePositionsView,
    (__int64)varGfxLightGridProbeData->probePositionsBuffer,
    (__int64)varGfxLightGridProbeData->probePositions,
    12 * varGfxLightGridProbeData->probeCount);
  varGfxShaderView = v23;
  v24 = varGfxLightGridProbeData;
  if ( varGfxLightGridProbeData->zones )
  {
    v25 = qword_1453E47B0;
    DB_PatchMem_FixStreamAlignment(63);
    v24->zones = (GfxGpuLightGridZone *)g_streamPos;
    qword_1453E47B0 = (__int64)g_streamPos;
    Load_Stream(1, g_streamPos, 88LL * varGfxLightGridProbeData->zoneCount);
    v24 = varGfxLightGridProbeData;
    qword_1453E47B0 = v25;
  }
  if ( v24->tetrahedrons )
  {
    v26 = qword_1453E47D0;
    DB_PatchMem_FixStreamAlignment(63);
    v24->tetrahedrons = (GfxGpuLightGridTetrahedron *)g_streamPos;
    qword_1453E47D0 = (__int64)g_streamPos;
    Load_Stream(1, g_streamPos, 16LL * varGfxLightGridProbeData->tetrahedronCount);
    v24 = varGfxLightGridProbeData;
    qword_1453E47D0 = v26;
  }
  p_tetrahedronBuffer = &v24->tetrahedronBuffer;
  v28 = varGfxBuffer;
  varGfxBuffer = (__int64)p_tetrahedronBuffer;
  Load_Stream(0, p_tetrahedronBuffer, 8u);
  Load_RawDataBuffer(
    (__int64)p_tetrahedronBuffer,
    (__int64)varGfxLightGridProbeData->tetrahedrons,
    16 * varGfxLightGridProbeData->tetrahedronCount,
    (__int64)"probeTets");
  p_tetrahedronView = &varGfxLightGridProbeData->tetrahedronView;
  varGfxBuffer = v28;
  v30 = varGfxShaderView;
  varGfxShaderView = (__int64)&varGfxLightGridProbeData->tetrahedronView;
  Load_Stream(0, &varGfxLightGridProbeData->tetrahedronView, 8u);
  Load_RawDataBufferView(
    (__int64)p_tetrahedronView,
    (__int64)varGfxLightGridProbeData->tetrahedronBuffer,
    (__int64)varGfxLightGridProbeData->tetrahedrons,
    16 * varGfxLightGridProbeData->tetrahedronCount);
  varGfxShaderView = v30;
  v31 = varGfxLightGridProbeData;
  if ( varGfxLightGridProbeData->tetrahedronNeighbors )
  {
    v32 = qword_1453E47F8;
    DB_PatchMem_FixStreamAlignment(63);
    v31->tetrahedronNeighbors = (GfxGpuLightGridTetrahedronNeighbors *)g_streamPos;
    qword_1453E47F8 = (__int64)g_streamPos;
    Load_Stream(1, g_streamPos, 16LL * varGfxLightGridProbeData->tetrahedronCount);
    v31 = varGfxLightGridProbeData;
    qword_1453E47F8 = v32;
  }
  p_tetrahedronNeighborsBuffer = &v31->tetrahedronNeighborsBuffer;
  v34 = varGfxBuffer;
  varGfxBuffer = (__int64)p_tetrahedronNeighborsBuffer;
  Load_Stream(0, p_tetrahedronNeighborsBuffer, 8u);
  Load_RawDataBuffer(
    (__int64)p_tetrahedronNeighborsBuffer,
    (__int64)varGfxLightGridProbeData->tetrahedronNeighbors,
    16 * varGfxLightGridProbeData->tetrahedronCount,
    (__int64)"probeTetNeighbors");
  p_tetrahedronNeighborsView = &varGfxLightGridProbeData->tetrahedronNeighborsView;
  varGfxBuffer = v34;
  v36 = varGfxShaderView;
  varGfxShaderView = (__int64)&varGfxLightGridProbeData->tetrahedronNeighborsView;
  Load_Stream(0, &varGfxLightGridProbeData->tetrahedronNeighborsView, 8u);
  Load_RawDataBufferView(
    (__int64)p_tetrahedronNeighborsView,
    (__int64)varGfxLightGridProbeData->tetrahedronNeighborsBuffer,
    (__int64)varGfxLightGridProbeData->tetrahedronNeighbors,
    16 * varGfxLightGridProbeData->tetrahedronCount);
  varGfxShaderView = v36;
  v37 = varGfxLightGridProbeData;
  if ( varGfxLightGridProbeData->tetrahedronVisibility )
  {
    v38 = qword_1453E4810;
    DB_PatchMem_FixStreamAlignment(63);
    v37->tetrahedronVisibility = (GfxGpuLightGridTetrahedronVisibility *)g_streamPos;
    qword_1453E4810 = (__int64)g_streamPos;
    Load_Stream(1, g_streamPos, (unsigned __int64)varGfxLightGridProbeData->tetrahedronCountVisible << 6);
    v37 = varGfxLightGridProbeData;
    qword_1453E4810 = v38;
  }
  p_tetrahedronVisibilityBuffer = &v37->tetrahedronVisibilityBuffer;
  v40 = varGfxBuffer;
  varGfxBuffer = (__int64)p_tetrahedronVisibilityBuffer;
  Load_Stream(0, p_tetrahedronVisibilityBuffer, 8u);
  Load_RawDataBuffer(
    (__int64)p_tetrahedronVisibilityBuffer,
    (__int64)varGfxLightGridProbeData->tetrahedronVisibility,
    varGfxLightGridProbeData->tetrahedronCountVisible << 6,
    (__int64)"probeTetVisibility");
  p_tetrahedronVisibilityView = &varGfxLightGridProbeData->tetrahedronVisibilityView;
  varGfxBuffer = v40;
  v42 = varGfxShaderView;
  varGfxShaderView = (__int64)&varGfxLightGridProbeData->tetrahedronVisibilityView;
  Load_Stream(0, &varGfxLightGridProbeData->tetrahedronVisibilityView, 8u);
  Load_RawDataBufferView(
    (__int64)p_tetrahedronVisibilityView,
    (__int64)varGfxLightGridProbeData->tetrahedronVisibilityBuffer,
    (__int64)varGfxLightGridProbeData->tetrahedronVisibility,
    varGfxLightGridProbeData->tetrahedronCountVisible << 6);
  varGfxShaderView = v42;
  v43 = varGfxLightGridProbeData;
  if ( varGfxLightGridProbeData->voxelStartTetrahedron )
  {
    v44 = qword_1453E4830;
    DB_PatchMem_FixStreamAlignment(63);
    v43->voxelStartTetrahedron = (GfxGpuLightGridVoxelStartTetrahedron *)g_streamPos;
    qword_1453E4830 = (__int64)g_streamPos;
    Load_Stream(1, g_streamPos, 4LL * varGfxLightGridProbeData->voxelStartTetrahedronCount);
    v43 = varGfxLightGridProbeData;
    qword_1453E4830 = v44;
  }
  p_voxelStartTetrahedronBuffer = &v43->voxelStartTetrahedronBuffer;
  v46 = varGfxBuffer;
  varGfxBuffer = (__int64)p_voxelStartTetrahedronBuffer;
  Load_Stream(0, p_voxelStartTetrahedronBuffer, 8u);
  Load_RawDataBuffer(
    (__int64)p_voxelStartTetrahedronBuffer,
    (__int64)varGfxLightGridProbeData->voxelStartTetrahedron,
    4 * varGfxLightGridProbeData->voxelStartTetrahedronCount,
    (__int64)"probeVoxelStartTet");
  p_voxelStartTetrahedronView = &varGfxLightGridProbeData->voxelStartTetrahedronView;
  varGfxBuffer = v46;
  v48 = varGfxShaderView;
  varGfxShaderView = (__int64)&varGfxLightGridProbeData->voxelStartTetrahedronView;
  Load_Stream(0, &varGfxLightGridProbeData->voxelStartTetrahedronView, 8u);
  Load_RawDataBufferView(
    (__int64)p_voxelStartTetrahedronView,
    (__int64)varGfxLightGridProbeData->voxelStartTetrahedronBuffer,
    (__int64)varGfxLightGridProbeData->voxelStartTetrahedron,
    4 * varGfxLightGridProbeData->voxelStartTetrahedronCount);
  varGfxShaderView = v48;
}


/* ===================== R_CreateSparseVoxelTree @ 140DD0B90 ===================== */
char *__fastcall R_CreateSparseVoxelTree(GfxWorld *a1)
{
  char *result; // rax
  int v3; // r14d
  __int64 v4; // r15
  GfxVoxelTree *voxelTree; // rbx
  __int64 v6; // rdi
  __int64 v7; // rdi
  __int64 *v8; // rsi
  _UNKNOWN *retaddr; // [rsp+100h] [rbp+5Fh] BYREF

  result = (char *)&retaddr;
  v3 = 0;
  if ( a1->voxelTreeCount > 0 )
  {
    v4 = 0;
    result = aOan;
    do
    {
      voxelTree = a1->voxelTree;
      if ( voxelTree[v4].voxelTopDownViewNodeCount )
      {
        R_CreateBufferInternal(
          0x40u,
          1u,
          0,
          4,
          (__int64)voxelTree[v4].voxelTreeHeader->rootNodeDimension,
          (__int64 *)&aOan[8 * v3 + 32]);
        v6 = 16LL * v3;
        sub_140DD3C50(
          12,
          voxelTree[v4].voxelTopDownViewNodeCount,
          8,
          1,
          0,
          4,
          (__int64)voxelTree[v4].voxelTopDownViewNodeArray,
          (__int64 *)((char *)&unk_148B1C020 + v6));
        sub_140DD3C50(
          16,
          voxelTree[v4].voxelInternalNodeCount,
          8,
          1,
          0,
          4,
          (__int64)voxelTree[v4].voxelInternalNodeArray->firstNodeIndex,
          (__int64 *)((char *)&unk_148B1C220 + v6));
        sub_140DD2810(
          2 * voxelTree[v4].voxelLeafNodeCount,
          voxelTree[v4].voxelLeafNodeCount,
          57,
          1,
          0,
          4,
          (__int64)voxelTree[v4].voxelLeafNodeArray,
          (__int64 *)((char *)&unk_148B1C420 + v6));
        sub_140DD2810(
          2 * voxelTree[v4].lightListArraySize,
          voxelTree[v4].lightListArraySize,
          57,
          1,
          0,
          4,
          (__int64)voxelTree[v4].lightListArray,
          (__int64 *)((char *)&unk_148B1C620 + v6));
        v7 = 2;
        v8 = (__int64 *)((char *)&unk_148B1C820 + 32 * v3);
        do
        {
          sub_140DD2810(
            4 * voxelTree[v4].voxelInternalNodeCount,
            voxelTree[v4].voxelInternalNodeCount,
            42,
            2,
            0x10000,
            4,
            0,
            v8);
          v8 += 2;
          --v7;
        }
        while ( v7 );
        *(_QWORD *)voxelTree[v4].__pad0 = voxelTree[v4].voxelTreeHeader;
        *(_QWORD *)&voxelTree[v4].__pad0[8] = voxelTree[v4].voxelTopDownViewNodeArray;
        *(_QWORD *)&voxelTree[v4].__pad0[16] = voxelTree[v4].voxelInternalNodeArray;
        result = aOan;
      }
      ++v3;
      ++v4;
    }
    while ( v3 < a1->voxelTreeCount );
  }
  return result;
}


/* ===================== sub_1404BE870 @ 1404BE870 ===================== */
__int64 __fastcall sub_1404BE870(int a1, int a2)
{
  sub_140DD2E80(a1 + 16, 0, 16, a2, 12, 0, (__int64)"light grid sampling requests");
  sub_140DD2E80(a1 + 48, 2, 112, 1, 10, 0, (__int64)"lightgrid sample constant buffer");
  return sub_140DD2E80(a1 + 80, 0, 4, a2, 9, 0, (__int64)"cached tet indices");
}


/* ===================== sub_1404BE800 @ 1404BE800 ===================== */
__int64 __fastcall sub_1404BE800(int a1)
{
  qword_1441E77A0[0] = 0;
  qword_1441E77AC = 0;
  dword_1441E77A8 = a1;
  sub_140DD2E80((unsigned int)&qword_1441E77B8, 1, 64, 0x2000, 8, 0, (__int64)"light grid sampling history");
  sub_1404BE870((int)dword_1441E77D8, a1);
  return sub_1404BE870((int)&dword_1441E7848, a1);
}


/* ===================== sub_1404BE7B0 @ 1404BE7B0 ===================== */
__int64 __fastcall sub_1404BE7B0(unsigned int a1)
{
  unsigned __int32 v1; // r8d

  v1 = _InterlockedExchangeAdd(&dword_1441E77D8[28 * LODWORD(qword_1441E77A0[0])], a1);
  if ( v1 + a1 <= (int)qword_1441E77AC + 0x2000 )
    return v1;
  R_WarnOncePerFrame(90);
  return (unsigned int)qword_1441E77AC;
}


/* ===================== sub_1404BE920 @ 1404BE920 ===================== */
unsigned int __fastcall sub_1404BE920(GfxWorld *a1)
{
  unsigned int gpuVisibleProbesCount; // r11d
  unsigned int v2; // ebx
  unsigned int result; // eax
  __int64 v5; // rdx
  __int64 v6; // r8
  GfxStaticModelDrawInst *smodelDrawInsts; // rcx
  __int64 v8; // r9
  unsigned int v9; // eax
  __int64 v10; // r8
  __int64 v11; // rdi
  GfxGpuLightGridProbePosition *gpuVisibleProbePositions; // rcx
  __int64 v13; // rdx
  GfxGpuLightGridProbePosition *v14; // rcx
  __int64 v15; // rdx
  GfxGpuLightGridProbePosition *v16; // rcx
  __int64 v17; // rdx
  GfxGpuLightGridProbePosition *v18; // rcx
  __int64 v19; // rdx
  GfxGpuLightGridProbePosition *v20; // rcx
  __int64 v21; // rdx
  GfxGpuLightGridProbePosition *v22; // rcx
  __int64 v23; // rdx
  GfxGpuLightGridProbePosition *v24; // rcx
  __int64 v25; // rdx
  GfxGpuLightGridProbePosition *v26; // rcx
  __int64 v27; // rdx
  __int64 v28; // r8
  __int64 v29; // r9
  __int64 v30; // r11
  GfxGpuLightGridProbePosition *v31; // rcx
  __int64 v32; // rdx
  GfxGpuLightGridProbePosition *v33; // rcx
  __int64 v34; // rdx
  int v35; // eax
  int v36; // eax

  gpuVisibleProbesCount = a1->lightGrid.probeData.gpuVisibleProbesCount;
  v2 = 0;
  LODWORD(qword_1441E77AC) = gpuVisibleProbesCount;
  dword_1441E77D8[0] = gpuVisibleProbesCount + 65;
  dword_1441E7848 = gpuVisibleProbesCount + 65;
  result = a1->dpvs.smodelCount;
  if ( result )
  {
    v5 = 0;
    v6 = result;
    do
    {
      smodelDrawInsts = a1->dpvs.smodelDrawInsts;
      ++v5;
      *(_DWORD *)smodelDrawInsts[v5 - 1].unk6 = *(_DWORD *)&smodelDrawInsts[v5 - 1].unk0;
      smodelDrawInsts[v5 - 1].unk6[2] = smodelDrawInsts[v5 - 1].unk2;
      result = smodelDrawInsts[v5 - 1].unk3;
      smodelDrawInsts[v5 - 1].unk6[3] = result;
      --v6;
    }
    while ( v6 );
  }
  if ( gpuVisibleProbesCount >= 4 )
  {
    v8 = 0;
    v9 = ((gpuVisibleProbesCount - 4) >> 2) + 1;
    v10 = 0;
    v11 = v9;
    v2 = 4 * v9;
    do
    {
      gpuVisibleProbePositions = a1->lightGrid.probeData.gpuVisibleProbePositions;
      v10 += 64;
      v13 = qword_1441E7800;
      v8 += 4;
      *(float *)(v10 + qword_1441E7800 - 64) = gpuVisibleProbePositions[v8 - 4].origin[0];
      *(_DWORD *)(v10 + v13 - 60) = *((_DWORD *)&gpuVisibleProbePositions[v8 - 3] - 2);
      *(_DWORD *)(v10 + v13 - 56) = *((_DWORD *)&gpuVisibleProbePositions[v8 - 3] - 1);
      *(_DWORD *)(v10 + v13 - 52) = 0xFFFFFF;
      v14 = a1->lightGrid.probeData.gpuVisibleProbePositions;
      v15 = qword_1441E7870;
      *(float *)(v10 + qword_1441E7870 - 64) = v14[v8 - 4].origin[0];
      *(_DWORD *)(v10 + v15 - 60) = *((_DWORD *)&v14[v8 - 3] - 2);
      *(_DWORD *)(v10 + v15 - 56) = *((_DWORD *)&v14[v8 - 3] - 1);
      *(_DWORD *)(v10 + v15 - 52) = 0xFFFFFF;
      v16 = a1->lightGrid.probeData.gpuVisibleProbePositions;
      v17 = qword_1441E7800;
      *(float *)(v10 + qword_1441E7800 - 48) = v16[v8 - 3].origin[0];
      *(_DWORD *)(v10 + v17 - 44) = *((_DWORD *)&v16[v8 - 2] - 2);
      *(_DWORD *)(v10 + v17 - 40) = *((_DWORD *)&v16[v8 - 2] - 1);
      *(_DWORD *)(v10 + v17 - 36) = 0xFFFFFF;
      v18 = a1->lightGrid.probeData.gpuVisibleProbePositions;
      v19 = qword_1441E7870;
      *(float *)(v10 + qword_1441E7870 - 48) = v18[v8 - 3].origin[0];
      *(_DWORD *)(v10 + v19 - 44) = *((_DWORD *)&v18[v8 - 2] - 2);
      *(_DWORD *)(v10 + v19 - 40) = *((_DWORD *)&v18[v8 - 2] - 1);
      *(_DWORD *)(v10 + v19 - 36) = 0xFFFFFF;
      v20 = a1->lightGrid.probeData.gpuVisibleProbePositions;
      v21 = qword_1441E7800;
      *(float *)(v10 + qword_1441E7800 - 32) = v20[v8 - 2].origin[0];
      *(_DWORD *)(v10 + v21 - 28) = *((_DWORD *)&v20[v8 - 1] - 2);
      *(_DWORD *)(v10 + v21 - 24) = *((_DWORD *)&v20[v8 - 1] - 1);
      *(_DWORD *)(v10 + v21 - 20) = 0xFFFFFF;
      v22 = a1->lightGrid.probeData.gpuVisibleProbePositions;
      v23 = qword_1441E7870;
      *(float *)(v10 + qword_1441E7870 - 32) = v22[v8 - 2].origin[0];
      *(_DWORD *)(v10 + v23 - 28) = *((_DWORD *)&v22[v8 - 1] - 2);
      *(_DWORD *)(v10 + v23 - 24) = *((_DWORD *)&v22[v8 - 1] - 1);
      *(_DWORD *)(v10 + v23 - 20) = 0xFFFFFF;
      v24 = a1->lightGrid.probeData.gpuVisibleProbePositions;
      v25 = qword_1441E7800;
      *(float *)(v10 + qword_1441E7800 - 16) = v24[v8 - 1].origin[0];
      *(float *)(v10 + v25 - 12) = v24[v8 - 1].origin[1];
      *(float *)(v10 + v25 - 8) = v24[v8 - 1].origin[2];
      *(_DWORD *)(v10 + v25 - 4) = 0xFFFFFF;
      v26 = a1->lightGrid.probeData.gpuVisibleProbePositions;
      v27 = qword_1441E7870;
      *(float *)(v10 + qword_1441E7870 - 16) = v26[v8 - 1].origin[0];
      *(float *)(v10 + v27 - 12) = v26[v8 - 1].origin[1];
      result = LODWORD(v26[v8 - 1].origin[2]);
      *(_DWORD *)(v10 + v27 - 8) = result;
      *(_DWORD *)(v10 + v27 - 4) = 0xFFFFFF;
      --v11;
    }
    while ( v11 );
  }
  if ( v2 < gpuVisibleProbesCount )
  {
    v28 = 16LL * v2;
    v29 = v2;
    v30 = gpuVisibleProbesCount - v2;
    do
    {
      v31 = a1->lightGrid.probeData.gpuVisibleProbePositions;
      v28 += 16;
      v32 = qword_1441E7800;
      *(float *)(v28 + qword_1441E7800 - 16) = v31[v29++].origin[0];
      *(float *)(v28 + v32 - 12) = v31[v29 - 1].origin[1];
      *(float *)(v28 + v32 - 8) = v31[v29 - 1].origin[2];
      *(_DWORD *)(v28 + v32 - 4) = 0xFFFFFF;
      v33 = a1->lightGrid.probeData.gpuVisibleProbePositions;
      v34 = qword_1441E7870;
      *(float *)(v28 + qword_1441E7870 - 16) = v33[v29 - 1].origin[0];
      *(float *)(v28 + v34 - 12) = v33[v29 - 1].origin[1];
      result = LODWORD(v33[v29 - 1].origin[2]);
      *(_DWORD *)(v28 + v34 - 8) = result;
      *(_DWORD *)(v28 + v34 - 4) = 0xFFFFFF;
      --v30;
    }
    while ( v30 );
  }
  if ( (_DWORD)qword_1441E77AC )
  {
    WaitForSingleObject(hMutex, 0xFFFFFFFF);
    v35 = dword_148B13898;
    if ( !dword_148B13898 )
    {
      dword_148B1389C = GetCurrentThreadId();
      v35 = dword_148B13898;
    }
    dword_148B13898 = v35 + 1;
    sub_1400C1300((_DWORD)ppImmediateContext, qword_1441E77E8, 0, 16 * qword_1441E77AC, qword_1441E7800);
    sub_1400C1300((_DWORD)ppImmediateContext, qword_1441E7858, 0, 16 * qword_1441E77AC, qword_1441E7870);
    v36 = dword_148B1389C;
    if ( !--dword_148B13898 )
      v36 = -1;
    dword_148B1389C = v36;
    return ReleaseMutex(hMutex);
  }
  return result;
}


/* ===================== sub_140E058B0 @ 140E058B0 ===================== */
signed __int64 sub_140E058B0()
{
  R_CreateBufferInternal(0x2000u, 2u, 0x10000, 1, 0, &qword_148B16D48);
  R_CreateBufferInternal(0x800u, 2u, 0x10000, 1, 0, &qword_148B16D50);
  R_CreateBufferInternal(0x800u, 2u, 0x10000, 1, 0, &qword_148B16D58);
  return R_CreateBufferInternal(0x800u, 2u, 0x10000, 1, 0, &qword_148B16D60);
}


/* ===================== sub_140E3C1F0 @ 140E3C1F0 ===================== */
void __fastcall sub_140E3C1F0(__int64 a1, float *a2)
{
  int voxelTreeCount; // ecx
  __int64 v5; // rdx
  int v6; // ebx
  GfxVoxelTree *voxelTree; // rax

  if ( *(_DWORD *)(a1 + 9820) && (!g_world || g_world->voxelTreeCount) )
  {
    voxelTreeCount = g_world->voxelTreeCount;
    v5 = 0;
    v6 = 32;
    if ( voxelTreeCount <= 0 )
      goto LABEL_16;
    voxelTree = g_world->voxelTree;
    while ( 1 )
    {
      if ( voxelTree->voxelTopDownViewNodeCount )
      {
        if ( v6 == 32 )
          v6 = v5;
        if ( fabs(a2[64] - voxelTree->zoneBound.midPoint[0]) < voxelTree->zoneBound.halfSize[0]
          && fabs(a2[65] - voxelTree->zoneBound.midPoint[1]) < voxelTree->zoneBound.halfSize[1]
          && fabs(a2[66] - voxelTree->zoneBound.midPoint[2]) < voxelTree->zoneBound.halfSize[2] )
        {
          break;
        }
      }
      v5 = (unsigned int)(v5 + 1);
      ++voxelTree;
      if ( (int)v5 >= voxelTreeCount )
        goto LABEL_15;
    }
    v6 = v5;
LABEL_15:
    if ( v6 == 32 )
LABEL_16:
      Sys_Error("No valid voxel trees.  Are there empty skyboxes in your map?", v5, a2);
    *(_DWORD *)(a1 + 7636) = v6;
  }
}


/* ===================== sub_1400BC7A0 @ 1400BC7A0 ===================== */
__int64 sub_1400BC7A0()
{
  _QWORD v1[3]; // [rsp+40h] [rbp-18h] BYREF

  v1[0] = &String;
  v1[1] = "ShaderUpload_InitializeBuffers";
  sub_140DD2E80((unsigned int)&unk_14B034300, 1, 294912, 1, 4, 0, (__int64)"ShaderUpload_RawBuffer");
  sub_140E20080(&unk_14B034320, 0);
  sub_1405F06B0(1, 32);
  sub_1405F0820(qword_14B034340, 0x20u, (__int64)&qword_14B034348);
  R_CreateComputeRawBuffer(64, 4, 0, &qword_14B034350, (__int64)v1);
  R_CreateComputeRawBuffer(64, 4, 0, &qword_14B034358, (__int64)v1);
  R_CreateComputeRawBufferView(qword_14B034350, (__int64)&qword_14B034360);
  R_CreateComputeRawBufferView(qword_14B034358, (__int64)&qword_14B034368);
  return sub_140DD2E80((unsigned int)&unk_14B034370, 0, 16, 1, 12, 0, (__int64)"light grid sampling requests");
}


/* ===================== sub_140A2D780 @ 140A2D780 ===================== */
void __fastcall sub_140A2D780(char a1)
{
  GfxLightGridProbeData *v1; // rdi
  __int64 v2; // rbx
  __int64 v3; // rdx
  __int64 v4; // rbx
  unsigned __int64 v5; // rdx
  __int64 v6; // rbx
  __int64 v7; // rbx
  __int64 v8; // rbx
  GfxLightGridProbeData *v9; // rdi
  __int64 v10; // rbx
  unsigned __int64 v11; // rcx
  __int64 v12; // rbx
  __int64 v13; // rbx
  __int64 v14; // rbx
  GfxLightGridProbeData *v15; // rdi
  __int64 v16; // rbx
  __int64 v17; // rdx
  __int64 v18; // rbx
  __int64 v19; // rbx
  GfxLightGridProbeData *v20; // rdi
  __int64 v21; // rbx
  __int64 v22; // rdx
  __int64 v23; // rbx
  __int64 v24; // rcx
  __int64 v25; // rbx
  __int64 v26; // rbx
  GfxLightGridProbeData *v27; // rdi
  __int64 v28; // rbx
  __int64 v29; // rcx
  __int64 v30; // rbx
  __int64 v31; // rbx
  GfxLightGridProbeData *v32; // rdi
  __int64 v33; // rbx
  unsigned __int64 v34; // rcx
  __int64 v35; // rbx
  __int64 v36; // rbx
  GfxLightGridProbeData *v37; // rdi
  __int64 v38; // rbx
  __int64 v39; // rcx
  __int64 v40; // rbx
  __int64 v41; // rbx

  if ( a1 )
    g_streamPos = (char *)g_streamPos + 240;
  v1 = varGfxLightGridProbeData;
  if ( varGfxLightGridProbeData->gpuVisibleProbePositions )
  {
    v2 = qword_1453E2FD0;
    DB_PatchMem_FixStreamAlignment(3);
    v1->gpuVisibleProbePositions = (GfxGpuLightGridProbePosition *)g_streamPos;
    v1 = varGfxLightGridProbeData;
    qword_1453E2FD0 = (__int64)g_streamPos;
    v3 = 12LL * varGfxLightGridProbeData->gpuVisibleProbesCount;
    if ( v3 )
      g_streamPos = (char *)g_streamPos + v3;
    qword_1453E2FD0 = v2;
  }
  if ( v1->gpuVisibleProbesData )
  {
    v4 = qword_1453E4790;
    DB_PatchMem_FixStreamAlignment(63);
    v1->gpuVisibleProbesData = (GfxProbeData *)g_streamPos;
    v1 = varGfxLightGridProbeData;
    qword_1453E4790 = (__int64)g_streamPos;
    v5 = (unsigned __int64)(varGfxLightGridProbeData->gpuVisibleProbesCount + 0x2000) << 6;
    if ( v5 )
      g_streamPos = (char *)g_streamPos + v5;
    qword_1453E4790 = v4;
  }
  v6 = varGfxBuffer;
  varGfxBuffer = (__int64)&v1->gpuVisibleProbesBuffer;
  sub_140DD2010(
    (bdVoteRank *)&v1->gpuVisibleProbesBuffer,
    (__int64)v1->gpuVisibleProbesData,
    (v1->gpuVisibleProbesCount + 0x2000) << 6,
    (__int64)"gpuVisibleProbes");
  varGfxBuffer = v6;
  v7 = varGfxShaderView;
  varGfxShaderView = (__int64)&varGfxLightGridProbeData->gpuVisibleProbesView;
  Load_RawDataBufferView(
    (__int64)&varGfxLightGridProbeData->gpuVisibleProbesView,
    (__int64)varGfxLightGridProbeData->gpuVisibleProbesBuffer,
    (__int64)varGfxLightGridProbeData->gpuVisibleProbesData,
    (varGfxLightGridProbeData->gpuVisibleProbesCount + 0x2000) << 6);
  varGfxShaderView = v7;
  v8 = qword_1453E30D0;
  qword_1453E30D0 = (__int64)&varGfxLightGridProbeData->gpuVisibleProbesRWView;
  Load_RawDataBufferRWView(
    (bdVoteRank *)&varGfxLightGridProbeData->gpuVisibleProbesRWView,
    (__int64)varGfxLightGridProbeData->gpuVisibleProbesBuffer,
    (__int64)varGfxLightGridProbeData->gpuVisibleProbesData,
    (varGfxLightGridProbeData->gpuVisibleProbesCount + 0x2000) << 6);
  v9 = varGfxLightGridProbeData;
  qword_1453E30D0 = v8;
  if ( varGfxLightGridProbeData->probes )
  {
    v10 = qword_1453E4790;
    DB_PatchMem_FixStreamAlignment(63);
    v9->probes = (GfxProbeData *)g_streamPos;
    v9 = varGfxLightGridProbeData;
    qword_1453E4790 = (__int64)g_streamPos;
    v11 = (unsigned __int64)varGfxLightGridProbeData->probeCount << 6;
    if ( v11 )
      g_streamPos = (char *)g_streamPos + v11;
    qword_1453E4790 = v10;
  }
  v12 = varGfxBuffer;
  varGfxBuffer = (__int64)&v9->probesBuffer;
  Load_RawDataBuffer((__int64)&v9->probesBuffer, (__int64)v9->probes, v9->probeCount << 6, (__int64)"probesData");
  varGfxBuffer = v12;
  v13 = varGfxShaderView;
  varGfxShaderView = (__int64)&varGfxLightGridProbeData->probesView;
  Load_RawDataBufferView(
    (__int64)&varGfxLightGridProbeData->probesView,
    (__int64)varGfxLightGridProbeData->probesBuffer,
    (__int64)varGfxLightGridProbeData->probes,
    varGfxLightGridProbeData->probeCount << 6);
  varGfxShaderView = v13;
  v14 = qword_1453E30D0;
  qword_1453E30D0 = (__int64)&varGfxLightGridProbeData->probesRWView;
  Load_RawDataBufferRWView(
    (bdVoteRank *)&varGfxLightGridProbeData->probesRWView,
    (__int64)varGfxLightGridProbeData->probesBuffer,
    0,
    0);
  v15 = varGfxLightGridProbeData;
  qword_1453E30D0 = v14;
  if ( varGfxLightGridProbeData->probePositions )
  {
    v16 = qword_1453E2FD0;
    DB_PatchMem_FixStreamAlignment(3);
    v15->probePositions = (GfxGpuLightGridProbePosition *)g_streamPos;
    v15 = varGfxLightGridProbeData;
    qword_1453E2FD0 = (__int64)g_streamPos;
    v17 = 12LL * varGfxLightGridProbeData->probeCount;
    if ( v17 )
      g_streamPos = (char *)g_streamPos + v17;
    qword_1453E2FD0 = v16;
  }
  v18 = varGfxBuffer;
  varGfxBuffer = (__int64)&v15->probePositionsBuffer;
  Load_RawDataBuffer(
    (__int64)&v15->probePositionsBuffer,
    (__int64)v15->probePositions,
    12 * v15->probeCount,
    (__int64)"probesPositions");
  varGfxBuffer = v18;
  v19 = varGfxShaderView;
  varGfxShaderView = (__int64)&varGfxLightGridProbeData->probePositionsView;
  Load_RawDataBufferView(
    (__int64)&varGfxLightGridProbeData->probePositionsView,
    (__int64)varGfxLightGridProbeData->probePositionsBuffer,
    (__int64)varGfxLightGridProbeData->probePositions,
    12 * varGfxLightGridProbeData->probeCount);
  v20 = varGfxLightGridProbeData;
  varGfxShaderView = v19;
  if ( varGfxLightGridProbeData->zones )
  {
    v21 = qword_1453E47B0;
    DB_PatchMem_FixStreamAlignment(63);
    v20->zones = (GfxGpuLightGridZone *)g_streamPos;
    v20 = varGfxLightGridProbeData;
    qword_1453E47B0 = (__int64)g_streamPos;
    v22 = 88LL * varGfxLightGridProbeData->zoneCount;
    if ( v22 )
      g_streamPos = (char *)g_streamPos + v22;
    qword_1453E47B0 = v21;
  }
  if ( v20->tetrahedrons )
  {
    v23 = qword_1453E47D0;
    DB_PatchMem_FixStreamAlignment(63);
    v20->tetrahedrons = (GfxGpuLightGridTetrahedron *)g_streamPos;
    v20 = varGfxLightGridProbeData;
    qword_1453E47D0 = (__int64)g_streamPos;
    v24 = 16LL * varGfxLightGridProbeData->tetrahedronCount;
    if ( v24 )
      g_streamPos = (char *)g_streamPos + v24;
    qword_1453E47D0 = v23;
  }
  v25 = varGfxBuffer;
  varGfxBuffer = (__int64)&v20->tetrahedronBuffer;
  Load_RawDataBuffer(
    (__int64)&v20->tetrahedronBuffer,
    (__int64)v20->tetrahedrons,
    16 * v20->tetrahedronCount,
    (__int64)"probeTets");
  varGfxBuffer = v25;
  v26 = varGfxShaderView;
  varGfxShaderView = (__int64)&varGfxLightGridProbeData->tetrahedronView;
  Load_RawDataBufferView(
    (__int64)&varGfxLightGridProbeData->tetrahedronView,
    (__int64)varGfxLightGridProbeData->tetrahedronBuffer,
    (__int64)varGfxLightGridProbeData->tetrahedrons,
    16 * varGfxLightGridProbeData->tetrahedronCount);
  v27 = varGfxLightGridProbeData;
  varGfxShaderView = v26;
  if ( varGfxLightGridProbeData->tetrahedronNeighbors )
  {
    v28 = qword_1453E47F8;
    DB_PatchMem_FixStreamAlignment(63);
    v27->tetrahedronNeighbors = (GfxGpuLightGridTetrahedronNeighbors *)g_streamPos;
    v27 = varGfxLightGridProbeData;
    qword_1453E47F8 = (__int64)g_streamPos;
    v29 = 16LL * varGfxLightGridProbeData->tetrahedronCount;
    if ( v29 )
      g_streamPos = (char *)g_streamPos + v29;
    qword_1453E47F8 = v28;
  }
  v30 = varGfxBuffer;
  varGfxBuffer = (__int64)&v27->tetrahedronNeighborsBuffer;
  Load_RawDataBuffer(
    (__int64)&v27->tetrahedronNeighborsBuffer,
    (__int64)v27->tetrahedronNeighbors,
    16 * v27->tetrahedronCount,
    (__int64)"probeTetNeighbors");
  varGfxBuffer = v30;
  v31 = varGfxShaderView;
  varGfxShaderView = (__int64)&varGfxLightGridProbeData->tetrahedronNeighborsView;
  Load_RawDataBufferView(
    (__int64)&varGfxLightGridProbeData->tetrahedronNeighborsView,
    (__int64)varGfxLightGridProbeData->tetrahedronNeighborsBuffer,
    (__int64)varGfxLightGridProbeData->tetrahedronNeighbors,
    16 * varGfxLightGridProbeData->tetrahedronCount);
  v32 = varGfxLightGridProbeData;
  varGfxShaderView = v31;
  if ( varGfxLightGridProbeData->tetrahedronVisibility )
  {
    v33 = qword_1453E4810;
    DB_PatchMem_FixStreamAlignment(63);
    v32->tetrahedronVisibility = (GfxGpuLightGridTetrahedronVisibility *)g_streamPos;
    v32 = varGfxLightGridProbeData;
    qword_1453E4810 = (__int64)g_streamPos;
    v34 = (unsigned __int64)varGfxLightGridProbeData->tetrahedronCountVisible << 6;
    if ( v34 )
      g_streamPos = (char *)g_streamPos + v34;
    qword_1453E4810 = v33;
  }
  v35 = varGfxBuffer;
  varGfxBuffer = (__int64)&v32->tetrahedronVisibilityBuffer;
  Load_RawDataBuffer(
    (__int64)&v32->tetrahedronVisibilityBuffer,
    (__int64)v32->tetrahedronVisibility,
    v32->tetrahedronCountVisible << 6,
    (__int64)"probeTetVisibility");
  varGfxBuffer = v35;
  v36 = varGfxShaderView;
  varGfxShaderView = (__int64)&varGfxLightGridProbeData->tetrahedronVisibilityView;
  Load_RawDataBufferView(
    (__int64)&varGfxLightGridProbeData->tetrahedronVisibilityView,
    (__int64)varGfxLightGridProbeData->tetrahedronVisibilityBuffer,
    (__int64)varGfxLightGridProbeData->tetrahedronVisibility,
    varGfxLightGridProbeData->tetrahedronCountVisible << 6);
  v37 = varGfxLightGridProbeData;
  varGfxShaderView = v36;
  if ( varGfxLightGridProbeData->voxelStartTetrahedron )
  {
    v38 = qword_1453E4830;
    DB_PatchMem_FixStreamAlignment(63);
    v37->voxelStartTetrahedron = (GfxGpuLightGridVoxelStartTetrahedron *)g_streamPos;
    v37 = varGfxLightGridProbeData;
    qword_1453E4830 = (__int64)g_streamPos;
    v39 = 4LL * varGfxLightGridProbeData->voxelStartTetrahedronCount;
    if ( v39 )
      g_streamPos = (char *)g_streamPos + v39;
    qword_1453E4830 = v38;
  }
  v40 = varGfxBuffer;
  varGfxBuffer = (__int64)&v37->voxelStartTetrahedronBuffer;
  Load_RawDataBuffer(
    (__int64)&v37->voxelStartTetrahedronBuffer,
    (__int64)v37->voxelStartTetrahedron,
    4 * v37->voxelStartTetrahedronCount,
    (__int64)"probeVoxelStartTet");
  varGfxBuffer = v40;
  v41 = varGfxShaderView;
  varGfxShaderView = (__int64)&varGfxLightGridProbeData->voxelStartTetrahedronView;
  Load_RawDataBufferView(
    (__int64)&varGfxLightGridProbeData->voxelStartTetrahedronView,
    (__int64)varGfxLightGridProbeData->voxelStartTetrahedronBuffer,
    (__int64)varGfxLightGridProbeData->voxelStartTetrahedron,
    4 * varGfxLightGridProbeData->voxelStartTetrahedronCount);
  varGfxShaderView = v41;
}

