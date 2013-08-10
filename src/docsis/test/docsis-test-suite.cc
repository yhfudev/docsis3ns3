/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */

// Include a header file from your module to test.
#include "ns3/docsis.h"

// An essential include is test.h
#include "ns3/test.h"
#include "ns3/simulator.h"


// Do not put your test classes in namespace ns3.  You may find it useful
// to use the using directive to access the ns3 namespace directly
using namespace ns3;

// This is an example TestCase.
class DocsisTestCase1 : public TestCase
{
public:
  DocsisTestCase1 ();
  virtual ~DocsisTestCase1 ();

private:
  virtual void DoRun (void);
  void SendOnePacket(Ptr<CmtsDevice> device);
};

// Add some help text to this case to describe what it is intended to test
DocsisTestCase1::DocsisTestCase1 ()
  : TestCase ("Docsis test case")
{
}

// This destructor does nothing but we include it as a reminder that
// the test case should clean up after itself
DocsisTestCase1::~DocsisTestCase1 ()
{
}

void
DocsisTestCase1::SendOnePacket (Ptr<CmtsDevice> device)
{
  Ptr<Packet> p = Create<Packet> ();
  device->Send (p, device->GetBroadcast (), 0x800);
}

//
// This method is the pure virtual method from class TestCase that every
// TestCase must implement
//
void
DocsisTestCase1::DoRun (void)
{
/*  // A wide variety of test macros are available in src/core/test.h
  NS_TEST_ASSERT_MSG_EQ (true, true, "true doesn't equal true for some reason");
  // Use this one for floating point comparisons
  NS_TEST_ASSERT_MSG_EQ_TOL (0.01, 0.01, 0.001, "Numbers are not equal within tolerance");*/

	  Ptr<Node> a = CreateObject<Node> ();
	  Ptr<Node> b = CreateObject<Node> ();
	  Ptr<CmtsDevice> devA = CreateObject<CmtsDevice> ();
	  Ptr<CmDevice> devB = CreateObject<CmDevice> ();
	  Ptr<Hfc> channel = CreateObject<Hfc> ();

	  devA->Attach (channel);
	  devA->SetAddress (Mac48Address::Allocate ());
	  devB->Attach (channel);
	  devB->SetAddress (Mac48Address::Allocate ());

	  a->AddDevice (devA);
	  b->AddDevice (devB);

	  Simulator::Schedule (Seconds (1.0), &DocsisTestCase1::SendOnePacket, this, devA);

	  Simulator::Run ();

	  Simulator::Destroy ();
}

// The TestSuite class names the TestSuite, identifies what type of TestSuite,
// and enables the TestCases to be run.  Typically, only the constructor for
// this class must be defined
//
class DocsisTestSuite : public TestSuite
{
public:
  DocsisTestSuite ();
};

DocsisTestSuite::DocsisTestSuite ()
  : TestSuite ("docsis", UNIT)
{
  // TestDuration for TestCase can be QUICK, EXTENSIVE or TAKES_FOREVER
  AddTestCase (new DocsisTestCase1, TestCase::QUICK);
}

// Do not forget to allocate an instance of this TestSuite
static DocsisTestSuite docsisTestSuite;

