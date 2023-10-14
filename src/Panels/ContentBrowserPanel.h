#ifndef BSE_EDITOR_CONTENTBROWSERPANEL_H
#define BSE_EDITOR_CONTENTBROWSERPANEL_H

#include <BSE_Client.h>

namespace BSE {
	class ContentBrowserPanel {
	public:
		ContentBrowserPanel();
		ContentBrowserPanel(Scene* scene);
		~ContentBrowserPanel();
		
		inline void SetContext(Scene* scene) { m_Context = scene; };
		void OnImGuiRender();
		
		// selection / deselection
		void SelectEntity(Entity& entity){ m_SelectionContext = entity; m_SelectedEntity = true;}
		void DeSelectEntity(){ m_SelectedEntity = false; }
		bool IsSelectedEntity(){ return m_SelectedEntity; }
	private:
		Scene* m_Context = nullptr;
		
		Entity m_SelectionContext; 
		bool m_SelectedEntity = false;
		
		std::filesystem::path m_CurrentDirectory;
	};
}

#endif
