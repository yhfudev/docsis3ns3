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
#include "cmts-device.h"
#include "cm-device.h"
#include <algorithm>
#include <assert.h>
#include "ns3/simulator.h"

#define Mbps *1048576

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

Hfc::Hfc () : m_cmts(NULL), m_upstreamChannelsAmount(1), m_downstreamChannelsAmount(1), m_stubDataRate(40 Mbps)
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
	assert(m_cmts != NULL);

	std::list< Ptr<CmDevice> >::iterator deviceIterator= std::find(m_cmList.begin(), m_cmList.end(), device);
	if (deviceIterator != m_cmList.end())
		return;

	m_cmList.push_front(device);
	m_cmts->CmAttached(device);
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

	if (m_cmts != NULL)
		m_cmts->CmDeattached(device);
}

void
Hfc::Deattach(Ptr<CmtsDevice> device)
{
	m_cmts = NULL;

	for(std::list< Ptr<CmDevice> >::iterator deviceIterator = m_cmList.begin(); deviceIterator != m_cmList.end(); deviceIterator++)
	{
		(*deviceIterator)->Deattach();
	}
}

DataRate
Hfc::GetUpstreamDataRate(int channel)
{
	return m_stubDataRate;
}

DataRate
Hfc::GetDownstreamDataRate(int channel)
{
	return m_stubDataRate;
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

void
Hfc::UpTransmitStart(int channel, Ptr<Packet> p, Ptr<CmDevice> cm, Time txTime)
{
  Simulator::ScheduleWithContext(m_cmts->GetNode()->GetId(), txTime, &CmtsDevice::Receive, m_cmts, p, cm);
}

void Hfc::DownTransmitStart(int channel, Ptr<Packet> p, Ptr<CmDevice> cm, Time txTime)
{
  Simulator::ScheduleWithContext(m_cmts->GetNode()->GetId(), txTime, &CmDevice::Receive, cm, p);
}

}
