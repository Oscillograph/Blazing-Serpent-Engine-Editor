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
		void DrawVec3Control(const std::string& label, glm::vec3& values, float resetValue = 0.0f, float columnWidth = 100.0f);
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
