﻿#include "dllmain.hpp"
#include <minecraft/src/common/world/level/block/registry/BlockTypeRegistry.hpp>
#include <minecraft/src/common/world/level/chunk/LevelChunk.hpp>
#include <minecraft/src/common/world/level/block/Block.hpp>
#include <src/common/network/packet/TurtleMovePacket.hpp>
#include <minecraft/src/common/network/PacketHandlerDispatcherInstance.hpp>
#include <minecraft/src-client/common/client/network/ClientNetworkHandler.hpp>
#include <minecraft/src-client/common/client/renderer/blockActor/BlockActorRendererDispatcher.hpp>
#include "src/common/world/level/turtle/TurtleAnimationManager.hpp"
#include "src-client/common/client/renderer/blockactor/TurtleRenderer.hpp"
#include <minecraft/src-client/common/client/renderer/block/BlockGraphics.hpp>
#include <minecraft/src-client/common/client/model/GeometryGroup.hpp>
#include <minecraft/src-client/common/client/model/Geometry.hpp>
#include <src/common/network/packet/TurtleRotatePacket.hpp>
#include <minecraft/src-deps/coregraphics/TextureSetLayerTypes.hpp>
#include <minecraft/src-client/common/client/renderer/TextureGroup.hpp>

AmethystContext* amethyst = nullptr;

template <>
class PacketHandlerDispatcherInstance<TurtleMovePacket, false> : public IPacketHandlerDispatcher {
public:
	virtual void handle(const NetworkIdentifier& networkId, NetEventCallback& netEvent, std::shared_ptr<Packet> packet) const {
		TurtleMovePacket& movementPacket = *(TurtleMovePacket*)packet.get();
		TurtleAnimationManager::OnTurtleMovePacket(movementPacket);
	}
};

template <>
class PacketHandlerDispatcherInstance<TurtleRotatePacket, false> : public IPacketHandlerDispatcher {
public:
	virtual void handle(const NetworkIdentifier& networkId, NetEventCallback& netEvent, std::shared_ptr<Packet> packet) const {
		TurtleRotatePacket& rotationPacket = *(TurtleRotatePacket*)packet.get();
		ClientNetworkHandler& clientNetwork = (ClientNetworkHandler&)netEvent;

		BlockSource& region = *clientNetwork.mClient.getRegion();
		TurtleBlockActor* turtle = const_cast<TurtleBlockActor*>((const TurtleBlockActor*)region.getBlockEntity(rotationPacket.mTurtlePos)); // <-- bad code lol

		if (!turtle) {
			Log::Warning("No turtle found at {} in PacketHandlerDispatcherInstance<TurtleRotatePacket>::handle", rotationPacket.mTurtlePos);
			return;
		}

		TurtleRotateAnimation animation(rotationPacket);
		turtle->mAnimation = animation;
		turtle->mTurtleRot = rotationPacket.mNewDir;
	}
};

static PacketHandlerDispatcherInstance<TurtleMovePacket, false> turtleMovePacketHandler;
static PacketHandlerDispatcherInstance<TurtleRotatePacket, false> turtleRotatePacketHandler;

WeakPtr<TurtleBlock> TURTLE_BLOCK;
WeakPtr<BlockItem> TURTLE_BLOCK_ITEM;

SafetyHookInline _createPacket;

std::shared_ptr<Packet> createPacket(MinecraftPacketIds id) {
	// Vanilla packets.
	if (id <= MinecraftPacketIds::EndId) {
		return _createPacket.call<std::shared_ptr<Packet>>(id);
	}

	// Custom packets
	if ((int)id == (int)MinecraftPacketIds::EndId + 1) {
		auto shared = std::make_shared<TurtleMovePacket>();
		shared->mHandler = &turtleMovePacketHandler;
		return shared;
	}

	else if ((int)id == (int)MinecraftPacketIds::EndId + 2) {
		auto shared = std::make_shared<TurtleRotatePacket>();
		shared->mHandler = &turtleRotatePacketHandler;
		return shared;
	}

	Assert("Recieved packet with unknown id: {}", (int)id);
}

SafetyHookInline _initializeBlockEntityRenderers;

extern BlockActorRendererId turtleBlockActorRendererId;
extern GeometryInfo* mTurtleMesh;
extern mce::TexturePtr mTurtleTexture;

void initializeBlockEntityRenderers(
	BlockActorRenderDispatcher* self, 
	const gsl::not_null<Bedrock::NonOwnerPointer<GeometryGroup>>& geometryGroup,
	std::shared_ptr<mce::TextureGroup> textures, 
	void* a4, void* a5,
	void* a6, void* a7, void* a8, void* a9
) {
	self->mRenderers[TurtleBlockActor::TURTLE_RENDERER_ID] = std::make_unique<TurtleRenderer>();

	HashedString turtleModelIdentifier("geometry.cc_turtle");
	auto turtleModel = geometryGroup->mGeometries.find(turtleModelIdentifier);

	if (turtleModel == geometryGroup->mGeometries.end()) {
		Log::Info("Could not find geometry.cc_turtle");
		return;
	}

	TurtleRenderer::TURTLE_GEOMETRY = turtleModel->second.get();
	TurtleRenderer::TURTLE_TEXTURE = textures->getTexture("textures/entity/cc_turtle", true, std::nullopt, cg::TextureSetLayerType::Normal);
	
	_initializeBlockEntityRenderers.call<void>(self, geometryGroup, textures, a4, a5, a6, a7, a8, a9);
}

void InitBlockGraphics(ResourcePackManager& resources, const Experiments& experiments) {
	HashedString hashedIdentifier("minecraft:turtle");
	BlockGraphics* graphics = BlockGraphics::createBlockGraphics(hashedIdentifier, BlockShape::INVISIBLE);

	if (graphics->mTextureItems.size() == 0) {
		graphics->setTextureItem("diamond_block", "diamond_block", "diamond_block", "diamond_block", "diamond_block", "diamond_block");
	}

	graphics->setDefaultCarriedTextures();
}

ModFunction void Initialize(AmethystContext* ctx) 
{
	InitializeVtablePtrs();
	amethyst = ctx;

    Amethyst::EventManager& events = ctx->mEventManager;
	Amethyst::EnumAllocator enums = ctx->mEnumAllocator;
	HookManager hooks = ctx->mHookManager;

	// Enums
	enums.RegisterEnum(BlockActorRendererId::PADDING);
	TurtleBlockActor::TURTLE_RENDERER_ID = enums.GetNextValue<BlockActorRendererId>();

	// Events
    events.registerBlocks.AddListener(&RegisterBlocks);
	events.registerItems.AddListener(&RegisterItems);
	events.initBlockGraphics.AddListener(&InitBlockGraphics);

	// Hooks
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