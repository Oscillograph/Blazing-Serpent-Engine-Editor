#ifndef BSEEDITOR_SCENEVIEWER_H
#define BSEEDITOR_SCENEVIEWER_H

#include <BSE_Client.h>
#include "./ClientData.h"

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
		m_CameraController = new BSE::OrthographicCameraController(uw, 1.5f, true, false);
		BSE::GameData::m_CameraController = m_CameraController;
		// ------------------------------------------------
		BSE::FrameBufferSpecification fbSpec;
		fbSpec.Width = m_Window->GetWidth();
		fbSpec.Height = m_Window->GetHeight();
		m_FrameBufferA = BSE::FrameBuffer::Create(fbSpec);
		// m_FrameBufferB = BSE::FrameBuffer::Create(fbSpec);
		BSE::GameData::m_FrameBuffer = m_FrameBufferA;
		
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
			
			// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
			// CAMERA
			// if no cameras bound, reset camera controller to one of the engine
			if (ClientData::m_ActiveScene->GetCameraController() == nullptr){
				ClientData::m_ActiveScene->SetCameraController(BSE::GameData::m_CameraController);
			}
			// and do not update camera if the scene viewport is not focused
			if (ClientData::ViewPortFocused) {
				ClientData::m_ActiveScene->GetCameraController()->OnUpdate(time);
			}
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
			
			if (ClientData::m_ActiveScene->GetCameraController() != nullptr){
				// BSE::Renderer2D::BeginScene(ClientData::m_ActiveScene->GetCamera());
				BSE::Renderer2D::BeginScene(ClientData::m_ActiveScene->GetCameraController()->GetCamera());
				
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
					BSE::Renderer2D::DrawQuadGeneral(transform.GetTransform(), nullptr, 1.0f, sprite.Color);
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
		// allow camera manipulation via scene
		if (ClientData::ViewPortFocused){
			ClientData::m_ActiveScene->GetCameraController()->OnEvent(event);
		}
	}
	
private:
	float layerTime = 0.0f;
	
	BSE::Window* m_Window = nullptr;
	std::string m_Title;
	BSE::OrthographicCameraController* m_CameraController = nullptr;
	BSE::OrthographicCamera* m_Camera = nullptr; 
	
	BSE::FrameBuffer* m_FrameBufferA;
	// BSE::FrameBuffer* m_FrameBufferB;
};

#endif
