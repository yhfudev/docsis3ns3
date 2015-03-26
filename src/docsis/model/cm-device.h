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
#ifndef CM_DEVICE_H
#define CM_DEVICE_H

#include <vector>
#include <list>
#include "docsis-enums.h"
#include "ns3/packet.h"
#include "ns3/net-device.h"
#include "ns3/node.h"
#include "ns3/mac48-address.h"
#include "ns3/traced-callback.h"
#include "ns3/nstime.h"

namespace ns3 {

  class Hfc;

  class CmDevice : public NetDevice
  {
  public:
    static TypeId GetTypeId (void);
    CmDevice ();
    ~CmDevice ();

    enum CmUpstreamState {
      kIdle,
      kDecision,
      kToSendRequest,
      kReqSend,
      kWaitForMap,
      kToSend,
      kContention,
      CmUpstreamStateCount
    };
    enum CmEvent {
      kNone,
      kNewPacket,
      kNewMap,
      kWaitToSend,
      kReadyToSend,
      EventCount
    };

    struct Slot {
      Slot() : startingTime(Seconds(0)), length(0) {}
      Time startingTime;
      uint16_t length;
    };
    struct ServiceStruct{
      uint32_t serviceId;
      uint32_t channel;
      Time timePerMinislot;
      DocsisUpstreamChannelMode mode;
      CmUpstreamState state;
      CmEvent currEvent;
      std::list<Slot> availableSlots;
      std::list<Packet> packetQueue;
    };

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

    void Receive(Ptr< Packet > packet, uint32_t channel);

    void SetTimeDistanceToCMTS(Time time);
    Time GetTimeDistanceToCMTS();

  private:
    void TransmitStart(Ptr< Packet > packet, uint32_t channel);
    void TransmitComplete(uint32_t channel);
    void ProcessPacket(Ptr< Packet > packet, uint32_t channel);
    void ProcessManagement(Ptr< Packet > packet, uint32_t channel);
    void ProcessMAP(Ptr< Packet > packet, uint32_t channel);
    void ProcessData(Ptr< Packet > packet, uint32_t channel);

    void ChangeState(std::list<ServiceStruct>::iterator service, CmEvent newEvent);
    void ProcessState(std::list<ServiceStruct>::iterator service);
    void ProcessIdle(std::list<ServiceStruct>::iterator service);
    void ProcessDecision(std::list<ServiceStruct>::iterator service);
    void ProcessToSendRequest(std::list<ServiceStruct>::iterator service);
    void ProcessRequestSend(std::list<ServiceStruct>::iterator service);
    void ProcessWaitForMap(std::list<ServiceStruct>::iterator service);
    void ProcessToSend(std::list<ServiceStruct>::iterator service);
    void ProcessContention(std::list<ServiceStruct>::iterator service);

    uint32_t m_channels;
    uint32_t* m_transferRate;
    uint32_t m_deviceIndex;
    uint32_t m_mtu;
    bool m_linkUp;
    Ptr<Node> m_node;
    Mac48Address m_address;
    Ptr<Hfc> m_channel;
    ReceiveCallback m_rxCallback;
    TracedCallback<> m_linkChangeCallbacks;
    std::list< Ptr<Packet> > m_packetQueue;
    std::vector<DocsisChannelStatus> m_uChannelStatus;
    std::list<ServiceStruct> m_services;
    Ptr<Packet> m_lastPacket;

    Time m_timeDistance;

    TracedCallback< Ptr<const Packet> > m_sendTrace;
    TracedCallback< Ptr<const Packet> > m_transmitStartTrace;
    TracedCallback< Ptr<const Packet> > m_transmitCompleteTrace;
    TracedCallback< Ptr<const Packet> > m_receiveTrace;
    TracedCallback< Ptr<const Hfc> > m_attachTrace;
    TracedCallback< Ptr<const Hfc> > m_deattachTrace;
    TracedCallback< Address > m_addressChangeTrace;
  };

}

#endif /* CM_DEVICE_H */

