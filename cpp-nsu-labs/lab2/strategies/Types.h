#pragma once

namespace Strategies {
  
  enum MoveType {
	  DEFECT = 'D',
	  COLLUDE = 'C',
  };
  
  using Rivals = std::tuple<std::string, std::string>;
  using MovesVector = std::vector<MoveType>;
}
