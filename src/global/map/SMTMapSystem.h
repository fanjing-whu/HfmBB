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

#ifndef SMTMAPSYSTEM_H_
#define SMTMAPSYSTEM_H_

#include "GlobalMapSystem.h"
#include "SMTCarInfoQueue.h"
using namespace Fanjing;

class SMTLane {
public:
    string id;  // 车道id
    string edge;    // 街道id
    string tl;  // 控制灯id
    int index;  // 控制灯索引号
};
class SMTEdge {
public:
    // FIXME 一条车道可能对应多个下一条道路
    // 同时，不同车道也有可能对应同一条下一条道路
    // 为了试验的方便，这里假设其都是一一对应的
    string id;  // 街道id
    vector<string> laneVec; // 车道向量
    vector<string> queueVec;    // 车道对应的队列管理向量
    vector<string> nextVec; // 车道对应的下一条街道向量
};
class SMTTrafficLight {
public:
    string id;  // 交通灯id
    vector<double> allowedInterval; // 交通灯对应索引号的通行允许时间向量
    vector<double> cyclePeriod; // 交通灯对应索引号的周期时间向量
    vector<double> cycleOffset; // 交通灯对应索引号的允许时间起始偏移时间向量
};
class SMTOutInfo {
public:
    double enterTime;   // 进入时间
    double throughTime; // 通过消耗
};
class SMTMapSystem : public GlobalMapSystem {
public:
    SMTMapSystem();
    virtual ~SMTMapSystem();

    virtual void uploadRoute(SMTCarInfo car, list<string> route, double time);
    virtual void enterRoad(SMTCarInfo car, string road, double time);
    virtual void disableOvertake(string car);
protected:
    list<string> normalEdgeList;    // 主要街道列表（非":"开头的街道）
    map<string, SMTEdge> edgeMap;   // 主要街道列表（非":"开头的街道）
    map<string, SMTLane> laneMap;   // 车道列表
    map<string, SMTCarInfoQueue> queueMap;  // 车道对应的队列管理列表
    map<string, SMTTrafficLight> tlMap; // 控制灯列表

    // 记录车道内包含的车辆，用于绘制轨迹之类的。
    map<string, list<string> > laneContainer;
    // 若车辆会经过目标车道，则对其进行记录
    map<string, string> laneNameByCarId;
    int releasedCarNum;
    string xmlName;

    // 用于记录通行历史的map
    map<string, double> enterTimeMap;
    map<string, list<SMTOutInfo> > outInfoMap;
    map<string, list<SMTOutInfo> > historicalInfoMap;

    cMessage* trajectoryMsg;
    virtual void initialize(int stage);
    virtual void finish();
    virtual void handleMessage(cMessage* msg);
    void ganerateMapTopology();
    void recordTrajectoryMsg();
    static bool compare_out_info(SMTOutInfo & first, SMTOutInfo &second);
};
class SMTMapSystemAccess {
public:
    SMTMapSystem* get() {
        return FindModule<SMTMapSystem*>::findGlobalModule();
    }
};
#endif /* SMTMAPSYSTEM_H_ */
