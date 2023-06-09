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

#ifndef __SMARTCITYFLOWRESERVED_GLOBALVEHICLEMANAGER_H_
#define __SMARTCITYFLOWRESERVED_GLOBALVEHICLEMANAGER_H_

#include <omnetpp.h>
#include "GlobalMapSystem.h"
#include "StatisticsRecordTools.h"
#include "SMTCarInfo.h"
#include "CarFlowGenerator.h"
using std::string;
using Fanjing::StatisticsRecordTools;
using Fanjing::SMTCarInfo;
using Fanjing::CarFlowGenerator;
/**
 * 车辆管理
 */
class GlobalVehicleManager : public cSimpleModule {
public:
    GlobalVehicleManager();
    virtual ~GlobalVehicleManager();
public:
    enum VehicleType {
        GVM_VEHICLETYPE_NORMAL = 0, // normal vehicle: use a random path
        GVM_VEHICLETYPE_ONDUTY,   // on-duty vehicle: drive to work place and back
        GVM_VEHICLETYPE_BUS,    // bus: fixed loop path
        GVM_VEHICLETYPE_TAXI,   // taxi: continually random path
        GVM_VEHICLETYPE_EMERGENCE,  // emergence vehicle: go to a random location and back to the station
        GVM_VEHICLETYPE_ADMIN,  // unnecessary! road administration: come out when the transport system is idle
        GVM_VEHICLETYPE_SHOPPING, // unimportant! shopping vehicle: go to a shop and back. just a car with a random destination now.
        // add new vehicle type above if any.
        GVM_VEHICLETYPE_NUMEND_MARK // means the number of the vehicle type
    };
public:
    virtual SMTCarInfo getCarInfo(string id);
    static string getStartPoint(string road);
    static string getEndPoint(string road);
    static bool compare_departTime(string& first, string& second);
protected:
    virtual void initialize();
    virtual void handleMessage(cMessage *msg);
    virtual void finish();

protected:
    unsigned int targetNum;
    std::map<string, int> intMap;
    std::map<string, cMessage*> msgMap;
    cMessage* testMsg;
    bool endAfterGenCFG;
    cMessage* endMsg;
    StatisticsRecordTools* srt;
    // members
    GlobalMapSystem *map;
    // used in vehicle generating process
    std::map<int, int> targetVehicleNumPerType;   // the target vehicles number of each vehicle type
    std::map<int, int> vehicleNumPerType;   // the vehicles number of each vehicle type
    // 记录对应道路上一次生成车辆的车道
    // 用来避免车辆生成过于密集，导致的车辆消失的bug
    std::map<string, int> curLastUsedLane;
    std::map<string, int> roadLaneNumber;

    // car map
    static std::map<string, SMTCarInfo> carMapByID;  // store car instance
    std::list<string> carIdMapByDepartTime; //
    std::list<string>::iterator itCarIdMapByDepartTime;

    // functions
    GlobalMapSystem* getMapSystem();

    // car generating process
    void updateVehicleList();   // generate vehicles to keep there are certain number vehicles in the network
    // functions to add vehicles
    // the mobility module must get its vehicle type by using function - getVehiclesType()
    // if else that the vehicles will be marked as normal vehicle
    virtual void addOneVehicle(SMTCarInfo car);   // add a car of a certain type
    virtual void addVehicles(VehicleType type, int num); // add several cars of a certain type
    // generating car flow and related
    bool disableSinFix;
    bool enableBurst;
    int nBurstNum;
    string strBurstIn;  // 突发车辆道路入口对应的位置
    string strBurstOut;  // 突发车辆道路出口对应的位置
    int nBurstOut;
    int carNumLimit;
    double carSpawnTimeLimit;
    string carFlowXMLPath;
    bool generateNewXMLFile;
    double maxCarFlowRate;
    double minCarFlowRate;
    double maxFreeCarFlowRate;
    double minFreeCarFlowRate;
    double carSpawnJudgeInterval;
    double carSpawnPeriod;
    double carSpawnOffset;
    bool useCarSpawnOffset;
    double carSpawnStartTime;
    double reverseCarFlowRate;
    // cfg class related
    string carVTypeXMLPath;
    CarFlowGenerator carFlowHelper;
    virtual void generateCarFlowFile();
    virtual void loadCarFlowFile();

    //
    // tools functions
    // if do not provide lauchFilePath then use the current launch config
    string getRouXMLFromLaunchConfig(string launchFilePath = "");


private:
    double SinFuncFixed(double t, double period, double up, double down);
};
class GlobalVehicleManagerAccess {
public:
    GlobalVehicleManager* get() {
        return FindModule<GlobalVehicleManager*>::findGlobalModule();
    }
};
#endif
