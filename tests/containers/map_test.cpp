#include <nostd/map.h>
#include <nostd/test_allocator.h>

#include <gtest/gtest.h>

class MapTest : public testing::Test {
public:
	typedef nostd::map<int, int> Map;
	typedef nostd::test_allocator Allocator;

	using size_type = Allocator::size_type;
	using pair_type = Map::pair_type;

protected:
	
	void SetUp() override
	{
		allocator = new Allocator();
		map = new Map(allocator);
		initial_allocated = allocator->count();
	}
	void TearDown() override
	{
		delete map;
		delete allocator;
	}
	Allocator * allocator;
	Map * map;
	size_type initial_allocated;
};

TEST_F(MapTest, Creation)
{
	EXPECT_EQ(map->empty(), true);
	EXPECT_EQ(map->begin(), map->end());
}

TEST_F(MapTest, Insert)
{
	size_type i;
	int values[] = {1,2,3};
	const size_type count = sizeof(values)/sizeof(values[0]);
	// At first fill the map
	for (i = 0; i < count; ++i)
	{
		int value = values[i];
		pair_type value_pair(value, value);
		auto pair = map->insert(value_pair);
		EXPECT_EQ(*pair.first, value_pair);
		EXPECT_EQ(pair.second, true);
		EXPECT_EQ(map->size(), i+1);
		EXPECT_EQ(allocator->count(), initial_allocated + i + 1);
	}
	// Then check the values
	i = 0;
	for (auto it = map->begin(); it != map->end(); ++it)
	{
		int value = values[i];
		pair_type value_pair(value, value);
		EXPECT_EQ(*it, value_pair);
		++i;
	}
	// Try to insert existing value
	{
		int existing_value = 1;
		pair_type value_pair(existing_value, existing_value);
		auto pair = map->insert(value_pair);
		EXPECT_EQ(*pair.first, value_pair);
		EXPECT_EQ(pair.second, false);
		EXPECT_EQ(map->size(), count);
		EXPECT_EQ(allocator->count(), initial_allocated + count);
	}
}

TEST_F(MapTest, Find)
{
	size_type i;
	int values[] = {1,2,3};
	const size_type count = sizeof(values)/sizeof(values[0]);
	// At first fill the map
	for (i = 0; i < count; ++i)
	{
		int value = values[i];
		pair_type value_pair(value, value);
		(void)map->insert(value_pair);
	}
	// Then try to find an existing value
	{
		int existing_value = 2;
		pair_type existing_value_pair(existing_value, existing_value);
		auto it = map->find(existing_value);
		EXPECT_NE(it, map->end());
		EXPECT_EQ(*it, existing_value_pair);
	}
	// Then try to find a non existing value
	{
		int non_existing_value = 5;
		auto it = map->find(non_existing_value);
		EXPECT_EQ(it, map->end());
	}
}

TEST_F(MapTest, Erase)
{
	size_type i;
	int values[] = {1,2,3};
	const size_type count = sizeof(values)/sizeof(values[0]);
	// At first fill the map
	for (i = 0; i < count; ++i)
	{
		int value = values[i];
		pair_type value_pair(value, value);
		(void)map->trusted_insert(value_pair);
	}
	// Then erase an existing value
	{
		int existing_value = 2;
		auto it = map->find(existing_value);
		map->erase(it);
		EXPECT_EQ(map->size(), count - 1);
		EXPECT_EQ(allocator->count(), initial_allocated + count - 1);
	}
	// Then check the rest values
	{
		auto it = map->begin();
		EXPECT_EQ((*it).first, values[0]);
		++it;
		EXPECT_EQ((*it).first, values[2]);
	}
}

TEST_F(MapTest, Clear)
{
	size_type i;
	int values[] = {1,2,3};
	const size_type count = sizeof(values)/sizeof(values[0]);
	// At first fill the map
	for (i = 0; i < count; ++i)
	{
		int value = values[i];
		pair_type value_pair(value, value);
		(void)map->trusted_insert(value_pair);
	}
	// Then clear the map
	{
		map->clear();
		EXPECT_EQ(map->empty(), true);
		EXPECT_EQ(map->begin(), map->end());
		EXPECT_EQ(allocator->count(), initial_allocated);
	}
}