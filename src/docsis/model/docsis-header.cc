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
		return 0;
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
						size = 6+m_headerLength;
						break;
					
					case kMacManagement:
						size = 6+m_headerLength+m_extendedHeaderLength;
						break;
						
					case kRequest:
						size = 6;
						break;
					
					case kFragmentation:
						size = 6+m_headerLength;
						break;
					
					case kQdbRequest:
						size = 7;
						break;
					
					case kConcatenation:
						size = 6+m_headerLength;
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
	
	void DocsisHeader::setupMSHFragmentation(size_t overheadSize, size_t partialPduLength, ExtendedHeader eh, DocsisChannelDirection direction) {
		m_phyOverhead = overheadSize;
		m_packetDirection = direction;
		
		m_fcType = kMacSpecific;
		m_macType = kFragmentation;
		m_extendedHeaderPresent = false;
		m_extendedHeaderLength = 0;
		m_headerLength = partialPduLength;
		
		addExtendedHeader(eh);
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
	
	void DocsisHeader::setupMSHConcatenation(size_t overheadSize, std::list<DocsisHeader> headers, DocsisChannelDirection direction) {
		m_phyOverhead = overheadSize;
		m_packetDirection = direction;
		
		m_fcType = kMacSpecific;
		m_macType = kConcatenation;
		m_extendedHeaderPresent = false;
		m_extendedHeaderLength = 0;
		m_concatenatedHeaders = headers;
		
		m_headerLength = 0;
		for (std::list<DocsisHeader>::iterator iter = headers.begin(); iter != headers.end(); iter++)
			m_headerLength += iter->GetSerializedSize();
	}
	
	void DocsisHeader::addExtendedHeader(ExtendedHeader eh) {
		size_t ehLength = eh.m_length + 1;
		m_extendedHeaderPresent = true;
		m_extendedHeaderLength += ehLength;
		m_headerLength += ehLength;
		
		m_extendedHeader.push_back(eh);
	}
}
