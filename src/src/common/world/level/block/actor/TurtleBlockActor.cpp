#include "TurtleBlockActor.hpp"

void TurtleBlockActor::tick(BlockSource& region)
{
	if (mLuaInstance == nullptr) return;

	std::lock_guard<std::mutex> lock(mLuaInstance->mLuaMutex);
	if (!mLuaInstance->mWaitingForMain) return;

	const Block& block = region.getBlock(mPosition.below());
	lua_pushstring(mLuaInstance->mL, block.mLegacyBlock->mNameInfo.mFullName.c_str());
	mLuaInstance->mReturnCount = 1;

	mLuaInstance->mWaitingForMain = false;
	mLuaInstance->mCv.notify_one();
}