/**
 * @file BridgeWatchdogServer.C
 *    Will call emergency stop if there is no update to the "/BridgeWatchdog" channel for a certain time.
 *
 * @author Christian Reuther
 * @date   2015/02/12
 */

#include <fw/Unit.h>
#include <serialization/Serialization.h>

namespace mira { 

////////////////////////////////////////////////////////////////////////////////

class BridgeWatchdogServer : public Unit
{
	MIRA_OBJECT(BridgeWatchdogServer)

public:
	// Spin with 100 milliseconds precision
	BridgeWatchdogServer() : Unit(Duration::milliseconds(100)) {
		mMaxPingAge = Duration::seconds(2);
	}

	template<typename Reflector> void reflect(Reflector& r)
	{
		// Reflect the basic service methods of the base class mira::Unit.
		Unit::reflect(r);
		
		// Set it as a property so it can be changed at runtime
		r.property("MaxPingAge", mMaxPingAge, "Maximum duration between pings before emergencyStop is triggered", Duration::seconds(2));
	}

protected:
	virtual void initialize() {
		mWatchdogChannel = subscribe<std::string>(resolveName("BridgeWatchdog"));
		
		// Definitely wait until robot service is available.
		waitForService("/robot/Robot", Duration::seconds(10));
	}
	
	virtual void process(const Timer& timer) {
		const Time now = Time::now();
		
		// Get data from watchdog channel and check the timestamp
		try {
			auto read = mWatchdogChannel.read();
			if(Duration(now - read->timestamp) > mMaxPingAge)
				triggerEmergencyStop();
		} catch(...) {
			// Something went wrong, we should definitely stop just in case
			triggerEmergencyStop();
		}
	}

protected: 
	void triggerEmergencyStop() {
		MIRA_LOG(ERROR) << "BridgeWatchdogServer: Triggered emergency stop!";
		
		auto rpcFuture = callService<void>("/robot/Robot", "emergencyStop");
		rpcFuture.wait();
		rpcFuture.get(); // get an exception if it occurred - nothing we can do though
	}

protected:
	Channel<std::string> mWatchdogChannel;
	Duration mMaxPingAge;
};

///////////////////////////////////////////////////////////////////////////////

}

MIRA_CLASS_SERIALIZATION(mira::BridgeWatchdogServer, mira::Unit);
