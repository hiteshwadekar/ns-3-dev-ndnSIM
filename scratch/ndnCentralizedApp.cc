#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/ndnSIM-module.h"

#include <set>
#include <unistd.h>

#define GetCurrentDir getcwd

namespace ns3{
//using namespace ndn;
//using namespace std;


int main(int argc, char *argv[]) {

	CommandLine cmd;
	cmd.Parse(argc, argv);

	cout << "ndnCentralizedController: Starting application: -> " << endl;

	char cCurrentPath[FILENAME_MAX];
	if (!GetCurrentDir(cCurrentPath, sizeof(cCurrentPath))) {
		return 1;
	}
	cCurrentPath[sizeof(cCurrentPath) - 1] = '\0'; /* not really required */
	cout << cCurrentPath << std::endl;

	// Read the topology from file and create the nodes based on it.
	AnnotatedTopologyReader topologyReader("", 25);
	topologyReader.SetFileName(
			"src/ndnSIM/examples/topologies/topo-centralized-controller.txt");
	topologyReader.Read();
	std::cout << "Link information" << std::endl;

	std::list<TopologyReader::Link> links = topologyReader.GetLinks();
	std::list<TopologyReader::Link>::const_iterator iterator;

	for (iterator = links.begin(); iterator != links.end(); ++iterator) {
	    std::cout << "\n Link from ->  " << iterator->GetFromNodeName()<<std::endl;
	    std::cout << "Link to  ->  " << iterator->GetToNodeName()<<std::endl;
	    std::cout << "Link OSPF attribute -> " << iterator->GetAttribute("OSPF");
	}

	// Install NDN stack on all nodes
	ndn::StackHelper ndnHelper;
	//ndnHelper.SetForwardingStrategy ("ns3::ndn::fw::BestRoute");
	//ndnHelper.SetForwardingStrategy("ns3::ndn::fw::Flooding");
	ndnHelper.SetOldContentStore("ns3::ndn::cs::Lru", "MaxSize", "1");
	ndnHelper.InstallAll();
	topologyReader.ApplyOspfMetric();

	// Getting containers for the consumer/producer
	Ptr<Node> consumer1 = Names::Find<Node>("Node0");
	Ptr<Node> consumer2 = Names::Find<Node>("Node2");
	Ptr<Node> consumer3 = Names::Find<Node>("Node3");

	Ptr<Node> controlllerNode = Names::Find<Node>("Node1");

	// Install NDN applications
	std::string prfx_node_controller_consumer1 = "/controller/Node0";
	std::string prfx_node_controller_consumer2 = "/controller/Node2";
	std::string prfx_node_controller_consumer3 = "/controller/Node3";

	std::string prfx_controller = "/controller";

	std::string prfx_controller_node_consumer1 = "/Node0";
	std::string prfx_controller_node_consumer2 = "/Node2";
	std::string prfx_controller_node_consumer3 = "/Node3";

	ndn::AppHelper consumerHelper("ns3::ndn::CustConsumer");
	consumerHelper.SetAttribute("PayloadSize", StringValue("2048"));
	consumerHelper.SetAttribute("Frequency", StringValue("1")); // 10 interests a s	econd


	consumerHelper.SetPrefix(prfx_controller_node_consumer1);
	ApplicationContainer app1 = consumerHelper.Install(consumer1);

	consumerHelper.SetPrefix(prfx_controller_node_consumer2);
	ApplicationContainer app2 =consumerHelper.Install(consumer2);

	consumerHelper.SetPrefix(prfx_controller_node_consumer3);
	ApplicationContainer app3 =consumerHelper.Install(consumer3);


	ndn::AppHelper controllerHelper("ns3::ndn::ControllerApp");
	controllerHelper.SetAttribute("Frequency", StringValue("1")); // 10 interests a second
	controllerHelper.SetAttribute("PayloadSize", StringValue("2048"));
	controllerHelper.SetPrefix(prfx_controller);
	controllerHelper.Install(controlllerNode);

	// Communication other node to controller through Node0
	ndn::FibHelper::AddRoute("Node2", prfx_controller, "Node0", 0);
	ndn::FibHelper::AddRoute("Node3", prfx_controller, "Node0", 0);
	ndn::FibHelper::AddRoute("Node0", prfx_controller, "Node1", 0);

	ndn::FibHelper::AddRoute("Node1", prfx_controller_node_consumer1, "Node0", 0);
	ndn::FibHelper::AddRoute("Node1", prfx_controller_node_consumer2, "Node0", 0);
	ndn::FibHelper::AddRoute("Node1", prfx_controller_node_consumer3, "Node0", 0);

	ndn::FibHelper::AddRoute("Node0", prfx_controller_node_consumer3, "Node3", 0);
	ndn::FibHelper::AddRoute("Node0", prfx_controller_node_consumer2, "Node2", 0);

	app1.Start(Seconds (15.0));
	app2.Start(Seconds (25.0));
	app3.Start(Seconds (35.0));

	Simulator::Stop(Seconds(100.0));

	Simulator::Run();
	Simulator::Destroy();
	cout << "\n ndnCentralizedController: Stopping application" << endl;
	return 0;

}

}

int
main(int argc, char* argv[])
{
      return ns3::main(argc, argv);
}


