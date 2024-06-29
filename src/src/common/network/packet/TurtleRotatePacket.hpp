#pragma once
#include <minecraft/src/common/network/packet/Packet.hpp>
#include <minecraft/src/common/world/phys/Vec3.hpp>
#include <minecraft/src/common/world/level/BlockPos.hpp>

class TurtleRotatePacket : public Packet {
public:
	BlockPos mTurtlePos;
	FacingID mOldDir;
	FacingID mNewDir;
	uint64_t mTimestamp;

public:
	TurtleRotatePacket() 
		: Packet(), mOldDir(FacingID::NORTH), mNewDir(FacingID::NORTH), mTimestamp(0), mTurtlePos(0, 0, 0)
	{}

	virtual MinecraftPacketIds getId() const override {
		return (MinecraftPacketIds)((int)MinecraftPacketIds::EndId + 2);
	}

	virtual std::string getName() const override {
		return "TurtleRotatePacket";
	}

	virtual void write(BinaryStream& stream) override {
		stream.write(mTurtlePos);
		stream.write(mOldDir);
		stream.write(mNewDir);
		stream.write(mTimestamp);
	}

	virtual Bedrock::Result<void, std::error_code> read(ReadOnlyBinaryStream& stream) override {
		mTurtlePos = stream.get<BlockPos>().value();
		mOldDir = stream.get<FacingID>().value();
		mNewDir = stream.get<FacingID>().value();
		mTimestamp = stream.get<uint64_t>().value();

		return Packet::read(stream);
	}

	virtual Bedrock::Result<void, std::error_code> _read(ReadOnlyBinaryStream& stream) override {
		return Bedrock::Result<void, std::error_code>();
	}
}; 