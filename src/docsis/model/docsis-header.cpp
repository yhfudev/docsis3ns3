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

namespace ns3 {
	
	uint32_t DocsisHeader::Deserialize (Buffer::Iterator start)
	{
		
	}
	
	uint32_t DocsisHeader::GetSerializedSize (void) const
	{
		
	}
	
	void DocsisHeader::Print (std::ostream &os) const
	{
		
	}
	
	void DocsisHeader::Serialize (Buffer::Iterator start) const
	{
		
	}
	
	TypeId DocsisHeader::GetTypeId (void)
	{
		static TypeId tid = TypeId ("ns3::EthernetHeader")
			.SetParent<Header> ()
			.AddConctructor<EthernetHeader> ();
		
		return tid;
	}
	
	TypeId DocsisHeader::GetInstanceTypeId (void) const
	{
		return GetTypeId();
	}
	
}
