#include <rito/lcu_process_handler.h>

#include <rito/exception.h>

#include <gtest/gtest.h>

#include <filesystem>
#include <string>

using namespace std::string_literals;

namespace {
const auto base_dir{"test_dir_helper/lcu_process_handler/"s};

}

TEST(LcuProcessHandlerLinux, ShouldThrowIfProcDirectoryDoesNotExist)
{
    const auto path_dir_does_not_exist{base_dir + "dir_which_does_not_exist"};

    rito::Lcu_process_handler process_handler{path_dir_does_not_exist};

    EXPECT_THROW(process_handler.get_lcu_process_parameters(), rito::Path_not_found_exception);
}

TEST(LcuProcessHandlerLinux, ShouldThrowIfProcDirectoryIsEmpty)
{
    const auto path_empty_dir{base_dir + "empty_dir"};

    rito::Lcu_process_handler process_handler{path_empty_dir};

    EXPECT_THROW(process_handler.get_lcu_process_parameters(), rito::Lcu_not_running_exception);
}

TEST(LcuProcessHandlerLinux, ShouldIgnoreNonNumericProcess)
{
    const auto path_non_numeric_process{base_dir + "non_numeric_process"};

    rito::Lcu_process_handler process_handler{path_non_numeric_process};

    EXPECT_THROW(process_handler.get_lcu_process_parameters(), rito::Lcu_not_running_exception);
}

TEST(LcuProcessHandlerLinux, ShouldThrowIfLcuProcessNotFound)
{
    const auto path_lcu_not_running{base_dir + "lcu_not_running"};

    rito::Lcu_process_handler process_handler{path_lcu_not_running};

    EXPECT_THROW(process_handler.get_lcu_process_parameters(), rito::Lcu_not_running_exception);
}

TEST(LcuProcessHandlerLinux, ShouldThrowIfFailedToExtractAuthToken)
{
    const auto path_lcu_running_wrong_auth_token{base_dir + "lcu_running_wrong_auth_token"};

    rito::Lcu_process_handler process_handler{path_lcu_running_wrong_auth_token};

    EXPECT_THROW(process_handler.get_lcu_process_parameters(), rito::Lcu_parameters_exception);
}

TEST(LcuProcessHandlerLinux, ShouldThrowIfAuthTokenIsMissing)
{
    const auto path_lcu_running_auth_token_missing{base_dir + "lcu_running_auth_token_missing"};

    rito::Lcu_process_handler process_handler{path_lcu_running_auth_token_missing};

    EXPECT_THROW(process_handler.get_lcu_process_parameters(), rito::Lcu_parameters_exception);
}

TEST(LcuProcessHandlerLinux, ShouldThrowIfFailedToExtractAppPort)
{
    const auto path_lcu_running_wrong_app_port{base_dir + "lcu_running_wrong_app_port"};

    rito::Lcu_process_handler process_handler{path_lcu_running_wrong_app_port};

    EXPECT_THROW(process_handler.get_lcu_process_parameters(), rito::Lcu_parameters_exception);
}

TEST(LcuProcessHandlerLinux, ShouldThrowIfAppPortIsMissing)
{
    const auto path_lcu_running_app_port_missing{base_dir + "lcu_running_app_port_missing"};

    rito::Lcu_process_handler process_handler{path_lcu_running_app_port_missing};

    EXPECT_THROW(process_handler.get_lcu_process_parameters(), rito::Lcu_parameters_exception);
}

TEST(LcuProcessHandlerLinux, ShouldReturnValuesIfLcuIsRunning)
{
    const auto path_lcu_running_with_id_124{base_dir + "lcu_running_with_id_124"};
    constexpr auto proc_id_124{124};
    constexpr auto proc_124_app_port{12345};
    const auto proc_124_remoting_auth_token{"MyExampleAuthToken"};

    rito::Lcu_process_handler process_handler{path_lcu_running_with_id_124};

    auto params{process_handler.get_lcu_process_parameters()};
    EXPECT_EQ(params.app_port, proc_124_app_port);
    EXPECT_EQ(params.remoting_auth_token, proc_124_remoting_auth_token);
}
