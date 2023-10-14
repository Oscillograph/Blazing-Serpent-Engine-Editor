#include "EditorCameraController.h"

namespace BSE {
	EditorCameraController::EditorCameraController(float fov, float aspectRatio, float znear, float zfar)
		// : GeneralCameraController(0, 0) // use parent dummy contructor to prevent it from doing harm here
	{
		BSE_CORE_TRACE("Calling Editor Camera Controller constructor");
		m_PerspectiveVerticalFOV = glm::radians(fov);
		m_PerspectiveHorizontalFOV = m_PerspectiveVerticalFOV;
		m_AspectRatio = aspectRatio;
		m_AspectRatioPrev = m_AspectRatio;
		m_ZoomLevel = 10.0f;
		// m_Size = size;
		m_Rotation = true;
		m_ConstantAspectRatio = false;
		m_ProjectionType = CameraProjectionType::Perspective;
		
		float orthoLeft 	= -m_AspectRatio * m_ZoomLevel * 0.5f;
		float orthoRight 	=  m_AspectRatio * m_ZoomLevel * 0.5f;
		float orthoTop 		=  m_ZoomLevel * 0.5f;
		float orthoBottom 	= -m_ZoomLevel * 0.5f;
		float orthoZNear 	= -2.0f;
		float orthoZFar 	=  16.0f;
		
		// m_Camera->SetProjection(orthoLeft, orthoRight, orthoTop, orthoBottom, orthoZNear, orthoZFar);
		m_CameraBounds = { orthoLeft, orthoRight, orthoTop, orthoBottom, orthoZNear, orthoZFar };
		
		m_Camera = new EditorCamera(
			m_PerspectiveVerticalFOV, 
			m_AspectRatio, 
			m_CameraBounds.ZNear, 
			m_CameraBounds.ZFar);
		
		m_CameraPosition = m_Camera->GetPosition();
	}
	
	EditorCameraController::~EditorCameraController(){
		
	}
	
	void EditorCameraController::OnUpdate(float time){
		BSE_INFO("Editor Camera Controller - OnUpdate()");
		m_CameraPosition = m_Camera->GetPosition();
		if (Input::IsKeyPressed(BSE_KEY_W)){
			m_CameraPosition.y += m_CameraMoveSpeed * time;
		}
		if (Input::IsKeyPressed(BSE_KEY_A)){
			m_CameraPosition.x -= m_CameraMoveSpeed * time;
		}
		if (Input::IsKeyPressed(BSE_KEY_S)){
			m_CameraPosition.y -= m_CameraMoveSpeed * time;
		}
		if (Input::IsKeyPressed(BSE_KEY_D)){
			m_CameraPosition.x += m_CameraMoveSpeed * time;
		}
		m_Camera->SetPosition(m_CameraPosition);
		
		m_Camera->OnUpdate(time);
	}
	
	void EditorCameraController::OnResize(float width, float height) {
		if ((width > 0.0f) && height > 0.0f){
			m_Width = width;
			m_Height = height;
			
			if (GetConstantAspectRatio()){
				// BSE_INFO("Aspect Ratio is set to be constant");
				m_AspectRatio = m_AspectRatioPrev;
				// m_ZoomLevel = m_ZoomLevel;
			} else {
				// BSE_INFO("Width: {0}, Height: {1}", width, height);
				m_AspectRatio = width / height;
			}
			
			// m_ZoomLevel = m_ZoomLevel * m_AspectRatio / m_AspectRatioPrev;
			m_AspectRatioPrev = m_AspectRatio;
			m_HeightPrev = width;
			m_WidthPrev = height;
			
			SetProjectionDefault();
		}
	}
	
	void EditorCameraController::OnEvent(Event& e){
		m_Camera->OnEvent(e);
		
		/*
		EventDispatcher dispatcher(e);
		
		dispatcher.Dispatch<MouseScrolledEvent>([this](MouseScrolledEvent& event){
			return OnMouseScrolled(event);
		});
		
		dispatcher.Dispatch<WindowResizeEvent>([this](WindowResizeEvent& event){
			return OnWindowResized(event);
		});
		*/
	}
	
	bool EditorCameraController::OnMouseScrolled(MouseScrolledEvent& e){
		m_ZoomLevel -= e.GetYOffset() * 0.1f;
		//m_ZoomLevel = (m_ZoomLevel < m_ZoomMin) ? (m_ZoomMin) : m_ZoomLevel;
		//m_ZoomLevel = (m_ZoomLevel > m_ZoomMax) ? (m_ZoomMax) : m_ZoomLevel;
		
		SetProjectionDefault();
		
		return false;
	}
	
	bool EditorCameraController::OnWindowResized(WindowResizeEvent& e){
		float widthNew = (float)(e.GetWidth());
		float heightNew = (float)(e.GetHeight());
		
		OnResize(widthNew, heightNew);
		return false;
	}
}
