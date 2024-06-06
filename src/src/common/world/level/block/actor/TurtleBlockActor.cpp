#include "TurtleBlockActor.hpp"

void TurtleBlockActor::tick(BlockSource& region)
{
	if (mLuaInstance == nullptr) return;

	std::lock_guard<std::mutex> lock(mLuaInstance->mLuaMutex);
	if (!mLuaInstance->mWaitingForMain) return;

	mLuaInstance->mReturnCount = mLuaInstance->mInternalCppCallback(*mLuaInstance, *this, region);
	mLuaInstance->mWaitingForMain = false;
	mLuaInstance->mCv.notify_one();
}