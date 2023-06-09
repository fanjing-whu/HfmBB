/*
 * BaseGlobalAppLayer.h
 *
 *  Created on: 2014-6-11
 *      Author: Fanjing-LAB
 */

#ifndef BASEGLOBALAPPLAYER_H_
#define BASEGLOBALAPPLAYER_H_

#include "BaseApplLayer.h"
#include "FindModule.h"
#include "GlobalStatistics.h"

class BaseGlobalAppLayer: public BaseApplLayer {
public:
    BaseGlobalAppLayer();
    virtual ~BaseGlobalAppLayer();
public:
    virtual void initialize(int stage);
protected:
    GlobalStatistics* gs;

private:
    GlobalStatistics* getGS();
};

#endif /* BASEGLOBALAPPLAYER_H_ */
