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
#ifndef HFC_H
#define HFC_H

#include "docsis-enums.h"
#include "cm-device.h"
#include "cmts-device.h"
#include "ns3/channel.h"
#include "ns3/ptr.h"
#include "ns3/data-rate.h"
#include <list>

namespace ns3 {

class CmDevice;

class Hfc : public Channel
{
public:
	static TypeId GetTypeId (void);
	Hfc ();
	virtual ~Hfc ();
	
	Ptr<NetDevice> GetDevice (uint32_t i ) const;
	uint32_t GetNDevices (void) const;

	void Attach(Ptr<CmDevice> device);
	void Attach(Ptr<CmtsDevice> device);
	void Deattach(Ptr<CmDevice> device);
	void Deattach(Ptr<CmtsDevice> device);

	DataRate GetUpstreamDataRate(int channel);
	DataRate GetDownstreamDataRate(int channel);

	uint32_t GetUpstreamChannelsAmount();
	uint32_t GetDownstreamChannelsAmount();

	void UpTransmitStart(int channel, Ptr<Packet> p, Ptr<CmDevice> cm, Time txTime);
	void DownTransmitStart(int channel, Ptr<Packet> p, Ptr<CmDevice> cm, Time txTime);

private:
	Ptr<CmtsDevice> m_cmts;
	std::list< Ptr<CmDevice> > m_cmList;
	uint32_t m_upstreamChannelsAmount;
	uint32_t m_downstreamChannelsAmount;
	DataRate m_stubDataRate;
};

}

#endif /* HFC_H */
