// ======================================================================
// \title fprime-freertos/FreeRTOS/Os/ConditionVariable.cpp
// \brief FreeRTOS implementations for Os::ConditionVariable
// ======================================================================
#include "FreeRTOS/Os/ConditionVariable.hpp"
#include <Fw/Types/Assert.hpp>

namespace Os {
namespace FreeRTOS {

FreeRTOSConditionVariable::FreeRTOSConditionVariable() {
    m_semaphore = xSemaphoreCreateBinary();
    FW_ASSERT(m_semaphore != NULL);
}

FreeRTOSConditionVariable::~FreeRTOSConditionVariable() {
    vSemaphoreDelete(m_semaphore);
}

ConditionVariable::Status FreeRTOSConditionVariable::pend(Os::Mutex& mutex) {
    // Release the mutex before waiting for the condition variable
    if (mutex.release() != Os::MutexInterface::Status::OP_OK) {
        return ConditionVariable::Status::ERROR_OTHER;
    }

    // Block the task and wait for the condition variable to be notified
    const BaseType_t result = xSemaphoreTake(m_semaphore, portMAX_DELAY);
    FW_ASSERT(result == pdTRUE);

    // Re-acquire the mutex after being notified
    if (mutex.take() != Os::MutexInterface::Status::OP_OK) {
        return ConditionVariable::Status::ERROR_OTHER;
    }

    return ConditionVariable::Status::OP_OK;
}

void FreeRTOSConditionVariable::notify() {
    const BaseType_t result = xSemaphoreGive(m_semaphore);
    FW_ASSERT(result == pdTRUE);
}

void FreeRTOSConditionVariable::notifyAll() {
    this->notify();
}

ConditionVariableHandle* FreeRTOSConditionVariable::getHandle() {
    return reinterpret_cast<ConditionVariableHandle*>(m_semaphore);
}

}  // namespace FreeRTOS
}  // namespace Os
