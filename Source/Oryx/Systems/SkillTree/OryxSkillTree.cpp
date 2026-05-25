#include "OryxSkillTree.h"

const FOryxSkillNode* UOryxSkillTree::FindNode(FName NodeId) const
{
	for (const FOryxSkillNode& Node : Nodes)
	{
		if (Node.NodeId == NodeId) return &Node;
	}
	return nullptr;
}

FPrimaryAssetId UOryxSkillTree::GetPrimaryAssetId() const
{
	const FName AssetType = TEXT("OryxSkillTree");
	const FName AssetName = GetFName();
	return FPrimaryAssetId(AssetType, AssetName);
}
