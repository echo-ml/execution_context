#pragma once

namespace echo { namespace execution_context {

template<class... Backends>
class ExecutionContext : public Backends... {
 public:
  ExecutionContext() {}
  ExecutionContext(const Backends&... backends)
    : Backends(backends)...
  {}
};

}}
