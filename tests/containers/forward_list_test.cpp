#include <nostd/forward_list.h>

#include <gtest/gtest.h>

class ForwardListTest : public testing::Test {
	typedef nostd::forward_list<int> List;
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

TEST_F(ForwardListTest, Creation)
{
	EXPECT_EQ(list->empty(), true);
}

TEST_F(ForwardListTest, PushFront)
{
	int value1 = 1;
	list->push_front(value1);
	EXPECT_EQ(list->size(), 1U);
	EXPECT_EQ(list->front(), value1);

	int value2 = 2;
	list->push_front(value2);
	EXPECT_EQ(list->size(), 2U);
	EXPECT_EQ(list->front(), value2);
}

TEST_F(ForwardListTest, PopFront)
{
	int value1 = 1;
	int value2 = 2;
	list->push_front(value1);
	list->push_front(value2);

	list->pop_front();
	EXPECT_EQ(list->size(), 1U);
	EXPECT_EQ(list->front(), value1);

	list->pop_front();
	EXPECT_EQ(list->size(), 0U);
}