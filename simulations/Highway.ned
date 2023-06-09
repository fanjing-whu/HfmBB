//
// Copyright (C) 2008 Christoph Sommer <christoph.sommer@informatik.uni-erlangen.de>
//
// Documentation for these modules is at http://veins.car2x.org/
//
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
//

package smartcity.flowreserved.simulations;

import smartcity.flowreserved.global.conManager.SMTConManager;
import org.mixim.base.modules.BaseWorldUtility;
import smartcity.flowreserved.global.obstacles.ObstacleControl_Switch;
import smartcity.flowreserved.global.annotations.AnnotationManager_Colorful;
import smartcity.flowreserved.global.map.SMTMapSystem;
import smartcity.flowreserved.global.launchd.GlobalMobilityLaunchd;
import smartcity.flowreserved.global.statistics.GlobalStatistics;
import smartcity.flowreserved.global.vehicleManager.GlobalVehicleManager;

module Highway
{
    parameters:
        double playgroundSizeX @unit(m); // x size of the area the nodes are in (in meters)
        double playgroundSizeY @unit(m); // y size of the area the nodes are in (in meters)
        double playgroundSizeZ @unit(m); // z size of the area the nodes are in (in meters)
        @display("bgb=$playgroundSizeX,$playgroundSizeY");
    submodules:
        obstacles: ObstacleControl_Switch {
            @display("p=240,50");
        }
        annotations: AnnotationManager_Colorful {
            @display("p=260,50");
        }
        connectionManager: SMTConManager {
            parameters:
                @display("p=150,0;i=abstract/multicast");
        }
        world: BaseWorldUtility {
            parameters:
                playgroundSizeX = playgroundSizeX;
                playgroundSizeY = playgroundSizeY;
                playgroundSizeZ = playgroundSizeZ;
                @display("p=30,0;i=misc/globe");
        }
        manager: GlobalMobilityLaunchd {
            parameters:
                @display("p=512,128");
        }
        map: SMTMapSystem {
            @display("p=200,50");
        }
        gs: GlobalStatistics {
            parameters:
                @display("p=200,100;i=block/blackboard;is=s");
        }
        vm: GlobalVehicleManager {
            @display("p=200,150");
        }
    connections allowunconnected:
}

