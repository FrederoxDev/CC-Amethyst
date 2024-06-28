#include "src-client/common/client/renderer/blockactor/TurtleRenderer.hpp"
#include <minecraft/src-client/common/client/renderer/Tessellator.hpp>

GeometryInfo* TurtleRenderer::TURTLE_GEOMETRY;
mce::TexturePtr TurtleRenderer::TURTLE_TEXTURE;

TurtleRenderer::TurtleRenderer() : BlockActorRenderer()
{
	mTurtleMaterial = reinterpret_cast<mce::MaterialPtr*>(SlideAddress(0x59BD7E0));
}

mce::Mesh TurtleRenderer::constructTurtleMesh(BaseActorRenderContext& ctx)
{
	Tessellator& tess = ctx.mScreenContext->tessellator;
	tess.begin(mce::PrimitiveMode::QuadList, 0);

	for (auto& node : TURTLE_GEOMETRY->mPtr->mNodes) {
		for (auto& box : node.second.mBoxes) {
			box.Tessellate(tess, 64, 64);
		}
	}

	return tess.end(0, "cc:turtle", 0);
}