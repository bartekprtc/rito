#include <rito/exception.h>
#include <rito/lcu_process_handler.h>

#include <gtest/gtest.h>

#include <filesystem>
#include <string>

using namespace std::string_literals;

namespace {
const auto base_dir{"test_dir_helper/lcu_process_handler/"s};
const auto path_dir_does_not_exist{base_dir + "dir_which_does_not_exist"};
const auto path_empty_dir{base_dir + "empty_dir"};
const auto path_non_numeric_process{base_dir + "non_numeric_process"};
const auto path_lcu_not_running{base_dir + "lcu_not_running"};
const auto path_lcu_running_wrong_params{base_dir + "lcu_running_wrong_params"};

const auto path_lcu_running_with_id_124{base_dir + "lcu_running_with_id_124"};
constexpr auto proc_id_124{124};
constexpr auto proc_124_app_port{12345};
const auto proc_124_remoting_auth_token{"MyExampleAuthToken"};

}

TEST(LcuProcessHandlerLinux, ShouldThrowIfProcDirectoryDoesNotExist)
{
    rito::Lcu_process_handler process_handler{path_dir_does_not_exist};

    EXPECT_THROW(process_handler.get_lcu_process_parameters(), rito::Path_not_found_exception);
}

TEST(LcuProcessHandlerLinux, ShouldThrowIfProcDirectoryIsEmpty)
{
    rito::Lcu_process_handler process_handler{path_empty_dir};

    EXPECT_THROW(process_handler.get_lcu_process_parameters(), rito::Lcu_not_running_exception);
}

TEST(LcuProcessHandlerLinux, ShouldIgnoreNonNumericProcess)
{
    rito::Lcu_process_handler process_handler{path_non_numeric_process};

    EXPECT_THROW(process_handler.get_lcu_process_parameters(), rito::Lcu_not_running_exception);
}

TEST(LcuProcessHandlerLinux, ShouldThrowIfLcuProcessNotFound)
{
    rito::Lcu_process_handler process_handler{path_lcu_not_running};

    EXPECT_THROW(process_handler.get_lcu_process_parameters(), rito::Lcu_not_running_exception);
}

TEST(LcuProcessHandlerLinux, ShouldThrowIfFailedToExtractProcessData)
{
    rito::Lcu_process_handler process_handler{path_lcu_running_wrong_params};

    EXPECT_THROW(process_handler.get_lcu_process_parameters(), rito::Lcu_parameters_exception);
}

TEST(LcuProcessHandlerLinux, ShouldReturnValuesIfLcuIsRunning)
{
    rito::Lcu_process_handler process_handler{path_lcu_running_with_id_124};

    auto params{process_handler.get_lcu_process_parameters()};
    EXPECT_EQ(params.app_port, proc_124_app_port);
    EXPECT_EQ(params.remoting_auth_token, proc_124_remoting_auth_token);
}
