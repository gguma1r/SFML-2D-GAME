#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <vector>
#include <cmath>
#include <random>
#include <iostream>
#include <sstream>
#include <iomanip>
#include <memory>

// Game states
enum GameState {
    MAIN_MENU,
    PLAYING_CLASSIC,
    PLAYING_TIME_TRIAL,
    GAME_OVER,
    VICTORY,
    TIME_TRIAL_RESULTS
};

// Powerup types
enum PowerupType {
    HEALTH_BOOST,
    SPEED_BOOST
};

// Enemy types
enum EnemyType {
    ENEMY_TYPE_1,
    ENEMY_TYPE_2
};

// Texture manager to hold shared textures
class TextureManager {
public:
    sf::Texture playerTexture;
    sf::Texture enemy1Texture;
    sf::Texture enemy2Texture;
    sf::Texture healthTexture;
    sf::Texture speedTexture;
    sf::Texture backgroundTexture;

    TextureManager() {
        // Load player texture
        if (!playerTexture.loadFromFile("player.png")) {
            sf::Image img;
            img.create(50, 50, sf::Color::Blue);
            playerTexture.loadFromImage(img);
            std::cout << "Warning: Could not load player.png, using blue placeholder" << std::endl;
        }
        else {
            std::cout << "Player texture loaded successfully" << std::endl;
        }

        // Load enemy1 texture
        if (!enemy1Texture.loadFromFile("enemy1.png")) {
            sf::Image img;
            img.create(40, 40, sf::Color::Red);
            enemy1Texture.loadFromImage(img);
            std::cout << "Warning: Could not load enemy1.png, using red placeholder" << std::endl;
        }
        else {
            std::cout << "Enemy1 texture loaded successfully" << std::endl;
        }

        // Load enemy2 texture
        if (!enemy2Texture.loadFromFile("enemy2.png")) {
            sf::Image img;
            img.create(40, 40, sf::Color::Magenta);
            enemy2Texture.loadFromImage(img);
            std::cout << "Warning: Could not load enemy2.png, using magenta placeholder" << std::endl;
        }
        else {
            std::cout << "Enemy2 texture loaded successfully" << std::endl;
        }

        // Load health powerup texture
        if (!healthTexture.loadFromFile("health.png")) {
            sf::Image img;
            img.create(30, 30, sf::Color::Green);
            healthTexture.loadFromImage(img);
            std::cout << "Warning: Could not load health.png, using green placeholder" << std::endl;
        }
        else {
            std::cout << "Health powerup texture loaded successfully" << std::endl;
        }

        // Load speed powerup texture
        if (!speedTexture.loadFromFile("speed.png")) {
            sf::Image img;
            img.create(30, 30, sf::Color::Cyan);
            speedTexture.loadFromImage(img);
            std::cout << "Warning: Could not load speed.png, using cyan placeholder" << std::endl;
        }
        else {
            std::cout << "Speed powerup texture loaded successfully" << std::endl;
        }

        // Load background texture
        if (!backgroundTexture.loadFromFile("3858.jpg")) {

            //if (!backgroundTexture.loadFromFile("ground3.png")) {
            sf::Image img;
            img.create(1600, 900, sf::Color(50, 100, 50));
            backgroundTexture.loadFromImage(img);
            std::cout << "Warning: Could not load ground3.png, using green placeholder" << std::endl;









        }
        else {
            std::cout << "Background texture loaded successfully" << std::endl;
        }
    }
};

// Player class
class Player {
public:
    sf::Sprite sprite;
    float speed;
    float baseSpeed;
    int health;
    int maxHealth;
    float speedBoostTimer;
    bool hasSpeedBoost;

    Player(float x, float y, sf::Texture& texture) {
        sprite.setTexture(texture);
        sprite.setPosition(x, y);
        sprite.setScale(0.4f, 0.4f); // Increased for visibility

        baseSpeed = 300.0f;
        speed = baseSpeed;
        health = 100;
        maxHealth = 100;
        speedBoostTimer = 0;
        hasSpeedBoost = false;
    }

    void reset(float x, float y) {
        sprite.setPosition(x, y);
        health = 100;
        speed = baseSpeed;
        speedBoostTimer = 0;
        hasSpeedBoost = false;
    }

    void update(float deltaTime) {
        if (hasSpeedBoost) {
            speedBoostTimer -= deltaTime;
            if (speedBoostTimer <= 0) {
                hasSpeedBoost = false;
                speed = baseSpeed;
            }
        }
    }

    void takeDamage(int damage) {
        health -= damage;
        if (health < 0) health = 0;
    }

    void heal(int amount) {
        health += amount;
        if (health > maxHealth) health = maxHealth;
    }

    void applySpeedBoost() {
        hasSpeedBoost = true;
        speed = baseSpeed * 1.5f;
        speedBoostTimer = 5.0f;
    }

    sf::Vector2f getCenter() {
        sf::FloatRect bounds = sprite.getGlobalBounds();
        return sf::Vector2f(bounds.left + bounds.width / 2, bounds.top + bounds.height / 2);
    }

    sf::FloatRect getBounds() {
        return sprite.getGlobalBounds();
    }
    void rotateTowards(sf::Vector2f targetPos) {
        sf::Vector2f playerCenter = getCenter();
        sf::Vector2f direction = targetPos - playerCenter;

        // Calculate distance to mouse
        float distance = sqrt(direction.x * direction.x + direction.y * direction.y);

        // Only rotate if mouse is far enough away (dead zone)
        if (distance > 30.0f) {  // 30 pixel dead zone - adjust this value as needed
            // Calculate angle in degrees
            float angle = atan2(direction.y, direction.x) * 180.0f / 3.14159f;

            // Set the rotation
            sprite.setRotation(angle);
        }
    }
    


};

// Bullet class
class Bullet {
public:
    sf::CircleShape shape;
    sf::Vector2f velocity;
    bool active;

    Bullet(float x, float y, sf::Vector2f direction) {
        shape.setRadius(4);
        shape.setFillColor(sf::Color::Yellow);
        shape.setPosition(x - 4, y - 4);


        float speed = 600.0f;
        velocity = direction * speed;
        active = true;
    }

    void update(float deltaTime) {
        if (active) {
            shape.move(velocity * deltaTime);
            sf::Vector2f pos = shape.getPosition();
            if (pos.x < 0 || pos.x > 1600 || pos.y < 0 || pos.y > 900) {
                active = false;
            }
        }
    }

    sf::FloatRect getBounds() {
        return shape.getGlobalBounds();
    }
};

// Enemy class
class Enemy {
public:
    sf::Sprite sprite;
    float speed;
    bool active;
    EnemyType type;
    int damage;

    Enemy(float x, float y, EnemyType enemyType, sf::Texture& texture, std::mt19937& rng) {
        type = enemyType;
        active = true;

        sprite.setTexture(texture);
        sprite.setPosition(x, y);
        sprite.setScale(0.25f, 0.25f); // Increased for visibility

        if (type == ENEMY_TYPE_1) {
            std::uniform_real_distribution<float> speedDist(80.0f, 120.0f);
            speed = speedDist(rng);
            damage = 15;
        }
        else {
            std::uniform_real_distribution<float> speedDist(120.0f, 180.0f);
            speed = speedDist(rng);
            damage = 30;
        }
    }

    void update(float deltaTime, sf::Vector2f playerPos) {
        if (active) {
            sf::Vector2f enemyCenter = getCenter();
            sf::Vector2f direction = playerPos - enemyCenter;
            float length = sqrt(direction.x * direction.x + direction.y * direction.y);
            if (length > 0) {
                direction /= length;
                sprite.move(direction * speed * deltaTime);
            }
        }
    }

    sf::Vector2f getCenter() {
        sf::FloatRect bounds = sprite.getGlobalBounds();
        return sf::Vector2f(bounds.left + bounds.width / 2, bounds.top + bounds.height / 2);
    }

    sf::FloatRect getBounds() {
        return sprite.getGlobalBounds();
    }
};

// Powerup class
class Powerup {
public:
    sf::Sprite sprite;
    PowerupType type;
    bool active;
    float lifetime;

    Powerup(float x, float y, PowerupType powerupType, sf::Texture& texture) {
        type = powerupType;
        active = true;
        lifetime = 10.0f;

        sprite.setTexture(texture);
        sprite.setPosition(x, y);
        sprite.setScale(0.1f, 0.1f); // Increased for visibility
    }

    void update(float deltaTime) {
        lifetime -= deltaTime;
        if (lifetime <= 0) {
            active = false;
        }
        float pulse = sin(lifetime * 5) * 0.02f + 1.0f;
        sprite.setScale(0.12f * pulse, 0.12f * pulse);
    }

    sf::FloatRect getBounds() {
        return sprite.getGlobalBounds();
    }
};

// Button class
class Button {
public:
    sf::RectangleShape shape;
    sf::Text text;
    bool isHovered;

    Button(float x, float y, float width, float height, const std::string& buttonText, sf::Font& font) {
        shape.setSize(sf::Vector2f(width, height));
        shape.setPosition(x, y);
        shape.setFillColor(sf::Color(50, 50, 50, 200));
        shape.setOutlineThickness(3);
        shape.setOutlineColor(sf::Color::White);

        text.setFont(font);
        text.setString(buttonText);
        text.setCharacterSize(36);
        text.setFillColor(sf::Color::White);

        sf::FloatRect textBounds = text.getLocalBounds();
        text.setPosition(x + (width - textBounds.width) / 2, y + (height - textBounds.height) / 2 - 5);
        isHovered = false;
    }

    void update(sf::Vector2i mousePos) {
        sf::FloatRect bounds = shape.getGlobalBounds();
        isHovered = bounds.contains(static_cast<sf::Vector2f>(mousePos));
        shape.setFillColor(isHovered ? sf::Color(100, 100, 100, 200) : sf::Color(50, 50, 50, 200));
    }

    bool isClicked(sf::Vector2i mousePos, const sf::Event& event) {
        return isHovered && event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Left;
    }

    void draw(sf::RenderWindow& window) {
        window.draw(shape);
        window.draw(text);
    }
};

// Utility functions
float distance(sf::Vector2f a, sf::Vector2f b) {
    return sqrt((a.x - b.x) * (a.x - b.x) + (a.y - b.y) * (a.y - b.y));
}

sf::Vector2f normalize(sf::Vector2f vector) {
    float length = sqrt(vector.x * vector.x + vector.y * vector.y);
    if (length > 0) {
        return sf::Vector2f(vector.x / length, vector.y / length);
    }
    return sf::Vector2f(0, 0);
}

bool checkCollision(const sf::FloatRect& a, const sf::FloatRect& b) {
    return a.intersects(b);
}

int main() {
    std::random_device rd;
    std::mt19937 rng(rd());

    sf::RenderWindow window(sf::VideoMode(1600, 900), "Hunt the Zombies");
    window.setFramerateLimit(60);

    GameState currentState = MAIN_MENU;

    sf::Font font;
    if (!font.loadFromFile("Montserrat-Bold.ttf")) {
        std::cout << "Warning: Could not load Montserrat-Bold.ttf, using default font" << std::endl;
    }

    TextureManager textures;
    sf::Sprite backgroundSprite;
    backgroundSprite.setTexture(textures.backgroundTexture);
    backgroundSprite.setScale(1600.0f / textures.backgroundTexture.getSize().x, 900.0f / textures.backgroundTexture.getSize().y);

    sf::SoundBuffer bulletSoundBuffer, hitSoundBuffer;
    sf::Sound bulletSound, hitSound;
    bool bulletSoundLoaded = bulletSoundBuffer.loadFromFile("bullet.ogg");
    if (!bulletSoundLoaded) {
        std::cout << "Warning: Could not load bullet.ogg" << std::endl;
    }
    else {
        bulletSound.setBuffer(bulletSoundBuffer);
    }

    bool hitSoundLoaded = hitSoundBuffer.loadFromFile("hit.ogg");
    if (!hitSoundLoaded) {
        std::cout << "Warning: Could not load hit.ogg" << std::endl;
    }
    else {
        hitSound.setBuffer(hitSoundBuffer);
    }

    sf::Music backgroundMusic;
    if (!backgroundMusic.openFromFile("bgmusic.ogg")) {
        std::cout << "Warning: Could not load bgmusic.ogg" << std::endl;
    }
    else {
        backgroundMusic.setLoop(true);
        backgroundMusic.setVolume(50);
        backgroundMusic.play();
    }

    sf::Text titleText;
    titleText.setFont(font);
    titleText.setString("HUNT THE ZOMBIES");
    titleText.setCharacterSize(96);
    titleText.setFillColor(sf::Color::Red);
    titleText.setStyle(sf::Text::Bold);
    sf::FloatRect titleBounds = titleText.getLocalBounds();
    titleText.setPosition((1600 - titleBounds.width) / 2, 200);

    Button classicModeButton(600, 350, 400, 80, "CLASSIC MODE", font);
    Button timeTrialButton(600, 450, 400, 80, "TIME TRIAL", font);
    Button exitButton(600, 550, 400, 80, "EXIT", font);

    Player player(800, 450, textures.playerTexture);
    std::vector<Bullet> bullets;
    std::vector<Enemy> enemies;
    std::vector<Powerup> powerups;
    enemies.reserve(50); // Reserve space to prevent reallocations
    powerups.reserve(10);

    int totalEnemiesClassic = 30;
    int enemiesKilled = 0;
    float enemySpawnTimer = 0;
    float enemySpawnDelay = 1.5f;
    float powerupSpawnTimer = 0;
    float powerupSpawnDelay = 7.0f;

    float timeTrialDuration = 60.0f;
    float timeTrialTimer = timeTrialDuration;
    int timeTrialKills = 0;
    int xpEarned = 0;

    sf::RectangleShape healthBarBg(sf::Vector2f(300, 30));
    healthBarBg.setPosition(20, 20);
    healthBarBg.setFillColor(sf::Color::Red);

    sf::RectangleShape healthBar(sf::Vector2f(300, 30));
    healthBar.setPosition(20, 20);
    healthBar.setFillColor(sf::Color::Green);

    sf::Text killCounterText;
    killCounterText.setFont(font);
    killCounterText.setCharacterSize(28);
    killCounterText.setFillColor(sf::Color::White);
    killCounterText.setPosition(20, 60);

    sf::Text timerText;
    timerText.setFont(font);
    timerText.setCharacterSize(32);
    timerText.setFillColor(sf::Color::Yellow);
    timerText.setPosition(20, 100);

    sf::Text speedBoostText;
    speedBoostText.setFont(font);
    speedBoostText.setCharacterSize(24);
    speedBoostText.setFillColor(sf::Color::Cyan);
    speedBoostText.setPosition(20, 140);

    sf::Text gameOverText;
    gameOverText.setFont(font);
    gameOverText.setCharacterSize(96);
    gameOverText.setFillColor(sf::Color::Red);
    gameOverText.setString("YOU LOSE!");
    sf::FloatRect gameOverBounds = gameOverText.getLocalBounds();
    gameOverText.setPosition((1600 - gameOverBounds.width) / 2, 350);

    sf::Text victoryText;
    victoryText.setFont(font);
    victoryText.setCharacterSize(96);
    victoryText.setFillColor(sf::Color::Green);
    victoryText.setString("VICTORY!");
    sf::FloatRect victoryBounds = victoryText.getLocalBounds();
    victoryText.setPosition((1600 - victoryBounds.width) / 2, 350);

    sf::Text restartText;
    restartText.setFont(font);
    restartText.setCharacterSize(32);
    restartText.setFillColor(sf::Color::White);
    restartText.setString("Press SPACE to return to menu");
    sf::FloatRect restartBounds = restartText.getLocalBounds();
    restartText.setPosition((1600 - restartBounds.width) / 2, 450);

    sf::Text timeTrialResultsText;
    timeTrialResultsText.setFont(font);
    timeTrialResultsText.setCharacterSize(48);
    timeTrialResultsText.setFillColor(sf::Color::White);
    timeTrialResultsText.setPosition(400, 300);

    sf::Clock clock;

    while (window.isOpen()) {
        float deltaTime = clock.restart().asSeconds();
        sf::Event event;

        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                window.close();
            }

            if (currentState == MAIN_MENU) {
                sf::Vector2i mousePos = sf::Mouse::getPosition(window);
                if (classicModeButton.isClicked(mousePos, event)) {
                    currentState = PLAYING_CLASSIC;
                    player.reset(800, 450);
                    bullets.clear();
                    enemies.clear();
                    powerups.clear();
                    enemiesKilled = 0;
                    enemySpawnTimer = 0;
                    powerupSpawnTimer = 0;
                }
                else if (timeTrialButton.isClicked(mousePos, event)) {
                    currentState = PLAYING_TIME_TRIAL;
                    player.reset(800, 450);
                    bullets.clear();
                    enemies.clear();
                    powerups.clear();
                    timeTrialTimer = timeTrialDuration;
                    timeTrialKills = 0;
                    xpEarned = 0;
                    enemySpawnTimer = 0;
                    powerupSpawnTimer = 0;
                }
                else if (exitButton.isClicked(mousePos, event)) {
                    window.close();
                }
            }

            if (currentState == PLAYING_CLASSIC || currentState == PLAYING_TIME_TRIAL) {
                if (event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Left) {
                    sf::Vector2i mousePos = sf::Mouse::getPosition(window);
                    sf::Vector2f playerCenter = player.getCenter();
                    sf::Vector2f mouseWorldPos = static_cast<sf::Vector2f>(mousePos);
                    sf::Vector2f direction = normalize(mouseWorldPos - playerCenter);
                    bullets.push_back(Bullet(playerCenter.x, playerCenter.y, direction));
                    if (bulletSoundLoaded) bulletSound.play();
                }
            }

            if (currentState == GAME_OVER || currentState == VICTORY || currentState == TIME_TRIAL_RESULTS) {
                if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Space) {
                    currentState = MAIN_MENU;
                }
            }
        }

        if (currentState == MAIN_MENU) {
            sf::Vector2i mousePos = sf::Mouse::getPosition(window);
            classicModeButton.update(mousePos);
            timeTrialButton.update(mousePos);
            exitButton.update(mousePos);
        }

        if (currentState == PLAYING_CLASSIC || currentState == PLAYING_TIME_TRIAL) {
            sf::Vector2f movement(0, 0);
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::W)) movement.y -= 1;
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::S)) movement.y += 1;
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::A)) movement.x -= 1;
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::D)) movement.x += 1;

            if (movement.x != 0 || movement.y != 0) {
                movement = normalize(movement);
            }

            player.sprite.move(movement * player.speed * deltaTime);
            player.update(deltaTime);

            sf::FloatRect playerBounds = player.getBounds();
            sf::Vector2f playerPos = player.sprite.getPosition();
            playerPos.x = std::max(0.0f, std::min(playerPos.x, 1600.0f - playerBounds.width));
            playerPos.y = std::max(0.0f, std::min(playerPos.y, 900.0f - playerBounds.height));
            player.sprite.setPosition(playerPos);

            sf::Vector2i mousePos = sf::Mouse::getPosition(window);
            sf::Vector2f mouseWorldPos = static_cast<sf::Vector2f>(mousePos);
            player.rotateTowards(mouseWorldPos);

            for (auto& bullet : bullets) {
                bullet.update(deltaTime);
            }
            bullets.erase(std::remove_if(bullets.begin(), bullets.end(),
                [](const Bullet& b) { return !b.active; }), bullets.end());

            enemySpawnTimer += deltaTime;
            bool shouldSpawnEnemy = false;
            if (currentState == PLAYING_CLASSIC) {
                shouldSpawnEnemy = (enemySpawnTimer >= enemySpawnDelay && enemies.size() + enemiesKilled < totalEnemiesClassic);
            }
            else {
                shouldSpawnEnemy = (enemySpawnTimer >= enemySpawnDelay);
            }

            if (shouldSpawnEnemy) {
                enemySpawnTimer = 0;
                std::uniform_int_distribution<int> edgeDist(0, 3);
                std::uniform_real_distribution<float> posDist(0, 1600);
                std::uniform_real_distribution<float> posYDist(0, 900);
                float x, y;
                switch (edgeDist(rng)) {
                case 0: x = posDist(rng); y = 0; break; // Top
                case 1: x = 1600; y = posYDist(rng); break; // Right
                case 2: x = posDist(rng); y = 900; break; // Bottom
                case 3: x = 0; y = posYDist(rng); break; // Left
                }
                std::uniform_int_distribution<int> typeDist(0, 99);
                EnemyType enemyType = (typeDist(rng) < 60) ? ENEMY_TYPE_1 : ENEMY_TYPE_2;
                sf::Texture& enemyTexture = (enemyType == ENEMY_TYPE_1) ? textures.enemy1Texture : textures.enemy2Texture;
                enemies.emplace_back(x, y, enemyType, enemyTexture, rng);
            }

            powerupSpawnTimer += deltaTime;
            if (powerupSpawnTimer >= powerupSpawnDelay) {
                powerupSpawnTimer = 0;
                std::uniform_real_distribution<float> xDist(100, 1500);
                std::uniform_real_distribution<float> yDist(100, 800);
                PowerupType powerupType = (std::uniform_int_distribution<int>(0, 1)(rng) == 0) ? HEALTH_BOOST : SPEED_BOOST;
                sf::Texture& powerupTexture = (powerupType == HEALTH_BOOST) ? textures.healthTexture : textures.speedTexture;
                powerups.emplace_back(xDist(rng), yDist(rng), powerupType, powerupTexture);
            }

            sf::Vector2f playerCenter = player.getCenter();
            for (auto& enemy : enemies) {
                enemy.update(deltaTime, playerCenter);
                if (checkCollision(enemy.getBounds(), player.getBounds()) && enemy.active) {
                    player.takeDamage(enemy.damage);
                    enemy.active = false;
                }
            }

            for (auto& powerup : powerups) {
                powerup.update(deltaTime);
                if (checkCollision(powerup.getBounds(), player.getBounds()) && powerup.active) {
                    if (powerup.type == HEALTH_BOOST && player.health < player.maxHealth) {
                        player.heal(20);
                    }
                    else if (powerup.type == SPEED_BOOST) {
                        player.applySpeedBoost();
                    }
                    powerup.active = false;
                }
            }

            for (auto& bullet : bullets) {
                for (auto& enemy : enemies) {
                    if (bullet.active && enemy.active && checkCollision(bullet.getBounds(), enemy.getBounds())) {
                        bullet.active = false;
                        enemy.active = false;
                        if (hitSoundLoaded) hitSound.play();
                        if (currentState == PLAYING_CLASSIC) {
                            enemiesKilled++;
                        }
                        else {
                            timeTrialKills++;
                        }
                    }
                }
            }

            enemies.erase(std::remove_if(enemies.begin(), enemies.end(),
                [](const Enemy& e) { return !e.active; }), enemies.end());
            powerups.erase(std::remove_if(powerups.begin(), powerups.end(),
                [](const Powerup& p) { return !p.active; }), powerups.end());

            healthBar.setSize(sf::Vector2f(300 * (static_cast<float>(player.health) / player.maxHealth), 30));

            if (currentState == PLAYING_TIME_TRIAL) {
                timeTrialTimer -= deltaTime;
                std::ostringstream ss;
                ss << "Time: " << std::fixed << std::setprecision(1) << timeTrialTimer;
                timerText.setString(ss.str());
                if (timeTrialTimer <= 0) {
                    xpEarned = timeTrialKills * 10 + static_cast<int>(timeTrialDuration * 5);
                    std::ostringstream resultss;
                    resultss << "TIME'S UP!\n\nKills: " << timeTrialKills << "\nXP Earned: " << xpEarned;
                    timeTrialResultsText.setString(resultss.str());
                    currentState = TIME_TRIAL_RESULTS;
                }
            }

            killCounterText.setString(currentState == PLAYING_CLASSIC ?
                "Kills: " + std::to_string(enemiesKilled) + "/" + std::to_string(totalEnemiesClassic) :
                "Kills: " + std::to_string(timeTrialKills));

            speedBoostText.setString(player.hasSpeedBoost ?
                "Speed Boost: " + (std::ostringstream() << std::fixed << std::setprecision(1) << player.speedBoostTimer << "s").str() :
                "");

            if (player.health <= 0) {
                currentState = GAME_OVER;
            }
            else if (currentState == PLAYING_CLASSIC && enemiesKilled >= totalEnemiesClassic) {
                currentState = VICTORY;
            }
        }

        window.clear();
        window.draw(backgroundSprite);

        if (currentState == MAIN_MENU) {
            window.draw(titleText);
            classicModeButton.draw(window);
            timeTrialButton.draw(window);
            exitButton.draw(window);
        }
        else if (currentState == PLAYING_CLASSIC || currentState == PLAYING_TIME_TRIAL) {
            int activeEnemies = 0, activePowerups = 0, activeBullets = 0;
            window.draw(player.sprite);
            std::cout << "Drawing player at (" << player.sprite.getPosition().x << ", " << player.sprite.getPosition().y << ")\n";
            for (const auto& bullet : bullets) {
                if (bullet.active) {
                    window.draw(bullet.shape);
                    activeBullets++;
                }
            }
            for (const auto& enemy : enemies) {
                if (enemy.active) {
                    window.draw(enemy.sprite);
                    activeEnemies++;
                    std::cout << "Drawing enemy at (" << enemy.sprite.getPosition().x << ", " << enemy.sprite.getPosition().y << ")\n";
                }
            }
            for (const auto& powerup : powerups) {
                if (powerup.active) {
                    window.draw(powerup.sprite);
                    activePowerups++;
                    std::cout << "Drawing powerup at (" << powerup.sprite.getPosition().x << ", " << powerup.sprite.getPosition().y << ")\n";
                }
            }
            std::cout << "Frame: Enemies=" << activeEnemies << ", Powerups=" << activePowerups << ", Bullets=" << activeBullets << "\n";

            window.draw(healthBarBg);
            window.draw(healthBar);
            window.draw(killCounterText);
            if (currentState == PLAYING_TIME_TRIAL) {
                window.draw(timerText);
            }
            if (!speedBoostText.getString().isEmpty()) {
                window.draw(speedBoostText);
            }
        }
        else if (currentState == GAME_OVER) {
            window.draw(gameOverText);
            window.draw(restartText);
        }
        else if (currentState == VICTORY) {
            window.draw(victoryText);
            window.draw(restartText);
        }
        else if (currentState == TIME_TRIAL_RESULTS) {
            window.draw(timeTrialResultsText);
            window.draw(restartText);
        }

        window.display();
    }

    return 0;
}