#include "bfunc.h"
#include <string>
#include <vector>

namespace bfunc {

void split_str(const string& str, const string& ch_set, vector<string>& result) {
	boost::algorithm::split(result, str, boost::algorithm::is_any_of(ch_set));
}

}


