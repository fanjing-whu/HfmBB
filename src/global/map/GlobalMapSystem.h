//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU Lesser General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
// 
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser General Public License for more details.
// 
// You should have received a copy of the GNU Lesser General Public License
// along with this program.  If not, see http://www.gnu.org/licenses/.
// 

#ifndef __SMARTCITYFLOWRESERVED_GLOBALMAPSYSTEM_H_
#define __SMARTCITYFLOWRESERVED_GLOBALMAPSYSTEM_H_

#include <string>
#include <sstream>
#include <list>
#include <map>
#include <stdexcept>

#include "FindModule.h"
#include "BaseModule.h"
#include "global/launchd/GlobalMobilityLaunchd.h"
#include "AnnotationManager_Colorful.h"

/**
 * TODO - Generated class
 */
using std::string;
using std::map;
using std::list;
using std::set;
using std::vector;

class GlobalMapSystem : public BaseModule {
public:
    GlobalMapSystem();
    virtual ~GlobalMapSystem();
public:
    class Lane;
    class Edge;
    class Lane {
    public:
        string name;
        Edge* edge;
        int linkNumber;
        set<Lane*> links;
        double length;
        list<AnnotationManager_Colorful::Line_Colorful*> visualRepresentations;
        void setColor(string color);
    };
    class Edge {
    public:
        string name;
        int laneNumber;
        set<Lane*> lanes;
        int linkNumber;
        set<Edge*> links;
        double length;
        void setColor(string color);
    };
    class Node {
        string name;
        string type;
        Coord pos;
        double r;
        void setColor(string color);
    };
protected:
    virtual void initialize(int stage);
    virtual void handleMessage(cMessage *msg);
    virtual void finish();
    virtual int numInitStages() const {
        return std::max(cSimpleModule::numInitStages(), 3);
    }

public:
    virtual int generateMap(int stage);
    virtual double getTravelTime(string edge, double time, double speed);
    virtual list<string> getFastestRoute(string fromEdge, string toEdge);
    virtual list<string> getRandomRoute(string from, string to);
    virtual GlobalMobilityLaunchd* getManager() const {
        if (!manager) {
            manager = GlobalMobilityLaunchdAccess().get();
        }
        ASSERT(manager);
        return manager;
    }
protected:
    list<string> commandGetLanes(Lane* lane);

protected:
    mutable GlobalMobilityLaunchd* manager;
    AnnotationManager_Colorful* annotations;
    AnnotationManager_Colorful::Group* annotationGroup;
    map<string, Lane*> laneMap;
    map<string, Edge*> edgeMap;

    cMessage* stateSwitchMsg;
    cMessage* startMsg;

    int mapstage;
    bool noconnect;
    bool initialized;
    int hostnum;
    int curHostnum;
private:
    class MapEdge;
    class MapRoute;
    class MapEdge {
    public:
        Edge* edge;
        set<MapRoute*> routes;
    };
    class MapRoute{
    public:
        string target;
        double length;
        list<string> edges;
    };
private:
    string getRandomEdgeFromCache();

    string int2str(int i);
    string double2color(double d);
    string rgb2color(int r, int g, int b);
private:
    // use for searching routes
    map<string, MapEdge*> cacheBackupEdges;
    mutable list<MapEdge*> cacheUntappedEdges;
    mutable list<MapEdge*> cacheTappedEdges;
};

#endif
