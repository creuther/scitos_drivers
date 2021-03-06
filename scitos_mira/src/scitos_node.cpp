#include <ros/ros.h>
#include <ros/console.h>

#include <scitos_mira/ScitosG5.h>

#include <boost/thread.hpp>
#include <boost/algorithm/string.hpp>

#include <string>
#include <vector>

#include <error/LoggingCore.h>

class RosLogSink : public mira::LogSink {
  void consume(const mira::LogRecord &record) {
	switch (record.level) {
	case mira::SeverityLevel::CRITICAL:
	case mira::SeverityLevel::ERROR:
	  ROS_ERROR_STREAM("(MIRA) " << record.message);
	  break;
	case mira::SeverityLevel::WARNING:
	  ROS_WARN_STREAM("(MIRA) " << record.message);
	  break;
	case mira::SeverityLevel::NOTICE:
	  ROS_INFO_STREAM("(MIRA) " << record.message);
	  break;
	case mira::SeverityLevel::DEBUG:
	  ROS_DEBUG_STREAM("(MIRA) " << record.message);
	  break;
	default:
	  break;
	}
  }
};

int main(int argc, char **argv) {
	ros::init(argc, argv, "scitos_node");

	MIRA_LOGGER.registerSink(RosLogSink());
	
	std::string config_file, server_port, debug_level, scitos_modules;
	std::vector<std::string> args;
	
	// Start our watchdog client with the mira process
	if(ros::param::get("~config_file", config_file))  {
		args.push_back(std::string("-c"));
		args.push_back(config_file);
	} else {
		// TODO: Throw exception / log something?
	}
	
	// Connect to the main mira instance using the provided port
	if(ros::param::get("~server_port", server_port))  {
		args.push_back(std::string("-k"));
		
		std::stringstream ss;
		ss << "127.0.0.1:" << server_port;
		args.push_back(ss.str());
	} else {
		// TODO: Throw exception / log something?
	}
	
	// Use the specified debug level
	if(ros::param::get("~mira_debug_level", debug_level))  {
		args.push_back(std::string("-d"));
		args.push_back(debug_level);
	}
	
	mira::Framework framework(args, true);
	
	while (!framework.isInExec()) {
	  boost::this_thread::sleep(boost::posix_time::milliseconds(50));
	}
	// above is no use.
	boost::this_thread::sleep(boost::posix_time::milliseconds(5000));

	// Find out what SCITOS hardware modules are to be interfaced
	// this could come from reading the MIRA config file but then different config files would be required.
	if (!(ros::param::get("~scitos_modules", scitos_modules)) ) {
	  ROS_ERROR("Can't read parameter 'scitos_modules'. This MUST be supplied as a space seperated "
		    "list of SCITOS hardware modules to interface into ROS"); 
	  return 1;
	}

	ROS_INFO_STREAM("Creating G5 with modules [" << scitos_modules << "]");
	std::vector<std::string> modules;
	boost::split(modules, scitos_modules, boost::is_any_of("\t "));
	ScitosG5 s(modules);
	
	ROS_INFO("Going into main loop.");
	s.spin();
	return 0;
}
