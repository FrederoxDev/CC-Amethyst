#include "dllmain.hpp"
#include <minecraft/src/common/world/level/block/registry/BlockTypeRegistry.hpp>
#include <minecraft/src/common/world/level/chunk/LevelChunk.hpp>
#include <minecraft/src/common/world/level/block/Block.hpp>
#include <src/common/network/packet/TurtleActionPacket.hpp>

WeakPtr<TurtleBlock> TURTLE_BLOCK;
WeakPtr<BlockItem> TURTLE_BLOCK_ITEM;

SafetyHookInline _createPacket;

std::shared_ptr<Packet> createPacket(MinecraftPacketIds id) {
	if (id > MinecraftPacketIds::EndId) {
		Log::Info("Recieved packet with id {}", (int)id);
		return std::make_shared<TurtleActionPacket>();
	}

	return _createPacket.call<std::shared_ptr<Packet>>(id);
}

ModFunction void Initialize(AmethystContext* ctx) 
{
	InitializeVtablePtrs();
    Amethyst::EventManager& events = ctx->mEventManager;
	HookManager hooks = ctx->mHookManager;

    events.registerBlocks.AddListener(&RegisterBlocks);
	events.registerItems.AddListener(&RegisterItems);

	hooks.RegisterFunction<&MinecraftPackets::createPacket>("40 53 48 83 EC ? 45 33 C0 48 8B D9 FF CA 81 FA");
	hooks.CreateHook<&MinecraftPackets::createPacket>(_createPacket, &createPacket);
}

void RegisterItems(ItemRegistry* registry) {
	TURTLE_BLOCK_ITEM = registry->registerItemShared<BlockItem>("minecraft:turtle", TURTLE_BLOCK->getBlockItemId());
}

void RegisterBlocks(BlockDefinitionGroup* blockDef) {
	Material testMaterial;
	testMaterial.mType = Dirt;
	testMaterial.mNeverBuildable = false;
	testMaterial.mAlwaysDestroyable = true;
	testMaterial.mLiquid = false;
	testMaterial.mTranslucency = 0.0f;
	testMaterial.mBlocksMotion = true;
	testMaterial.mBlocksPrecipitation = true;
	testMaterial.mSolid = true;
	testMaterial.mSuperHot = false;

	// Todo: the game seems to crash when registering a block in a non minecraft namespace..
	TURTLE_BLOCK = BlockTypeRegistry::registerBlock<TurtleBlock>("minecraft:turtle", ++blockDef->mLastBlockId, testMaterial);
}