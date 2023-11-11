/**
 * @file lcu_process_handler
 * @brief Contains definition of Lcu_process_handler class.
 */

#pragma once

#include "rito_export.h"

#include <cstdint>
#include <filesystem>
#include <memory>
#include <string>

namespace rito {

/**
 * @brief Struct holding pair of parameters extracted from LCU process.
 */
struct RITO_EXPORT Lcu_parameters
{
    /**
     * @brief Holds remoting-auth-token parameter. Value consists of base64 characters.
     */

    std::string remoting_auth_token;
    /**
     * @brief Holds app-port parameter.
     */
    std::uint16_t app_port;
};

class Lcu_process_handler_impl;

/**
 * @brief Handler for extracting LCU parameters required for connection.
 */
class RITO_EXPORT Lcu_process_handler
{
public:
    /**
     * @param proc_dir - path to directory containing processes. Ignored on Windows.
     */
    Lcu_process_handler(std::filesystem::path proc_dir = "/proc");
    ~Lcu_process_handler();

    Lcu_process_handler(const Lcu_process_handler&) = delete;
    Lcu_process_handler(Lcu_process_handler&&) = delete;
    auto operator=(const Lcu_process_handler&) -> Lcu_process_handler& = delete;
    auto operator=(Lcu_process_handler&&) -> Lcu_process_handler& = delete;

    /**
     * @brief Obtains connection parameters of running LCU process.
     *
     * @throws Lcu_not_running_exception
     * @throws Lcu_parameters_exception
     * @throws Path_not_found_exception (Linux only)
     *
     * @return Set of parameters required for connection to LCU.
     *
     * If LCU process is found, but parameters are not there (for any reason),
     *   Lcu_parameters_exception is thrown. If LCU process is not found,
     *   Lcu_not_running_exception is thrown.
     *
     * Windows specific: this method uses WinApi calls on dynamically loaded DLL. If DLL fails to
     *   load or anything goes wrong with WinApi calls, Lcu_parameters_exception is thrown.
     *
     * Linux specific: this method scans through processes (/proc directory by default) to find LCU.
     *   If process directory could not be found (e.g., Lcu_process_handler was created with
     *   incorrect parameter), Path_not_found_exception is thrown.
     */
    auto get_lcu_process_parameters() -> Lcu_parameters;

private:
    std::unique_ptr<Lcu_process_handler_impl> m_pimpl;
};

} // namespace rito
