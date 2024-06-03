#pragma once
#include <minecraft/src/common/world/level/block/BlockLegacy.hpp>
#include "TurtleBlockActor.hpp"

class TurtleBlock : public BlockLegacy {
public: 
	TurtleBlock(const std::string& nameId, short id, const Material& material)
		: BlockLegacy(nameId, id, material) 
	{
		mBlockEntityType = (BlockActorType)58;
	}

	virtual std::shared_ptr<BlockActor> newBlockEntity(const BlockPos& pos, const Block& block) const override {
		// Todo: We probably need to hook BlockActorFactory::createBlockEntity for it to work on load.
		// Todo: This 58 should be being allocated via amethyst and not hardcoded.
		return std::make_shared<TurtleBlockActor>((BlockActorType)58, pos, "minecraft:turtle");
	}

	virtual void movedByPiston(BlockSource& region, const BlockPos& pos) const override {
		Log::Info("MovedByPiston");
	};

protected:
	virtual bool use(Player& player, const BlockPos& pos, unsigned char face) const override {
		const Dimension& dimension = player.getDimensionConst();
		BlockSource& region = dimension.getBlockSourceFromMainChunkSource();

		TurtleBlockActor* blockActor = (TurtleBlockActor*)region.getBlockEntity(pos);
		if (blockActor == nullptr) {
			Log::Warning("Failed to get TurtleBlockActor at position {}", pos);
			return true;
		}

		blockActor->onBlockClicked(region);

		return true;
	}
};