#include <nostd/set.h>
#include <nostd/test_allocator.h>

#include <gtest/gtest.h>

class SetTest : public testing::Test {
public:
	typedef nostd::set<int> Set;
	typedef nostd::test_allocator Allocator;

	using size_type = Allocator::size_type;
protected:
	void SetUp() override
	{
		allocator = new Allocator();
		set = new Set(allocator);
		initial_allocated = allocator->count();
	}
	void TearDown() override
	{
		delete set;
		delete allocator;
	}
	Allocator * allocator;
	Set * set;
	size_type initial_allocated;
};

TEST_F(SetTest, Creation)
{
	EXPECT_EQ(set->empty(), true);
	EXPECT_EQ(set->begin(), set->end());
}

TEST_F(SetTest, Insert)
{
	size_type i;
	int values[] = {1,2,3};
	const size_type count = sizeof(values)/sizeof(values[0]);
	// At first fill the set
	for (i = 0; i < count; ++i)
	{
		int value = values[i];
		auto pair = set->insert(value);
		EXPECT_EQ(*pair.first, value);
		EXPECT_EQ(pair.second, true);
		EXPECT_EQ(set->size(), i+1);
		EXPECT_EQ(allocator->count(), initial_allocated + i + 1);
	}
	// Then check the values
	i = 0;
	for (auto it = set->begin(); it != set->end(); ++it)
	{
		int value = values[i];
		EXPECT_EQ(*it, value);
		++i;
	}
	// Try to insert existing value
	{
		int existing_value = 1;
		auto pair = set->insert(existing_value);
		EXPECT_EQ(*pair.first, existing_value);
		EXPECT_EQ(pair.second, false);
		EXPECT_EQ(set->size(), count);
		EXPECT_EQ(allocator->count(), initial_allocated + count);
	}
}

TEST_F(SetTest, Find)
{
	size_type i;
	int values[] = {1,2,3};
	const size_type count = sizeof(values)/sizeof(values[0]);
	// At first fill the set
	for (i = 0; i < count; ++i)
	{
		int value = values[i];
		(void)set->insert(value);
	}
	// Then try to find an existing value
	{
		int existing_value = 2;
		auto it = set->find(existing_value);
		EXPECT_NE(it, set->end());
		EXPECT_EQ(*it, existing_value);
	}
	// Then try to find a non existing value
	{
		int non_existing_value = 5;
		auto it = set->find(non_existing_value);
		EXPECT_EQ(it, set->end());
	}
}

TEST_F(SetTest, Erase)
{
	size_type i;
	int values[] = {1,2,3};
	const size_type count = sizeof(values)/sizeof(values[0]);
	// At first fill the set
	for (i = 0; i < count; ++i)
	{
		int value = values[i];
		(void)set->trusted_insert(value);
	}
	// Then erase an existing value
	{
		int existing_value = 2;
		auto it = set->find(existing_value);
		set->erase(it);
		EXPECT_EQ(set->size(), count - 1);
		EXPECT_EQ(allocator->count(), initial_allocated + count - 1);
	}
	// Then check the rest values
	{
		auto it = set->begin();
		EXPECT_EQ(*it, values[0]);
		++it;
		EXPECT_EQ(*it, values[2]);
	}
}

TEST_F(SetTest, Clear)
{
	size_type i;
	int values[] = {1,2,3};
	const size_type count = sizeof(values)/sizeof(values[0]);
	// At first fill the set
	for (i = 0; i < count; ++i)
	{
		int value = values[i];
		(void)set->trusted_insert(value);
	}
	// Then clear the set
	{
		set->clear();
		EXPECT_EQ(set->empty(), true);
		EXPECT_EQ(set->begin(), set->end());
		EXPECT_EQ(allocator->count(), initial_allocated);
	}
}