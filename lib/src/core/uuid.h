#pragma once
#include "animo.h"

namespace Animo {

class UUID {
  public:
    UUID();
    UUID(u64 uuid);
    UUID(const UUID&) = default;

    operator u64() const { return m_UUID; }

  private:
    uint64_t m_UUID;
};

} // namespace Animo

namespace std {
template <typename T> struct hash;

template <> struct hash<Animo::UUID> {
    size_t operator()(const Animo::UUID& uuid) const { return (u64)uuid; }
};

} // namespace std
