#include <boost/python.hpp>
#include <cstdio>

char const* greet() {
	return "hello, world.\n";
}

BOOST_PYTHON_MODULE(api) {
	using namespace boost::python;
	def("greet", greet);
} 
