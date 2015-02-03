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
#include "docsis.h"
#include "docsis-header.h"

namespace ns3 {
	
	uint32_t DocsisHeader::Deserialize (Buffer::Iterator start)
	{
		uint32_t readBytes = 0;
		
		
		uint8_t buffer[m_phyOverhead];
		start.Read(buffer, m_phyOverhead);	//PHY Overhead
		readBytes += m_phyOverhead;
		
		
		buffer[0] = start.ReadU8();	//FC
		m_fcType = (FrameControlType)(buffer[0] >> 6);
		m_macType = (MacHeaderType)((buffer[0] & 0x3E) >> 1);
		m_extendedHeaderPresent = (buffer[0] & 0x1) == 1;
		readBytes++;
		
		
		if (m_fcType == kMacSpecific && m_macType == kRequest)
			m_requestedSlots = start.ReadU8();
		else if (m_fcType == kMacSpecific && m_macType == kQdbRequest)
			m_qdbRequestedSlots = start.ReadU16();
		else if (m_fcType == kMacSpecific && m_macType == kConcatenation)
			m_concatenatedPackets = start.ReadU8();
		else
			m_extendedHeaderLength = start.ReadU8();
		
		if (m_fcType == kMacSpecific && m_macType == kQdbRequest)
			readBytes += 2;
		else
			readBytes++;
		
		
		m_headerLength = start.ReadU16() - m_extendedHeaderLength;	//LEN
		readBytes += 2;
		
		if (m_extendedHeaderPresent)
			readBytes += m_extendedHeader.Deserialize(m_extendedHeaderLength, start); // EHDR
		
		
		start.ReadU16();	//HCS
		readBytes += 2;
		
		
		return readBytes;
	}
	
	uint32_t DocsisHeader::GetSerializedSize (void) const
	{
		uint32_t size=0;
		
		switch(m_fcType) {
			case kPacketPDU:
			case kIsolationPacketPDU:
				size = 6+m_headerLength+m_extendedHeaderLength;
				break;
				
			case kMacSpecific:
				switch(m_macType) {
					case kTiming:
					case kFragmentation:
					case kConcatenation:
						size = 6+m_headerLength;
						break;
					
					case kMacManagement:
						size = 6+m_headerLength+m_extendedHeaderLength;
						break;
						
					case kRequest:
						size = 6;
						break;
					
					case kQdbRequest:
						size = 7;
						break;
				}
				break;
			
			default:
				size = 0;
		}
		
		return size;
	}
	
	void DocsisHeader::Print (std::ostream &os) const
	{
		
	}
	
	void DocsisHeader::Serialize (Buffer::Iterator start) const
	{
		start.WriteU8(0, m_phyOverhead);	//PHY Overhead
		
		start.WriteU8((m_fcType<<6) + (m_macType<<1) + (m_extendedHeaderPresent?1:0));	//FC
		
		if (m_fcType == kMacSpecific && m_macType == kRequest)
			start.WriteU8(m_requestedSlots);
		else if (m_fcType == kMacSpecific && m_macType == kQdbRequest)
			start.WriteU16(m_qdbRequestedSlots);
		else if (m_fcType == kMacSpecific && m_macType == kConcatenation)
			start.WriteU8(m_concatenatedPackets);
		else
			start.WriteU8(m_extendedHeaderLength);
		
		start.WriteU16(m_headerLength+m_extendedHeaderLength);	//LEN
		
		if (m_extendedHeaderPresent)
			m_extendedHeader.Serialize(start); // EHDR
		
		start.WriteU16(0);	//HCS
	}
	
	TypeId DocsisHeader::GetTypeId (void)
	{
		static TypeId tid = TypeId ("ns3::DocsisHeader")
			.SetParent<Header> ()
			.AddConstructor<DocsisHeader> ();
		
		return tid;
	}
	
	TypeId DocsisHeader::GetInstanceTypeId (void) const
	{
		return GetTypeId();
	}
	
	void DocsisHeader::setupPduPacket(size_t overheadSize, size_t pduLength, DocsisChannelDirection direction) {
		m_phyOverhead = overheadSize;
		m_packetDirection = direction;
		
		m_fcType = kPacketPDU;
		m_extendedHeaderPresent = false;
		m_extendedHeaderLength = 0;
		m_headerLength = pduLength;
	}
	
	void DocsisHeader::setupIsolationPduPacket(size_t overheadSize, size_t pduLength, DocsisChannelDirection direction) {
		m_phyOverhead = overheadSize;
		m_packetDirection = direction;
		
		m_fcType = kIsolationPacketPDU;
		m_extendedHeaderPresent = false;
		m_extendedHeaderLength = 0;
		m_headerLength = pduLength;
	}
	
	void DocsisHeader::setupMSHTiming(size_t overheadSize, size_t pduLength, DocsisChannelDirection direction) {
		m_phyOverhead = overheadSize;
		m_packetDirection = direction;
		
		m_fcType = kMacSpecific;
		m_macType = kTiming;
		m_extendedHeaderPresent = false;
		m_extendedHeaderLength = 0;
		m_headerLength = pduLength;
	}
	
	void DocsisHeader::setupMSHManagement(size_t overheadSize, size_t macMsgLength, DocsisChannelDirection direction) {
		m_phyOverhead = overheadSize;
		m_packetDirection = direction;
		
		m_fcType = kMacSpecific;
		m_macType = kMacManagement;
		m_extendedHeaderPresent = false;
		m_extendedHeaderLength = 0;
		m_headerLength = macMsgLength;
	}
	
	void DocsisHeader::setupMSHRequest(size_t overheadSize, uint16_t sid, uint8_t count, DocsisChannelDirection direction) {
		m_phyOverhead = overheadSize;
		m_packetDirection = direction;
		
		m_fcType = kMacSpecific;
		m_macType = kRequest;
		m_requestedSlots = count;
		m_extendedHeaderPresent = false;
		m_extendedHeaderLength = 0;
		m_headerLength = sid;
	}
	
	void DocsisHeader::setupMSHFragmentation(size_t overheadSize, size_t partialPduLength, ExtendedHeader::ExtendedHeaderElement ehe, DocsisChannelDirection direction) {
		m_phyOverhead = overheadSize;
		m_packetDirection = direction;
		
		m_fcType = kMacSpecific;
		m_macType = kFragmentation;
		m_extendedHeaderPresent = false;
		m_extendedHeaderLength = 0;
		m_headerLength = partialPduLength;
		
		addExtendedHeader(ehe);
	}
	
	void DocsisHeader::setupMSHRequestQD(size_t overheadSize, uint16_t sid, uint16_t bytesMultiples, DocsisChannelDirection direction) {
		m_phyOverhead = overheadSize;
		m_packetDirection = direction;
		
		m_fcType = kMacSpecific;
		m_macType = kQdbRequest;
		m_extendedHeaderPresent = false;
		m_extendedHeaderLength = 0;
		m_requestedSlots = bytesMultiples;
		m_headerLength = sid;
	}
	
	void DocsisHeader::setupMSHConcatenation(size_t overheadSize, uint8_t packets, uint16_t packetsSize, DocsisChannelDirection direction) {
		m_phyOverhead = overheadSize;
		m_packetDirection = direction;
		
		m_fcType = kMacSpecific;
		m_macType = kConcatenation;
		m_extendedHeaderPresent = false;
		m_extendedHeaderLength = 0;
		m_concatenatedPackets = packets;
		
		m_headerLength = packetsSize;
	}
	
	void DocsisHeader::addExtendedHeader(ExtendedHeader::ExtendedHeaderElement ehe) {
		m_extendedHeaderPresent = true;
		
		m_extendedHeaderLength += ehe.m_length + 1;
		m_headerLength += ehe.m_length + 1;
		
		m_extendedHeader.elements.push_back(ehe);
	}
	
	void DocsisHeader::ExtendedHeader::Serialize(Buffer::Iterator start) const {
		for (std::list<ExtendedHeaderElement>::const_iterator iter = elements.begin();
					iter != elements.end(); iter++) {
			start.WriteU8( ((uint8_t)iter->m_type<<4) + iter->m_length );
			
			switch (iter->m_type) {
				case kEHRequest:
					start.WriteU8(iter->m_minislots);
					start.WriteU16(iter->m_sid);
					break;
				
				case kAckRequest:
					start.WriteU16(iter->m_sid);
					break;
				
				case kUpstreamServiceFlow:
					start.WriteU8(0);
					start.WriteU8(((iter->m_queueIndicator?1:0) << 7) + iter->m_activeGrants);
					break;
				
				case kDownstreamService:
					if (iter->m_length == 1)
						start.WriteU8(iter->m_traficPriority << 5);
					else if (iter->m_length == 3) {
						start.WriteU8((iter->m_traficPriority << 5) + (iter->m_sid >> 16));
						start.WriteU16(iter->m_sid & 0xFFFF);
					} else if (iter->m_length == 5) {
						start.WriteU8((iter->m_traficPriority << 5) + ((iter->m_sequenceChangeCount?0:1) << 4) + (iter->m_sid >> 16));
						start.WriteU16(iter->m_sid & 0xFFFF);
						start.WriteU16(iter->m_packetSequenceNumber);
					}
					break;
				
				default:
					for(uint8_t i=0; i<iter->m_length; i++)
						start.WriteU8(0);
			}
		}
	}
	
	uint32_t DocsisHeader::ExtendedHeader::GetSerializedSize(void) const {
		uint32_t size = 0;
		for (std::list<ExtendedHeaderElement>::const_iterator iter = elements.begin();
					iter != elements.end(); iter++)
			size += iter->m_length + 1;
		
		return size;
	}
	
	uint32_t DocsisHeader::ExtendedHeader::Deserialize(uint8_t ehLength, Buffer::Iterator start) {
		uint32_t readLength = 0;
		
		while (ehLength > 0) {
			ExtendedHeaderElement ehe;
			
			uint8_t buffer = start.ReadU8();
			ehe.m_type = (ExtendedHeaderType)(buffer >> 4);
			ehe.m_length = buffer & 0x0F;
			readLength++;
			if (ehLength < ehe.m_length+1)
				return readLength;
			
			switch(ehe.m_type) {
				case kEHRequest:
					ehe.m_minislots = start.ReadU8();
					ehe.m_sid = start.ReadU16();
					break;
				
				case kAckRequest:
					ehe.m_sid = start.ReadU16();
					break;
				
				case kUpstreamServiceFlow:
					start.ReadU8();
					buffer = start.ReadU8();
					ehe.m_queueIndicator = (buffer >> 7) > 0;
					ehe.m_activeGrants = buffer & 0x7F;
					break;
				
				case kDownstreamService:
					if (ehe.m_length == 1)
						ehe.m_traficPriority = start.ReadU8() >> 5;
					else if (ehe.m_length == 3) {
						buffer = start.ReadU8();
						ehe.m_traficPriority = buffer >> 5;
						ehe.m_sid = ((buffer & 0x0F) << 16) + start.ReadU16();
					} else if (ehe.m_length == 5) {
						buffer = start.ReadU8();
						ehe.m_traficPriority = buffer >> 5;
						ehe.m_sequenceChangeCount = (buffer & 0x10) > 0;
						ehe.m_sid = ((buffer & 0x0F) << 16) + start.ReadU16();
						ehe.m_packetSequenceNumber = start.ReadU16();
					}
					break;
				
				default:
					for(uint8_t i=0; i<ehe.m_length; i++)
						start.ReadU8();
			}
			
			readLength += ehe.m_length;
			ehLength -= ehe.m_length+1;
		}
		
		return readLength;
	}
}
