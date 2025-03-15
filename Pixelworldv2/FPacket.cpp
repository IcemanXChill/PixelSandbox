#include "FPacket.h"
#include <cstring>


FPacket::FPacket() :
	m_flag(None)
{

}


FPacket::FPacket(PFlag flag) :
	m_flag(flag)
{

}



void FPacket::setFlag(PFlag flag)
{
	m_flag = flag;
}



void FPacket::clear()
{
	static_cast<sf::Packet*>(this)->clear();
	m_flag = None;
}



void FPacket::onReceive(const void* data, std::size_t size)
{
	std::size_t so_flag = sizeof m_flag;
	std::memcpy(&m_flag, &data + size - so_flag, so_flag);

	append(data, size - so_flag);
}



const void* FPacket::onSend(std::size_t& size)
{
	*this << m_flag;

	size = getDataSize();
	return getData();
}



bool operator== (const FPacket& p1, const FPacket::PFlag& p2)
{
	return p1.flag() == p2;
}

bool operator!= (const FPacket& p1, const FPacket::PFlag& p2)
{
	return p1.flag() != p2;
}