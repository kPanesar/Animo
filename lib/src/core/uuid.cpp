#include "uuid.h"
#include "ampch.h"

#include <random>
#include <unordered_map>

namespace Animo {

static std::random_device s_RandomDevice;
static std::mt19937_64 s_Engine(s_RandomDevice());
static std::uniform_int_distribution<uint64_t> s_UniformDistribution;

UUID::UUID() : m_UUID(s_UniformDistribution(s_Engine)) {}

UUID::UUID(u64 uuid) : m_UUID(uuid) {}

} // namespace Animo
