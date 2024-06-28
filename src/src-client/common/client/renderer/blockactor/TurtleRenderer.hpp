#pragma once
#include <minecraft/src-client/common/client/renderer/blockActor/BlockActorRenderer.hpp>
#include <minecraft/src-client/common/client/renderer/BaseActorRenderer.hpp>
#include <minecraft/src-client/common/client/renderer/screen/ScreenContext.hpp>
#include <minecraft/src-client/common/client/model/Geometry.hpp>
#include <minecraft/src-client/common/client/model/GeometryInfo.hpp>
#include <minecraft/src-deps/minecraftrenderer/renderer/BedrockTexture.hpp>
#include <minecraft/src/common/world/phys/Vec3.hpp>
#include <src/common/world/level/block/actor/TurtleBlockActor.hpp>

constexpr int turtleMoveDuration = 600; // ms

class TurtleRenderer : public BlockActorRenderer {
public:
	static GeometryInfo* TURTLE_GEOMETRY;
	static mce::TexturePtr TURTLE_TEXTURE;
	mce::MaterialPtr* mTurtleMaterial;

public:
	TurtleRenderer();

	virtual void render(BaseActorRenderContext& ctx, BlockActorRenderData& data) {
		static mce::Mesh turtleMesh = constructTurtleMesh(ctx);

		TurtleBlockActor& turtle = (TurtleBlockActor&)data.entity;

		Matrix& matrix = ctx.mScreenContext->camera->worldMatrixStack.stack.top();
		Matrix originalMatrix = matrix;

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
			Vec3 renderPos = position - ctx.mCameraTargetPosition;
			matrix.translate(renderPos.x + 0.5f, renderPos.y, renderPos.z + 0.5f);
		}
		else {
			matrix.translate(data.position.x + 0.5f, data.position.y, data.position.z + 0.5f);
		}

		turtleMesh.renderMesh(*ctx.mScreenContext, *mTurtleMaterial, TURTLE_TEXTURE);
		matrix = originalMatrix;
	}

	static mce::Mesh constructTurtleMesh(BaseActorRenderContext& ctx);
};