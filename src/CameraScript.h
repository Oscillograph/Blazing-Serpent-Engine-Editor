#ifndef BSE_EDITOR_CAMERASCRIPT_H
#define BSE_EDITOR_CAMERASCRIPT_H

#include <BSE_Client.h>

class CameraScript : public BSE::ScriptableEntity {
public:
	void OnCreate(){}
	
	void OnDestroy(){}
	
	void OnUpdate(float time){
		auto& transform = GetComponent<BSE::TransformComponent>();
		// auto& cameraController = GetComponent<BSE::CameraControllerComponent>().CameraController;
		// auto cameraPosition = cameraController->GetCamera()->GetPosition();
		float speed = 5.0f;
		
		if (BSE::Input::IsKeyPressed(BSE::KeyCode::I)){
			// cameraPosition.y += speed * time;
			transform.Translation.y += speed * time;
		}
		if (BSE::Input::IsKeyPressed(BSE::KeyCode::J)){
			// cameraPosition.x -= speed * time;
			transform.Translation.x -= speed * time;
		}
		if (BSE::Input::IsKeyPressed(BSE::KeyCode::K)){
			// cameraPosition.y -= speed * time;
			transform.Translation.y -= speed * time;
		}
		if (BSE::Input::IsKeyPressed(BSE::KeyCode::L)){
			// cameraPosition.x += speed * time;
			transform.Translation.x += speed * time;
		}
		// cameraController->GetCamera()->SetPosition(cameraPosition);
	}
};

#endif
