#ifndef BSEEDITOR_SCENEHIERARCHYPANEL_H
#define BSEEDITOR_SCENEHIERARCHYPANEL_H

#include <BSE_Client.h>
#include "../ClientData.h"
// #include <BSE/systems/scene/Scene.h>

namespace BSE {
	class SceneHierarchyPanel {
		using VoidFn = std::function<void(Entity& entity)>;
		using EmptyFn = std::function<void()>;
	public:
		SceneHierarchyPanel() = default;
		SceneHierarchyPanel(Scene* scene);
		
		void SetContext(Scene* scene);
		
		void OnImGuiRender();

		template <typename Component>
		void DrawComponent(const char* label, Entity& entity, VoidFn func, bool canBeDeleted = true);
		void DrawVec3Control(	const std::string& label, 
								glm::vec3& values, 
								EmptyFn func = [](){}, 
								float resetValue = 0.0f, 
								float columnWidth = 100.0f);
		void DrawComponents(Entity& entity);
		Entity& GetSelectedEntity(){ return m_SelectionContext; }
		bool IsSelectedEntity(){ return m_SelectedEntity; }
	protected:
		void DrawEntityNode(Entity& entity);
		
	private:
		Scene* m_Context;
		Entity m_SelectionContext; 
		// Entity m_CurrentlySelectedEntity;
		bool m_SelectedEntity = false;
	};
}

#endif
