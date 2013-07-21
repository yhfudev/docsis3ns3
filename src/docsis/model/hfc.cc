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

Hfc::Hfc () : dummy(0)
{
}

Hfc::~Hfc()
{
}

Ptr<NetDevice>
Hfc::GetDevice (uint32_t i) const
{
	return dummy;
}


uint32_t
Hfc::GetNDevices (void) const
{
	return 0;
}

}
