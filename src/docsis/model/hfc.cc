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
#include "hfc.h"
#include <algorithm>

namespace ns3 {

NS_OBJECT_ENSURE_REGISTERED (Hfc);

TypeId
Hfc::GetTypeId (void)
{
	static TypeId tid = TypeId("ns3::Hfc")
		.SetParent<Channel> ()
		.AddConstructor<Hfc> ()
		;
	
	return tid;
}

Hfc::Hfc () : m_cmts(NULL), m_upstreamChannelsAmount(1), m_downstreamChannelsAmount(1)
{
}

Hfc::~Hfc()
{
}

Ptr<NetDevice>
Hfc::GetDevice (uint32_t i) const
{
	return m_cmts;
}


uint32_t
Hfc::GetNDevices (void) const
{
	return 0;
}

void
Hfc::Attach(Ptr<CmDevice> device)
{
	std::list< Ptr<CmDevice> >::iterator deviceIterator= std::find(m_cmList.begin(), m_cmList.end(), device);
	if (deviceIterator != m_cmList.end())
		return;

	m_cmList.push_front(device);
}

void
Hfc::Attach(Ptr<CmtsDevice> device)
{
	if (m_cmts)
	{
		m_cmts->Deattach();
	}

	m_cmts = device;
}

void
Hfc::Deattach(Ptr<CmDevice> device)
{
	std::list< Ptr<CmDevice> >::iterator deviceIterator = std::find(m_cmList.begin(), m_cmList.end(), device);
	if (deviceIterator != m_cmList.end())
		return;
	else
		m_cmList.erase(deviceIterator);
}

void
Hfc::Deattach(Ptr<CmtsDevice> device)
{
	m_cmts = NULL;
}

uint32_t
Hfc::GetUpstreamChannelsAmount()
{
	return m_upstreamChannelsAmount;
}

uint32_t
Hfc::GetDownstreamChannelsAmount()
{
	return m_downstreamChannelsAmount;
}

}
