#include "./ContentBrowserPanel.h"
#include "./../ClientData.h"

namespace BSE {
	// pre-defines
	// ClientData::g_AssetsDirectory = "assets";
	
	ContentBrowserPanel::ContentBrowserPanel()
		: m_CurrentDirectory(ClientData::g_AssetsDirectory) 
	{
	}
	
	ContentBrowserPanel::ContentBrowserPanel(Scene* scene)
		: m_Context(scene), m_CurrentDirectory(ClientData::g_AssetsDirectory) 
	{
	}
	
	void ContentBrowserPanel::OnImGuiRender(){
		//if (!m_SelectedEntity)
		//	m_SelectionContext = {};
		
		ImGui::Begin("Браузер файлов");
		
		if (m_CurrentDirectory != ClientData::g_AssetsDirectory){
			ImGui::Button("..##GoUpDirectory");
			if (ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left)){
				m_CurrentDirectory = m_CurrentDirectory.parent_path();
			}
		}
		
		for (auto& directoryEntry : std::filesystem::directory_iterator{m_CurrentDirectory}){
			auto& path = directoryEntry.path();
			auto relativePath = std::filesystem::relative(path, ClientData::g_AssetsDirectory);
			std::string filenameString = relativePath.filename().string();
			
			ImGui::PushID(filenameString.c_str());
			ImGui::Button(filenameString.c_str());
			
			if (ImGui::BeginDragDropSource()){
				std::string itemPath = relativePath.string();
				if (relativePath.extension() == ".png"){
					ImGui::SetDragDropPayload("Asset_Texture", itemPath.c_str(), (itemPath.size()) * sizeof(char), ImGuiCond_Once);
				}
				if (relativePath.extension() == ".BSEScene"){
					ImGui::SetDragDropPayload("Content_Browser_Item", itemPath.c_str(), (itemPath.size()) * sizeof(char), ImGuiCond_Once);
				}
				
				ImGui::EndDragDropSource();   
			}
			
			if (ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left)){
				if (directoryEntry.is_directory()){
					// go into the directory
					m_CurrentDirectory /= path.filename();
				} else {
					// do something depending on file extension
				}
			}
			ImGui::PopID();
		}
		
		ImGui::End();
	}
}
