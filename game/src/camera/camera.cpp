#include "camera.hpp"
#include "../background/background.hpp"
#include "../config.hpp"

Camera::Camera() 
{
    m_view.setSize({SCREEN_WIDTH, SCREEN_HEIGHT});
    m_view.setCenter({SCREEN_WIDTH / 2.f, SCREEN_HEIGHT / 2.f});
}

void Camera::Update(const sf::Vector2f& targetPos, const Background& background) 
{
    sf::Vector2f cameraPos = targetPos;

    float mapWidthPx = background.GetWidth() * background.GetTileSize();
    float mapHeightPx = background.GetHeight() * background.GetTileSize();

    float halfWinX = SCREEN_WIDTH / 2.f;
    float halfWinY = SCREEN_HEIGHT / 2.f;

    if (cameraPos.x < halfWinX) {
        cameraPos.x = halfWinX;
    } else if (cameraPos.x > mapWidthPx - halfWinX) {
        cameraPos.x = mapWidthPx - halfWinX;
    }

    if (cameraPos.y < halfWinY) {
        cameraPos.y = halfWinY;
    } else if (cameraPos.y > mapHeightPx - halfWinY) {
        cameraPos.y = mapHeightPx - halfWinY;
    }

    if (mapWidthPx < SCREEN_WIDTH) cameraPos.x = mapWidthPx / 2.f;
    if (mapHeightPx < SCREEN_HEIGHT) cameraPos.y = mapHeightPx / 2.f;
    
    m_view.setCenter(cameraPos);
}

const sf::View& Camera::GetView() const 
{
    return m_view;
}