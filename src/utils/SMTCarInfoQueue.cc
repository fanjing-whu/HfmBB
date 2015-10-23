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

#include <SMTCarInfoQueue.h>
#include <cmath>

namespace Fanjing {
bool SMTCarInfoQueue::overtakeAllowed = false;
double SMTCarInfoQueue::updateInterval = 0.1;
XMLDocument* SMTCarInfoQueue::doc = NULL;
bool SMTCarInfoQueue::XMLHasLoaded = false;
SMTCarInfoQueue::SMTCarInfoQueue() {
    // TODO Auto-generated constructor stub
    init();
}

SMTCarInfoQueue::SMTCarInfoQueue(string lane, string xmlpath, double length, double outLength) {
    init();
    laneName = lane;
    laneLength = length;
    laneOutLength = outLength;
    if (!XMLHasLoaded) {
        // load xml file only if has not
        doc->LoadFile(xmlpath.c_str());
    }
    root = doc->FirstChildElement("results");
    if(root == NULL){
        root = doc->NewElement("results");
        doc->LinkEndChild(root);
    }
    root = root->FirstChildElement("set");
    while(root != NULL){
        if(root->Attribute("laneName", lane.c_str()) != NULL){
            break;
        }
        root = root->NextSiblingElement("set");
    }
    if(root == NULL){
        root = doc->NewElement("set");
        root->SetAttribute("laneName", lane.c_str());
        doc->LinkEndChild(root);
    }
}

void SMTCarInfoQueue::saveResults(string filename) {
    doc->SaveFile(filename.c_str());
    releaseXML();
}

void SMTCarInfoQueue::releaseXML() {
    if (doc!=NULL) {
        doc->Clear();
        delete doc;
        doc = NULL;
    }
}

void SMTCarInfoQueue::updateCarQueueInfoAt(string id) {
    // todo
    // 1st. loose the queue time map

    // 2nd.

}

void SMTCarInfoQueue::setThePairMapAtFrontOfCar(map<double, list<string> >& carListMapByTime,
        map<string, double>& timeMapByCar, string id, string otherId) {
    // todo
    // 1st. seek to other id

}

void SMTCarInfoQueue::setThePairMapAtBackOfCar(map<double, list<string> >& carListMapByTime,
        map<string, double>& timeMapByCar, string id, string otherId) {
    // todo
}

void SMTCarInfoQueue::init() {
    laneLength = 0;
    allowedInterval = 0;
    cyclePeriod = 0;
    cycleOffset = 0;
    if(doc == NULL){
        doc = new XMLDocument();
    }
    root = NULL;
    element = NULL;
}

SMTCarInfoQueue::~SMTCarInfoQueue() {
    // TODO Auto-generated destructor stub
}

double SMTCarInfoQueue::insertCar(SMTCarInfo car, double time, double neighborFrozenSpace) {
    // todo
    // 1st. set the time entering this lane
    carMapById[car.id] = car;
    setEnterTimeOfCar(car.id, time);
    // 1st. judge the length of queue and the time entering the queue
    double queueLength = 0;
    string otherId = getFirstCarIdByOutTime(time);
    while(otherId != ""){
        // while there are some still in this lane
        // and if those car have already enter the queue area when this car entered this lane
        // add its gap and length into the queue length
        if(queueTimeMapById[otherId] <= time){
            SMTCarInfo otherCar = getCarInfoById(otherId);
            queueLength += otherCar.minGap + otherCar.length;
            otherId = getNextCarIdByOutTime();
        }
    }
    // 2nd. judge the length of queue considering the cars have not entered the queue yet
    otherId = getFirstCarIdByQueueTime(time);
    double preCarQueueTime;
    string preQueueCarId;   // the car before current after current enter queue area.
    // there still have some cars not entered the queue
    if(overtakeAllowed){
        while(otherId != ""){
            // if the car has not reach queue area yet
            SMTCarInfo otherCar = getCarInfoById(otherId);
            // 2nd(optional). overtake judgement process
            // process only if overtake process is enabled
            // if overtake is enable, the car befor this car may not increase its queue length
            // otherwise, all cars before this car will be added into the queue length.

            // calculate the problem of distance
            if(enterTimeMapById[otherId] <= enterTimeMapById[car.id]){
                // if the other car enter this lane first
                // assume current car can overtake other car
                // then when the current car reach the queue area
                // the other car should not reach the length + gap of both cars
                // caculate the distance from the start to the queue area if overtake successfully
                // this distance equals to the overtake allowed distance
                double overtakeLengtheForCurrentCar =
                        laneLength - queueLength > neighborFrozenSpace ? queueLength : neighborFrozenSpace;
                // before the queue area
                double overtakeLengthForOtherCar = overtakeLengtheForCurrentCar - car.minGap - car.length;
                // caculate the time of the current car reach the queue area if overtake successfully
                double reachTimeForCurrentCar = getTheReachTime(car, overtakeLengtheForCurrentCar, time, false, true);
                double reachTimeForOtherCar = getTheReachTime(otherCar, overtakeLengthForOtherCar,
                        enterTimeMapById[otherId], false, true);
                // decide overtake or not
                bool beOvertake = reachTimeForCurrentCar < reachTimeForOtherCar;
                // compare two time and decide overtake or not
                // and if overtake, the car enter queue area later needs to be updated
                if(beOvertake){
                    // if overtake happens, the time that current car enter the queue become seated.
                    // since the rest existing car enter queue later than current car.
                    // and if overtake, the car enter queue area later needs to be updated after caculated current car
                    // set the queue time
                    reachTimeForCurrentCar = getTheReachTime(car, laneLength - queueLength, time, false, true);
                    // make sure the current queue time not earlier than the previous one.
                    if(reachTimeForCurrentCar > preCarQueueTime){
                        setQueueTimeOfCar(car.id, reachTimeForCurrentCar);
                    }else{
                        setQueueTimeOfCar(car.id, preCarQueueTime);
                    }
                    // set the other car's queue time after current car
                    // set the same time after current will make other car in later position than current car
                    // and even more, the new queue time for other will become earlier than its old one
                    // and the queue time of current will be more earlier than the other's old one
                    // so this operation will not break the original order of the queue time
                    reachTimeForOtherCar = getTheReachTime(otherCar, laneLength - queueLength - car.minGap - car.length,
                            enterTimeMapById[otherId], false, true);
                    // make sure the current queue time not earlier than the previous one.
                    if(reachTimeForOtherCar > preCarQueueTime){
                        setQueueTimeOfCar(car.id, reachTimeForOtherCar);
                    }else{
                        setQueueTimeOfCar(car.id, preCarQueueTime);
                    }
                    break;
                }else{
                    // if current car reach later, other car get to the queue before current car
                    // and then judge next car reach the queue area
                    queueLength += otherCar.minGap + otherCar.length;
                    preCarQueueTime = queueTimeMapById[otherId];
                    otherId = getNextCarIdByQueueTime();
                    if(otherId == ""){
                        // fixme !! resort the time map, important... the overtak process can not work proper
                        // there no other cars, the current car's queue time is seated.
                        reachTimeForCurrentCar = getTheReachTime(car, laneLength - queueLength, time, false, true);
                        // make sure the current queue time not earlier than the previous one.
                        if(reachTimeForCurrentCar > preCarQueueTime){
                            setQueueTimeOfCar(car.id, reachTimeForCurrentCar);
                        }else{
                            setQueueTimeOfCar(car.id, preCarQueueTime);
                        }
                    }
                }
            }else{
                // if the other car enter this lane later
                // assume other car can overtake current car
                // then the same as the above
                double overtakeLengthForOtherCar =
                        laneLength - queueLength > neighborFrozenSpace ? queueLength : neighborFrozenSpace;
                double overtakeLengtheForCurrentCar = overtakeLengthForOtherCar - car.minGap - car.length;
                // caculate the time of the current car reach the queue area if overtake successfully
                double reachTimeForCurrentCar = getTheReachTime(car, overtakeLengtheForCurrentCar, time, false, true);
                double reachTimeForOtherCar = getTheReachTime(otherCar, overtakeLengthForOtherCar,
                        enterTimeMapById[otherId], false, true);
                // decide overtake or not
                // if other car reach before then it overtake current car
                bool beOvertake = reachTimeForOtherCar < reachTimeForCurrentCar;
                // compare two time and decide overtake or not
                // and if overtake, nothing change, continue to judge next car
                if(beOvertake){
                    // if current car reach first, the other car get to the queue before current car
                    // in this situation, the queue time of other car do not change
                    // and then judge next car reach the queue area
                    queueLength += otherCar.minGap + otherCar.length;
                    preCarQueueTime = queueTimeMapById[otherId];
                    otherId = getNextCarIdByQueueTime();
                    {
                        // fixme !! resort the time map, important... the overtak process can not work proper
                        if(otherId == ""){
                            // there no other cars, the current car's queue time is seated.
                            reachTimeForCurrentCar = getTheReachTime(car, laneLength - queueLength, time, false, true);
                            // make sure the current queue time not earlier than the previous one.
                            if(reachTimeForCurrentCar > preCarQueueTime){
                                setQueueTimeOfCar(car.id, reachTimeForCurrentCar);
                            }else{
                                setQueueTimeOfCar(car.id, preCarQueueTime);
                            }
                        }
                    }
                }else{
                    // if overtake not happens, the time that current car enter the queue become seated.
                    // since the rest existing car enter queue later than current car.
                    // and if not overtake
                    // the car enter queue area later needs to be updated after caculated current car
                    // set the queue time
                    reachTimeForCurrentCar = getTheReachTime(car, laneLength - queueLength, time, false, true);
                    // set the other later cars' queue time after current car, if they were before current one
                    // get car list that needs to be modified
                    // fixme !! resort the time map, important... the overtak process can not work proper
                    list<string> modifiedCarList;
                    // list<string>::it
                    setQueueTimeOfCar(car.id, reachTimeForCurrentCar);
                    while(otherId != ""){
                        if(queueTimeMapById[otherId] <= reachTimeForCurrentCar){
                            setQueueTimeOfCar(otherId, reachTimeForCurrentCar);
                            //otherId =
                        }else{
                            break;
                        }
                    }
                    break;
                }
            }
        }

    }else{
        // if overtake process is disabled
        // then all cars before this car will be added into the queue length.

        // get the previous car entering this lane
        string preEnterCarId = getFirstCarIdByEnterTime(time);
        string nextToPreEnterCarId = getNextCarIdByEnterTime();
        // if the next one of the preEnterCarId is later than current car, find next.
        // actually, this time can only equal or bigger than time, if equal, find next.
        while(enterTimeMapById[nextToPreEnterCarId] <= time){
            // find next
            preEnterCarId = nextToPreEnterCarId;
            nextToPreEnterCarId = getNextCarIdByEnterTime();
        }
        while(otherId != nextToPreEnterCarId){
            // if the car has not reach queue area yet
            // when overtake disabled, the last other car is the car enter next to the pre enter.
            SMTCarInfo otherCar = getCarInfoById(otherId);
            // if other car enter this lane before current car, increase queue length
            queueLength += otherCar.minGap + otherCar.length;
            otherId = getNextCarIdByQueueTime();
        }
        // cacluate the reach queue time if no other car affecting it
        double reachQueueTimeForCurrentCar = getTheReachTime(car, laneLength - queueLength, time, false, true);
        if(queueTimeMapById[preEnterCarId] > reachQueueTimeForCurrentCar){
            // current car is obstruct by previous car
            reachQueueTimeForCurrentCar = queueTimeMapById[preEnterCarId];
        }
        setQueueTimeOfCar(car.id, reachQueueTimeForCurrentCar);
    }
    // 3rd. update the affected cars from this car
    updateCarQueueInfoAt(car.id);
    // 4th. caculate the time current car start to level the queue.
    // before this setp, the queue time should be updated.
    double startOutQueueTime = queueTimeMapById[car.id];
    if(startOutQueueTime < outQueueTimeMapById[preQueueCarId] + updateInterval){
        startOutQueueTime = outQueueTimeMapById[preQueueCarId] + updateInterval;
    }
    double outTimeWithoutAffected = getTheReachTime(car, queueLength, startOutQueueTime, true, false);
    // calculate the out time affected by previous cars
    if(outTimeWithoutAffected<outTimeMapById[preQueueCarId]+updateInterval){
        outTimeWithoutAffected = outTimeMapById[preQueueCarId]+updateInterval;
    }
    outTimeMapById[car.id] = outTimeWithoutAffected;
    // 5th. return the finial out time
    return outTimeMapById[car.id];
}
double SMTCarInfoQueue::getTheReachTime(SMTCarInfo car, double length, double startTime, bool considerAccel,
        bool considerDecel) {
    // if consider both accel phases
    double time = 0;
    if(considerAccel && considerDecel){
        // if accel finished
        // S > v^2/2a(decel) + v^2/2a(accel)
        if(length > car.maxSpeed * car.maxSpeed / (2 * car.decel) + car.maxSpeed * car.maxSpeed / (2 * car.accel)){
            // S = v*(t-v/a(decel)-v/a(accel)) + v^2/2a(decel) + v^2/2a(accel)
            // ==> t = S/v + v/2a(decel)-v/2a(accel)
            time = car.maxSpeed / (2 * car.accel) + car.maxSpeed / (2 * car.decel) + length / car.maxSpeed;
        }else{
            // a(accel)t(accel) = a(decel)t(decel) && S = a(accel)t(accel)^2/2 + a(decel)t(decel)^2/2
            // ==> t = sqrt(2*S*(a(accel)+a(decel))/(a(accel)a(decel)))
            time = sqrt(2 * length * (car.accel + car.decel) / (car.accel * car.decel));
        }
    }else if(considerAccel){
        // if only consider accel phase
        if(length > car.maxSpeed * car.maxSpeed / (2 * car.accel)){
            // if accel finished
            // S = vt - v^2/2a ==> t = (S+v^2/2a)/v = S/v + v/2a
            time = length / car.maxSpeed + car.maxSpeed / (2 * car.accel);
        }else{
            // S = at^2/2 ==> t = sqrt(2S/a)
            time = sqrt(2 * length / car.accel);
        }
    }else if(considerDecel){
        // if only consider decel phase
        if(length > car.maxSpeed * car.maxSpeed / (2 * car.decel)){
            // if accel finished
            // S = vt - v^2/2a ==> t = (S+v^2/2a)/v = S/v + v/2a
            time = length / car.maxSpeed + car.maxSpeed / (2 * car.decel);
        }else{
            // S = at^2/2 ==> t = sqrt(2S/a)
            time = sqrt(2 * length / car.decel);
        }
    }else{
        // consider neither accel or decel phase
        time = length / car.maxSpeed;
    }
    return time + startTime;
}

SMTCarInfo SMTCarInfoQueue::getCarInfoById(string id) {
    return carMapById[id];
}

void SMTCarInfoQueue::setCurrentTime(double time) {
// not necessary at now...
}

double SMTCarInfoQueue::releaseCar(string id, double time) {
// make recording
    element = doc->NewElement("result");
    element->SetAttribute("car", id.c_str());
    element->SetAttribute("enterTime", enterTimeMapById[id]);
    element->SetAttribute("queueTime", queueTimeMapById[id]);
    element->SetAttribute("startOutQueueTime", outQueueTimeMapById[id]);
    element->SetAttribute("outTime", outTimeMapById[id]);
    element->SetAttribute("actualOutTime", time);
    root->LinkEndChild(element);
// release the old car and return the predicted out time
    double outTime = outTimeMapById[id];
    removeCar(id);
    return outTime;
}

void SMTCarInfoQueue::setCycleInfo(double period, double allowTime, double offset) {
    cyclePeriod = period;
    allowedInterval = allowTime;
    cycleOffset = offset;
}

void SMTCarInfoQueue::removeCar(string id) {
    carMapById.erase(id);
    carMapByEnterTime[enterTimeMapById[id]].remove(id);
    carMapByQueueTime[queueTimeMapById[id]].remove(id);
    carMapByOutTime[outTimeMapById[id]].remove(id);
    outQueueTimeMapById.erase(id);
    // fixme needs update later car or not?
}
void SMTCarInfoQueue::setEnterTimeOfCar(string id, double time) {
// set the enter time of a car and update both carMapByEnterTime and enterTimeMapById
    setThePairMap(carMapByEnterTime, enterTimeMapById, id, time);
}

void SMTCarInfoQueue::setQueueTimeOfCar(string id, double time) {
// set the enter time of a car and update both carMapByQueueTime and queueTimeMapById
    setThePairMap(carMapByQueueTime, queueTimeMapById, id, time);
}

void SMTCarInfoQueue::setOutTimeOfCar(string id, double time) {
// set the enter time of a car and update both carMapByOutTime and outTimeMapById
    setThePairMap(carMapByOutTime, outTimeMapById, id, time);
}

void SMTCarInfoQueue::setThePairMap(map<double, list<string> > &carListMapByTime, map<string, double>&timeMapByCar,
        string id, double time) {
// set the pair map
    if(timeMapByCar.find(id) == timeMapByCar.end()){
        // insert new car
        timeMapByCar[id] = time;
        // insert this car to time
        carListMapByTime[time].push_back(id);
    }else{
        // if the car is already here, update the related information
        // 1st. remove the old information
        carListMapByTime[timeMapByCar[id]].remove(id);
        // 2nd. add this car at new time
        carListMapByTime[time].push_back(id);
    }
}

string SMTCarInfoQueue::getFirstCarIdByEnterTime(double time) {
    return getFirstCarIdByCertainTime(carMapByEnterTime, itCarMapByEnterTime, litCarMapByEnterTime, time);
}

string SMTCarInfoQueue::getFirstCarIdByQueueTime(double time) {
    return getFirstCarIdByCertainTime(carMapByQueueTime, itcarMapByQueueTime, litcarMapByQueueTime, time);
}

string SMTCarInfoQueue::getFirstCarIdByOutTime(double time) {
    return getFirstCarIdByCertainTime(carMapByOutTime, itcarMapByOutTime, litcarMapByOutTime, time);
}

string SMTCarInfoQueue::getNextCarIdByEnterTime() {
    return getNextCarIdByCertainTime(carMapByEnterTime, itCarMapByEnterTime, litCarMapByEnterTime);
}

string SMTCarInfoQueue::getNextCarIdByQueueTime() {
    return getNextCarIdByCertainTime(carMapByQueueTime, itcarMapByQueueTime, litcarMapByQueueTime);
}

string SMTCarInfoQueue::getNextCarIdByOutTime() {
    return getNextCarIdByCertainTime(carMapByOutTime, itcarMapByOutTime, litcarMapByOutTime);
}

string SMTCarInfoQueue::getPreviousCarIdByEnterTime() {
    return getPreviousCarIdByCertainTime(carMapByEnterTime, itCarMapByEnterTime, litCarMapByEnterTime);
}

string SMTCarInfoQueue::getPreviousCarIdByQueueTime() {
    return getPreviousCarIdByCertainTime(carMapByQueueTime, itcarMapByQueueTime, litcarMapByQueueTime);
}

string SMTCarInfoQueue::getPreviousCarIdByOutTime() {
    return getPreviousCarIdByCertainTime(carMapByOutTime, itcarMapByOutTime, litcarMapByOutTime);
}
string SMTCarInfoQueue::getFirstCarIdByCertainTime(map<double, list<string> >& carListMapByCertainTime,
        map<double, list<string> >::iterator& it, list<string>::iterator& lit, double time) {
    // todo
    // get first time node (include given time)
    it = carListMapByCertainTime.lower_bound(time);
    lit = it->second.begin();
    if(lit != it->second.begin()){
        return *lit;
    }
    return "";
}

string SMTCarInfoQueue::getPreviousCarIdByCertainTime(map<double, list<string> >& carListMapByCertainTime,
        map<double, list<string> >::iterator& it, list<string>::iterator& lit) {
    if(lit != it->second.begin()){
        // if the list has more objects before, get previous
        lit--;
        return *lit;
    }else if(it != carListMapByCertainTime.begin()){
        // if the list has no object any more , get cars in previous time list and return the last car
        it--;
        lit = it->second.end();
        if(lit != it->second.begin()){
            lit--;
            return *lit;
        }
    }
    // if the list has no more object and no car after the time return blank string
    return "";
}
string SMTCarInfoQueue::getNextCarIdByCertainTime(map<double, list<string> >& carListMapByCertainTime,
        map<double, list<string> >::iterator& it, list<string>::iterator& lit) {
    if(lit != it->second.end()){
        // if the list has more objects, get next
        lit++;
        return *lit;
    }else if(it != carListMapByCertainTime.end()){
        // if the list has no object any more get later time list and return the first car
        it++;
        lit = it->second.begin();
        if(lit != it->second.end()){
            return *lit;
        }
    }
// if the list has no more object and no car after the time return blank string
    return "";
}

// if the out time is not allowed to get out, then return the start of next allowed time
double SMTCarInfoQueue::getFixedOutTime(double time) {
// fixme the final solution is read the xml file, now just use the cycle period
    double preAllowedTime = cycleOffset + cyclePeriod * (int) ((time - cycleOffset) / cyclePeriod);
    if(time - preAllowedTime > allowedInterval){
        time = preAllowedTime + cyclePeriod;
    }
    return time;
}

} /* namespace Fanjing */
