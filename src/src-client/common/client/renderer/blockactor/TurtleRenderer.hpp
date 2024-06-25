#pragma once
#include <minecraft/src-client/common/client/renderer/blockActor/BlockActorRenderer.hpp>

class TurtleRenderer : public BlockActorRenderer {
public:
	TurtleRenderer() : BlockActorRenderer() {}

	virtual void render(BaseActorRenderContext& ctx, BlockActorRenderData& data) {
		Log::Info("render");
	}
};