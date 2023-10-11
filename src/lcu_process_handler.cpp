#include "lcu_process_handler.h"

#include "lcu_process_handler_impl.h"

namespace rito {

Lcu_process_handler::Lcu_process_handler() : m_pimpl{std::make_unique<Lcu_process_handler_impl>()}
{
}

Lcu_process_handler::~Lcu_process_handler() {}

Lcu_parameters Lcu_process_handler::get_lcu_process_parameters()
{
    return m_pimpl->get_lcu_process_parameters();
}

} // namespace rito
