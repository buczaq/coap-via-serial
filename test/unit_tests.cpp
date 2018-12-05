#include <string.h>
#include "../server/functions.c"
#include "../client/functions.c"
#include "gtest/gtest.h"

using namespace ::testing;

namespace constant
{
    unsigned char* dummy_buffer = (unsigned char*)"abcd123qwerty\0\0\0\0\0\0\0\0";
    const int buffer_size_valid = 13;
    const int buffer_size_invalid = 12;

    char* http_message = "GET 0.0.0.0/temperature ";
    unsigned char* byte_message = (unsigned char*)"@\x1{\f70.0.0.0\x8Btemperature";
    unsigned char* byte_message_with_header = (unsigned char*)"\xA1\x18\1\1\1\1@\x1{\f70.0.0.0\x8Btemperature";

    unsigned int dummy_length = 0;
} // namespace constant

TEST(ut_server, count_buffer_size_ok)
{
    ASSERT_EQ(count_actual_buffer_size(constant::dummy_buffer), constant::buffer_size_valid);
}

TEST(ut_server, count_buffer_size_nok)
{
    ASSERT_NE(count_actual_buffer_size(constant::dummy_buffer), constant::buffer_size_invalid);
}

TEST(ut_server, convert_message_to_bytes)
{
    ASSERT_STREQ((char*)http_to_coap(constant::http_message), (char*)constant::byte_message);
}

TEST(ut_client, unpack_coap_message)
{
    ASSERT_STREQ((char*)data_to_coap(constant::byte_message_with_header, &constant::dummy_length), (char*)constant::byte_message);
}

TEST(ut_client, get_payload_ok)
{
    
}

TEST(ut_server, get_payload_nok)
{
    
}

int main(int argc, char **argv) {
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
