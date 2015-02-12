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
#ifndef DOCSIS_MMM_H
#define DOCSIS_MMM_H

#include "docsis.h"
#include "docsis-enums.h"

namespace ns3 {
	class MacManagementMessageHeader : public Header {
	public:
		enum MmmType {
			kGenericUCD = 2,
			kMAP = 3,
			kUCCRequest = 8,
			kUCCResponse = 9,
			k23UCD = 29,
			kDCD = 32,
			kMDD = 33,
			k3UCD = 35
		};
		
		virtual uint32_t Deserialize (Buffer::Iterator start);
		virtual uint32_t GetSerializedSize (void) const;
		virtual void Print (std::ostream &os) const;
		virtual void Serialize (Buffer::Iterator start) const;
		
		static TypeId GetTypeId (void);
		virtual TypeId GetInstanceTypeId (void) const;
		
	private:
		uint64_t m_destinationAddress;
		uint64_t m_sourceAddress;
		uint16_t m_length;
		uint8_t m_version;
		MmmType m_type;
	};
	
	class MAPHeader : public Header {
	public:
		enum IEType {
			kRequest = 0,
			kReqData,
			kInitialManteinance,
			kStationManteinance,
			kShortDataGrant,
			kLargeDataGrant,
			kNull,
			kDataAck,
			kAdvancedShortDataGrant,
			kAdvancedLargeDataGrant,
			kUnsolicitedGrant,
			kExpansion
		};
		struct InformationElement {
			IEType m_type;
			uint16_t m_sid;
			uint16_t m_offset;
		};
		
		virtual uint32_t Deserialize (Buffer::Iterator start);
		virtual uint32_t GetSerializedSize (void) const;
		virtual void Print (std::ostream &os) const;
		virtual void Serialize (Buffer::Iterator start) const;
		
		static TypeId GetTypeId (void);
		virtual TypeId GetInstanceTypeId (void) const;
		
	private:
		uint8_t m_ucId;
		uint8_t m_ucdCount;
		uint8_t m_elementCount;
		uint32_t m_startTime;
		uint32_t m_ackTime;
		uint8_t m_rangingStart;
		uint8_t m_rangingEnd;
		uint8_t m_dataStart;
		uint8_t m_dataEnd;
		
		std::list<InformationElement> m_ies;
	};
	
	class MacManagementMessage {
		
	};
}
#endif
