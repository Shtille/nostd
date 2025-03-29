#include <nostd/stack.h>

#include <gtest/gtest.h>

class StackTest : public testing::Test {
	typedef nostd::stack<int> Stack;
protected:
	void SetUp() override
	{
		stack = new Stack();
	}
	void TearDown() override
	{
		delete stack;
	}
	Stack * stack;
};

TEST_F(StackTest, Creation)
{
	EXPECT_EQ(stack->empty(), true);
}

TEST_F(StackTest, Push)
{
	int value1 = 1;
	stack->push(value1);
	EXPECT_EQ(stack->size(), 1U);
	EXPECT_EQ(stack->top(), value1);

	int value2 = 2;
	stack->push(value2);
	EXPECT_EQ(stack->size(), 2U);
	EXPECT_EQ(stack->top(), value2);
}

TEST_F(StackTest, Pop)
{
	int value1 = 1;
	int value2 = 2;
	stack->push(value1);
	stack->push(value2);

	stack->pop();
	EXPECT_EQ(stack->size(), 1U);
	EXPECT_EQ(stack->top(), value1);

	stack->pop();
	EXPECT_EQ(stack->size(), 0U);
}

TEST_F(StackTest, Clear)
{
	int value1 = 1;
	int value2 = 2;
	stack->push(value1);
	stack->push(value2);

	stack->clear();
	EXPECT_EQ(stack->size(), 0U);
}