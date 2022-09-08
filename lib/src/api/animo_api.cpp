#include "api/animo_api.h"
#include <iostream>

namespace Animo {

namespace Common {
void CloseAnimo(bool no_questions) { std::cout << "Closing Animo..."; }
void RestartAnimo() { std::cout << "Restarting Animo..."; }
} // namespace Common
} // namespace Animo
