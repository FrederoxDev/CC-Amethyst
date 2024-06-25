#include "dllmain.hpp"
#include <minecraft/src/common/world/level/block/registry/BlockTypeRegistry.hpp>
#include <minecraft/src/common/world/level/chunk/LevelChunk.hpp>
#include <minecraft/src/common/world/level/block/Block.hpp>
#include <src/common/network/packet/TurtleMovePacket.hpp>
#include <minecraft/src/common/network/PacketHandlerDispatcherInstance.hpp>
#include <minecraft/src-client/common/client/network/ClientNetworkHandler.hpp>
#include <minecraft/src-client/common/client/renderer/blockActor/BlockActorRendererDispatcher.hpp>
#include "src/common/world/level/turtle/TurtleAnimationManager.hpp"
#include "src-client/common/client/renderer/blockactor/TurtleRenderer.hpp"

template <>
class PacketHandlerDispatcherInstance<TurtleMovePacket, false> : public IPacketHandlerDispatcher {
public:
	virtual void handle(const NetworkIdentifier& networkId, NetEventCallback& netEvent, std::shared_ptr<Packet> packet) const {
		// I am making the assumption here that netEvent is an instance of ClientNetworkHandler 
		// This assumption only really works because this packet is only sent from: server => client
		//ClientNetworkHandler& clientHandler = (ClientNetworkHandler&)netEvent;
		//
		//// Send an event to the BlockActor on the client to play the move animation..
		//TurtleMovePacket* turtleMoveData = (TurtleMovePacket*)packet.get();

		//BlockSource* region = clientHandler.mClient.getRegion();
		//if (!region) {
		//	Log::Warning("PacketHandlerDispatcherInstance<TurtleMovePacket>::handle recieved packet, but CI->getRegion() == nullptr");
		//	return;
		//}
		//
		//const Block& beforeBlock = region->getBlock(turtleMoveData->mTurtlePosBefore);
		//const Block& afterBlock = region->getBlock(turtleMoveData->mTurtlePosTo);

		//Log::Info("{}", beforeBlock.mLegacyBlock->mNameInfo.mFullName.getString());
		//Log::Info("{}", afterBlock.mLegacyBlock->mNameInfo.mFullName.getString());

		TurtleMovePacket& movementPacket = *(TurtleMovePacket*)packet.get();
		TurtleAnimationManager::OnTurtleMovePacket(movementPacket);
	}
};

static PacketHandlerDispatcherInstance<TurtleMovePacket, false> turtleActionHandler;

WeakPtr<TurtleBlock> TURTLE_BLOCK;
WeakPtr<BlockItem> TURTLE_BLOCK_ITEM;

SafetyHookInline _createPacket;

std::shared_ptr<Packet> createPacket(MinecraftPacketIds id) {
	if (id > MinecraftPacketIds::EndId) {
		Log::Info("Recieved packet with id {}", (int)id);

		auto shared = std::make_shared<TurtleMovePacket>();
		shared->mHandler = &turtleActionHandler;
		return shared;
	}

	return _createPacket.call<std::shared_ptr<Packet>>(id);
}

SafetyHookInline _initializeBlockEntityRenderers;

void initializeBlockEntityRenderers(
	BlockActorRenderDispatcher* self, void* a2, void* a3, void* a4, void* a5,
	void* a6, void* a7, void* a8, void* a9
) {
	_initializeBlockEntityRenderers.call<void>(self, a2, a3, a4, a5, a6, a7, a8, a9);
	self->mRenderers[(BlockActorRendererId)26] = std::make_unique<TurtleRenderer>();
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

	hooks.RegisterFunction<&BlockActorRenderDispatcher::initializeBlockEntityRenderers>("40 55 53 56 57 41 54 41 55 41 56 41 57 48 8D AC 24 ? ? ? ? 48 81 EC ? ? ? ? 48 8B 05 ? ? ? ? 48 33 C4 48 89 85 ? ? ? ? 4D 8B F9 49 8B F8 48 89 55");
	hooks.CreateHook<&BlockActorRenderDispatcher::initializeBlockEntityRenderers>(_initializeBlockEntityRenderers, &initializeBlockEntityRenderers);
}

void RegisterItems(ItemRegistry* registry) {
	TURTLE_BLOCK_ITEM = registry->registerItemShared<BlockItem>("minecraft:turtle", TURTLE_BLOCK->getBlockItemId(), HashedString::EMPTY);
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