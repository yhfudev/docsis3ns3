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
#include "cm-device.h"
#include "hfc.h"
#include "ns3/simulator.h"
#include "docsis-header.h"
#include "mac-management-message.h"
#include "ns3/llc-snap-header.h"

NS_LOG_COMPONENT_DEFINE ("CmNetDevice");

namespace ns3 {

  TypeId
  CmDevice::GetTypeId (void)
  {
    static TypeId tid = TypeId("ns3::CmDevice")
        .SetParent<NetDevice> ()
        .AddConstructor<CmDevice> ()
        .AddTraceSource("MacTx",
                        "Trace source indicating a packet has arrived for transmission by this device",
                        MakeTraceSourceAccessor(&CmDevice::m_sendTrace) )
        .AddTraceSource("MacRx",
                        "A packet has been received by this device, has been passed up from the physical layer "
                        "and is being forwarded up the local protocol stack.  This is a non-promiscuous trace,",
                        MakeTraceSourceAccessor(&CmDevice::m_receiveTrace) )
        .AddTraceSource("PhyTxBegin",
                        "Trace source indicating a packet has begun transmitting over the channel",
                        MakeTraceSourceAccessor(&CmDevice::m_transmitStartTrace) )
        .AddTraceSource("PhyTxEnd",
                        "Trace source indicating a packet has been completely transmitted over the channel",
                        MakeTraceSourceAccessor(&CmDevice::m_transmitCompleteTrace) )
        .AddTraceSource("ChannelConnect",
                        "Trace source indicating a connection to a channel",
                        MakeTraceSourceAccessor(&CmDevice::m_attachTrace) )
        .AddTraceSource("ChannelDisconnect",
                        "Trace source indicating a disconnection from a channel",
                        MakeTraceSourceAccessor(&CmDevice::m_deattachTrace) )
        .AddTraceSource("AddressChange",
                        "Trace source indicating an address change",
                        MakeTraceSourceAccessor(&CmDevice::m_addressChangeTrace) )
        ;

    return tid;
  }

  CmDevice::CmDevice () : m_channels(0), m_transferRate(NULL), m_deviceIndex(0),
                          m_mtu(1), m_linkUp(false), m_node(NULL),
                          m_channel(NULL), m_uChannelStatus(0), m_lastPacket(),
                          m_timeDistance(0)
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
    NS_LOG_FUNCTION (this << packet << dest << protocolNumber);
    m_sendTrace(packet);

    m_packetQueue.push_back(packet);
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
    NS_LOG_FUNCTION (this << address);
    m_addressChangeTrace(address);

    Address old_address = m_address;
    m_address = Mac48Address::ConvertFrom (address);
    m_channel->CmChangedAddress(this, old_address);
  }


  void
  CmDevice::SetIfIndex (const uint32_t index)
  {
    NS_LOG_FUNCTION (this << index);
    m_deviceIndex = index;
  }


  bool
  CmDevice::SetMtu (const uint16_t mtu)
  {
    NS_LOG_FUNCTION (this << mtu);
    m_mtu = mtu;
    return true;
  }


  void
  CmDevice::SetNode (Ptr< Node > node)
  {
    NS_LOG_FUNCTION (this << node);
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
    NS_LOG_FUNCTION (this << channel);
    m_attachTrace(channel);

    if (!m_channel)
      {
        Deattach();
      }

    m_channel = channel;
    m_channel->Attach(this);
    m_uChannelStatus.resize((int)m_channel->GetUpstreamChannelsAmount());
    m_linkUp = true;
    m_linkChangeCallbacks();
  }

  void
  CmDevice::Deattach()
  {
    NS_LOG_FUNCTION (this);
    m_deattachTrace(m_channel);

    if (!m_channel)
      return;

    m_channel->Deattach(this);
    m_channel = NULL;
    m_uChannelStatus.resize(0);
    m_linkUp = false;
    m_linkChangeCallbacks();
  }

  void
  CmDevice::Receive(Ptr< Packet > packet, uint32_t channel)
  {
    NS_LOG_FUNCTION (this << packet);
    m_receiveTrace(packet);

    ProcessPacket(packet, channel);
  }

  void
  CmDevice::SetTimeDistanceToCMTS(Time time)
  {
    m_timeDistance = time;
  }

  Time
  CmDevice::GetTimeDistanceToCMTS()
  {
    return m_timeDistance;
  }

  void
  CmDevice::TransmitStart(Ptr< Packet > packet, uint32_t channel)
  {
    NS_LOG_FUNCTION (this << packet);
    m_transmitStartTrace(packet);

    Time txTime = Seconds (m_channel->GetUpstreamDataRate(channel).CalculateTxTime(packet->GetSize()));

    Simulator::Schedule(txTime, &CmDevice::TransmitComplete, this, channel);
    m_channel->UpTransmitStart(channel, packet, this, txTime);

    m_lastPacket = packet;
  }

  void
  CmDevice::TransmitComplete(uint32_t channel)
  {
    NS_LOG_FUNCTION (this);
    m_transmitCompleteTrace(m_lastPacket);
    m_lastPacket = NULL;

    if (!m_packetQueue.empty())
      {
        TransmitStart(m_packetQueue.front(), channel);
        m_packetQueue.pop_front();
      }
  }

  void
  CmDevice::ProcessPacket(Ptr< Packet > packet, uint32_t channel)
  {
    DocsisHeader dh;
    packet->RemoveHeader(dh);

    if (dh.IsDataPacket())
      ProcessData(packet, channel);

    if (dh.IsManagementPacket())
      ProcessManagement(packet, channel);
  }

  void
  CmDevice::ProcessManagement(Ptr< Packet > packet, uint32_t channel)
  {
    MacManagementMessageHeader mmmh;
    packet->RemoveHeader (mmmh);

    if (!mmmh.IsValidDestination (m_address)) return;

    if (mmmh.IsMAPPacket ())
      ProcessMAP (packet, channel);
  }

  void
  CmDevice::ProcessMAP(Ptr< Packet > packet, uint32_t channel)
  {
    MAPHeader mh;
    packet->RemoveHeader (mh);

    for(MAPHeader::InfoElementIterator ies=mh.InfoElementBegin (); ies != mh.InfoElementEnd (); ies++)
      {
        if (ies->m_type == MAPHeader::kShortDataGrant || ies->m_type == MAPHeader::kLargeDataGrant)
          {
            for (std::list<ServiceStruct>::iterator service = m_services.begin (); service != m_services.end (); service++)\
              {
                if (service->channel == mh.GetUpstreamChannelId () && service->serviceId == ies->m_sid)
                  {
                    Slot slot;
                    MAPHeader::InfoElementIterator nextInfo = ies; nextInfo++;

                    slot.startingTime = Time(service->timePerMinislot.GetDouble() * mh.GetSlotNumber (*ies));
                    slot.length = nextInfo->m_offset - ies->m_offset;

                    service->availableSlots.push_back(slot);
                  }
              }
          }
      }
  }

  void
  CmDevice::ProcessData(Ptr< Packet > packet, uint32_t channel)
  {
    PDUHeader pduh;
    packet->RemoveHeader (pduh);
    packet->RemoveAtEnd (4);
    if (pduh.GetDestination () != m_address) return;

    uint16_t protocol = pduh.GetTypeLength ();
    if (protocol <= 1500)
      {
        uint32_t padlen = packet->GetSize () - protocol;
        if (padlen > 0) packet->RemoveAtEnd (padlen);

        LlcSnapHeader llc;
        packet->RemoveHeader (llc);
        protocol = llc.GetType ();
      }

    m_rxCallback(this, packet, protocol, pduh.GetSource ());
  }

}
