/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */

#include "ns3/core-module.h"
#include "ns3/docsis-helper.h"
#include "ns3/docsis.h"
#include "ns3/simulator.h"

using namespace ns3;

void
SendOnePacket (Ptr<NetDevice> device, Address address)
{
  Ptr<Packet> p = Create<Packet> ();
  device->Send (p, address, 0x800);
}

int 
main (int argc, char *argv[])
{
  /* ... */

  LogComponentEnable ("CmNetDevice", LOG_LEVEL_FUNCTION);
  LogComponentEnable ("CmtsNetDevice", LOG_LEVEL_FUNCTION);

  // Allow the user to override any of the defaults and the above
  // SetDefaults() at run-time, via command-line arguments
/*  CommandLine cmd;
  cmd.Parse (argc, argv);*/

  // Now, we will create a few objects using the low-level API
  Ptr<Node> n0 = CreateObject<Node> ();
  Ptr<Node> n1 = CreateObject<Node> ();
  Ptr<Hfc> c = CreateObject<Hfc> ();

  Ptr<CmtsDevice> net0 = CreateObject<CmtsDevice> ();
  net0->Attach (c);
  net0->SetAddress (Mac48Address::Allocate ());
  n0->AddDevice (net0);

  Ptr<CmDevice> net1 = CreateObject<CmDevice> ();
  net1->Attach (c);
  net1->SetAddress (Mac48Address::Allocate ());
  n1->AddDevice (net1);

  Simulator::Schedule (Seconds (1.0), &SendOnePacket, net0, net1->GetAddress());
//  Simulator::Schedule (Seconds (2.0), &SendOnePacket, net1, net0->GetAddress());

  Simulator::Run ();
  Simulator::Destroy ();
  return 0;
}


