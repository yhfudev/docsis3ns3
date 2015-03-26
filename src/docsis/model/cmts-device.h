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
#include "mac-management-message.h"
#include "ns3/net-device.h"
#include "ns3/node.h"
#include "ns3/mac48-address.h"
#include "ns3/traced-callback.h"
#include "ns3/simulator.h"

namespace ns3 {

class Hfc;
class CmDevice;

struct PacketAddress
{
  Ptr<Packet> packet;
  Address address;
  uint32_t channel;
};

class CmtsDevice : public NetDevice
{
public:
  static TypeId GetTypeId (void);
  CmtsDevice ();
  ~CmtsDevice ();

  struct Grant
  {
    uint16_t sid;
    uint16_t slots;
    MAPHeader::IEType type;
  };

  struct UpstreamChannelDescription
  {
    Time timePerMinislot;

    std::list<Grant> grants;
    uint32_t lastMinislotGrantSent;
    uint32_t lastMinislotRequestReceived;
  };
  struct DownstreamChannelDescription
  {
  };

  struct UpServiceStruct
  {
    UpServiceStruct() : serviceId(0), channel(0), mode(kUnsolicitedGrant) {}
    uint32_t serviceId;
    uint32_t channel;
    DocsisUpstreamChannelMode mode;
  };
  struct DownServiceStruct{
    DownServiceStruct() : serviceId(0), channel(0) {}
    uint32_t serviceId;
    uint32_t channel;
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
  void CmAttached(Ptr<CmDevice> cm);
  void CmDeattached(Ptr<CmDevice> cm);
  void CmChangedAddress(Ptr<CmDevice> cm, Address old_address);

#define TEMPLATE_SELECTOR_T uint32_t, Ptr<Hfc>, std::vector< std::list< PacketAddress > >, std::list<DownServiceStruct>
typedef Callback< TEMPLATE_SELECTOR_T > selector_t;
  void RegisterChannelSelector(selector_t selector);
  void SetUpstreamChannelDescription(uint32_t channel, UpstreamChannelDescription desc);
  void SetDownstreamChannelDescription(uint32_t channel, DownstreamChannelDescription desc);

  double TimeToMinislot(uint32_t channel, Time time);
  Time MinislotToTime(uint32_t channel, uint32_t minislot);
  void ForceSendMAP();
  void ForceSendMAP(uint32_t channel);
  bool Receive(Ptr<Packet> packet, Ptr<CmDevice> sender);

private:
  Time CalculateMaxRTT();
  Time LatestMomentToSendMAP(Time startOfMAP);

  void TransmitStart(Ptr< Packet > packet, Ptr<CmDevice> destiny, uint32_t channel);
  void TransmitComplete(uint32_t channel);
  void SendMAP(uint32_t channel);

  uint32_t m_downstreamOverhead;
  bool m_useLLC;
  Time m_startupTime;

  uint32_t m_deviceIndex;
  uint32_t m_mtu;
  Ptr<Node> m_node;
  Mac48Address m_address;
  Ptr<Hfc> m_hfc;
  TracedCallback<> m_linkChangeCallbacks;
  ReceiveCallback m_rxCallback;
  std::vector< std::list< PacketAddress > > m_packetQueues;
  std::vector< Time > m_packetQueuesTransmissionEndTime;
  std::vector< UpstreamChannelDescription > m_upChannelDescs;
  std::vector< DownstreamChannelDescription > m_downChannelDescs;
  std::map< Address, Ptr<CmDevice> > m_connectedDevices;
  std::map< Address, std::list<UpServiceStruct> > m_upstreamServices;
  std::map< Address, std::list<DownServiceStruct> > m_downstreamServices;
  std::vector< Ptr<Packet> > m_lastPackets;

  selector_t m_channelSelector;

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

