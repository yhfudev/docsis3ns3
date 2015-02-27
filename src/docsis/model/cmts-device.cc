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

#include "ns3/log.h"
#include "cmts-device.h"
#include "docsis-header.h"
#include "hfc.h"
#include "ns3/simulator.h"
#include "ns3/llc-snap-header.h"

NS_LOG_COMPONENT_DEFINE ("CmtsNetDevice");

namespace ns3 {

TypeId
CmtsDevice::GetTypeId (void)
{
	static TypeId tid = TypeId("ns3::CmtsDevice")
		.SetParent<NetDevice> ()
		.AddConstructor<CmtsDevice> ()
		.AddTraceSource("MacTx",
						"Trace source indicating a packet has arrived for transmission by this device",
						MakeTraceSourceAccessor(&CmtsDevice::m_sendTrace) )
		.AddTraceSource("MacRx",
						"A packet has been received by this device, has been passed up from the physical layer "
                     	"and is being forwarded up the local protocol stack.  This is a non-promiscuous trace,",
						MakeTraceSourceAccessor(&CmtsDevice::m_receiveTrace) )
		.AddTraceSource("PhyTxBegin",
						"Trace source indicating a packet has begun transmitting over the channel",
						MakeTraceSourceAccessor(&CmtsDevice::m_transmitStartTrace) )
		.AddTraceSource("PhyTxEnd",
						"Trace source indicating a packet has been completely transmitted over the channel",
						MakeTraceSourceAccessor(&CmtsDevice::m_transmitCompleteTrace) )
		.AddTraceSource("ChannelConnect",
						"Trace source indicating a connection to a channel",
						MakeTraceSourceAccessor(&CmtsDevice::m_attachTrace) )
		.AddTraceSource("ChannelDisconnect",
						"Trace source indicating a disconnection from a channel",
						MakeTraceSourceAccessor(&CmtsDevice::m_deattachTrace) )
		.AddTraceSource("AddressChange",
						"Trace source indicating an address change",
						MakeTraceSourceAccessor(&CmtsDevice::m_addressChangeTrace) )
		;

	return tid;
}

CmtsDevice::CmtsDevice () : m_channels(0), m_transferRate(NULL), m_deviceIndex(0), m_mtu(1), m_linkUp(false), m_node(NULL), m_channel(NULL), m_dChannelsStatus(0), m_lastPacket(NULL)
{
}

CmtsDevice::~CmtsDevice ()
{
}

void
CmtsDevice::AddLinkChangeCallback (Callback<void> callback)
{
	m_linkChangeCallbacks.ConnectWithoutContext(callback);
}


Address
CmtsDevice::GetAddress (void) const
{
	return m_address;
}


Address
CmtsDevice::GetBroadcast (void) const
{
	return Mac48Address ("ff:ff:ff:ff:ff:ff");
}


Ptr< Channel >
CmtsDevice::GetChannel (void) const
{
	return m_channel;
}


uint32_t
CmtsDevice::GetIfIndex (void) const
{
	return m_deviceIndex;
}


uint16_t
CmtsDevice::GetMtu (void) const
{
	return m_mtu;
}


Address
CmtsDevice::GetMulticast (Ipv4Address multicastGroup) const
{
	return Mac48Address ("00:00:00:00:00:00");
}


Address
CmtsDevice::GetMulticast (Ipv6Address addr) const
{
	return Mac48Address ("00:00:00:00:00:00");
}


Ptr< Node >
CmtsDevice::GetNode (void) const
{
	return m_node;
}


bool
CmtsDevice::IsBridge (void) const
{
	return false;
}


bool
CmtsDevice::IsBroadcast (void) const
{
	return false;
}


bool
CmtsDevice::IsLinkUp (void) const
{
	return m_linkUp;
}


bool
CmtsDevice::IsMulticast (void) const
{
	return false;
}


bool
CmtsDevice::IsPointToPoint (void) const
{
	return true;
}


bool
CmtsDevice::NeedsArp (void) const
{
	return false;
}


bool
CmtsDevice::Send (Ptr< Packet > packet, const Address &dest, uint16_t protocolNumber)
{
  return SendFrom(packet, m_address, dest, protocolNumber);
}


bool
CmtsDevice::SendFrom (Ptr< Packet > packet, const Address &source, const Address &dest, uint16_t protocolNumber)
{
  NS_LOG_FUNCTION (this << packet << dest << protocolNumber);
  m_sendTrace(packet);

  // **** Headers section ****
  uint16_t typeLength = protocolNumber;
  if (m_useLLC)
    {
      LlcSnapHeader llc;
      llc.SetType (protocolNumber);
      packet->AddHeader (llc);

      typeLength = packet->GetSize ();
    }

  PDUHeader pduh;
  pduh.Setup (m_address, Mac48Address::ConvertFrom (dest), typeLength);
  packet->AddHeader (pduh);

  DocsisHeader dh;
  dh.setupPduPacket (m_downstreamOverhead, packet->GetSize (), kDownstream);
  packet->AddHeader (dh);

  PacketAddress pa;
  pa.packet = packet;
  pa.address = dest;
  // **** Headers section ****

  m_packetQueue.push_back(pa);
  if (m_dChannelsStatus[0] == kIdle)
  {
    TransmitStart(m_packetQueue.front().packet, m_connectedDevices[m_packetQueue.front().address]);
    m_packetQueue.pop_front();
  }
  return true;
}


void
CmtsDevice::SetAddress (Address address)
{
	NS_LOG_FUNCTION (this << address);
	m_addressChangeTrace(address);

	m_address = Mac48Address::ConvertFrom (address);
}


void
CmtsDevice::SetIfIndex (const uint32_t index)
{
	NS_LOG_FUNCTION (this << index);

	m_deviceIndex = index;
}


bool
CmtsDevice::SetMtu (const uint16_t mtu)
{
	NS_LOG_FUNCTION (this << mtu);

	m_mtu = mtu;
	return true;
}


void
CmtsDevice::SetNode (Ptr< Node > node)
{
	NS_LOG_FUNCTION (this << node);

	m_node = node;
}


void
CmtsDevice::SetPromiscReceiveCallback (PromiscReceiveCallback cb)
{
	return;
}


void
CmtsDevice::SetReceiveCallback (ReceiveCallback cb)
{
	m_rxCallback = cb;
}


bool
CmtsDevice::SupportsSendFrom (void) const
{
	return false;
}

void
CmtsDevice::Attach(Ptr<Hfc> channel)
{
	NS_LOG_FUNCTION (this << channel);
	m_attachTrace(channel);

	if (!m_channel)
	{
		Deattach();
	}

	m_channel = channel;
	m_channel->Attach(this);

	m_dChannelsStatus.resize((int)m_channel->GetDownstreamChannelsAmount());

	m_linkUp = true;
	m_linkChangeCallbacks();
}

void
CmtsDevice::Deattach()
{
	NS_LOG_FUNCTION (this);
	m_deattachTrace(m_channel);

	if (!m_channel)
		return;

	m_channel->Deattach(this);
	m_channel = NULL;
	m_linkUp = false;
	m_linkChangeCallbacks();
}

void
CmtsDevice::CmAttached(Ptr<CmDevice> cm)
{
	m_connectedDevices[cm->GetAddress()] = cm;
}

void
CmtsDevice::CmDeattached(Ptr<CmDevice> cm)
{
	m_connectedDevices.erase(cm->GetAddress());
}

void
CmtsDevice::CmChangedAddress(Ptr<CmDevice> cm, Address old_address)
{
	m_connectedDevices.erase(old_address);
	m_connectedDevices[cm->GetAddress()] = cm;
}

bool
CmtsDevice::Receive(Ptr< Packet > packet, Ptr<CmDevice> sender)
{
	NS_LOG_FUNCTION (this << packet << sender);
	m_receiveTrace(packet);

	uint16_t protocol = 0;
	Address address;
	return m_rxCallback(this, packet, protocol, address);
}

void
CmtsDevice::TransmitStart(Ptr< Packet > packet, Ptr<CmDevice> destiny)
{
	NS_LOG_FUNCTION (this << packet << destiny);
	m_transmitStartTrace(packet);

	m_dChannelsStatus[0] = kBusy;

	Time txTime = Seconds (m_channel->GetUpstreamDataRate(0).CalculateTxTime(packet->GetSize()));

	Simulator::Schedule(txTime, &CmtsDevice::TransmitComplete, this);
	m_channel->DownTransmitStart(0, packet, destiny, txTime);

	m_lastPacket = packet;
}

void
CmtsDevice::TransmitComplete()
{
	NS_LOG_FUNCTION (this);
	m_transmitCompleteTrace(m_lastPacket);

	m_lastPacket = NULL;

	m_dChannelsStatus[0] = kIdle;
	if (!m_packetQueue.empty())
	{
		TransmitStart(m_packetQueue.front().packet, m_connectedDevices[m_packetQueue.front().address]);
		m_packetQueue.pop_front();
	}
}

}
