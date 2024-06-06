#pragma once
#include <minecraft/src/common/world/level/block/BlockLegacy.hpp>
#include "src/common/world/level/block/actor/TurtleBlockActor.hpp"
#include "src/common/world/level/computer/LuaInstanceManager.hpp"

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

protected:
	virtual bool use(Player& player, const BlockPos& pos, unsigned char face) const override {
		const Dimension& dimension = player.getDimensionConst();
		BlockSource& region = dimension.getBlockSourceFromMainChunkSource();

		if (!region.mLevel->isClientSide) {
			LuaInstance* luaInstance = LuaInstanceManager::GetOrCreateInstanceAt(pos);
			luaInstance->RunLua(R"(
				
			if turtle.inspectDown() == "minecraft:grass" then
				turtle.up()
				turtle.up()
			else
				turtle.up()
			end


			)");
		}

		return true;
	}
};