#ifndef BSEEDITOR_CLIENTDATA_H
#define BSEEDITOR_CLIENTDATA_H

#include <BSE_Client.h>
#include <filesystem>

struct ClientData {
	static bool ViewPortFocused;
	
	static std::filesystem::path g_AssetsDirectory;
	
	// Scene Viewer
	static BSE::Scene* m_ActiveScene;
	static BSE::Entity* m_Square;
	static BSE::Entity* m_CameraA;
	static BSE::Entity* m_CameraB;
	
	static BSE::FrameBuffer* m_FrameBuffer;
	
	static int quadsMaxX;
	static int quadsMaxY;
	
	static bool EditorCameraOn;
	
	// routines
	static void NewScene(){
		delete m_ActiveScene;
		m_ActiveScene = nullptr;
		BSE_CORE_INFO("New Scene created");
		m_ActiveScene = new BSE::Scene();
		// BSE::GameData::m_App->PushLayer(m_ActiveScene);
		m_ActiveScene->SetCameraController(BSE::GameData::m_CameraController);
		BSE_CORE_INFO("New Scene camera controller set");
	}
};

#endif
