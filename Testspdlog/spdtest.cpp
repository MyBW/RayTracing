
#include "spdlog/spdlog.h"
#include "spdlog/sinks/stdout_color_sinks.h"
#include "spdlog/sinks/basic_file_sink.h"
void stdout_example()
{
	// create color multi threaded logger
	auto console = spdlog::stdout_color_mt("console");
	auto err_logger = spdlog::stderr_color_mt("stderr");
	spdlog::get("console")->info("loggers can be retrieved from a global registry using the spdlog::get(logger_name)");
}

#include "spdlog/stopwatch.h"
void stopwatch_example()
{
	spdlog::stopwatch sw;
	spdlog::debug("Elapsed {}", sw);
	Sleep(1000);
	spdlog::debug("Elapsed {:.3}", sw);
}
#define BW_LOG(...) spdlog::info(__VA_ARGS__);

//#define BW_LOG(format, ...) printf(format, ##__VA_ARGS__)

int main(int argc, char **argv)
{

	spdlog::info("Welcome to spdlog!");
	spdlog::error("Some error message with arg: {}", 1);

	spdlog::warn("Easy padding in numbers like {:08d}", 12);
	spdlog::critical("Support for int: {0:d};  hex: {0:x};  oct: {0:o}; bin: {0:b}", 42);
	spdlog::info("Support for floats {:03.2f}", 1.23456);
	spdlog::info("Positional args are {1} {0}..", "too", "supported");
	spdlog::error("Positional args are {1} {0}..", "too", "supported");
	spdlog::info("{:<30}", "left aligned");

	BW_LOG("fuck {0}", "you!!");

	spdlog::set_level(spdlog::level::debug); // Set global log level to debug
	spdlog::debug("This message should be displayed..");

	// change log pattern
	spdlog::set_pattern("[%H:%M:%S %z] [%n] [%^---%L---%$] [thread %t] %v");

	// Compile time log levels
	// define SPDLOG_ACTIVE_LEVEL to desired level
	SPDLOG_TRACE("Some trace message with param {}", 42);
	SPDLOG_DEBUG("Some debug message");

	auto logger = spdlog::basic_logger_mt("basic_logger", "logs/basic-log.txt");
	logger->error("Fuck!!!!!!!!!");


	spdlog::enable_backtrace(32); // Store the latest 32 messages in a buffer. Older messages will be dropped.
								  // or my_logger->enable_backtrace(32)..
	for (int i = 0; i < 100; i++)
	{
		spdlog::debug("Backtrace message {}", i); // not logged yet..
	}
	// e.g. if some error happened:
	spdlog::dump_backtrace(); // log them now! show the last 32 messages


	stopwatch_example();
	return 0;
}