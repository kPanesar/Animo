#include "Animo.h"
#include <filesystem>
#include <fstream>
#include <iostream>

namespace Animo {
// [Optional] Notify the system of Tabs/Windows closure that happened outside the regular tab interface.
// If a tab has been closed programmatically (aka closed from another source such as the Checkbox() in the demo,
// as opposed to clicking on the regular tab closing button) and stops being submitted, it will take a frame for
// the tab bar to notice its absence. During this frame there will be a gap in the tab bar, and if the tab that has
// disappeared was the selected one, the tab bar will report no selected tab during the frame. This will effectively
// give the impression of a flicker for one frame.
// We call SetTabItemClosed() to manually notify the Tab Bar or Docking system of removed tabs to avoid this glitch.
// Note that this completely optional, and only affect tab bars with the ImGuiTabBarFlags_Reorderable flag.
static void NotifyOfDocumentsClosedElsewhere(ExampleAppDocuments& app) {
    for (int doc_n = 0; doc_n < app.Documents.Size; doc_n++) {
        MyDocument* doc = &app.Documents[doc_n];
        if (!doc->Open && doc->OpenPrev) ImGui::SetTabItemClosed(doc->Name);
        doc->OpenPrev = doc->Open;
    }
}

void ShowDocumentList() {
    // When (opt_target == Target_DockSpaceAndWindow) there is the possibily that one of our child Document window (e.g. "Eggplant")
    // that we emit gets docked into the same spot as the parent window ("Example: Documents").
    // This would create a problematic feedback loop because selecting the "Eggplant" tab would make the "Example: Documents" tab
    // not visible, which in turn would stop submitting the "Eggplant" window.
    // We avoid this problem by submitting our documents window even if our parent window is not currently visible.
    // Another solution may be to make the "Example: Documents" window use the ImGuiWindowFlags_NoDocking.

    /*bool window_contents_visible = ImGui::Begin("Example: Documents", p_open, ImGuiWindowFlags_MenuBar);
    if (!window_contents_visible && opt_target != Target_DockSpaceAndWindow)
    {
        ImGui::End();
        return;
    }*/

    // Menu
    if (ImGui::BeginMenuBar()) {
        if (ImGui::BeginMenu("File")) {
            int open_count = 0;
            for (int doc_n = 0; doc_n < app.Documents.Size; doc_n++) open_count += app.Documents[doc_n].Open ? 1 : 0;

            if (ImGui::BeginMenu("Open", open_count < app.Documents.Size)) {
                for (int doc_n = 0; doc_n < app.Documents.Size; doc_n++) {
                    MyDocument* doc = &app.Documents[doc_n];
                    if (!doc->Open)
                        if (ImGui::MenuItem(doc->Name)) doc->DoOpen();
                }
                ImGui::EndMenu();
            }
            if (ImGui::MenuItem("Close All Documents", NULL, false, open_count > 0))
                for (int doc_n = 0; doc_n < app.Documents.Size; doc_n++) app.Documents[doc_n].DoQueueClose();
            if (ImGui::MenuItem("Exit", "Alt+F4")) {
            }
            ImGui::EndMenu();
        }
        ImGui::EndMenuBar();
    }

    // [Debug] List documents with one checkbox for each
    for (int doc_n = 0; doc_n < app.Documents.Size; doc_n++) {
        MyDocument* doc = &app.Documents[doc_n];
        if (doc_n > 0) ImGui::SameLine();
        ImGui::PushID(doc);
        if (ImGui::Checkbox(doc->Name, &doc->Open))
            if (!doc->Open) doc->DoForceClose();
        ImGui::PopID();
    }
    ImGui::PushItemWidth(ImGui::GetFontSize() * 12);
    ImGui::Combo("Output", (int*)&opt_target, "None\0TabBar+Tabs\0DockSpace+Window\0");
    ImGui::PopItemWidth();
}

void ShowExampleAppDocuments(bool* p_open) {
    bool redock_all = false;
    if (opt_target == Target_Tab) {
        ImGui::SameLine();
    }
    if (opt_target == Target_DockSpaceAndWindow) {
        ImGui::SameLine();
        redock_all = ImGui::Button("Redock all");
    }
    // About the ImGuiWindowFlags_UnsavedDocument / ImGuiTabItemFlags_UnsavedDocument flags.
    // They have multiple effects:
    // - Display a dot next to the title.
    // - Tab is selected when clicking the X close button.
    // - Closure is not assumed (will wait for user to stop submitting the tab).
    //   Otherwise closure is assumed when pressing the X, so if you keep submitting the tab may reappear at end of tab bar.
    //   We need to assume closure by default otherwise waiting for "lack of submission" on the next frame would leave an empty
    //   hole for one-frame, both in the tab-bar and in tab-contents when closing a tab/window.
    //   The rarely used SetTabItemClosed() function is a way to notify of programmatic closure to avoid the one-frame hole.

    // Tabs
    if (opt_target == Target_Tab) {
        ImGuiTabBarFlags tab_bar_flags = (opt_fitting_flags);
        if (ImGui::BeginTabBar("##tabs", tab_bar_flags)) {
            // [DEBUG] Stress tests
            // if ((ImGui::GetFrameCount() % 30) == 0) docs[1].Open ^= 1;            // [DEBUG] Automatically show/hide a tab. Test various interactions e.g.
            // dragging with this on. if (ImGui::GetIO().KeyCtrl) ImGui::SetTabItemSelected(docs[1].Name);  // [DEBUG] Test SetTabItemSelected(), probably not
            // very useful as-is anyway..

            // Submit Tabs
            for (int doc_n = 0; doc_n < app.Documents.Size; doc_n++) {
                MyDocument* doc = &app.Documents[doc_n];
                if (!doc->Open) continue;

                ImGuiTabItemFlags tab_flags = (doc->Dirty ? ImGuiTabItemFlags_UnsavedDocument : 0);
                bool visible = ImGui::BeginTabItem(doc->Name, &doc->Open, tab_flags);

                // Cancel attempt to close when unsaved add to save queue so we can display a popup.
                if (!doc->Open && doc->Dirty) {
                    doc->Open = true;
                    doc->DoQueueClose();
                }

                MyDocument::DisplayContextMenu(doc);
                if (visible) {
                    MyDocument::DisplayContents(doc);
                    ImGui::EndTabItem();
                }
            }

            ImGui::EndTabBar();
        }
    } else if (opt_target == Target_DockSpaceAndWindow) {
        if (ImGui::GetIO().ConfigFlags & ImGuiConfigFlags_DockingEnable) {
            NotifyOfDocumentsClosedElsewhere(app);

            // Create a DockSpace node where any window can be docked
            ImGuiID dockspace_id = ImGui::GetID("MyDockSpace");
            ImGui::DockSpace(dockspace_id);

            // Create Windows
            for (int doc_n = 0; doc_n < app.Documents.Size; doc_n++) {
                MyDocument* doc = &app.Documents[doc_n];
                if (!doc->Open) continue;

                ImGui::SetNextWindowDockID(dockspace_id, redock_all ? ImGuiCond_Always : ImGuiCond_FirstUseEver);
                ImGuiWindowFlags window_flags = (doc->Dirty ? ImGuiWindowFlags_UnsavedDocument : 0);
                bool visible = ImGui::Begin(doc->Name, &doc->Open, window_flags);

                // Cancel attempt to close when unsaved add to save queue so we can display a popup.
                if (!doc->Open && doc->Dirty) {
                    doc->Open = true;
                    doc->DoQueueClose();
                }

                MyDocument::DisplayContextMenu(doc);
                if (visible) MyDocument::DisplayContents(doc);

                ImGui::End();
            }
        }
    }

    // Early out other contents
    /*if (!window_contents_visible)
    {
        ImGui::End();
        return;
    }*/

    // Update closing queue
    static ImVector<MyDocument*> close_queue;
    if (close_queue.empty()) {
        // Close queue is locked once we started a popup
        for (int doc_n = 0; doc_n < app.Documents.Size; doc_n++) {
            MyDocument* doc = &app.Documents[doc_n];
            if (doc->WantClose) {
                doc->WantClose = false;
                close_queue.push_back(doc);
            }
        }
    }

    // Display closing confirmation UI
    if (!close_queue.empty()) {
        int close_queue_unsaved_documents = 0;
        for (int n = 0; n < close_queue.Size; n++)
            if (close_queue[n]->Dirty) close_queue_unsaved_documents++;

        if (close_queue_unsaved_documents == 0) {
            // Close documents when all are unsaved
            for (int n = 0; n < close_queue.Size; n++) close_queue[n]->DoForceClose();
            close_queue.clear();
        } else {
            if (!ImGui::IsPopupOpen("Save?")) ImGui::OpenPopup("Save?");
            if (ImGui::BeginPopupModal("Save?", NULL, ImGuiWindowFlags_AlwaysAutoResize)) {
                ImGui::Text("Save change to the following items?");
                float item_height = ImGui::GetTextLineHeightWithSpacing();
                if (ImGui::BeginChildFrame(ImGui::GetID("frame"), ImVec2(-FLT_MIN, 6.25f * item_height))) {
                    for (int n = 0; n < close_queue.Size; n++)
                        if (close_queue[n]->Dirty) ImGui::Text("%s", close_queue[n]->Name);
                    ImGui::EndChildFrame();
                }

                ImVec2 button_size(ImGui::GetFontSize() * 7.0f, 0.0f);
                if (ImGui::Button("Yes", button_size)) {
                    for (int n = 0; n < close_queue.Size; n++) {
                        if (close_queue[n]->Dirty) close_queue[n]->DoSave();
                        close_queue[n]->DoForceClose();
                    }
                    close_queue.clear();
                    ImGui::CloseCurrentPopup();
                }
                ImGui::SameLine();
                if (ImGui::Button("No", button_size)) {
                    for (int n = 0; n < close_queue.Size; n++) close_queue[n]->DoForceClose();
                    close_queue.clear();
                    ImGui::CloseCurrentPopup();
                }
                ImGui::SameLine();
                if (ImGui::Button("Cancel", button_size)) {
                    close_queue.clear();
                    ImGui::CloseCurrentPopup();
                }
                ImGui::EndPopup();
            }
        }
    }

    // ImGui::End();
}

static void HelpMarker(const char* desc) {
    ImGui::TextDisabled("(?)");
    if (ImGui::IsItemHovered()) {
        ImGui::BeginTooltip();
        ImGui::PushTextWrapPos(ImGui::GetFontSize() * 35.0f);
        ImGui::TextUnformatted(desc);
        ImGui::PopTextWrapPos();
        ImGui::EndTooltip();
    }
}

void ResetDockSpace(ImGuiID dockspace_id, ImGuiViewport* viewport) {
    static ImGuiDockNodeFlags dockspace_flags = ImGuiDockNodeFlags_PassthruCentralNode;

    static auto first_time = true;
    if (first_time) {
        first_time = false;

        ImGui::DockBuilderRemoveNode(dockspace_id); // clear any previous layout
        ImGui::DockBuilderAddNode(dockspace_id, dockspace_flags | ImGuiDockNodeFlags_DockSpace);
        ImGui::DockBuilderSetNodeSize(dockspace_id, viewport->Size);

        auto dock_id_left = ImGui::DockBuilderSplitNode(dockspace_id, ImGuiDir_Left, 0.25f, nullptr, &dockspace_id);
        auto dock_id_left_down = ImGui::DockBuilderSplitNode(dock_id_left, ImGuiDir_Down, 0.5f, nullptr, &dock_id_left);

        ImGui::DockBuilderDockWindow("Viewport", dockspace_id); // Main
        ImGui::DockBuilderDockWindow("Documents", dock_id_left);
        ImGui::DockBuilderDockWindow("Codes", dock_id_left_down);
        ImGui::DockBuilderFinish(dockspace_id);
    }
}

// Note that shortcuts are currently provided for display only
// (future version will add explicit flags to BeginMenu() to request processing shortcuts)
static void ShowMenuBar() {
    if (ImGui::BeginMenuBar()) {
        if (ImGui::BeginMenu("File")) {
            if (ImGui::MenuItem("New")) {
            }
            if (ImGui::MenuItem("Open", "Ctrl+O")) {
            }
            if (ImGui::BeginMenu("Open Recent")) {
                ImGui::MenuItem("fish_hat.c");
                ImGui::MenuItem("fish_hat.inl");
                ImGui::MenuItem("fish_hat.h");
                if (ImGui::BeginMenu("More..")) {
                    ImGui::MenuItem("Hello");
                    ImGui::MenuItem("Sailor");
                    ImGui::EndMenu();
                }
                ImGui::EndMenu();
            }
            if (ImGui::MenuItem("Save", "Ctrl+S")) {
            }
            if (ImGui::MenuItem("Save As..")) {
            }

            ImGui::Separator();

            // Here we demonstrate appending again to the "Options" menu (which we already created above)
            // Of course in this demo it is a little bit silly that this function calls BeginMenu("Options") twice.
            // In a real code-base using it would make senses to use this feature from very different code locations.
            if (ImGui::BeginMenu("Options")) // <-- Append!
            {
                static bool b = true;
                ImGui::Checkbox("SomeOption", &b);
                ImGui::EndMenu();
            }

            if (ImGui::BeginMenu("Disabled", false)) // Disabled
            {
                IM_ASSERT(0);
            }
            if (ImGui::MenuItem("Checked", NULL, true)) {
            }
            if (ImGui::MenuItem("Quit", "Alt+F4")) {
            }
            ImGui::EndMenu();
        }
        ImGui::EndMenuBar();
    }
}

void RenderUI() {
    static ImGuiDockNodeFlags dockspace_flags = ImGuiDockNodeFlags_PassthruCentralNode;
    ImGuiWindowFlags window_flags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking;

    ImGuiViewport* viewport = ImGui::GetMainViewport();
    ImGui::SetNextWindowPos(viewport->Pos);
    ImGui::SetNextWindowSize(viewport->Size);
    ImGui::SetNextWindowViewport(viewport->ID);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
    window_flags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
    window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;

    // When using ImGuiDockNodeFlags_PassthruCentralNode, DockSpace() will render our background and handle the pass-thru hole, so we ask Begin() to not render
    // a background.
    if (dockspace_flags & ImGuiDockNodeFlags_PassthruCentralNode) window_flags |= ImGuiWindowFlags_NoBackground;

    // Important: note that we proceed even if Begin() returns false (aka window is collapsed).
    // This is because we want to keep our DockSpace() active. If a DockSpace() is inactive,
    // all active windows docked into it will lose their parent and become undocked.
    // We cannot preserve the docking relationship between an active window and an inactive docking, otherwise
    // any change of dockspace/settings would lead to windows being stuck in limbo and never being visible.
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
    ImGui::Begin("DockSpace", nullptr, window_flags);
    ImGui::PopStyleVar();
    ImGui::PopStyleVar(2);

    ShowMenuBar();

    ImGuiID dockspace_id = ImGui::GetID("MyDockSpace");
    ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), dockspace_flags);
    ImGuiWindowSettings* settings = ImGui::FindWindowSettings(ImGui::GetCurrentWindow()->ID);
    if (!settings) {
        ResetDockSpace(dockspace_id, viewport);
    }

    ImGui::Begin("Documents", 0);
    const char* items[] = {"Apple", "Banana", "Cherry", "Kiwi", "Mango", "Orange", "Pineapple", "Strawberry", "Watermelon"};
    ShowDocumentList();
    static int item_current = 1;
    ImGui::ListBox("Document List", &item_current, items, IM_ARRAYSIZE(items), 4);
    ImGui::SameLine();
    HelpMarker("Using the simplified one-liner ListBox API here.\nRefer to the \"List boxes\" section below for an explanation of how to use the more flexible "
               "and general BeginListBox/EndListBox API.");
    ImGui::End();

    ImGui::Begin("Directory list", 0);
    const std::filesystem::path sandbox{"./"};

    if (ImGui::BeginListBox("combo")) {
        // directory_iterator can be iterated using a range-for loop
        for (auto& dir_entry : std::filesystem::directory_iterator{sandbox}) {
            const auto& path = dir_entry.path();
            std::string item_name = dir_entry.path().filename().string();
            ImGui::Selectable(item_name.c_str());
        }
        ImGui::EndListBox();
    }

    // if (files.empty()) files.push_back("Empty directory");
    // if (ImGui::BeginListBox("combo")) {
    //     for (int i = 0; i < files.size(); ++i) {
    //         const bool isSelected = (item_current == i);
    //         if (ImGui::Selectable(files[i].c_str(), isSelected)) {
    //             item_current = i;
    //         }

    //        // Set the initial focus when opening the combo
    //        // (scrolling + keyboard navigation focus)
    //        if (isSelected) {
    //            ImGui::SetItemDefaultFocus();
    //        }
    //    }
    //    ImGui::EndListBox();
    //}
    ImGui::End();

    ImGui::Begin("Codes", 0);
    ImGui::Button("Animo");
    static float value = 0.0f;
    ImGui::DragFloat("Value", &value);
    ImGui::End();

    ImGui::Begin("Viewport", 0);
    ShowExampleAppDocuments(NULL);
    ImGui::End();

    ImGui::ShowDemoWindow();
    ImGui::End();
}
} // namespace Animo
