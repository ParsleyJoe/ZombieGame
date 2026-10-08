#pragma once

#include <raylib.h>

#include <vector>
#include <functional>
#include <optional>
#include <algorithm>

#include "player.hpp"
#include "enemy.hpp"
#include "particlesystem.hpp"
#include "spawner.hpp"


struct FloorDecal {
	Vector2 pos;
	float rotation;
};

class Map {
public:
	Map()
	{
		m_objects = {
			{ 400,  300,  500,   50 },
			{ 400,  300,   50,  400 },
			{ 1200, 500,  300,   50 },
			{ 1500, 500,   50,  400 }
		};
	}

	void draw() const
	{
		DrawRectangle(0, 0, 3000, 2000, m_backgroundColor);
		Color gridColor = Color{ 30, 33, 34, 255 };
		for (int x = 0; x < 3000; x += 50) {
			DrawLine(x, 0, x, 2000, gridColor);
		}

		for (int y = 0; y < 2000; y += 50) {
			DrawLine(0, y, 3000, y, gridColor);
		}
		Color outlineColor = Color{ 100, 105, 108, 255 };
		for (const auto& obj : m_objects) {
			DrawRectangleRec(obj, m_wallColor);
			DrawRectangleLinesEx(obj, 3.0f, outlineColor);
			DrawRectangle(
				obj.x,
				obj.y + obj.height - 6, obj.width,
				6, Color{ 35, 37, 38, 255 });
		}
	}

	const std::vector<Rectangle>& getObjects() const {
		return m_objects;
	}

private:
	std::vector<Rectangle> m_objects;
	std::vector<FloorDecal> m_decals;
	Color m_backgroundColor = Color{ 24, 27, 28, 255 };
	Color m_wallColor = Color{ 55, 58, 60, 255 };
};


class Game {
public:
	Game() // Need to pass in spawnBullet callback
		: m_player([this](Bullet blt) { spawnBullets(blt); })
		, m_camera(Camera2D{
				.offset = {GetScreenWidth() / 2.0f, GetScreenHeight() / 2.0f},
				.target = m_player.getPosition(),
				.rotation = 0.0f,
				.zoom = 1.0f
			})
		, m_spawner([this](int dmg) { m_player.takeDamage(dmg); }) // player damage callback
	{
	}

	void update() {
		float dt = GetFrameTime();

		Vector2 mouseWorldPos = GetScreenToWorld2D(GetMousePosition(), m_camera);
		m_player.update(dt, mouseWorldPos);
		updateCamera(dt);
		for (auto& blt : m_bullets) {
			blt.update(dt);
		}
		for (auto& enemy : m_enemies) {
			enemy.update(dt, m_player.getPosition());
		}
		m_spawner.update(dt, m_enemies);
		m_particleSystem.update(dt);

		// Collisions
		const auto& mapObjects = m_map.getObjects(); // just a vec of Rectangles rn
		for (const auto& obj : mapObjects) {
			// Check for player
			Rectangle playerBox = m_player.getHitbox();
			if (CheckCollisionRecs(playerBox, obj)) {
				float overlapX = std::min(playerBox.x + playerBox.width, obj.x + obj.width) - std::max(playerBox.x, obj.x);
				float overlapY = std::min(playerBox.y + playerBox.height, obj.y + obj.height) - std::max(playerBox.y, obj.y);

				if (overlapX < overlapY) {
					// resolve horizontally
					if (playerBox.x < obj.x)
						m_player.push(-overlapX, 0);
					else
						m_player.push(overlapX, 0);
				}
				else {
					// resolve vertically
					if (playerBox.y < obj.y)
						m_player.push(0, -overlapY);
					else
						m_player.push(0, overlapY);
				}
			}

			// Bullets-Map obj
			for (auto& blt : m_bullets) {
				if (CheckCollisionRecs(blt.getHitbox(), obj)) {
					blt.setActive(false);
				}
			}
		}

		// Bullet-Enemy
		for (auto& blt : m_bullets) {
			for (auto& enemy : m_enemies) {
				if (CheckCollisionRecs(blt.getHitbox(), enemy.getHitbox())) {
					blt.setActive(false);
					m_particleSystem.emit(enemy.getPosition(), 20);
					enemy.takeDamage(blt.getDamage());
				}
			}
		}

		// delete blts
		std::erase_if(m_bullets, [](const Bullet& blt) {
			return !blt.getActive();
		});
		// delete enemies
		std::erase_if(m_enemies, [](const Enemy& enemy) {
			return !enemy.getActive();
		});
	}

	void draw() const {
		BeginMode2D(m_camera);

		m_map.draw();
		m_player.draw();
		m_particleSystem.draw();
		for (auto& enemy : m_enemies) {
			enemy.draw();
		}
		for (auto& blt : m_bullets) {
			blt.draw();
		}
		EndMode2D();

		drawHealthBar();
	}

	void spawnBullets(Bullet& blt) {
		m_bullets.push_back(blt);
	}

	void updateCamera(float dt) {
		Vector2 playerPos = m_player.getPosition();

		Vector2 screenPlayerPos = GetWorldToScreen2D(playerPos, m_camera);

		const float marginX = 200.0f;
		const float marginY = 150.0f;

		float screenWidth = static_cast<float>(GetScreenWidth());
		float screenHeight = static_cast<float>(GetScreenHeight());

		Vector2 desiredTarget = m_camera.target;
		if (screenPlayerPos.x > screenWidth - marginX) {
			desiredTarget.x += screenPlayerPos.x - (screenWidth - marginX);

		}
		else if (screenPlayerPos.x < marginX) {
			desiredTarget.x -= marginX - screenPlayerPos.x;
		}

		if (screenPlayerPos.y < marginY) {
			desiredTarget.y -= marginY - screenPlayerPos.y;
		}
		else if (screenPlayerPos.y > screenHeight - marginY) {
			desiredTarget.y += screenPlayerPos.y - (screenHeight - marginY);
		}

		m_camera.target.x = Lerp(m_camera.target.x, desiredTarget.x, 8.0f * dt);
		m_camera.target.y = Lerp(m_camera.target.y, desiredTarget.y, 8.0f * dt);
	}
private:
	void drawHealthBar() const
	{
		const float barWidth = 300.0f;
		const float barHeight = 25.0f;

		const float x = 20.0f;
		const float y = 20.0f;

		float healthPercent = static_cast<float>(m_player.getHealth()) / static_cast<float>(m_player.getMaxHealth());

		healthPercent = std::clamp(healthPercent, 0.0f, 1.0f);

		// Background
		DrawRectangle(x, y, barWidth, barHeight, DARKGRAY);

		// Health
		DrawRectangle(x, y, barWidth * healthPercent, barHeight, RED);

		// Border
		DrawRectangleLines(x, y, barWidth, barHeight, BLACK);

		// Text
		DrawText(TextFormat("%d / %d", m_player.getHealth(), m_player.getMaxHealth()), x + 10, y + 3, 18, WHITE);
	}

	Player m_player;
	std::vector<Bullet> m_bullets{};
	Camera2D m_camera{};
	Map m_map{};
	ParticleSystem m_particleSystem;
	Spawner m_spawner;
	std::vector<Enemy> m_enemies;
};