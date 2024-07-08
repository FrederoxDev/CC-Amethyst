#pragma once
#include <minecraft/src-client/common/client/renderer/blockActor/BlockActorRenderer.hpp>
#include <minecraft/src-client/common/client/renderer/BaseActorRenderer.hpp>
#include <minecraft/src-client/common/client/renderer/screen/ScreenContext.hpp>
#include <minecraft/src-client/common/client/model/Geometry.hpp>
#include <minecraft/src-client/common/client/model/GeometryInfo.hpp>
#include <minecraft/src-deps/minecraftrenderer/renderer/BedrockTexture.hpp>
#include <minecraft/src/common/world/phys/Vec3.hpp>
#include <src/common/world/level/block/actor/TurtleBlockActor.hpp>
#include <minecraft/src-deps/minecraftrenderer/renderer/Mesh.hpp>
#include <minecraft/src-deps/renderer/Matrix.hpp>
#include <minecraft/src-deps/renderer/Camera.hpp>
#include <minecraft/src-deps/minecraftrenderer/renderer/BedrockTexture.hpp>

constexpr int turtleMoveDuration = 600; // ms

uint64_t getTimeElapsed(std::chrono::milliseconds since);

class TurtleRenderer : public BlockActorRenderer {
public:
	static GeometryInfo* TURTLE_GEOMETRY;
	static mce::TexturePtr TURTLE_TEXTURE;
	mce::MaterialPtr* mTurtleMaterial;

public:
	TurtleRenderer();

	virtual ~TurtleRenderer() {}

	virtual void render(BaseActorRenderContext& ctx, BlockActorRenderData& data) {
		static mce::Mesh turtleMesh = constructTurtleMesh(ctx);

		TurtleBlockActor& turtle = (TurtleBlockActor&)data.entity;

		Matrix& matrix = ctx.mScreenContext->camera->worldMatrixStack.stack.top();
		Matrix originalMatrix = matrix;

		if (std::holds_alternative<std::monostate>(turtle.mAnimation)) {
			Vec3 renderPos = Vec3(turtle.mPosition) - ctx.mCameraTargetPosition;

			matrix.translate(renderPos.x + 0.5f, renderPos.y, renderPos.z + 0.5f);
			matrix.rotate(Facing::getYAngle(turtle.mTurtleRot), 0.f, 1.0f, 0.f);
			matrix.translate(-0.5f, 0, -0.5f);
		}

		else if (std::holds_alternative<TurtleMoveAnimation>(turtle.mAnimation)) {
			auto& animation = std::get<TurtleMoveAnimation>(turtle.mAnimation);
			uint64_t timeElapsed = getTimeElapsed(animation.mStartTimestamp);

			if (timeElapsed >= turtleMoveDuration) {
				turtle.mAnimation = std::monostate();
				return;
			}

			float t = timeElapsed / (float)turtleMoveDuration;
			Vec3 position = Vec3::lerp(animation.mTurtleStartPos, animation.mTurtleEndPos, t);
			Vec3 renderPos = position - ctx.mCameraTargetPosition;
			matrix.translate(renderPos.x, renderPos.y, renderPos.z);
		}

		else if (std::holds_alternative<TurtleRotateAnimation>(turtle.mAnimation)) {
			auto& animation = std::get<TurtleRotateAnimation>(turtle.mAnimation);
			uint64_t timeElapsed = getTimeElapsed(animation.mTimestamp);

			if (timeElapsed >= turtleMoveDuration) {
				turtle.mAnimation = std::monostate();
				return;
			}

			float t = timeElapsed / (float)turtleMoveDuration;
			float start = Facing::getYAngle(animation.mOldDir);
			float end = Facing::getYAngle(animation.mNewDir);

			// Find shortest interpolation path
			float delta = fmodf(end - start, 360.0f);
			if (delta > 180.0f) delta -= 360.0f;
			else if (delta <= -180.0f) delta += 360.0f;

			Vec3 renderPos = Vec3(turtle.mPosition) - ctx.mCameraTargetPosition;

			matrix.translate(renderPos.x + 0.5f, renderPos.y, renderPos.z + 0.5f);
			matrix.rotate(start + t * delta, 0.f, 1.0f, 0.f);
			matrix.translate(-0.5f, 0, -0.5f);
		}

		turtleMesh.renderMesh(*ctx.mScreenContext, *mTurtleMaterial, TURTLE_TEXTURE);
		matrix = originalMatrix;
	}

	static mce::Mesh constructTurtleMesh(BaseActorRenderContext& ctx);
};