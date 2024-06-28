#pragma once
#include <minecraft/src-client/common/client/renderer/blockActor/BlockActorRenderer.hpp>
#include <minecraft/src/common/world/phys/Vec3.hpp>
#include <minecraft/src-client/common/client/model/Geometry.hpp>
#include <minecraft/src-client/common/client/model/GeometryInfo.hpp>
#include <minecraft/src-deps/minecraftrenderer/renderer/BedrockTexture.hpp>

std::array<Vec3, 8> cubeVertexes = {
	Vec3(0, 0, 0),
	Vec3(0, 1, 0),
	Vec3(1, 0, 0),
	Vec3(1, 1, 0),
	Vec3(0, 0, 1),
	Vec3(0, 1, 1),
	Vec3(1, 0, 1),
	Vec3(1, 1, 1),
};

std::array<std::array<char, 4>, 6> cubeFaces = { {
	{ 0, 1, 3, 2 },
	{ 2, 3, 7, 6 },
	{ 6, 7, 5, 4 },
	{ 4, 5, 1, 0 },
	{ 4, 0, 2, 6 },
	{ 1, 5, 7, 3 }
}};

constexpr int turtleMoveDuration = 600; // ms

static GeometryInfo* mTurtleMesh;
static mce::TexturePtr mTurtleTexture;

class TurtleRenderer : public BlockActorRenderer {
public:
	TurtleRenderer() : BlockActorRenderer() {}

	virtual void render(BaseActorRenderContext& ctx, BlockActorRenderData& data) {
		mce::Mesh turtleMesh = constructTurtleMesh(ctx);
		mce::MaterialPtr* material = reinterpret_cast<mce::MaterialPtr*>(SlideAddress(0x59BD7E0));
		TurtleBlockActor& turtle = (TurtleBlockActor&)data.entity;

		Matrix& matrix = ctx.mScreenContext->camera->worldMatrixStack.stack.top();
		Matrix originalMatrix = matrix;

		Vec3* playerPos = ctx.mClientInstance->getLocalPlayer()->getPosition();

		if (turtle.mMoveAnimation) {
			TurtleMoveAnimation& anim = turtle.mMoveAnimation.value();
			
			auto epoc = std::chrono::system_clock::now().time_since_epoch();
			auto now = std::chrono::duration_cast<std::chrono::milliseconds>(epoc);
			auto timeElapsed = (now - anim.mStartTimestamp).count();

			if (timeElapsed >= turtleMoveDuration) {
				turtle.mMoveAnimation.reset();
				return;
			}

			float t = timeElapsed / (float)turtleMoveDuration;

			Vec3 position = Vec3::lerp(anim.mTurtleStartPos, anim.mTurtleEndPos, t);
			Vec3 renderPos = position - *playerPos;
			matrix.translate(renderPos.x + 0.5f, renderPos.y, renderPos.z + 0.5f);
		}
		else {
			matrix.translate(data.position.x + 0.5f, data.position.y, data.position.z + 0.5f);
		}

		turtleMesh.renderMesh(*ctx.mScreenContext, *material, mTurtleTexture);
		matrix = originalMatrix;
	}

	__declspec(noinline) mce::Mesh constructTurtleMesh(BaseActorRenderContext& ctx) {
		Tessellator& tess = ctx.mScreenContext->tessellator;
		tess.begin(mce::PrimitiveMode::QuadList, 1000);

		auto& nodes = mTurtleMesh->mPtr->mNodes;

		for (auto& node : nodes) {
			for (auto& box : node.second.mBoxes) {
				box.Tessellate(tess, 64, 64);
			}
		}

		return tess.end(0, "cc:turtle", 0);
	}
};