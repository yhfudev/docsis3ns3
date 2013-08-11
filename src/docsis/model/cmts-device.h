/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/*
 * Copyright (c) 2013 Martín Javier Di Liscia
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation;
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 * Author: Martín Javier Di Liscia
 */
#ifndef CMTS_DEVICE_H
#define CMTS_DEVICE_H

#include <map>
#include "docsis-enums.h"
#include "ns3/net-device.h"
#include "ns3/node.h"
#include "ns3/mac48-address.h"
#include "ns3/traced-callback.h"

namespace ns3 {

class Hfc;
class CmDevice;

struct PacketAddress
{
	Ptr<Packet> packet;
	Address address;
};

class CmtsDevice : public NetDevice
{
public:
	static TypeId GetTypeId (void);
	CmtsDevice ();
	~CmtsDevice ();

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

	void Attach(Ptr<Hfc> channel);
	void Deattach();
	void CmAttached(Ptr<CmDevice> cm);
	void CmDeattached(Ptr<CmDevice> cm);
	void CmChangedAddress(Ptr<CmDevice> cm, Address old_address);

	bool Receive(Ptr<Packet> packet, Ptr<CmDevice> sender);
	void TransmitStart(Ptr< Packet > packet, Ptr<CmDevice> destiny);
	void TransmitComplete();

private:
	uint32_t m_channels;
	uint32_t* m_transferRate;
	uint32_t m_deviceIndex;
	uint32_t m_mtu;
	bool m_linkUp;
	Ptr<Node> m_node;
	Mac48Address m_address;
	Ptr<Hfc> m_channel;
	TracedCallback<> m_linkChangeCallbacks;
	ReceiveCallback m_rxCallback;
	std::list< PacketAddress > m_packetQueue;
	std::vector<DocsisChannelStatus> m_dChannelsStatus;
	std::map< Address, Ptr<CmDevice> > m_connectedDevices;
	Ptr<Packet> m_lastPacket;

	TracedCallback< Ptr<const Packet> > m_sendTrace;
	TracedCallback< Ptr<const Packet> > m_transmitStartTrace;
	TracedCallback< Ptr<const Packet> > m_transmitCompleteTrace;
	TracedCallback< Ptr<const Packet> > m_receiveTrace;
	TracedCallback< Ptr<const Hfc> > m_attachTrace;
	TracedCallback< Ptr<const Hfc> > m_deattachTrace;
	TracedCallback< Address > m_addressChangeTrace;
};

}

#endif /* CMTS_DEVICE_H */

