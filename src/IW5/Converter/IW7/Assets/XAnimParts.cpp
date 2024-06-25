#include "stdafx.hpp"
#include "../Include.hpp"

#include "XAnimParts.hpp"

namespace ZoneTool::IW5
{
	namespace IW7Converter
	{
		IW7::XAnimParts* GenerateIW7XAnimParts(XAnimParts* asset, allocator& mem)
		{
			// allocate IW7 XAnimParts structure
			const auto IW7_asset = mem.allocate<IW7::XAnimParts>();

			IW7_asset->name = asset->name;
			IW7_asset->dataByteCount = asset->dataByteCount;
			IW7_asset->dataShortCount = asset->dataShortCount;
			IW7_asset->dataIntCount = asset->dataIntCount;
			IW7_asset->randomDataByteCount = asset->randomDataByteCount;
			IW7_asset->randomDataIntCount = asset->randomDataIntCount;
			IW7_asset->numframes = asset->numframes;
			IW7_asset->flags = asset->flags;
			memcpy(&IW7_asset->boneCount, &asset->boneCount, sizeof(asset->boneCount));
			IW7_asset->notifyCount = asset->notifyCount;
			IW7_asset->assetType = asset->assetType;
			//IW7_asset->ikType = asset->ikType;
			IW7_asset->randomDataShortCount = asset->randomDataShortCount;
			IW7_asset->indexCount = asset->indexCount;
			IW7_asset->framerate = asset->framerate;
			IW7_asset->frequency = asset->frequency;
			IW7_asset->names = mem.allocate<IW7::scr_string_t>(IW7_asset->boneCount[9]);
			for (auto i = 0; i < asset->boneCount[9]; i++)
			{
				IW7_asset->names[i] = static_cast<IW7::scr_string_t>(asset->names[i]);
			}
			IW7_asset->dataByte = reinterpret_cast<unsigned char* __ptr64>(asset->dataByte);
			IW7_asset->dataShort = reinterpret_cast<short* __ptr64>(asset->dataShort);
			IW7_asset->dataInt = reinterpret_cast<int* __ptr64>(asset->dataInt);
			IW7_asset->randomDataShort = reinterpret_cast<short* __ptr64>(asset->randomDataShort);
			IW7_asset->randomDataByte = reinterpret_cast<unsigned char* __ptr64>(asset->randomDataByte);
			IW7_asset->randomDataInt = reinterpret_cast<int* __ptr64>(asset->randomDataInt);

			IW7_asset->indices.data = reinterpret_cast<void*>(asset->indices.data);

			IW7_asset->notify = mem.allocate<IW7::XAnimNotifyInfo>(IW7_asset->notifyCount);
			for (auto i = 0; i < asset->notifyCount; i++)
			{
				IW7_asset->notify[i].name = static_cast<IW7::scr_string_t>(asset->notify[i].name);
				IW7_asset->notify[i].time = asset->notify[i].time;
			}

			if (asset->deltaPart)
			{
				IW7_asset->deltaPart = mem.allocate<IW7::XAnimDeltaPart>();
				if (asset->deltaPart->trans)
				{
					auto extra_size = 0;

					if (asset->deltaPart->trans->size)
					{
						if (asset->numframes >= 256)
						{
							extra_size += (asset->deltaPart->trans->size * 2) + 2;
						}
						else
						{
							extra_size += asset->deltaPart->trans->size + 1;
						}
					}

					IW7_asset->deltaPart->trans = mem.manual_allocate<IW7::XAnimPartTrans>(sizeof(IW7::XAnimPartTrans) + extra_size);

					IW7_asset->deltaPart->trans->size = asset->deltaPart->trans->size;
					IW7_asset->deltaPart->trans->smallTrans = static_cast<unsigned short>(asset->deltaPart->trans->smallTrans);

					if (asset->deltaPart->trans->size)
					{
						for (auto i = 0; i < 3; i++)
						{
							IW7_asset->deltaPart->trans->u.frames.mins[i] = asset->deltaPart->trans->u.frames.mins[i];
							IW7_asset->deltaPart->trans->u.frames.size[i] = asset->deltaPart->trans->u.frames.size[i];
						}
						if (asset->numframes >= 256)
						{
							for (auto i = 0; i < asset->deltaPart->trans->size + 1; i++)
							{
								IW7_asset->deltaPart->trans->u.frames.indices._2[i] = asset->deltaPart->trans->u.frames.indices._2[i];
							}
						}
						else
						{
							for (auto i = 0; i < asset->deltaPart->trans->size + 1; i++)
							{
								IW7_asset->deltaPart->trans->u.frames.indices._1[i] = static_cast<unsigned char>(asset->deltaPart->trans->u.frames.indices._1[i]);
							}
						}
						if (asset->deltaPart->trans->u.frames.frames._1)
						{
							if (asset->deltaPart->trans->smallTrans)
							{
								IW7_asset->deltaPart->trans->u.frames.frames._1 = mem.allocate<unsigned char[3]>(asset->deltaPart->trans->size + 1);
								for (auto i = 0; i < asset->deltaPart->trans->size + 1; i++)
								{
									IW7_asset->deltaPart->trans->u.frames.frames._1[i][0] = static_cast<unsigned char>(asset->deltaPart->trans->u.frames.frames._1[i][0]);
									IW7_asset->deltaPart->trans->u.frames.frames._1[i][1] = static_cast<unsigned char>(asset->deltaPart->trans->u.frames.frames._1[i][1]);
									IW7_asset->deltaPart->trans->u.frames.frames._1[i][2] = static_cast<unsigned char>(asset->deltaPart->trans->u.frames.frames._1[i][2]);
								}
							}
							else
							{
								IW7_asset->deltaPart->trans->u.frames.frames._2 = mem.allocate<unsigned short[3]>(asset->deltaPart->trans->size + 1);
								for (auto i = 0; i < asset->deltaPart->trans->size + 1; i++)
								{
									IW7_asset->deltaPart->trans->u.frames.frames._2[i][0] = asset->deltaPart->trans->u.frames.frames._2[i][0];
									IW7_asset->deltaPart->trans->u.frames.frames._2[i][1] = asset->deltaPart->trans->u.frames.frames._2[i][1];
									IW7_asset->deltaPart->trans->u.frames.frames._2[i][2] = asset->deltaPart->trans->u.frames.frames._2[i][2];
								}
							}
						}
					}
					else
					{
						for (auto i = 0; i < 3; i++)
						{
							IW7_asset->deltaPart->trans->u.frame0[i] = asset->deltaPart->trans->u.frame0[i];
						}
					}
				}

				if (asset->deltaPart->quat)
				{
					auto extra_size = 0;

					if (asset->deltaPart->quat->size)
					{
						if (asset->numframes >= 256)
						{
							extra_size += (asset->deltaPart->quat->size * 2) + 2;
						}
						else
						{
							extra_size += asset->deltaPart->quat->size + 1;
						}
					}
					else
					{
						// quat data contains 4 extra bytes
						extra_size += 4;
					}

					IW7_asset->deltaPart->quat = mem.manual_allocate<IW7::XAnimDeltaPartQuat>(sizeof(IW7::XAnimDeltaPartQuat) + extra_size);
					IW7_asset->deltaPart->quat->size = asset->deltaPart->quat->size;

					if (asset->deltaPart->quat->size)
					{
						if (asset->numframes >= 256)
						{
							for (auto i = 0; i < asset->deltaPart->quat->size + 1; i++)
							{
								IW7_asset->deltaPart->quat->u.frames.indices._2[i] = asset->deltaPart->quat->u.frames.indices._2[i];
							}
						}
						else
						{
							for (auto i = 0; i < asset->deltaPart->quat->size + 1; i++)
							{
								IW7_asset->deltaPart->quat->u.frames.indices._1[i] = static_cast<unsigned char>(asset->deltaPart->quat->u.frames.indices._1[i]);
							}
						}
						if (asset->deltaPart->quat->u.frames.frames)
						{
							IW7_asset->deltaPart->quat->u.frames.frames = mem.allocate<short[4]>(asset->deltaPart->quat->size + 1);
							for (auto i = 0; i < asset->deltaPart->quat->size + 1; i++)
							{
								IW7_asset->deltaPart->quat->u.frames.frames[i][0] = asset->deltaPart->quat->u.frames.frames[i][0];
								IW7_asset->deltaPart->quat->u.frames.frames[i][1] = asset->deltaPart->quat->u.frames.frames[i][1];
							}
						}
					}
					else
					{
						for (auto i = 0; i < 2; i++)
						{
							IW7_asset->deltaPart->quat->u.frame0[i] = asset->deltaPart->quat->u.frame0[i];
						}
					}
				}
			}

			return IW7_asset;
		}

		IW7::XAnimParts* convert(XAnimParts* asset, allocator& allocator)
		{
			// generate IW7 anims
			return GenerateIW7XAnimParts(asset, allocator);
		}
	}
}