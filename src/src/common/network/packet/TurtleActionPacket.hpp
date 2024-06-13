#include <minecraft/src/common/network/packet/Packet.hpp>

class TurtleActionPacket : public Packet {
public:
	TurtleActionPacket() : Packet() {}

	virtual MinecraftPacketIds getId() const override {
		Log::Info("getId :)");
		return (MinecraftPacketIds)((int)MinecraftPacketIds::EndId + 1);
	}

	virtual std::string getName() const override {
		Log::Info("getName");
		return "TurtleActionPacket";
	}

	virtual Bedrock::Result<void, std::error_code> checkSize(uint64_t packetSize, bool receiverIsServer) const override {
		Log::Info("packetSize: {:d}, reciever = {}", packetSize, receiverIsServer ? "server" : "client");
		return Packet::checkSize(packetSize, receiverIsServer);
	}

	virtual void write(BinaryStream& binaryStream) override {
		Log::Info("TurtleActionPacket::write");
	}

	virtual Bedrock::Result<void, std::error_code> _read(ReadOnlyBinaryStream&) override {
		Assert("_read");
	}
}; 