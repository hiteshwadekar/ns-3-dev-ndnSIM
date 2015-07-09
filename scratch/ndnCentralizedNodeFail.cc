#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/ndnSIM-module.h"

#include <set>
#include <unistd.h>
#include "ns3/ndnSIM/helper/ndn-link-control-helper.hpp"

#define GetCurrentDir getcwd

namespace ns3{
//using namespace ndn;
//using namespace std;


int main(int argc, char *argv[]) {

	CommandLine cmd;
	cmd.Parse(argc, argv);

	cout << "ndnCentralizedNodeFail: Starting application -> " << endl;

	char cCurrentPath[FILENAME_MAX];
	if (!GetCurrentDir(cCurrentPath, sizeof(cCurrentPath))) {
		return 1;
	}
	cCurrentPath[sizeof(cCurrentPath) - 1] = '\0'; /* not really required */
	cout << cCurrentPath << std::endl;

	// Read the topology from file and create the nodes based on it.
	AnnotatedTopologyReader topologyReader("", 25);
	topologyReader.SetFileName(
			"src/ndnSIM/examples/topologies/topo-centralized-controller-test-consumer-producer.txt");
	topologyReader.Read();

	/*
	std::cout << "Link information" << std::endl;
	std::list<TopologyReader::Link> links = topologyReader.GetLinks();
	std::list<TopologyReader::Link>::const_iterator iterator;

	for (iterator = links.begin(); iterator != links.end(); ++iterator) {
	    std::cout << "\n Link from ->  " << iterator->GetFromNodeName()<<std::endl;
	    std::cout << "Link to  ->  " << iterator->GetToNodeName()<<std::endl;
	    std::cout << "Link OSPF attribute -> " << iterator->GetAttribute("OSPF");
	}
*/


	// Install NDN stack on all nodes
	ndn::StackHelper ndnHelper;
	//ndnHelper.SetForwardingStrategy ("ns3::ndn::fw::BestRoute");
	//ndnHelper.SetForwardingStrategy("ns3::ndn::fw::Flooding");
	ndnHelper.SetOldContentStore("ns3::ndn::cs::Lru", "MaxSize", "1");
	ndnHelper.InstallAll();
	topologyReader.ApplyOspfMetric();
	ndn::StrategyChoiceHelper::InstallAll("/", "/localhost/nfd/strategy/best-route");
	//ndn::StrategyChoiceHelper::InstallAll("/", "/localhost/nfd/strategy/broadcast");
	//ndn::StrategyChoiceHelper::InstallAll("/", "/localhost/nfd/strategy/ClientControlStrategy");
	//ndn::StrategyChoiceHelper::InstallAll("/", "/localhost/nfd/strategy/BestRouteStrategy2");
	// Getting containers for the consumer/producer
	Ptr<Node> consumer1 = Names::Find<Node>("Node1");
	Ptr<Node> consumer2 = Names::Find<Node>("Node2");
	Ptr<Node> consumer3 = Names::Find<Node>("Node3");
	Ptr<Node> consumer = Names::Find<Node>("Consumer");
	Ptr<Node> producer = Names::Find<Node>("Producer");
	Ptr<Node> controlllerNode = Names::Find<Node>("controller");

	// Install NDN applications

	std::string prfx_controller = "/controller";
	std::string prfx_controller_node_consumer1 = "/Node1";
	std::string prfx_controller_node_consumer2 = "/Node2";
	std::string prfx_controller_node_consumer3 = "/Node3";
	std::string prfx_controller_node_consumer = "/Consumer";
	std::string prfx_controller_node_producer = "/Producer";

	ndn::AppHelper consumerHelper("ns3::ndn::CustConsumer");

	consumerHelper.SetAttribute("PayloadSize", StringValue("3048"));
	consumerHelper.SetAttribute("Frequency", StringValue("1")); // 10 interests a s	econd

	consumerHelper.SetPrefix(prfx_controller_node_consumer1);
	ApplicationContainer app1 = consumerHelper.Install(consumer1);

	consumerHelper.SetPrefix(prfx_controller_node_consumer2);
	ApplicationContainer app2 =consumerHelper.Install(consumer2);

	consumerHelper.SetPrefix(prfx_controller_node_consumer3);
	ApplicationContainer app3 =consumerHelper.Install(consumer3);

	consumerHelper.SetPrefix(prfx_controller_node_consumer);
	ApplicationContainer app4 =consumerHelper.Install(consumer);

	consumerHelper.SetPrefix(prfx_controller_node_producer);
	ApplicationContainer app5 =consumerHelper.Install(producer);


	ndn::AppHelper controllerHelper("ns3::ndn::ControllerApp");
	controllerHelper.SetAttribute("Frequency", StringValue("1")); // 10 interests a second,
	controllerHelper.SetAttribute("PayloadSize", StringValue("3048"));
	controllerHelper.SetPrefix(prfx_controller);
	ApplicationContainer app0 = controllerHelper.Install(controlllerNode);

	// Communication other node to controller through Node0
	ndn::FibHelper::AddRoute("Node1", prfx_controller, "controller", 0);
	ndn::FibHelper::AddRoute("Node2", prfx_controller, "Node1", 0);
	ndn::FibHelper::AddRoute("Node3", prfx_controller, "Node1", 0);
	ndn::FibHelper::AddRoute("Producer", prfx_controller, "Node1", 0);
	ndn::FibHelper::AddRoute("Consumer", prfx_controller, "Node3", 0);
	ndn::FibHelper::AddRoute("Consumer", prfx_controller, "Node2", 0);

	ndn::FibHelper::AddRoute("controller", prfx_controller_node_consumer1, "Node1", 0);
	ndn::FibHelper::AddRoute("controller", prfx_controller_node_consumer2, "Node1", 0);
	ndn::FibHelper::AddRoute("controller", prfx_controller_node_consumer3, "Node1", 0);
	ndn::FibHelper::AddRoute("controller", prfx_controller_node_consumer, "Node1", 0);
	ndn::FibHelper::AddRoute("controller", prfx_controller_node_producer, "Node1", 0);

	ndn::FibHelper::AddRoute("Node1", prfx_controller_node_consumer3, "Node3", 0);
	ndn::FibHelper::AddRoute("Node1", prfx_controller_node_consumer2, "Node2", 0);
	ndn::FibHelper::AddRoute("Node1", prfx_controller_node_consumer, "Node3", 0);
	ndn::FibHelper::AddRoute("Node1", prfx_controller_node_consumer, "Node2", 0);
	ndn::FibHelper::AddRoute("Node2", prfx_controller_node_consumer, "Consumer", 0);
	ndn::FibHelper::AddRoute("Node3", prfx_controller_node_consumer, "Consumer", 0);
	ndn::FibHelper::AddRoute("Node1", prfx_controller_node_producer, "Producer", 0);


	app1.Start(Seconds (15.0));
	app2.Start(Seconds (45.0));
	app3.Start(Seconds (75.0));
	app4.Start(Seconds (105.0));
	app5.Start(Seconds (135.0));

	Simulator::Stop(Seconds(500.0));

	//Simulator::Schedule(Seconds(60.0), ndn::LinkControlHelper::FailLink, consumer2, consumer3);
	//Simulator::Schedule(Seconds(18.0), ndn::LinkControlHelper::FailLink, consumer3, consumer2);
	//Simulator::Schedule(Seconds(50.0), ndn::LinkControlHelper::UpLink, consumer2, consumer3);
	//Simulator::Schedule(Seconds(80.0), ndn::LinkControlHelper::UpLink, consumer2, consumer3);

	ndn::L3RateTracer::InstallAll("rate-trace.txt", Seconds(1.0));
	//ndn::AppDelayTracer::InstallAll("app-delays-trace.txt");
	//ndn::CsTracer::InstallAll("cs-trace.txt", Seconds(1.0));

	Simulator::Run();
	Simulator::Destroy();
	cout << "\n ndnCentralizedNodeFail: Stopping application" << endl;
	return 0;
}

}

int
main(int argc, char* argv[])
{
      return ns3::main(argc, argv);
}
