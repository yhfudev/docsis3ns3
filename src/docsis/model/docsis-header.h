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
#ifndef DOCSIS_HEADER_H
#define DOCSIS_HEADER_H

#include "docsis-enums.h"

namespace ns3 {
	class DocsisHeader : public Header {
	public:
		enum FrameControlType {
			kPacketPDU,
			kAtmPDU,
			kIsolationPacketPDU,
			kMacSpecific,
			FrameControlTypeCount
		};
		enum MacHeaderType {
			kTiming = 0,
			kMacManagement,
			kRequest,
			kFragmentation,
			kQdbRequest,
			kConcatenation = 28
		};
		enum ExtendedHeaderType {
			kNull,
			kEHRequest,
			kAckRequest,
			kUpstreamPrivacy,
			kDownstreamPrivacy,
			kDownstreamServiceFlow,
			kUpstreamServiceFlow,
			kUpstreamPrivacy2,
			kDownstreamService,
			kDocsisPathVerify,
			kReserved10,
			kReserved11,
			kReserved12,
			kReserved13,
			kReserved14,
			kExtended,
			ExtendedHeaderTypeCount
		};
		struct ExtendedHeader {
			ExtendedHeaderType m_type;
			uint8_t m_length;	// In bytes
			uint8_t m_sid;
			uint8_t m_minislots;
		};
		
		virtual uint32_t Deserialize (Buffer::Iterator start);
		virtual uint32_t GetSerializedSize (void) const;
		virtual void Print (std::ostream &os) const;
		virtual void Serialize (Buffer::Iterator start) const;
		
		static TypeId GetTypeId (void);
		virtual TypeId GetInstanceTypeId (void) const;
		
		void setupPduPacket(size_t overheadSize, size_t pduLength, DocsisChannelDirection direction);
		void setupIsolationPduPacket(size_t overheadSize, size_t pduLength, DocsisChannelDirection direction);
		
		void setupMSHTiming(size_t overheadSize, size_t pduLength, DocsisChannelDirection direction);
		void setupMSHManagement(size_t overheadSize, size_t macMsgLength, DocsisChannelDirection direction);
		void setupMSHRequest(size_t overheadSize, uint16_t sid, uint8_t count, DocsisChannelDirection direction);
		void setupMSHFragmentation(size_t overheadSize, size_t partialPduLength, ExtendedHeader eh, DocsisChannelDirection direction);
		void setupMSHRequestQD(size_t overheadSize, uint16_t sid, uint16_t bytesMultiples, DocsisChannelDirection direction);
		void setupMSHConcatenation(size_t overheadSize, std::list<DocsisHeader> headers, DocsisChannelDirection direction);
		
		void addExtendedHeader(ExtendedHeader eh);
		
	private:
		DocsisChannelDirection m_packetDirection;
		size_t m_phyOverhead;
		FrameControlType m_fcType;
		MacHeaderType m_macType;
		bool m_extendedHeaderPresent;
		uint8_t m_extendedHeaderLength;
		uint8_t m_frameCount;
		uint8_t m_requestedSlots;
		uint16_t m_qdbRequestedSlots;
		uint16_t m_headerLength;
		std::list<ExtendedHeader> m_extendedHeader;
		std::list<DocsisHeader> m_concatenatedHeaders;
	};
}

#endif /* DOCSIS_HEADER_H */

