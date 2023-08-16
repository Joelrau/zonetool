#pragma once

#include <cassert>

#define _BYTE  uint8_t
#define BYTEn(x, n)   (*((_BYTE*)&(x)+n))
#define BYTE1(x)   BYTEn(x,  1)         // byte 1 (counting from 0)
#define BYTE2(x)   BYTEn(x,  2)

namespace
{
	static const unsigned int crcLut[256] =
	{
	0x0,
	0xF26B8303,
	0xE13B70F7,
	0x1350F3F4,
	0xC79A971F,
	0x35F1141C,
	0x26A1E7E8,
	0xD4CA64EB,
	0x8AD958CF,
	0x78B2DBCC,
	0x6BE22838,
	0x9989AB3B,
	0x4D43CFD0,
	0xBF284CD3,
	0xAC78BF27,
	0x5E133C24,
	0x105EC76F,
	0xE235446C,
	0xF165B798,
	0x30E349B,
	0xD7C45070,
	0x25AFD373,
	0x36FF2087,
	0xC494A384,
	0x9A879FA0,
	0x68EC1CA3,
	0x7BBCEF57,
	0x89D76C54,
	0x5D1D08BF,
	0xAF768BBC,
	0xBC267848,
	0x4E4DFB4B,
	0x20BD8EDE,
	0xD2D60DDD,
	0xC186FE29,
	0x33ED7D2A,
	0xE72719C1,
	0x154C9AC2,
	0x61C6936,
	0xF477EA35,
	0xAA64D611,
	0x580F5512,
	0x4B5FA6E6,
	0xB93425E5,
	0x6DFE410E,
	0x9F95C20D,
	0x8CC531F9,
	0x7EAEB2FA,
	0x30E349B1,
	0xC288CAB2,
	0xD1D83946,
	0x23B3BA45,
	0xF779DEAE,
	0x5125DAD,
	0x1642AE59,
	0xE4292D5A,
	0xBA3A117E,
	0x4851927D,
	0x5B016189,
	0xA96AE28A,
	0x7DA08661,
	0x8FCB0562,
	0x9C9BF696,
	0x6EF07595,
	0x417B1DBC,
	0xB3109EBF,
	0xA0406D4B,
	0x522BEE48,
	0x86E18AA3,
	0x748A09A0,
	0x67DAFA54,
	0x95B17957,
	0xCBA24573,
	0x39C9C670,
	0x2A993584,
	0xD8F2B687,
	0xC38D26C,
	0xFE53516F,
	0xED03A29B,
	0x1F682198,
	0x5125DAD3,
	0xA34E59D0,
	0xB01EAA24,
	0x42752927,
	0x96BF4DCC,
	0x64D4CECF,
	0x77843D3B,
	0x85EFBE38,
	0xDBFC821C,
	0x2997011F,
	0x3AC7F2EB,
	0xC8AC71E8,
	0x1C661503,
	0xEE0D9600,
	0xFD5D65F4,
	0xF36E6F7,
	0x61C69362,
	0x93AD1061,
	0x80FDE395,
	0x72966096,
	0xA65C047D,
	0x5437877E,
	0x4767748A,
	0xB50CF789,
	0xEB1FCBAD,
	0x197448AE,
	0xA24BB5A,
	0xF84F3859,
	0x2C855CB2,
	0xDEEEDFB1,
	0xCDBE2C45,
	0x3FD5AF46,
	0x7198540D,
	0x83F3D70E,
	0x90A324FA,
	0x62C8A7F9,
	0xB602C312,
	0x44694011,
	0x5739B3E5,
	0xA55230E6,
	0xFB410CC2,
	0x92A8FC1,
	0x1A7A7C35,
	0xE811FF36,
	0x3CDB9BDD,
	0xCEB018DE,
	0xDDE0EB2A,
	0x2F8B6829,
	0x82F63B78,
	0x709DB87B,
	0x63CD4B8F,
	0x91A6C88C,
	0x456CAC67,
	0xB7072F64,
	0xA457DC90,
	0x563C5F93,
	0x82F63B7,
	0xFA44E0B4,
	0xE9141340,
	0x1B7F9043,
	0xCFB5F4A8,
	0x3DDE77AB,
	0x2E8E845F,
	0xDCE5075C,
	0x92A8FC17,
	0x60C37F14,
	0x73938CE0,
	0x81F80FE3,
	0x55326B08,
	0xA759E80B,
	0xB4091BFF,
	0x466298FC,
	0x1871A4D8,
	0xEA1A27DB,
	0xF94AD42F,
	0xB21572C,
	0xDFEB33C7,
	0x2D80B0C4,
	0x3ED04330,
	0xCCBBC033,
	0xA24BB5A6,
	0x502036A5,
	0x4370C551,
	0xB11B4652,
	0x65D122B9,
	0x97BAA1BA,
	0x84EA524E,
	0x7681D14D,
	0x2892ED69,
	0xDAF96E6A,
	0xC9A99D9E,
	0x3BC21E9D,
	0xEF087A76,
	0x1D63F975,
	0xE330A81,
	0xFC588982,
	0xB21572C9,
	0x407EF1CA,
	0x532E023E,
	0xA145813D,
	0x758FE5D6,
	0x87E466D5,
	0x94B49521,
	0x66DF1622,
	0x38CC2A06,
	0xCAA7A905,
	0xD9F75AF1,
	0x2B9CD9F2,
	0xFF56BD19,
	0xD3D3E1A,
	0x1E6DCDEE,
	0xEC064EED,
	0xC38D26C4,
	0x31E6A5C7,
	0x22B65633,
	0xD0DDD530,
	0x417B1DB,
	0xF67C32D8,
	0xE52CC12C,
	0x1747422F,
	0x49547E0B,
	0xBB3FFD08,
	0xA86F0EFC,
	0x5A048DFF,
	0x8ECEE914,
	0x7CA56A17,
	0x6FF599E3,
	0x9D9E1AE0,
	0xD3D3E1AB,
	0x21B862A8,
	0x32E8915C,
	0xC083125F,
	0x144976B4,
	0xE622F5B7,
	0xF5720643,
	0x7198540,
	0x590AB964,
	0xAB613A67,
	0xB831C993,
	0x4A5A4A90,
	0x9E902E7B,
	0x6CFBAD78,
	0x7FAB5E8C,
	0x8DC0DD8F,
	0xE330A81A,
	0x115B2B19,
	0x20BD8ED,
	0xF0605BEE,
	0x24AA3F05,
	0xD6C1BC06,
	0xC5914FF2,
	0x37FACCF1,
	0x69E9F0D5,
	0x9B8273D6,
	0x88D28022,
	0x7AB90321,
	0xAE7367CA,
	0x5C18E4C9,
	0x4F48173D,
	0xBD23943E,
	0xF36E6F75,
	0x105EC76,
	0x12551F82,
	0xE03E9C81,
	0x34F4F86A,
	0xC69F7B69,
	0xD5CF889D,
	0x27A40B9E,
	0x79B737BA,
	0x8BDCB4B9,
	0x988C474D,
	0x6AE7C44E,
	0xBE2DA0A5,
	0x4C4623A6,
	0x5F16D052,
	0xAD7D5351,
	};
}

class Umbra
{
public:

	struct Vector3;

	struct DataPtr;

	struct SerializedTreeData;

	struct Vector3
	{
		float x;
		float y;
		float z;
	};

	struct DataPtr
	{
		unsigned int m_offset;
	};

	struct SerializedTreeData
	{
		unsigned int m_nodeCount_mapWidth;
		DataPtr m_treeData;
		DataPtr m_map;
		unsigned int m_numSplitValues;
		DataPtr m_splitValues;
	};

	class Tome
	{
	public:

		enum Status
		{
			STATUS_OK = 0x0,
			STATUS_UNINITIALIZED = 0x1,
			STATUS_CORRUPT = 0x2,
			STATUS_OLDER_VERSION = 0x3,
			STATUS_NEWER_VERSION = 0x4,
			STATUS_BAD_ENDIAN = 0x5,
			STATUS_BAD_ALIGN = 0x6,
			STATUS_OUT_OF_MEMORY = 0x7,
		};
	};

	class ImpTome
	{
	public:
		unsigned int m_versionMagic;
		unsigned int m_crc32;
		unsigned int m_size;
		float m_lodBaseDistance;
		unsigned int m_flags;
		Vector3 m_treeMin;
		Vector3 m_treeMax;
		SerializedTreeData m_tileTree;
		int m_numObjects;
		DataPtr m_objBounds;
		DataPtr m_objDistances;
		DataPtr m_userIDStarts;
		DataPtr m_userIDs;
		unsigned int m_listWidths;
		DataPtr m_objectLists;
		int m_objectListSize;
		DataPtr m_clusterLists;
		int m_clusterListSize;
		int m_numGates;
		DataPtr m_gateIndexMap;
		DataPtr m_gateVertices;
		int m_numGateVertices;
		DataPtr m_gateIndices;
		int m_numClusters;
		DataPtr m_clusters;
		DataPtr m_clusterPortals;
		DataPtr m_cellStarts;
		int m_numLeafTiles;
		int m_numTiles;
		int m_bitsPerSlotPath;
		DataPtr m_slotPaths;
		DataPtr m_tileLodLevels;
		DataPtr m_tiles;
		DataPtr m_tileMatchingData;
		DataPtr m_matchingTrees;
		int m_numMatchingTrees;
		int m_numTomes;
		DataPtr m_tomeClusterStarts;
		DataPtr m_tomeClusterPortalStarts;
		char m_computationString[128];
		DataPtr m_objectDepthmaps;
		DataPtr m_depthmapFaces;
		DataPtr m_depthmapPalettes;
		int m_numFaces;
		DataPtr m_tilePortalExpands;

		enum Flags : unsigned int
		{
			TOMEFLAG_DEPTHMAPS = 0x1,
			TOMEFLAG_SHADOW_DEPTHMAPS = 0x2,
			TOMEFLAG_NO_OUTPUTBOUNDS = 0x4,
		};

		static uint32_t crc32Hash(const uint32_t* ptr, uint32_t length)
		{
			unsigned int remainingLength = length;
			unsigned int intermediateChecksum = -1; // Initialized to 0xFFFFFFFF

			if ((length & 3) != 0)
			{
				_wassert(_CRT_WIDE("!(length & 3)"), _CRT_WIDE(__FILE__), __LINE__);
			}

			if (remainingLength)
			{
				do
				{
					if (remainingLength < 4)
					{
						_wassert(_CRT_WIDE("length >= sizeof(uint32_t)"), _CRT_WIDE(__FILE__), __LINE__);
					}

					unsigned int v5 = *ptr >> 8;
					unsigned int v6 = (unsigned __int8)(intermediateChecksum ^ *ptr++);
					unsigned int v7 = crcLut[(unsigned __int8)(v5 ^ BYTE1(intermediateChecksum) ^ LOBYTE(crcLut[v6]))] ^ (((intermediateChecksum >> 8) ^ crcLut[v6]) >> 8);
					intermediateChecksum = (((v7 >> 8) ^ crcLut[(unsigned __int8)(v7 ^ BYTE1(v5))]) >> 8) ^ crcLut[(unsigned __int8)(BYTE1(v7) ^ LOBYTE(crcLut[(unsigned __int8)(v7 ^ BYTE1(v5))]) ^ ((unsigned __int16)(v5 >> 8) >> 8))];
					remainingLength -= 4i64;
				} while (remainingLength);
			}

			return ~intermediateChecksum; // Returns the bitwise NOT of the intermediate checksum
		}

		static uint32_t computeCRC32(Umbra::ImpTome* tome)
		{
			return crc32Hash(&tome->m_size, tome->m_size - 8);
		}
	};

	static_assert(sizeof(ImpTome) == 336);
	static_assert(offsetof(ImpTome, m_versionMagic) == 0);
};