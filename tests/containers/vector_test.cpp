#include <nostd/vector.h>

#include <gtest/gtest.h>

class VectorTest : public testing::Test {
public:
	typedef nostd::vector<int> Vector;

	using size_type = Vector::size_type;
protected:
	void SetUp() override
	{
		array = new Vector();
	}
	void TearDown() override
	{
		delete array;
	}
	Vector * array;
};

TEST_F(VectorTest, Creation)
{
	EXPECT_EQ(array->empty(), true);
}

TEST_F(VectorTest, PushBack)
{
	int value1 = 1;
	array->push_back(value1);
	EXPECT_EQ(array->size(), 1U);
	EXPECT_EQ(array->front(), value1);
	EXPECT_EQ(array->back(), value1);

	int value2 = 2;
	array->push_back(value2);
	EXPECT_EQ(array->size(), 2U);
	EXPECT_EQ(array->front(), value1);
	EXPECT_EQ(array->back(), value2);
}

TEST_F(VectorTest, PopBack)
{
	int value1 = 1;
	int value2 = 2;
	array->push_back(value1);
	array->push_back(value2);

	array->pop_back();
	EXPECT_EQ(array->size(), 1U);
	EXPECT_EQ(array->front(), value1);
	EXPECT_EQ(array->back(), value1);

	array->pop_back();
	EXPECT_EQ(array->size(), 0U);
}

TEST_F(VectorTest, Reserve)
{
	size_type capacity = 4U;
	array->reserve(capacity);
	EXPECT_EQ(array->size(), 0U);
	EXPECT_EQ(array->capacity(), capacity);
}

TEST_F(VectorTest, Resize)
{
	size_type size = 4U;
	array->resize(size);
	EXPECT_EQ(array->size(), size);
	EXPECT_GE(array->capacity(), size);
}