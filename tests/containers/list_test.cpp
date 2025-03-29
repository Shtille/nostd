#include <nostd/list.h>
#include <nostd/pool_allocator.h>

#include <gtest/gtest.h>

class ListWithDefaultAllocatorTest : public testing::Test {
	typedef nostd::list<int> List;
protected:
	void SetUp() override
	{
		list = new List();
	}
	void TearDown() override
	{
		delete list;
	}
	List * list;
};

TEST_F(ListWithDefaultAllocatorTest, Creation)
{
	EXPECT_EQ(list->empty(), true);
}

TEST_F(ListWithDefaultAllocatorTest, PushFront)
{
	int value1 = 1;
	list->push_front(value1);
	EXPECT_EQ(list->size(), 1U);
	EXPECT_EQ(list->front(), value1);
	EXPECT_EQ(list->back(), value1);

	int value2 = 2;
	list->push_front(value2);
	EXPECT_EQ(list->size(), 2U);
	EXPECT_EQ(list->front(), value2);
	EXPECT_EQ(list->back(), value1);
}

TEST_F(ListWithDefaultAllocatorTest, PushBack)
{
	int value1 = 1;
	list->push_back(value1);
	EXPECT_EQ(list->size(), 1U);
	EXPECT_EQ(list->front(), value1);
	EXPECT_EQ(list->back(), value1);

	int value2 = 2;
	list->push_back(value2);
	EXPECT_EQ(list->size(), 2U);
	EXPECT_EQ(list->front(), value1);
	EXPECT_EQ(list->back(), value2);
}

TEST_F(ListWithDefaultAllocatorTest, PopFront)
{
	int value1 = 1;
	int value2 = 2;
	list->push_back(value1);
	list->push_back(value2);

	list->pop_front();
	EXPECT_EQ(list->size(), 1U);
	EXPECT_EQ(list->front(), value2);
	EXPECT_EQ(list->back(), value2);

	list->pop_front();
	EXPECT_EQ(list->size(), 0U);
}

TEST_F(ListWithDefaultAllocatorTest, PopBack)
{
	int value1 = 1;
	int value2 = 2;
	list->push_back(value1);
	list->push_back(value2);

	list->pop_back();
	EXPECT_EQ(list->size(), 1U);
	EXPECT_EQ(list->front(), value1);
	EXPECT_EQ(list->back(), value1);

	list->pop_back();
	EXPECT_EQ(list->size(), 0U);
}

TEST_F(ListWithDefaultAllocatorTest, Insert)
{
	int value1 = 1;
	int value2 = 2;
	int value3 = 3;
	list->insert(list->end(), value1);
	EXPECT_EQ(list->size(), 1U);
	EXPECT_EQ(list->front(), value1);

	list->insert(list->begin(), value2);
	EXPECT_EQ(list->size(), 2U);
	EXPECT_EQ(list->front(), value2);

	list->insert(list->end(), value3);
	EXPECT_EQ(list->size(), 3U);
	EXPECT_EQ(list->back(), value3);
}

TEST_F(ListWithDefaultAllocatorTest, Erase)
{
	int value1 = 1;
	int value2 = 2;
	int value3 = 3;
	list->push_back(value1);
	list->push_back(value2);
	list->push_back(value3);

	auto it = list->erase(list->begin());
	EXPECT_EQ(list->size(), 2U);
	EXPECT_EQ(list->front(), value2);
	EXPECT_EQ(*it, value2);

	it = list->erase(list->begin());
	EXPECT_EQ(list->size(), 1U);
	EXPECT_EQ(list->back(), value3);
	EXPECT_EQ(*it, value3);
}

TEST_F(ListWithDefaultAllocatorTest, Remove)
{
	int value1 = 1;
	int value2 = 2;
	int value3 = 3;
	list->push_back(value1);
	list->push_back(value2);
	list->push_back(value3);

	list->remove(value2);
	EXPECT_EQ(list->size(), 2U);
	EXPECT_EQ(list->front(), value1);
	EXPECT_EQ(list->back(), value3);
}

TEST_F(ListWithDefaultAllocatorTest, Find)
{
	int value1 = 1;
	int value2 = 2;
	int value3 = 3;
	list->push_back(value1);
	list->push_back(value2);
	list->push_back(value3);

	auto it = list->find(value2);
	EXPECT_EQ(list->size(), 3U);
	EXPECT_EQ(*it, value2);
}


/**
 * The same test, but with pool allocator
 */
class ListWithPoolAllocatorTest : public testing::Test {
	typedef nostd::list<int> List;
	typedef nostd::pool_allocator Allocator;
protected:
	void SetUp() override
	{
		allocator = new Allocator(4);
		list = new List(allocator);
	}
	void TearDown() override
	{
		delete list;
		delete allocator;
	}
	Allocator * allocator;
	List * list;
};

TEST_F(ListWithPoolAllocatorTest, Creation)
{
	EXPECT_EQ(list->empty(), true);
}

TEST_F(ListWithPoolAllocatorTest, PushFront)
{
	int value1 = 1;
	list->push_front(value1);
	EXPECT_EQ(list->size(), 1U);
	EXPECT_EQ(list->front(), value1);
	EXPECT_EQ(list->back(), value1);

	int value2 = 2;
	list->push_front(value2);
	EXPECT_EQ(list->size(), 2U);
	EXPECT_EQ(list->front(), value2);
	EXPECT_EQ(list->back(), value1);
}

TEST_F(ListWithPoolAllocatorTest, PushBack)
{
	int value1 = 1;
	list->push_back(value1);
	EXPECT_EQ(list->size(), 1U);
	EXPECT_EQ(list->front(), value1);
	EXPECT_EQ(list->back(), value1);

	int value2 = 2;
	list->push_back(value2);
	EXPECT_EQ(list->size(), 2U);
	EXPECT_EQ(list->front(), value1);
	EXPECT_EQ(list->back(), value2);
}

TEST_F(ListWithPoolAllocatorTest, PopFront)
{
	int value1 = 1;
	int value2 = 2;
	list->push_back(value1);
	list->push_back(value2);

	list->pop_front();
	EXPECT_EQ(list->size(), 1U);
	EXPECT_EQ(list->front(), value2);
	EXPECT_EQ(list->back(), value2);

	list->pop_front();
	EXPECT_EQ(list->size(), 0U);
}

TEST_F(ListWithPoolAllocatorTest, PopBack)
{
	int value1 = 1;
	int value2 = 2;
	list->push_back(value1);
	list->push_back(value2);

	list->pop_back();
	EXPECT_EQ(list->size(), 1U);
	EXPECT_EQ(list->front(), value1);
	EXPECT_EQ(list->back(), value1);

	list->pop_back();
	EXPECT_EQ(list->size(), 0U);
}