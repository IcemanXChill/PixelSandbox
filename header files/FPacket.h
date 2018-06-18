#ifndef FPACKET_H
#define FPACKET_H
#include <SFML/Network.hpp>

class FPacket : public sf::Packet
{
public:
	/** Enumerates the possible flags a packet can have.
	It will be automatically sent and received when using this class for sharing data across sockets.
	Feel free to extend this enumeration to match your need.  */
	enum PFlag {
		None,
		Ping,
		Pong,
		Connect,
		Disconnect,
		Connected,
		Disconnected,
		Text,
		Move
		// ... Add your flags here
	};

	/** Default constructor. */
	FPacket();
	/** Overloaded constructor. Constructs a Packet and sets the flag as specified. */
	FPacket(PFlag);

	/** Sets the flag to the packet */
	void setFlag(PFlag);

	/** Reads the flag. */
	inline PFlag        flag() const { return static_cast<PFlag>(m_flag); }
	/** Reads the flag as an 8 bytes unsigned int. */
	inline sf::Uint8 flagAsU() const { return m_flag; }
	/** Checks if a flag has been set on this Packet. Returns false if flag equals to FPacket::None. */
	inline bool    isFlagged() const { return m_flag == FPacket::None ? false : true; }

	/** Hiding base class clear() function.
	Clears the packet as usual and sets the flag to 'None', so isFlagged() will return false.  */
	virtual void clear();


private:
	sf::Uint8    m_flag;

	virtual void onReceive(const void*, std::size_t);
	virtual const void* onSend(std::size_t&);

};


/** Overloading of comparison operator. Checks a FPacket object against a PFlag enumeration type instance (FPacket::PFlag). */
bool operator== (const FPacket& p1, const FPacket::PFlag& p2);
/** Overloading of comparison operator. Checks a Packet object against a PFlag enumeration type instance (FPacket::PFlag). */
bool operator!= (const FPacket& p1, const FPacket::PFlag& p2);


#endif // FPACKET_HPP