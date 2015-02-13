/**
 * @file BridgeWatchdogClient.C
 *    Will continuously send pings to a channel to notify a BridgeWatchdogServer that everything is okay.
 *
 * @author Christian Reuther
 * @date   2015/02/12
 */

#include <fw/Unit.h>
#include <serialization/Serialization.h>

namespace mira { 

////////////////////////////////////////////////////////////////////////////////

class BridgeWatchdogClient : public Unit
{
	MIRA_OBJECT(BridgeWatchdogClient)

public:
	// Spin with 100 milliseconds precision. This can be altered using the serialization interface
	BridgeWatchdogClient() : Unit(Duration::milliseconds(100)) {
		mMaxPingAge = Duration::seconds(2);
	}

	template<typename Reflector> void reflect(Reflector& r)
	{
		// Reflect the basic service methods of the base class mira::Unit.
		Unit::reflect(r);
	}

protected:
	virtual void initialize() {
		mWatchdogChannel = publish<std::string>("/BridgeWatchdog");
	}
	
	virtual void process(const Timer& timer) {
		// Will get the current timestamp
		mWatchdogChannel.post("OK");
	}
	
protected:
	Channel<std::string> mWatchdogChannel;
};

///////////////////////////////////////////////////////////////////////////////

}

MIRA_CLASS_SERIALIZATION(mira::BridgeWatchdogClient, mira::Unit);
