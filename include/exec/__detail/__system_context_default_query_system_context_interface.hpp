/*
 * Copyright (c) 2024 Lucian Radu Teodorescu
 *
 * Licensed under the Apache License Version 2.0 with LLVM Exceptions
 * (the "License"); you may not use this file except in compliance with
 * the License. You may obtain a copy of the License at
 *
 *   https://llvm.org/LICENSE.txt
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
#pragma once

#include "__system_context_backend.hpp"
#if STDEXEC_ENABLE_LIBDISPATCH
#  include "__system_context_libdispatch_impl.hpp"
#else
#  include "__system_context_default_impl.hpp"
#endif

namespace exec::__system_context_defaut_query_system_context_interface {

  using system_context_replaceability::system_scheduler;
  using system_context_replaceability::__system_context_replaceability;

  /// Keeps track of the object implementing the system context interfaces.
  /// TODO: apply Eric's fix.
  struct __instance_holder {

    /// Get the only instance of this class.
    static __instance_holder& __singleton() {
      static __instance_holder __this_instance_;
      return __this_instance_;
    }

    /// Get the currently selected system context object.
    system_scheduler* __get_current_instance() const noexcept {
      return __current_instance_;
    }

    /// Allows changing the currently selected system context object; used for testing.
    void __set_current_instance(system_scheduler* __instance) noexcept {
      __current_instance_ = __instance;
    }

   private:
    __instance_holder() {
#if STDEXEC_ENABLE_LIBDISPATCH
      static __system_context_libdispatch_impl::__system_scheduler_impl __default_instance_;
#else
      static __system_context_default_impl::__system_scheduler_impl __default_instance_;
#endif
      __current_instance_ = &__default_instance_;
    }

    system_scheduler* __current_instance_;
  };

  struct __system_context_replaceability_impl : __system_context_replaceability {
    //! Globally replaces the system scheduler backend.
    //! This needs to be called within `main()` and before the system scheduler is accessed.
    void __set_system_scheduler(system_scheduler* __backend) noexcept override {
      __instance_holder::__singleton().__set_current_instance(__backend);
    }
  };

  void* __default_query_system_context_interface(std::type_index __id) noexcept {
    if (__id == typeid(system_scheduler)) {
      return __instance_holder::__singleton().__get_current_instance();
    } else if (__id == typeid(__system_context_replaceability)) {
      static __system_context_replaceability_impl __impl;
      return &__impl;
    }

    return nullptr;
  }

} // namespace exec::__system_context_defaut_query_system_context_interface
