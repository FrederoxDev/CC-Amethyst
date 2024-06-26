#pragma once
#include <minecraft/src/common/network/packet/Packet.hpp>
#include <minecraft/src/common/world/phys/Vec3.hpp>
#include <minecraft/src/common/world/level/BlockPos.hpp>

class TurtleMovePacket : public Packet {
public:
	BlockPos mTurtlePosBefore;
	BlockPos mTurtlePosTo;
	uint64_t mTimestamp;

public:
	TurtleMovePacket() 
		: Packet(), mTurtlePosBefore(0, 0, 0), mTurtlePosTo(0, 0, 0), mTimestamp(0) 
	{}

	virtual MinecraftPacketIds getId() const override {
		return (MinecraftPacketIds)((int)MinecraftPacketIds::EndId + 1);
	}

	virtual std::string getName() const override {
		return "TurtleMovePacket";
	}

	virtual void write(BinaryStream& stream) override {
		stream.write(mTurtlePosBefore);
		stream.write(mTurtlePosTo);
		stream.write(mTimestamp);
	}

	virtual Bedrock::Result<void, std::error_code> read(ReadOnlyBinaryStream& stream) override {
		mTurtlePosBefore = stream.get<BlockPos>().value();
		mTurtlePosTo = stream.get<BlockPos>().value();
		mTimestamp = stream.get<uint64_t>().value();

		Log::Info("[read] before: {}, to: {}, at: {:d}", mTurtlePosBefore, mTurtlePosTo, mTimestamp);
		return Packet::read(stream);
	}

	virtual Bedrock::Result<void, std::error_code> _read(ReadOnlyBinaryStream& stream) override {
		return Bedrock::Result<void, std::error_code>();
	}
}; 