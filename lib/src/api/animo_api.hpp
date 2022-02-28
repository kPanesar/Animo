#pragma once

#include <animo.hpp>

#include <list>
#include <optional>
#include <string>
#include <vector>

using ImGuiID = unsigned int;
struct ImVec2;

namespace Animo
{
    namespace Common
    {
        void closeAnimo(bool noQuestions = false);
        void restartAnimo();
    }
}
