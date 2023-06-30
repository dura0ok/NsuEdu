#include <iostream>
#include "GameInfo.h"
#include "argparse/argparse.hpp"
#include "strategies/StrategyFactory.h"

std::string strategiesInfoToString(Strategies::StrategyFactory factory) {
	std::string res;
	auto infoMap = factory.getElementsInfo();
	for (auto const &key : infoMap) {
		res += std::string(key.first) + "\n" + std::string("Description: ") + key.second + "\n\n";
	}
	return res;
}

int main(int argc, char *argv[]) {
	argparse::ArgumentParser program("prisonerDilemma");
	program.add_argument("--matrix");
	program.add_argument("--config").default_value(std::string(""));
	auto tempFactory = Strategies::StrategyFactory();
	auto info = strategiesInfoToString(tempFactory);
	
	program.add_argument("--mode")
		.help("Mode: [detailed|fast|tournament]")
		.default_value(std::string("detailed"))
		.action([](const std::string &value) {
		  static const std::vector<std::string> choices = {"detailed", "fast", "tournament"};
		  if (std::find(choices.begin(), choices.end(), value) != choices.end()) {
			  return value;
		  }
		  throw std::invalid_argument("mode invalid argument");
		});
	
	program.add_description("Possible strategies:\n\n" + info);
	
	program.add_argument("--steps")
		.default_value(1)
		.scan<'i', int>();
	
	program.add_argument("--strategies")
		.help("The list of strategies")
		.nargs(argparse::nargs_pattern::at_least_one)
		.required();
	
	try {
		program.parse_args(argc, argv);
		auto mode = program.get<std::string>("--mode");
		auto strategies = program.get<std::vector<std::string>>("--strategies");
		if ((mode == "detailed" || mode == "fast") && strategies.size() != 3) {
			if (strategies.size() < 3) {
				throw std::invalid_argument("in fast or detailed mode number of strategies must be 3");
			}
			mode = "tournament";
		}
		
		auto storagePath = program.get<std::string>("--config");
		auto steps = program.get<int>("--steps");
		
		if (!program.present("--matrix")) {
			auto gameInfo = PrisonerConfig::GameInfo(strategies, mode, storagePath);
			gameInfo.start(steps);
		} else {
			std::ifstream file(program.get<std::string>("--matrix"));
			auto gameInfo = PrisonerConfig::GameInfo(file, strategies, mode, storagePath);
			gameInfo.start(steps);
		}
		
	}
	catch (const std::exception &e) {
		std::cerr << e.what();
		return 1;
	}
	return 0;
}

