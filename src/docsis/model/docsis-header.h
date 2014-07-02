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
			ExtendedHeaderTypeCount
		};
		struct ExtendedHeader {
			ExtendedHeaderType m_type;
			uint8_t m_length;
			uint8_t m_sid;
			uint8_t m_minislots;
		};
		
		virtual uint32_t Deserialize (Buffer::Iterator start);
		virtual uint32_t GetSerializedSize (void) const;
		virtual void Print (std::ostream &os) const;
		virtual void Serialize (Buffer::Iterator start) const;
		
		static TypeId GetTypeId (void);
		virtual TypeId GetInstanceTypeId (void) const;
		
		void setupPduPacket(size_t overheadSize, DocsisChannelDirection direction);
		void setupAtmPacket(size_t overheadSize, DocsisChannelDirection direction);
		void setupIsolationPduPacket(size_t overheadSize, DocsisChannelDirection direction,
									
		
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
		ExtendedHeader m_extendedHeader;
	};
}

#endif /* DOCSIS_HEADER_H */

