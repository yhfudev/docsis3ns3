/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
#ifndef HFC_H
#define HFC_H

#include "cm.h"
#include "ns3/channel.h"
#include "ns3/ptr.h"

namespace ns3 {

class CmDevice;

class HFC : public Channel
{
public:
	static TypeId GetTypeId (void);
	HFC ();
	virtual ~HFC ();
	
	Ptr<NetDevice> GetDevice (uint32_t i ) const;
	uint32_t GetNDevices (void) const;

private:
	Ptr<CmDevice> dummy;
};

}

#endif /* HFC_H */
