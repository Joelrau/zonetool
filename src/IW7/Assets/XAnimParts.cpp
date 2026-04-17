#include "stdafx.hpp"

namespace ZoneTool::IW7
{
	void IXAnimParts::dump(XAnimParts* asset)
	{
		const auto path = "xanim\\"s + asset->name + ".xab";

		assetmanager::dumper dump;
		if (!dump.open(path))
		{
			return;
		}

		dump.dump_single(asset);
		dump.dump_string(asset->name);

		if (asset->names)
		{
			for (unsigned char bone = 0; bone < asset->boneCount[9]; bone++)
			{
				dump.dump_string(SL_ConvertToString(asset->names[bone]));
			}
		}

		if (asset->dataByte)
		{
			dump.dump_array(asset->dataByte, asset->dataByteCount);
		}
		if (asset->dataShort)
		{
			dump.dump_array(asset->dataShort, asset->dataShortCount);
		}
		if (asset->dataInt)
		{
			dump.dump_array(asset->dataInt, asset->dataIntCount);
		}
		if (asset->randomDataShort)
		{
			dump.dump_array(asset->randomDataShort, asset->randomDataShortCount);
		}
		if (asset->randomDataByte)
		{
			dump.dump_array(asset->randomDataByte, asset->randomDataByteCount);
		}
		if (asset->randomDataInt)
		{
			dump.dump_array(asset->randomDataInt, asset->randomDataIntCount);
		}

		if (asset->indices.data)
		{
			if (asset->numframes >= 256)
			{
				dump.dump_array(asset->indices._2, asset->indexCount);
			}
			else
			{
				dump.dump_array(asset->indices._1, asset->indexCount);
			}
		}

		if (asset->notify)
		{
			dump.dump_array(asset->notify, asset->notifyCount);
			for (unsigned char i = 0; i < asset->notifyCount; i++)
			{
				dump.dump_string(SL_ConvertToString(asset->notify[i].name));
			}
		}

		if (asset->deltaPart)
		{
			dump.dump_single(asset->deltaPart);

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

				dump.dump_raw(asset->deltaPart->trans, sizeof(XAnimPartTrans) + extra_size);

				if (asset->deltaPart->trans->size)
				{
					if (asset->deltaPart->trans->u.frames.frames._1)
					{
						if (asset->deltaPart->trans->smallTrans)
						{
							dump.dump_raw(asset->deltaPart->trans->u.frames.frames._1, (3 * asset->deltaPart->trans->size) + 3);
						}
						else
						{
							dump.dump_raw(asset->deltaPart->trans->u.frames.frames._2, (6 * asset->deltaPart->trans->size) + 6);
						}
					}
				}
			}

			if (asset->deltaPart->quat2)
			{
				auto extra_size = 0;

				if (asset->deltaPart->quat2->size)
				{
					if (asset->numframes >= 256)
					{
						extra_size += (asset->deltaPart->quat2->size * 2) + 2;
					}
					else
					{
						extra_size += asset->deltaPart->quat2->size + 1;
					}
				}
				else
				{
					// no extra size required, quat2 data fits inside the frame0 buffer
					extra_size += 0;
				}

				dump.dump_raw(asset->deltaPart->quat2, sizeof(XAnimDeltaPartQuat2) + extra_size);

				if (asset->deltaPart->quat2->size && asset->deltaPart->quat2->u.frames.frames)
				{
					dump.dump_raw(asset->deltaPart->quat2->u.frames.frames, (asset->deltaPart->quat2->size * 4) + 4);
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

				dump.dump_raw(asset->deltaPart->quat, sizeof(XAnimDeltaPartQuat) + extra_size);

				if (asset->deltaPart->quat->size && asset->deltaPart->quat->u.frames.frames)
				{
					dump.dump_raw(asset->deltaPart->quat->u.frames.frames, (asset->deltaPart->quat->size * 8) + 8);
				}
			}
		}

		dump.close();
	}
}