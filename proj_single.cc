/*s program is free software; you can redistribute it and/or modify
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
 */

// ECE6110 Project 1, Fall 2010
// YOUR NAME HERE

#include <iostream>

// This is the list of needed header modules for P1.
#include "ns3/core-module.h"
#include "ns3/simulator-module.h"
#include "ns3/node-module.h"
#include "ns3/helper-module.h"
#include "ns3/onoff-application.h"
#include "ns3/packet-sink.h"
#include "ns3/visualizer.h"
#include "ns3/cadproj4.h"

using namespace ns3;
using namespace std;

// Used by the ns-3 "logging" subsystem; not really needed for this 
// assignment, but include it anyway.

NS_LOG_COMPONENT_DEFINE ("ECE6110-P1");

int  main (int argc, char *argv[])
{
  // Create variables to store the value of the various "input parameters"
  // (receiver window, segment size, etc)
  // Then create a CommandLine object and use "AddValue" to add command
  // line parameters for the input parameters. Call the "Parse" function
  // to actually parse the command line.  See any of the examples for
  // guidance on how to do this syntactically.
  std::string tcpModel ("ns3::TcpReno");
  uint16_t port = 50000;
  static uint32_t segSize; 
  static uint32_t recSize; 
  static uint32_t qSize; 


  CommandLine cmd;
  cmd.AddValue("segSize","Segment Size",segSize);
  cmd.AddValue("recSize","Rec wnd Size",recSize);
  cmd.AddValue("qSize","Queue Size",qSize);
  cmd.AddValue("tcpModel","Tcp variant",tcpModel);

  cmd.Parse(argc,argv);
  //cout<<"segsize="<<segSize;

  // Next use Config::SetDefault to set the tcp parameters
  // "RcvBufSize" (receiver window), "SegmentSize" (segment size)
  // DelAckCount (set to 1) and the TclL4Protocol paramter
  // "SocketType" to specify which TCP variant you want.
  // See any of the TCP examples for guidance.

  // Next, use Config::SetDefault to configure the DropTailQueue
  // parameters.  These are "Mode" (set to BYTES) and "MaxBytes"
  // (maximum queue length in bytes);  The Mode line is given below.

  Config::SetDefault ("ns3::TcpL4Protocol::SocketType", StringValue(tcpModel));
  Config::SetDefault ("ns3::TcpSocket::SegmentSize",UintegerValue(segSize)); 
  Config::SetDefault ("ns3::TcpSocket::DelAckCount",UintegerValue (1));
  Config::SetDefault ("ns3::TcpSocket::RcvBufSize",UintegerValue(recSize)); 

  Config::SetDefault ("ns3::DropTailQueue::Mode", EnumValue(DropTailQueue::BYTES));
  Config::SetDefault ("ns3::DropTailQueue::MaxBytes", UintegerValue (qSize));


  // Netx use Config::SetDefault to configure the OnOffApplication
  // as follows.  Set MaxBytes to 100000000 and DataRate to 5Mbps.
  // Set the OnTime variable to a ConstantVariable of 10 and the
  // OffTime variable to a ConstantVariable of 0.  Several of the
  // examples do this.


  Config::SetDefault ("ns3::OnOffApplication::MaxBytes", UintegerValue (100000000));
  Config::SetDefault ("ns3::OnOffApplication::DataRate", StringValue ("5Mb/s"));
  // Create the PointToPointHelper objects with correct link characteristics
  Config::SetDefault ("ns3::OnOffApplication::OnTime", RandomVariableValue (ConstantVariable (1)));
  Config::SetDefault ("ns3::OnOffApplication::OffTime", RandomVariableValue (ConstantVariable (0)));


  // one with 5Mbps and 10ms delay and a second with 1Mbps and 20ms delay.
  PointToPointHelper pointToPoint1;
  PointToPointHelper pointToPoint2;

  pointToPoint1.SetDeviceAttribute ("DataRate", StringValue ("5Mbps"));
  pointToPoint1.SetChannelAttribute ("Delay", StringValue ("10ms"));

  pointToPoint2.SetDeviceAttribute ("DataRate", StringValue ("1Mbps"));
  pointToPoint2.SetChannelAttribute ("Delay", StringValue ("20ms"));



  // Create the dumbbell topology using the above helpers, and specifying
  // one leaf at each end.

  PointToPointDumbbellHelper dumbell (1,pointToPoint1,2,pointToPoint1,pointToPoint2);

  // Use the InternetStackHelper and the dumbbell "InstallStack" method
  // to add a protocol stack to each node in the dumbbell.
  // Install the protocol stack

  InternetStackHelper internet;
  //internet.InstallAll ();
  dumbell.InstallStack (internet); 

  //Assign the IP Addresses to the dumbbell. You will need three different
  // ones, one for each set of leaf nodes (left and right) and one for
  // the router network.  Use the Ipv4AddressHelper.  Most examples
  // assign ip addresses in this way.

  Ipv4AddressHelper leftip;
  leftip.SetBase ("10.1.1.0","255.255.255.0");


  Ipv4AddressHelper rightip;
  rightip.SetBase("10.2.1.0","255.255.255.0");

  Ipv4AddressHelper router;
  router.SetBase ("10.3.1.0","255.255.255.0");

  dumbell.AssignIpv4Addresses(leftip,rightip,router); 	


  //  cout<<"Left Ip is "<<dumbell.GetLeftIpv4Address(0)<<"\n";
  //  cout<<"Right Ip is "<<dumbell.GetRightIpv4Address(0)<<"\n";

  Ipv4GlobalRoutingHelper::PopulateRoutingTables ();

  // Use the PacketSinkHelper ot install packet sink on left side leaf.
  // Ge sure to "Start" the packet sink app (see any example).



  Address sinkLocalAddress(InetSocketAddress (dumbell.GetLeftIpv4Address(0),port));
  PacketSinkHelper sinkHelper ("ns3::TcpSocketFactory",sinkLocalAddress);
  ApplicationContainer sinkApp = sinkHelper.Install (dumbell.GetLeft(0));
  sinkApp.Start (Seconds (0));
  sinkApp.Stop (Seconds (10.0));
  
  /*Copying Server Address to Global Variable ServerIP*/
  ServerIP[0].ip = dumbell.GetLeftIpv4Address(0).Get();

  Address remote (InetSocketAddress(dumbell.GetLeftIpv4Address(0),port));
  OnOffHelper onOffHelper("ns3::TcpSocketFactory",remote);
  ApplicationContainer app= onOffHelper.Install(dumbell.GetRight(0));
  app.Start(Seconds(0));
  app.Stop(Seconds(10.0));
  
  /*Copy Attacker IP to Global Variable AttackerIP*/
  AttackerIP[0].ip = dumbell.GetRightIpv4Address(0).Get();   
  
  Address remote1 (InetSocketAddress(dumbell.GetLeftIpv4Address(0),port));
  OnOffHelper onOffHelper1("ns3::TcpSocketFactory",remote1);
  ApplicationContainer app1= onOffHelper1.Install(dumbell.GetRight(1));
  app1.Start(Seconds(0));
  app1.Stop(Seconds(10.0));

  // Use Simulator::Stop to specify the stop time at 10 seconds.
  // Use Simulator::Run to actualy run the simulation
  Simulator::Stop(Seconds(10));
  // Simulator::Run ();
  Visualizer::Run();  
  Simulator::Destroy();

  // Get a pointer to the packet sink app and compute the throughput
  // Note you will need to loop 10 times for the second part.
  // The code is shown below, but it assumes that the ApplicationContainer
  // returned by the prior call to the PacketSinkHelper "Install" 
  // method is called "sinkApps".  You will need to edit the line
  // below to get the right variable name in your case.
  Ptr<PacketSink> sink = DynamicCast<PacketSink>(sinkApp.Get(0));

  cout <<"recSize= "<<recSize<<" SegSize= "<<segSize<<" qSize= " <<qSize<<" TPut= "<<sink->GetTotalRx() / 10.0 << endl;
  // All done!
  return 0;
}

