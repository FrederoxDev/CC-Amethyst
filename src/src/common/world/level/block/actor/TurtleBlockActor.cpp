#include "TurtleBlockActor.hpp"

void TurtleBlockActor::tick(BlockSource& region)
{
	if (region.mLevel->isClientSide) return;

	[[unlikely]]
	if (!mLuaInstance) {
		// This really shouldn't have happened
		if (!LuaInstanceManager::IsInstanceAt(mPosition)) return;

		// This will be the case when the entity has just been spawned, and has never had tick called before
		mLuaInstance = LuaInstanceManager::GetOrCreateInstanceAt(mPosition);
	}

	std::lock_guard<std::mutex> lock(mLuaInstance->mLuaMutex);
	if (!mLuaInstance->mWaitingForMain) return;

	mLuaInstance->mReturnCount = mLuaInstance->mInternalCppCallback(*mLuaInstance, *this, region);
	mLuaInstance->mWaitingForMain = false;
	mLuaInstance->mCv.notify_one();
}