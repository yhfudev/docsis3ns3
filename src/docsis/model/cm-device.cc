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
#include "cm-device.h"
#include "hfc.h"
#include "ns3/simulator.h"
#include "ns3/nstime.h"

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

CmDevice::CmDevice () : m_channels(0), m_transferRate(NULL), m_deviceIndex(0), m_mtu(1), m_linkUp(false), m_node(NULL), m_address(NULL), m_channel(NULL), m_uChannelStatus(kDown)
{
}

CmDevice::~CmDevice ()
{
}

void
CmDevice::AddLinkChangeCallback (Callback<void> callback)
{
	m_linkChangeCallbacks.ConnectWithoutContext(callback);
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
	m_packetQueue.push_back(packet);
	if (m_uChannelStatus == kIdle)
	{
		TransmitStart(m_packetQueue.front());
		m_packetQueue.pop_front();
	}
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
	m_rxCallback = cb;
}


bool
CmDevice::SupportsSendFrom (void) const
{
	return false;
}

void
CmDevice::Attach(Ptr<Hfc> channel)
{
	if (!m_channel)
	{
		Deattach();
	}

	m_channel = channel;
	m_channel->Attach(this);
	m_uChannelStatus = kIdle;
	m_linkUp = true;
	m_linkChangeCallbacks();
}

void
CmDevice::Deattach()
{
	if (!m_channel)
		return;

	m_channel->Deattach(this);
	m_channel = NULL;
	m_uChannelStatus = kDown;
	m_linkUp = false;
	m_linkChangeCallbacks();
}

bool
CmDevice::Receive(Ptr< Packet > packet)
{
	uint16_t protocol = 0;
	Address address;

	return m_rxCallback(this, packet, protocol, address);
}

void
CmDevice::TransmitStart(Ptr< Packet > packet)
{
	m_uChannelStatus = kBusy;

	Time txTime = Seconds (m_channel->GetUpstreamDataRate(0).CalculateTxTime(packet->GetSize()));

	Simulator::Schedule(txTime, &CmDevice::TransmitComplete, this);
	m_channel->UpTransmitStart(0, packet, this, txTime);
}

void
CmDevice::TransmitComplete()
{
	m_uChannelStatus = kIdle;
	if (!m_packetQueue.empty())
	{
		TransmitStart(m_packetQueue.front());
		m_packetQueue.pop_front();
	}
}

}
