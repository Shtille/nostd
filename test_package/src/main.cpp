#include <nostd/vector.h>

#include <cstdio>

int main(int argc, char const *argv[])
{
	nostd::vector<int> v;
	v.push_back(1);
	v.push_back(2);
	std::printf("vector size = %u\n", v.size());
	return 0;
}