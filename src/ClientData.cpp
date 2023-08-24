#include "./ClientData.h"

bool ClientData::ViewPortFocused = false;
BSE::Scene* ClientData::m_ActiveScene = nullptr;
BSE::Entity* ClientData::m_Square = nullptr;
BSE::Entity* ClientData::m_CameraA = nullptr;
BSE::Entity* ClientData::m_CameraB = nullptr;

int ClientData::quadsMaxX = 100;
int ClientData::quadsMaxY = 100;
