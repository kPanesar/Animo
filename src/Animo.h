#pragma once
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <cstring>
#include <vector>

#include "imgui.h"
#include "imgui_internal.h"

using namespace std;

namespace Animo
{
	// Simplified structure to mimic a Document model
	struct MyDocument
	{
		const char* Name;       // Document title
		const char*	FilePath;   // Path to the file containing document contents
		ImGuiTextBuffer	Content;	// Document contents read from file
		bool		FileLoaded; // Whether the file has been opened and its contents loaded 
		bool        Open;       // Set when open (we keep an array of all available documents to simplify demo code!)
		bool        OpenPrev;   // Copy of Open from last update.
		bool        Dirty;      // Set when the document has been modified
		bool        WantClose;  // Set when the document
		ImVec4      Color;      // An arbitrary variable associated to the document
		string something;
		const char* APIUrl;
		const char* APIKey;

		MyDocument(const char* name, const char* filePath, bool open = false, const ImVec4& color = ImVec4(1.0f, 1.0f, 1.0f, 1.0f))
		{
			Name = name;
			Open = OpenPrev = open;
			Dirty = false;
			WantClose = false;
			Color = color;
			FileLoaded = false;
			FilePath = filePath;
			APIUrl = "https://api.nal.usda.gov/fdc/v1/";
			APIKey = "zsEPS6GddAVdwf8suiDBbaKsBH7CeHPxWFge5bLl";
		}
		void DoOpen() {
			if (!FileLoaded)
			{
				ifstream stream(FilePath);
				stringstream buffer;
				buffer << stream.rdbuf();
				// store the reference to the content (it is destroyed otherwise)
				const string& text = buffer.str();
				Content.clear();
				Content.append(text.c_str());
				FileLoaded = true;
			}
			Open = true;
		}
		void DoQueueClose() { 
			WantClose = true; 
			Content.clear();
		}
		void DoForceClose() { Open = false; Dirty = false; }
		void DoSave() { Dirty = false; }

		// Display placeholder contents for the Document
		static void DisplayContents(MyDocument* doc)
		{
			cout << doc->something;
			ImGui::PushID(doc);
			ImGui::Text("Document \"%s\"", doc->Name);
			ImGui::PushStyleColor(ImGuiCol_Text, doc->Color);
			doc->DoOpen();
			if (doc->FileLoaded)
			{
				ImGui::TextWrapped(doc->Content.begin(), doc->Content.end());
			}
			else {
				ImGui::TextWrapped("Opening document...");
			}
			ImGui::PopStyleColor();
			if (ImGui::Button("Modify", ImVec2(100, 0)))
				doc->Dirty = true;
			ImGui::SameLine();
			if (ImGui::Button("Save", ImVec2(100, 0)))
				doc->DoSave();
			ImGui::ColorEdit3("color", &doc->Color.x);  // Useful to test drag and drop and hold-dragged-to-open-tab behavior.
			ImGui::PopID();
		}

		// Display context menu for the Document
		static void DisplayContextMenu(MyDocument* doc)
		{
			if (!ImGui::BeginPopupContextItem())
				return;

			char buf[256];
			sprintf(buf, "Save %s", doc->Name);
			if (ImGui::MenuItem(buf, "CTRL+S", false, doc->Open))
				doc->DoSave();
			if (ImGui::MenuItem("Close", "CTRL+W", false, doc->Open))
				doc->DoQueueClose();
			ImGui::EndPopup();
		}
	};

	struct ExampleAppDocuments
	{
		ImVector<MyDocument> Documents;

		ExampleAppDocuments()
		{
			Documents.push_back(MyDocument("Lettuce", "../../../resources/docs/lettuce.txt", false, ImVec4(0.4f, 0.8f, 0.4f, 1.0f)));
			Documents.push_back(MyDocument("Eggplant", "../../../resources/docs/eggplant.txt", false, ImVec4(0.8f, 0.5f, 1.0f, 1.0f)));
			Documents.push_back(MyDocument("Carrot", "../../../resources/docs/carrot.txt", false, ImVec4(1.0f, 0.8f, 0.5f, 1.0f)));
			Documents.push_back(MyDocument("Tomato", "../../../resources/docs/tomato.txt", false, ImVec4(1.0f, 0.3f, 0.4f, 1.0f)));
			Documents.push_back(MyDocument("A Rather Long Title", "../../../resources/docs/sample.txt", false));
			Documents.push_back(MyDocument("Some Document", "../../../resources/docs/sample.txt", false));
		}
	};

    static ExampleAppDocuments app;

    // Options
    enum Target
    {
        Target_None,
        Target_Tab,                 // Create documents as local tab into a local tab bar
        Target_DockSpaceAndWindow   // Create documents as regular windows, and create an embedded dockspace
    };
    static Target opt_target = Target_Tab;
    static ImGuiTabBarFlags opt_fitting_flags = ImGuiTabBarFlags_FittingPolicyDefault_;

	static std::vector<std::string> files;

    void RenderUI();
}
