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
#include "mac-management-message.h"

NS_LOG_COMPONENT_DEFINE ("MacManagementMessageHeader");

namespace ns3 {

  // ************* MacManagementMessageHeader ***********************
  uint32_t MacManagementMessageHeader::Deserialize (Buffer::Iterator start) {
    uint8_t buffer[6];

    start.Read (buffer, 6);
    m_destinationAddress.CopyFrom (buffer);

    start.Read (buffer, 6);
    m_sourceAddress.CopyFrom (buffer);

    m_length = start.ReadU16();
    start.ReadU16(); start.ReadU8();
    m_version = start.ReadU8();
    m_type = (MmmType)start.ReadU8();
    start.ReadU8();

    return 18;
  }

  uint32_t MacManagementMessageHeader::GetSerializedSize (void) const {
    return 20;
  }

  void MacManagementMessageHeader::Print (std::ostream &os) const {

  }

  void MacManagementMessageHeader::Serialize (Buffer::Iterator start) const {
    uint8_t buffer[6];

    m_destinationAddress.CopyTo (buffer);
    start.Write (buffer, 6);

    m_sourceAddress.CopyTo (buffer);
    start.Write (buffer, 6);

    start.WriteU16(m_length);
    start.WriteU16(0x0000); // DSAP & SSAP
    start.WriteU8(0x03);	// Control
    start.WriteU8(m_version);
    start.WriteU8((uint8_t)m_type);
    start.WriteU8(0x00);	// RSVD
  }

  TypeId MacManagementMessageHeader::GetTypeId (void) {
    static TypeId tid = TypeId ("ns3::MacManagementMessageHeader")
        .SetParent<Header> ()
        .AddConstructor<MacManagementMessageHeader> ();

    return tid;
  }

  TypeId MacManagementMessageHeader::GetInstanceTypeId (void) const
  {
    return GetTypeId();
  }

  bool MacManagementMessageHeader::IsMAPPacket(void) const
  {
    return m_type == kMAP;
  }

  bool MacManagementMessageHeader::IsValidDestination(Mac48Address address) const
  {
    return address == m_destinationAddress;
  }

  // ************* MAPHeader ****************************************
  uint32_t MAPHeader::Deserialize (Buffer::Iterator start) {
    m_ucId = start.ReadU8();
    m_ucdCount = start.ReadU8();
    m_elementCount = start.ReadU8();
    start.ReadU8();

    m_startTime = start.ReadU32();
    m_ackTime = start.ReadU32();

    m_rangingStart = start.ReadU8();
    m_rangingEnd = start.ReadU8();
    m_dataStart = start.ReadU8();
    m_dataEnd = start.ReadU8();

    m_ies.clear();
    for(uint8_t i=0; i < m_elementCount; i++) {
        InformationElement ie;
        ie.m_sid = start.ReadU16();
        ie.m_offset = start.ReadU16();

        ie.m_type = (IEType)( ((ie.m_sid & 0x3)<<2) + (ie.m_offset>>14) );
        ie.m_sid = ie.m_sid>>2;
        ie.m_offset &= 0x3FFF;

        m_ies.push_back(ie);
      }

    return 16 + m_elementCount*4;
  }

  uint32_t MAPHeader::GetSerializedSize (void) const {
    return 16 + m_ies.size()*4;
  }

  void MAPHeader::Print (std::ostream &os) const {

  }

  void MAPHeader::Serialize (Buffer::Iterator start) const {
    start.WriteU8(m_ucId);
    start.WriteU8(m_ucdCount);
    start.WriteU8(m_elementCount);
    start.WriteU8(0);

    start.WriteU32(m_startTime);
    start.WriteU32(m_ackTime);

    start.WriteU8(m_rangingStart);
    start.WriteU8(m_rangingEnd);
    start.WriteU8(m_dataStart);
    start.WriteU8(m_dataEnd);

    for(InfoElementIterator ie=m_ies.begin(); ie != m_ies.end(); ie++) {
        start.WriteU16( (ie->m_sid<<2) + ((uint8_t)(ie->m_type)>>2) );
        start.WriteU16( ie->m_offset + ( ((uint8_t)(ie->m_type)&0x03) << 14) );
      }
  }

  TypeId MAPHeader::GetTypeId (void) {
    static TypeId tid = TypeId ("ns3::MAPHeader")
        .SetParent<Header> ()
        .AddConstructor<MAPHeader> ();

    return tid;
  }

  TypeId MAPHeader::GetInstanceTypeId (void) const
  {
    return GetTypeId();
  }

  MAPHeader::InfoElementIterator
  MAPHeader::InfoElementBegin() const
  {
    return m_ies.begin ();
  }

  MAPHeader::InfoElementIterator
  MAPHeader::InfoElementEnd() const
  {
    return m_ies.end ();
  }

}
