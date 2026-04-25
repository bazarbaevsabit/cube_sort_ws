#include <gtest/gtest.h>
#include "mobile/color_utils.hpp"

TEST(ColorTest, RedLow) {
    EXPECT_EQ(getColor(5, 200, 200), "RED");
}
TEST(ColorTest, RedHigh) {
    EXPECT_EQ(getColor(175, 200, 200), "RED");
}
TEST(ColorTest, Orange) {
    EXPECT_EQ(getColor(15, 200, 200), "ORANGE");
}
TEST(ColorTest, Yellow) {
    EXPECT_EQ(getColor(30, 200, 200), "YELLOW");
}
TEST(ColorTest, Green) {
    EXPECT_EQ(getColor(60, 200, 200), "GREEN");
}
TEST(ColorTest, Cyan) {
    EXPECT_EQ(getColor(90, 200, 200), "CYAN");
}
TEST(ColorTest, Blue) {
    EXPECT_EQ(getColor(110, 200, 200), "BLUE");
}
TEST(ColorTest, Purple) {
    EXPECT_EQ(getColor(140, 200, 200), "PURPLE");
}
TEST(ColorTest, LowSaturation) {
    EXPECT_EQ(getColor(5, 50, 200), "NO_COLOR");
}
TEST(ColorTest, BorderSaturation) {
    EXPECT_EQ(getColor(5, 80, 200), "RED");
}
TEST(ColorTest, JustBelowSaturation) {
    EXPECT_EQ(getColor(5, 79, 200), "NO_COLOR");
}

int main(int argc, char** argv) {
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
