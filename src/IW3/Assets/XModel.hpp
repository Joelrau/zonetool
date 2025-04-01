#pragma once

namespace ZoneTool
{
	namespace IW3
	{
		IW4::XModel* GenerateIW4Model(XModel* asset, allocator& mem);

		class IXModel
		{
		public:
			static void dump(XModel* asset);
		};
	}
}