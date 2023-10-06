#ifndef BSEEDITOR_SCENEVIEWER_H
#define BSEEDITOR_SCENEVIEWER_H

#include <BSE_Client.h>
#include "./ClientData.h"
// #include "./EditorCamera.h"
// #include "./EditorCameraController.h"

class SceneViewer : public BSE::Layer {
public:
	SceneViewer()
	: BSE::Layer("SceneViewer")
	{
		BSE_TRACE("Layer Constructor Enter");
		m_Window = BSE::Application::Get()->GetWindow();
		m_Title = m_Window->GetTitle();
	}
	
	~SceneViewer()
	{
		BSE::SceneSerializer serializer(ClientData::m_ActiveScene);
		serializer.SerializeToFile("./assets/scenes/MainScene.BSEScene");
		
		if (m_CameraController != nullptr){
			delete m_CameraController;
			m_CameraController = nullptr;
		}
	}
	
	void OnAttach() override {
		// ------------------------------------------------
		// OpenGL camera controller
		unsigned int w = BSE::Application::Get()->GetWindow()->GetWidth();
		unsigned int h = BSE::Application::Get()->GetWindow()->GetHeight();
		float uw = (float)w/(float)h;
		// float uh = 2*(float)h/(float)h;
		// TODO: Make a separate camera controller class for Editor Camera
		// m_CameraController = new BSE::GeneralCameraController(uw, 1.5f, true, false);
		m_CameraController = new BSE::CameraController(45.0f, 1.778f, 0.1f, 1000.0f);
		m_CameraController->editorCamera = true;
		BSE::GameData::m_CameraController = m_CameraController;
		// ------------------------------------------------
		BSE::FrameBufferSpecification fbSpec;
		fbSpec.Attachments = { 
			BSE::FrameBufferTextureFormat::RGBA8,
			BSE::FrameBufferTextureFormat::RED_INTEGER,
			// BSE::FrameBufferTextureFormat::RGBA8,
			BSE::FrameBufferTextureFormat::Depth 
		};
		fbSpec.Width = m_Window->GetWidth();
		fbSpec.Height = m_Window->GetHeight();
		m_FrameBufferA = BSE::FrameBuffer::Create(fbSpec);
		// m_FrameBufferB = BSE::FrameBuffer::Create(fbSpec);
		BSE::GameData::m_FrameBuffer = m_FrameBufferA;
		ClientData::m_FrameBuffer = m_FrameBufferA;
		
		// Scene setup
		ClientData::m_ActiveScene = new BSE::Scene();
		ClientData::m_ActiveScene->SetCameraController(BSE::GameData::m_CameraController);
		
		/*
		BSE::SceneSerializer serializer(ClientData::m_ActiveScene);
		if (!serializer.DeserializeFromFile("./assets/scenes/MainScene.BSEScene")){
			// Create defaults
			ClientData::m_Square = ClientData::m_ActiveScene->CreateEntity("Square");
			
			// TODO: find out why SpriteComponent breaks the app
			ClientData::m_Square->AddComponent<BSE::SpriteComponent>(glm::vec4({1.0f, 0.3f, 0.2f, 1.0f}));
			ClientData::m_Square->AddComponent<BSE::NativeScriptComponent>().Bind<CameraScript>();
			if (ClientData::m_Square->HasComponent<BSE::SpriteComponent>())
				BSE_INFO("Added sprite component");
			
			ClientData::m_CameraA = ClientData::m_ActiveScene->CreateEntity("Camera Default");
			ClientData::m_CameraA->AddComponent<BSE::CameraControllerComponent>(uw, 1.5f, false, false);
			ClientData::m_CameraA->GetComponent<BSE::CameraControllerComponent>().CameraController->Rotate(0.0f);
			// ClientData::m_CameraA->AddComponent<BSE::NativeScriptComponent>().Bind<CameraScript>();
			
			ClientData::m_CameraB = ClientData::m_ActiveScene->CreateEntity("Camera Custom");
			ClientData::m_CameraB->AddComponent<BSE::CameraControllerComponent>(uw, 1.5f, true, true);
			ClientData::m_CameraB->GetComponent<BSE::CameraControllerComponent>().CameraController->Rotate(35.0f);
			
			// set default camera for the scene and cameracontroller
			ClientData::m_ActiveScene->SetCameraController(ClientData::m_CameraA->GetComponent<BSE::CameraControllerComponent>().CameraController);
		}
		*/
	}
	
	void OnDetach() override {
		
	}
	
	int OnUpdate(float time) override {
		BSE_PROFILER(u8"Sandbox2D Layer OnUpdate");

		static int frame = 0;
		// BSE_TRACE("Layer time counter increased");
		
		// --------------------------------------------------
		// SCENE
		ClientData::m_ActiveScene->OnUpdate(time);
		
		// RENDER
		if (BSE::RenderCommand::GetAPI() != nullptr){
			BSE_PROFILE_SCOPE("Sandbox2D OnUpdate");
			// BSE_CORE_INFO("Editor Camera? {0}", ClientData::m_ActiveScene->GetCameraController()->IsEditorCamera());
			// BSE_CORE_INFO("Editor Camera? {0}", (int)ClientData::m_ActiveScene->GetCameraController()->GetProjectionType());
			// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
			// CAMERA
			// if no cameras bound, reset camera controller to one of the engine
			// TODO: Fix Editor Camera Controller crashing the app
			//BSE_INFO("1. Is this the stop point?");
			if (ClientData::m_ActiveScene->GetCameraController() == nullptr){
			//	BSE_INFO("2. Is this the stop point?");
				ClientData::m_ActiveScene->SetCameraController(BSE::GameData::m_CameraController);
			//	BSE_INFO("3. Is this the stop point?");
				// ClientData::EditorCameraOn = true;
			}
			// and do not update camera if the scene viewport is not focused
			// BSE_INFO("4. Is this the stop point?");
			if (ClientData::ViewPortFocused) {
				// BSE_INFO("Is this the stop point?");
				ClientData::m_ActiveScene->GetCameraController()->OnUpdate(time);
			}
			// BSE_INFO("5. Is this the stop point?");
			// BSE_TRACE("Camera controller updated");
			// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
			
			// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
			// Show some info about FPS on WindowTitle
			int fps = round(1 / time);
			if (frame == fps){
				frame = 0;
			}
			char buf[5];
			sprintf(buf, "%d", fps);
			m_Window->SetTitle(m_Title + " :: FPS = " + buf);
			// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
			// Bind a buffer to work with
			{
				BSE_PROFILE_SCOPE(u8"Framebuffer Bind");
				//if (BSE::GameData::m_FrameBuffer == m_FrameBufferA) {
				//	m_FrameBufferB->Bind();
				//} else {
					m_FrameBufferA->Bind();
				//}
			}
			
			// BSE_TRACE("Window Title changed");
			BSE::Renderer2D::Clear({0.2f, 0.2f, 0.4f, 1});
			// BSE::Renderer2D::Clear({0.0f, 0.0f, 0.0f, 0});
			m_FrameBufferA->ClearAttachment(1, -1); // clear entity attachment
			
			if (ClientData::m_ActiveScene->GetCameraController() != nullptr){
				// BSE::Renderer2D::BeginScene(ClientData::m_ActiveScene->GetCamera());
				// BSE_INFO("6. Is this the stop point?"); // this is the last message !!!!!!!!!!!!!!!!!!
				BSE::Renderer2D::BeginScene(ClientData::m_ActiveScene->GetCameraController()->GetCamera());
				// BSE_INFO("7. Is this the stop point?");
				
				for (int x = 0; x < ClientData::quadsMaxX; x++){
					for (int y = 0; y < ClientData::quadsMaxY; y++){
						BSE::Renderer2D::DrawFilledRect(
							{-1.0f + x*0.06f, -1.0f + y*0.06f}, 
							{0.05f, 0.05f},
							BSE::GameData::CustomColor
							);
					}
					// BSE_INFO("x = {0}, y = {1}", x , y);
				}
				
				// m_ActiveScene->OnUpdate(layerTime);
				auto group = ClientData::m_ActiveScene->Registry().view<BSE::TransformComponent, BSE::SpriteComponent>();
				for (auto entity : group){
					auto [transform, sprite] = group.get<BSE::TransformComponent, BSE::SpriteComponent>(entity);
					// Renderer::Submit(sprite, transform);
					// TODO: remove temporary usage of renderer
					int32_t entityID = (int32_t)((uint32_t)entity);
					// BSE_CORE_WARN("Render2D Entity: {0}", entityID);
					BSE::Renderer2D::DrawQuadGeneral(transform.GetTransform(), nullptr, 1.0f, sprite.Color, nullptr, entityID);
				}
				
				BSE::Renderer2D::EndScene();
			}
			
			// Unbind current buffer
			{
				BSE_PROFILE_SCOPE(u8"Framebuffer Unbind");
				//if (BSE::GameData::m_FrameBuffer == m_FrameBufferA) {
				//	m_FrameBufferB->Unbind();
				//} else {
					m_FrameBufferA->Unbind();
				//}
			}
			
			// swap buffers
			// BSE::GameData::m_FrameBuffer = (BSE::GameData::m_FrameBuffer == m_FrameBufferA) ? m_FrameBufferB : m_FrameBufferA;
			
			frame++;
			return 1; // 1 - changed something
		} else {
			return 0; // 0 - no changes on screen
		}
		return 0; // RendererAPI == nullptr
	}
	
	void OnEvent(BSE::Event& event) override {
		BSE::EventDispatcher dispatcher(event);
		dispatcher.Dispatch<BSE::MouseButtonPressed>([this](BSE::MouseButtonPressed& event){
			return OnMouseButtonPressed(event);
		});
		
		// allow camera manipulation via scene
		if (ClientData::ViewPortFocused){
			ClientData::m_ActiveScene->GetCameraController()->OnEvent(event);
		}
	}
	
	bool OnMouseButtonPressed(BSE::MouseButtonPressed& event){
		
		return true;
	}
	
private:
	float layerTime = 0.0f;
	
	BSE::Window* m_Window = nullptr;
	std::string m_Title;
	BSE::CameraController* m_CameraController = nullptr;
	BSE::GeneralCamera* m_Camera = nullptr; 
	
	BSE::FrameBuffer* m_FrameBufferA;
	// BSE::FrameBuffer* m_FrameBufferB;
};

#endif
