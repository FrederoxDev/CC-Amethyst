#include "src-client/common/client/renderer/blockactor/TurtleRenderer.hpp"
#include <minecraft/src-client/common/client/renderer/Tessellator.hpp>
#include <minecraft/src-client/common/client/renderer/TexturePtr.hpp>
#include <minecraft/src-deps/renderer/Camera.hpp>

GeometryInfo* TurtleRenderer::TURTLE_GEOMETRY;
mce::TexturePtr TurtleRenderer::TURTLE_TEXTURE;

uint64_t getTimeElapsed(std::chrono::milliseconds since) {
	auto epoc = std::chrono::system_clock::now().time_since_epoch();
	auto now = std::chrono::duration_cast<std::chrono::milliseconds>(epoc);
	return (now - since).count();
}

TurtleRenderer::TurtleRenderer() : BlockActorRenderer()
{
	mTurtleMaterial = reinterpret_cast<mce::MaterialPtr*>(SlideAddress(0x59BD7E0));
}

mce::Mesh TurtleRenderer::constructTurtleMesh(BaseActorRenderContext& ctx)
{
	Tessellator& tess = ctx.mScreenContext->tessellator;
	tess.begin(mce::PrimitiveMode::QuadList, 0);

	// center the model
	tess.mPostTransformOffset = Vec3(0.5f, 0, 0.5f);

	for (auto& node : TURTLE_GEOMETRY->mPtr->mNodes) {
		for (auto& box : node.second.mBoxes) {
			box.Tessellate(tess, 64, 64);
		}
	}

	return tess.end(0, "cc:turtle", 0);
}