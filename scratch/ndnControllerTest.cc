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

	cout << "ndnControllerTest: Starting application -> " << endl;

	// Read the topology from file and create the nodes based on it.
	AnnotatedTopologyReader topologyReader("", 25);
	topologyReader.SetFileName(
			"src/ndnSIM/examples/topologies/topo-centalized-controller-6-node.txt");
	topologyReader.Read();

	// Install NDN stack on all nodes
	ndn::StackHelper ndnHelper;
	ndnHelper.SetOldContentStore("ns3::ndn::cs::Lru", "MaxSize", "1");
	ndnHelper.InstallAll();
	topologyReader.ApplyOspfMetric();

	// Getting containers for the consumer/producer
	Ptr<Node> consumer1 = Names::Find<Node>("Node1");
	Ptr<Node> consumer2 = Names::Find<Node>("Node2");
	Ptr<Node> consumer4 = Names::Find<Node>("Node4");
	Ptr<Node> consumer5 = Names::Find<Node>("Node5");
	Ptr<Node> consumer6 = Names::Find<Node>("Node6");


	Ptr<Node> controlllerNode = Names::Find<Node>("Node3");

	// Install NDN applications
	std::string prfx_node_controller_consumer1 = "/controller/Node1";
	std::string prfx_node_controller_consumer2 = "/controller/Node2";
	std::string prfx_node_controller_consumer4 = "/controller/Node4";
	std::string prfx_node_controller_consumer5 = "/controller/Node5";
	std::string prfx_node_controller_consumer6 = "/controller/Node6";

	std::string prfx_controller = "/controller";

	std::string prfx_controller_node_consumer1 = "/Node1";
	std::string prfx_controller_node_consumer2 = "/Node2";
	std::string prfx_controller_node_consumer4 = "/Node4";
	std::string prfx_controller_node_consumer5 = "/Node5";
	std::string prfx_controller_node_consumer6 = "/Node6";

	ndn::AppHelper consumerHelper("ns3::ndn::CustConsumer");

	consumerHelper.SetAttribute("PayloadSize", StringValue("3048"));
	consumerHelper.SetAttribute("Frequency", StringValue("1")); // 10 interests a s	econd

	consumerHelper.SetPrefix(prfx_controller_node_consumer1);
	ApplicationContainer app1 = consumerHelper.Install(consumer1);

	consumerHelper.SetPrefix(prfx_controller_node_consumer2);
	ApplicationContainer app2 =consumerHelper.Install(consumer2);

	consumerHelper.SetPrefix(prfx_controller_node_consumer4);
	ApplicationContainer app4 =consumerHelper.Install(consumer4);

	consumerHelper.SetPrefix(prfx_controller_node_consumer5);
	ApplicationContainer app5 =consumerHelper.Install(consumer5);

	consumerHelper.SetPrefix(prfx_controller_node_consumer6);
	ApplicationContainer app6 =consumerHelper.Install(consumer6);

	ndn::AppHelper controllerHelper("ns3::ndn::ControllerApp");
	controllerHelper.SetAttribute("Frequency", StringValue("1")); // 10 interests a second,
	controllerHelper.SetAttribute("PayloadSize", StringValue("3048"));
	controllerHelper.SetPrefix(prfx_controller);
	controllerHelper.Install(controlllerNode);

	ndn::FibHelper::AddRoute("Node1", prfx_controller, "Node3", 0);
	ndn::FibHelper::AddRoute("Node1", prfx_controller_node_consumer2, "Node3", 0);
	ndn::FibHelper::AddRoute("Node4", prfx_controller, "Node3", 0);
	ndn::FibHelper::AddRoute("Node4", prfx_controller_node_consumer2, "Node3", 0);
	ndn::FibHelper::AddRoute("Node5", prfx_controller, "Node3", 0);
	ndn::FibHelper::AddRoute("Node6", prfx_controller, "Node3", 0);

	ndn::FibHelper::AddRoute("Node2", prfx_controller, "Node1", 0);
	ndn::FibHelper::AddRoute("Node2", prfx_controller, "Node4", 0);

	ndn::FibHelper::AddRoute("Node1", prfx_controller_node_consumer2, "Node2", 0);
	ndn::FibHelper::AddRoute("Node4", prfx_controller_node_consumer2, "Node2", 0);


	ndn::FibHelper::AddRoute("Node3", prfx_controller_node_consumer1, "Node1", 0);
	ndn::FibHelper::AddRoute("Node3", prfx_controller_node_consumer2, "Node1", 0);
	ndn::FibHelper::AddRoute("Node3", prfx_controller_node_consumer2, "Node4", 0);
	ndn::FibHelper::AddRoute("Node3", prfx_controller_node_consumer4, "Node4", 0);
	ndn::FibHelper::AddRoute("Node3", prfx_controller_node_consumer5, "Node5", 0);
	ndn::FibHelper::AddRoute("Node3", prfx_controller_node_consumer6, "Node6", 0);

	app1.Start(Seconds (15.0));
	app2.Start(Seconds (45.0));
	app4.Start(Seconds (75.0));
	app5.Start(Seconds (100.0));
	app6.Start(Seconds (125.0));

	Simulator::Stop(Seconds(300.0));

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
