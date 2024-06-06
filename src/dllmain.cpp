#include "dllmain.hpp"
#include <minecraft/src/common/world/level/block/registry/BlockTypeRegistry.hpp>
#include <minecraft/src/common/world/level/chunk/LevelChunk.hpp>
#include <minecraft/src/common/world/level/block/Block.hpp>

WeakPtr<TurtleBlock> TURTLE_BLOCK;
WeakPtr<BlockItem> TURTLE_BLOCK_ITEM;

ModFunction void Initialize(AmethystContext* ctx) 
{
	InitializeVtablePtrs();
    Amethyst::EventManager& events = ctx->mEventManager;
	HookManager hooks = ctx->mHookManager;

    events.registerBlocks.AddListener(&RegisterBlocks);
	events.registerItems.AddListener(&RegisterItems);
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