#ifndef BSEEDITOR_SCENEHIERARCHYPANEL_H
#define BSEEDITOR_SCENEHIERARCHYPANEL_H

#include <BSE_Client.h>
#include "../ClientData.h"
// #include <BSE/systems/scene/Scene.h>

namespace BSE {
	class SceneHierarchyPanel {
		using VoidFn = std::function<void(Entity& entity)>;
	public:
		SceneHierarchyPanel() = default;
		SceneHierarchyPanel(Scene* scene);
		
		void SetContext(Scene* scene);
		
		void OnImGuiRender();

		template <typename Component>
		void DrawComponent(const char* label, Entity& entity, VoidFn func);
		
		void DrawComponents(Entity& entity);
	protected:
		void DrawEntityNode(Entity& entity);
		
	private:
		Scene* m_Context;
		Entity m_SelectionContext; 
		bool m_SelectedEntity = false;
	};
}

#endif
