/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
#ifndef CM_DEVICE_H
#define CM_DEVICE_H

#include "hfc.h"
#include "ns3/net-device.h"
#include "ns3/node.h"
#include "ns3/mac48-address.h"

namespace ns3 {

class HFC;

class CmDevice : public NetDevice
{
public:
	static TypeId GetTypeId (void);
	CmDevice ();
	~CmDevice ();
	
	void AddLinkChangeCallback (Callback<void> callback);
	Address GetAddress (void) const;
	Address GetBroadcast (void) const;
	Ptr< Channel > 	GetChannel (void) const;
	uint32_t GetIfIndex (void) const;
	uint16_t GetMtu (void) const;
	Address GetMulticast (Ipv4Address multicastGroup) const;
	Address GetMulticast (Ipv6Address addr) const;
	Ptr< Node > GetNode (void) const;
	bool IsBridge (void) const;
	bool IsBroadcast (void) const;
	bool IsLinkUp (void) const;
	bool IsMulticast (void) const;
	bool IsPointToPoint (void) const;
	bool NeedsArp (void) const;
	bool Send (Ptr< Packet > packet, const Address &dest, uint16_t protocolNumber);
	bool SendFrom (Ptr< Packet > packet, const Address &source, const Address &dest, uint16_t protocolNumber);
	void SetAddress (Address address);
	void SetIfIndex (const uint32_t index);
	bool SetMtu (const uint16_t mtu);
	void SetNode (Ptr< Node > node);
	void SetPromiscReceiveCallback (PromiscReceiveCallback cb);
	void SetReceiveCallback (ReceiveCallback cb);
	bool SupportsSendFrom (void) const;
	
private:
	uint32_t m_channels;
	uint32_t* m_transferRate;
	uint32_t m_deviceIndex;
	uint32_t m_mtu;
	bool m_linkUp;
	Ptr<Node> m_node;
	Mac48Address m_address;
	Ptr<HFC> m_channel;
};

}

#endif /* CM_DEVICE_H */

