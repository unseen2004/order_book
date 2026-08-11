#include <gtest/gtest.h>
#include <string>
#include "fix_encoder.h"

TEST(FixEncoderTest, FastItoaWritesDigitsAndReturnsEndPointer) {
    char buf[16]{};
    char* end = fast_itoa(buf, 123456u);
    ASSERT_EQ(end - buf, 6);
    EXPECT_EQ(std::string(buf, static_cast<std::size_t>(end - buf)), "123456");
}

TEST(FixEncoderTest, EncodesBuyNewOrderWithSohSeparators) {
    FixEncoder encoder;
    std::size_t len = encoder.encode_new_order(42u, 15025u, 100u, true);

    const std::string actual(encoder.data(), len);
    const std::string expected =
        "8=FIX.4.2\x01"
        "35=D\x01"
        "11=42\x01"
        "54=1\x01"
        "38=100\x01"
        "44=15025\x01";

    EXPECT_EQ(actual, expected);
}

TEST(FixEncoderTest, EncodesSellSideCorrectly) {
    FixEncoder encoder;
    std::size_t len = encoder.encode_new_order(99u, 2500u, 7u, false);

    const std::string actual(encoder.data(), len);
    EXPECT_NE(actual.find("54=2\x01"), std::string::npos);
    EXPECT_NE(actual.find("38=7\x01"), std::string::npos);
    EXPECT_NE(actual.find("44=2500\x01"), std::string::npos);
}

int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
