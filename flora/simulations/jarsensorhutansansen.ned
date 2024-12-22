package flora.simulations;

import inet.applications.udpapp.UdpBasicApp;
import inet.common.geometry.common.IGeographicCoordinateSystem;
import inet.common.geometry.common.SimpleGeographicCoordinateSystem;
import inet.node.inet.Router;
import inet.node.internetcloud.InternetCloud;
import flora.LoRaPhy.LoRaMedium;
import flora.LoraNode.LoRaNode;
import flora.LoraNode.LoRaGW;
import inet.node.inet.StandardHost;
import inet.networklayer.configurator.ipv4.Ipv4NetworkConfigurator;
import inet.node.ethernet.Eth1G;
import inet.visualizer.canvas.integrated.IntegratedCanvasVisualizer;
import inet.visualizer.canvas.scene.OpenStreetMapSceneCanvasVisualizer;


@license(LGPL);
network jarsensorhutansansen
{
    parameters:
        int numberOfNodes = default(1);
        int numberOfGateways = default(1);
        int networkSizeX = default(500);
        int networkSizeY = default(500);
        @display("bgb=1845,1560");
    submodules:

        visualizer: IntegratedCanvasVisualizer {
            @display("p=798,341");
        }
        coordinateSystem: SimpleGeographicCoordinateSystem {
            @display("p=654,341");
        }
        loRaNodes[numberOfNodes]: LoRaNode {
            @display("p=97,84;is=s");
        }
        loRaGW[numberOfGateways]: LoRaGW {
            @display("p=210,166;is=s");
        }
        LoRaMedium: LoRaMedium {
            @display("p=935,163");
        }
        networkServer: StandardHost {
            parameters:
                @display("p=125,569");
        }
        configurator: Ipv4NetworkConfigurator {
            parameters:
                assignDisjunctSubnetAddresses = false;
                @display("p=938,341");
        }
        internetCloud: InternetCloud {
            @display("p=369,569");
        }
        gwRouter[numberOfGateways]: Router {
            @display("p=369,344");
        }
        nsRouter: Router {
            @display("p=247,569");
        }
    connections:
        networkServer.ethg++ <--> Eth1G <--> nsRouter.ethg++;
        nsRouter.pppg++ <--> Eth1G <--> internetCloud.pppg++;
        for i=0..numberOfGateways-1 {
            internetCloud.pppg++ <--> Eth1G <--> gwRouter[i].pppg++;
            gwRouter[i].ethg++ <--> Eth1G <--> loRaGW[i].ethg++;
        }
}

