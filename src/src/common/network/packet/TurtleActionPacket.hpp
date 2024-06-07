#include <minecraft/src/common/network/packet/Packet.hpp>

class TurtleActionPacket : public Packet {
public:
	TurtleActionPacket() : Packet() {}

	virtual MinecraftPacketIds getId() const override {
		return (MinecraftPacketIds)((int)MinecraftPacketIds::EndId + 1);
	}

	virtual std::string getName() const override {
		return "TurtleActionPacket";
	}

	virtual void write(BinaryStream& binaryStream) override {
		Log::Info("TurtleActionPacket::write");
	}

	virtual Bedrock::Result<void, std::error_code> _read(ReadOnlyBinaryStream&) override {
		Assert("_read");
	}
}; 