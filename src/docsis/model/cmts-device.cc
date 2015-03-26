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
#include "cm-device.h"
#include "docsis-header.h"
#include "mac-management-message.h"
#include "hfc.h"
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

CmtsDevice::CmtsDevice () : m_deviceIndex(0), m_mtu(1), m_node(NULL), m_hfc(NULL), m_packetQueues(0), m_packetQueuesTransmissionEndTime(0), m_lastPackets(0), m_channelSelector(MakeNullCallback< TEMPLATE_SELECTOR_T >())
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
  return m_hfc;
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
  return m_hfc;
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
  // **** Headers section ****

  PacketAddress pa;
  pa.packet = packet;
  pa.address = dest;
  pa.channel = m_channelSelector.IsNull () ? m_downstreamServices[dest].begin()->channel : m_channelSelector(m_hfc, m_packetQueues, m_downstreamServices[dest]);

  m_packetQueues[pa.channel].push_back(pa);
  Time packetSizeInTime = Seconds (m_hfc->GetUpstreamDataRate(pa.channel).CalculateTxTime(packet->GetSize()));

  if (m_hfc->GetDownstreamChannelStatus (pa.channel) == kIdle)
    {
      m_packetQueuesTransmissionEndTime[pa.channel] = Simulator::Now () + packetSizeInTime;
      TransmitStart(m_packetQueues[pa.channel].front().packet, m_connectedDevices[m_packetQueues[pa.channel].front().address], m_packetQueues[pa.channel].front ().channel);
      m_packetQueues[pa.channel].pop_front();
    }
  else
    {
      m_packetQueuesTransmissionEndTime[pa.channel] += packetSizeInTime;
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

  if (!m_hfc)
    {
      Deattach();
    }

  m_hfc = channel;
  m_hfc->Attach(this);

  uint32_t downChannels = m_hfc->GetDownstreamChannelsAmount();
  uint32_t upChannels = m_hfc->GetUpstreamChannelsAmount ();

  m_packetQueues.resize ((int)downChannels);
  m_packetQueuesTransmissionEndTime.resize ((int)downChannels);
  m_lastPackets.resize ((int)downChannels);
  m_upChannelDescs.resize ((int)upChannels);
  m_downChannelDescs.resize ((int)downChannels);

  m_linkChangeCallbacks();
}

void
CmtsDevice::Deattach()
{
  NS_LOG_FUNCTION (this);
  m_deattachTrace(m_hfc);

  if (!m_hfc)
    return;

  m_hfc->Deattach(this);
  m_hfc = NULL;
  m_linkChangeCallbacks();
}

void
CmtsDevice::CmAttached(Ptr<CmDevice> cm)
{
  m_connectedDevices[cm->GetAddress()] = cm;

  UpServiceStruct defaultUpService;
  m_upstreamServices[cm->GetAddress()] = std::list<UpServiceStruct>();
  m_upstreamServices[cm->GetAddress()].push_back(defaultUpService);

  DownServiceStruct defaultDownService;
  m_downstreamServices[cm->GetAddress()] = std::list<DownServiceStruct>();
  m_downstreamServices[cm->GetAddress()].push_back(defaultDownService);
}

void
CmtsDevice::CmDeattached(Ptr<CmDevice> cm)
{
  m_connectedDevices.erase(cm->GetAddress());
  m_upstreamServices.erase (cm->GetAddress());
  m_downstreamServices.erase (cm->GetAddress());
}

void
CmtsDevice::CmChangedAddress(Ptr<CmDevice> cm, Address old_address)
{
  if (cm->GetAddress () == old_address) return;

  m_connectedDevices[cm->GetAddress()] = cm;
  m_upstreamServices[cm->GetAddress()] = m_upstreamServices[old_address];
  m_downstreamServices[cm->GetAddress()] = m_downstreamServices[old_address];

  m_connectedDevices.erase(old_address);
  m_upstreamServices.erase (old_address);
  m_downstreamServices.erase (old_address);
}

void
CmtsDevice::RegisterChannelSelector(selector_t selector)
{
  m_channelSelector = selector;
}

void
CmtsDevice::SetUpstreamChannelDescription(uint32_t channel, UpstreamChannelDescription desc)
{
  m_upChannelDescs[channel] = desc;
}

void
CmtsDevice::SetDownstreamChannelDescription(uint32_t channel, DownstreamChannelDescription desc)
{
  m_downChannelDescs[channel] = desc;
}

double
CmtsDevice::TimeToMinislot(uint32_t channel, Time time)
{
  return time.ToDouble (Time::S)/ m_upChannelDescs[channel].timePerMinislot.ToDouble(Time::S);
}

Time
CmtsDevice::MinislotToTime(uint32_t channel, uint32_t minislot)
{
  return Time::FromDouble (m_upChannelDescs[channel].timePerMinislot.ToDouble(Time::S) * minislot, Time::S); //TODO
}

void
CmtsDevice::ForceSendMAP()
{
  for(uint32_t i=0; i < m_hfc->GetUpstreamChannelsAmount (); i++)
    ForceSendMAP (i);
}

void
CmtsDevice::ForceSendMAP(uint32_t channel)
{
  SendMAP(channel);
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

Time
CmtsDevice::CalculateMaxRTT()
{
  Time maxTimeDistance;
  for (std::map< Address, Ptr<CmDevice> >::const_iterator pair = m_connectedDevices.begin (); pair != m_connectedDevices.end (); pair++)
    {
      Ptr<CmDevice> cmDevice = pair->second;
      if (cmDevice->GetTimeDistanceToCMTS () > maxTimeDistance)
        maxTimeDistance = cmDevice->GetTimeDistanceToCMTS ();
    }

  return Time::FromDouble (maxTimeDistance.GetDouble ()* 2, maxTimeDistance.GetResolution ());
}

Time
CmtsDevice::LatestMomentToSendMAP(Time startOfMAP)
{
  DataRate minDataRate = m_hfc->GetUpstreamDataRate (0);
  for (uint32_t channel = 1; channel < m_hfc->GetUpstreamChannelsAmount (); channel++)
    if (minDataRate < m_hfc->GetUpstreamDataRate (channel))
      minDataRate = m_hfc->GetUpstreamDataRate (channel);

  Time maxMAPTxTime = Time::FromDouble (minDataRate.CalculateTxTime (MacManagementMessageHeader::MAX_MMM_PACKET_SIZE), Time::S);
  return startOfMAP - maxMAPTxTime - CalculateMaxRTT ();
}

void
CmtsDevice::TransmitStart(Ptr< Packet > packet, Ptr<CmDevice> destiny, uint32_t channel)
{
  NS_LOG_FUNCTION (this << packet << destiny << channel);
  m_transmitStartTrace(packet);

  Time txTime = Seconds (m_hfc->GetUpstreamDataRate(channel).CalculateTxTime(packet->GetSize()));

  Simulator::Schedule(txTime, &CmtsDevice::TransmitComplete, this, channel);
  m_hfc->DownTransmitStart(channel, packet, destiny, txTime);

  m_lastPackets[channel] = packet;
}

void
CmtsDevice::TransmitComplete(uint32_t channel)
{
  NS_LOG_FUNCTION (this);
  m_transmitCompleteTrace(m_lastPackets[channel]);

  m_lastPackets[channel] = NULL;

  if (!m_packetQueues[channel].empty())
    {
      PacketAddress &pa = m_packetQueues[channel].front();

      TransmitStart(m_packetQueues[pa.packet, m_connectedDevices[pa.address], pa.channel);

      Time packetSizeInTime = Seconds (m_hfc->GetUpstreamDataRate(pa.channel).CalculateTxTime(pa.packet->GetSize()));
      m_packetQueuesTransmissionEndTime[pa.channel] += packetSizeInTime;

      m_packetQueues[channel].pop_front();
    }
}

void
CmtsDevice::SendMAP(uint32_t channel)
{
  UpstreamChannelDescription &ucd = m_upChannelDescs[channel];
  MAPHeader mh;

  mh.SetupMAP (channel, 0, ucd.lastMinislotGrantSent+1, ucd.lastMinislotRequestReceived, 0,0,0,0);

  uint16_t slotNbr = 0;
  while (ucd.grants.size () > 0)
    {
      MAPHeader::InformationElement ie;
      Grant grant = *ucd.grants.begin ();

      ie.m_offset = slotNbr; slotNbr += grant.slots;
      ie.m_sid = grant.sid;
      ie.m_type = grant.type;

      mh.AddIE (ie);
      ucd.grants.pop_front ();
    }

  MAPHeader::InformationElement nullIE;
  nullIE.m_offset = slotNbr;
  nullIE.m_sid = 0;
  nullIE.m_type = MAPHeader::kNull;
  mh.AddIE (nullIE);
}

}
