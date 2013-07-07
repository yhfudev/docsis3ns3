/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */

#include "cm.h"

namespace ns3 {

TypeId
CmDevice::GetTypeId (void)
{
	static TypeId tid = TypeId("ns3::CmDevice")
		.SetParent<NetDevice> ()
		.AddConstructor<CmDevice> ()
		;
	
	return tid;
}

CmDevice::CmDevice () : m_channels(0), m_transferRate(NULL), m_deviceIndex(0), m_mtu(1), m_linkUp(false), m_node(NULL), m_address(NULL), m_channel(NULL)
{
}

CmDevice::~CmDevice ()
{
}

void
CmDevice::AddLinkChangeCallback (Callback<void> callback)
{
	// TODO
}


Address
CmDevice::GetAddress (void) const
{
	return m_address;
}


Address
CmDevice::GetBroadcast (void) const
{
	return Mac48Address ("ff:ff:ff:ff:ff:ff");
}


Ptr< Channel >
CmDevice::GetChannel (void) const
{
	return m_channel;
}


uint32_t
CmDevice::GetIfIndex (void) const
{
	return m_deviceIndex;
}


uint16_t
CmDevice::GetMtu (void) const
{
	return m_mtu;
}


Address
CmDevice::GetMulticast (Ipv4Address multicastGroup) const
{
	return Mac48Address ("00:00:00:00:00:00");
}


Address
CmDevice::GetMulticast (Ipv6Address addr) const
{
	return Mac48Address ("00:00:00:00:00:00");
}


Ptr< Node >
CmDevice::GetNode (void) const
{
	return m_node;
}


bool
CmDevice::IsBridge (void) const
{
	return false;
}


bool
CmDevice::IsBroadcast (void) const
{
	return false;
}


bool
CmDevice::IsLinkUp (void) const
{
	return m_linkUp;
}


bool
CmDevice::IsMulticast (void) const
{
	return false;
}


bool
CmDevice::IsPointToPoint (void) const
{
	return true;
}


bool
CmDevice::NeedsArp (void) const
{
	return false;
}


bool
CmDevice::Send (Ptr< Packet > packet, const Address &dest, uint16_t protocolNumber)
{
	return true;
}


bool
CmDevice::SendFrom (Ptr< Packet > packet, const Address &source, const Address &dest, uint16_t protocolNumber)
{
	return true;
}


void
CmDevice::SetAddress (Address address)
{
	m_address = Mac48Address::ConvertFrom (address);
}


void
CmDevice::SetIfIndex (const uint32_t index)
{
	m_deviceIndex = index;
}


bool
CmDevice::SetMtu (const uint16_t mtu)
{
	m_mtu = mtu;
	return true;
}


void
CmDevice::SetNode (Ptr< Node > node)
{
	m_node = node;
}


void
CmDevice::SetPromiscReceiveCallback (PromiscReceiveCallback cb)
{
	return;
}


void
CmDevice::SetReceiveCallback (ReceiveCallback cb)
{
	return;
}


bool
CmDevice::SupportsSendFrom (void) const
{
	return false;
}

}
