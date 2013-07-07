/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */

#include "hfc.h"

namespace ns3 {

NS_OBJECT_ENSURE_REGISTERED (HFC);

TypeId
HFC::GetTypeId (void)
{
	static TypeId tid = TypeId("ns3::HFC")
		.SetParent<Channel> ()
		.AddConstructor<HFC> ()
		;
	
	return tid;
}

HFC::HFC () : dummy(0)
{
}

HFC::~HFC()
{
}

Ptr<NetDevice>
HFC::GetDevice (uint32_t i) const
{
	return dummy;
}


uint32_t
HFC::GetNDevices (void) const
{
	return 0;
}

}
