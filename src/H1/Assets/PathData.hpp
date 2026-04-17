#pragma once

namespace ZoneTool::H1
{
	class IPathData
	{
	private:
		static void dump_node_tree_children(assetmanager::dumper& dumper, pathnode_tree_t* node_tree);
		static void dump_node_tree(assetmanager::dumper& dumper, pathnode_tree_t* node_tree);

	public:
		static void dump(PathData* asset);

	};
}