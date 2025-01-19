#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include <SFML/System.hpp>
#include <SFML/Audio.hpp>
#include <vector>
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <memory>
#include <iomanip>
#include <cstdlib>

// Struktura pocisku
struct Bullet {
    sf::RectangleShape shape;
    float speed;
};

// Struktura przeciwnika
struct Enemy {
    sf::Sprite sprite;
    sf::Vector2f velocity;
    int health;
};

// Struktura wybuchu
struct Explosion {
    sf::Sprite sprite;
    sf::Clock clock;
};

// Klasa gry
class SpaceInvaders {
private:
    sf::RenderWindow window;
    sf::Sprite player;
    sf::Texture playerTexture;
    sf::Texture enemyTexture1;
    sf::Texture enemyTexture2;
    sf::Texture explosionTexture;
    sf::Texture backgroundTexture;
    sf::Sprite background;
    std::vector<Enemy> enemies;
    std::vector<Bullet> bullets;
    std::vector<Explosion> explosions;
    sf::Font font;
    sf::Text infoText;
    sf::Text helpText;
    sf::Text menuText;
    sf::Text countdownText;
    sf::Text levelTimerText;
    sf::Text levelText;
    sf::RectangleShape helpBackground;

    // Ekran startowy
    bool showStartScreen = true;
    sf::Text titleText;
    sf::Text startText;
    sf::Text authorText;

    bool isPaused = false;
    bool showHelp = false;
    bool levelTransition = false;
    bool gameOver = false;
    bool isLevelComplete = false;
    sf::Text levelCompleteText;
    sf::Text nextLevelText;

    int score = 0;
    int level = 1;
    float enemySpawnInterval = 1.0f;
    int maxEnemiesOnScreen = 50;
    float enemySpeed = 1.0f;
    float bulletSpeed = 7.0f;

    sf::Clock levelClock;
    sf::Clock transitionClock;
    sf::Clock bulletClock;
    sf::Clock levelCompleteClock;
    float levelDuration = 20.0f;
    float bulletCooldown = 0.5f;

    void centerText(sf::Text& text, float yPosition) {
        sf::FloatRect bounds = text.getGlobalBounds();
        text.setPosition(
            (window.getSize().x - bounds.width) / 2.0f,
            yPosition
        );
    }

    void initWindow() {
        window.create(sf::VideoMode(800, 600), "Space Invaders", sf::Style::Close);
        window.setFramerateLimit(60);
    }

    void initPlayer() {
        if (!playerTexture.loadFromFile("SPACESHIP.png")) {
            throw std::runtime_error("Could not load player texture.");
        }
        player.setTexture(playerTexture);
        player.setScale(0.2f, 0.2f);
        player.setPosition(375.0f, 500.0f);
    }

    void initEnemies() {
        if (!enemyTexture1.loadFromFile("ENEMY_1.png") || !enemyTexture2.loadFromFile("ENEMY_2.png") || !explosionTexture.loadFromFile("EXPLOSION.png")) {
            throw std::runtime_error("Could not load enemy or explosion textures.");
        }
    }

    void initBackground() {
        if (!backgroundTexture.loadFromFile("SPACE.jpg")) {
            throw std::runtime_error("Could not load background texture.");
        }
        background.setTexture(backgroundTexture);
        background.setScale(
            window.getSize().x / background.getLocalBounds().width,
            window.getSize().y / background.getLocalBounds().height
        );
    }

    void initStartScreen() {
        titleText.setFont(font);
        titleText.setString("SPACE INVADERS");
        titleText.setCharacterSize(80);
        titleText.setFillColor(sf::Color::Green);
        centerText(titleText, 100.0f);

        startText.setFont(font);
        startText.setString("Nacisnij ENTER aby rozpoczac\nNacisnij L aby wczytac zapisana gre");
        startText.setCharacterSize(30);
        startText.setFillColor(sf::Color::White);
        centerText(startText, 300.0f);

        authorText.setFont(font);
        authorText.setString("Sterowanie:\nStrzalki - ruch\nSpacja - strzal\nS - zapis gry\nF1 - pomoc\nM - powrot do menu");
        authorText.setCharacterSize(28);
        authorText.setFillColor(sf::Color::Yellow);
        centerText(authorText, 400.0f);
    }

    void initFont() {
        if (!font.loadFromFile("arial.ttf")) {
            throw std::runtime_error("Could not load font.");
        }

        infoText.setFont(font);
        infoText.setCharacterSize(30);
        infoText.setFillColor(sf::Color::White);

        helpText.setFont(font);
        helpText.setCharacterSize(28);
        helpText.setFillColor(sf::Color::White);
        helpText.setString(
            "=== SPACE INVADERS - POMOC ===\n\n"
            "Sterowanie:\n"
            "- Strzalki lewo/prawo - ruch statku\n"
            "- Spacja - strzal\n"
            "- S - zapisz gre\n"
            "- F1 - pokazuje/ukrywa pomoc\n"
            "- ESC - pauza/wyjscie\n\n"
            "Poziomy:\n"
            "- Kazdy poziom trwa 20 sekund\n"
            "- Z kazdym poziomem przeciwnicy sa szybsi\n"
            "- Zbieraj punkty za trafienie przeciwnikow\n\n"
            "Nacisnij F1, aby wrocic do gry"
        );
        centerText(helpText, 50.0f);

        helpBackground.setSize(sf::Vector2f(800.0f, 600.0f));
        helpBackground.setFillColor(sf::Color(0, 0, 50, 230));

        menuText.setFont(font);
        menuText.setCharacterSize(30);
        menuText.setFillColor(sf::Color::Yellow);
        menuText.setString("Czy na pewno chcesz wyjsc? (T/N)");
        centerText(menuText, 300.0f);

        levelCompleteText.setFont(font);
        levelCompleteText.setCharacterSize(48);
        levelCompleteText.setFillColor(sf::Color::Green);
        levelCompleteText.setString("");
        centerText(levelCompleteText, 250.0f);

        nextLevelText.setFont(font);
        nextLevelText.setCharacterSize(36);
        nextLevelText.setFillColor(sf::Color::Yellow);
        centerText(nextLevelText, 350.0f);

        levelTimerText.setFont(font);
        levelTimerText.setCharacterSize(30);
        levelTimerText.setFillColor(sf::Color::White);
        levelTimerText.setPosition(600.0f, 10.0f);

        levelText.setFont(font);
        levelText.setCharacterSize(30);
        levelText.setFillColor(sf::Color::White);
        levelText.setPosition(10.0f, 10.0f);
    }

    void saveGame() {
        std::ofstream file("savegame.txt");
        if (file.is_open()) {
            file << score << " " << level << "\n";
            file << player.getPosition().x << " " << player.getPosition().y << "\n";
            file << enemies.size() << "\n";
            for (const auto& enemy : enemies) {
                file << enemy.sprite.getPosition().x << " "
                    << enemy.sprite.getPosition().y << " "
                    << enemy.velocity.x << " "
                    << enemy.velocity.y << " "
                    << enemy.health << "\n";
            }
            file.close();

            sf::Text saveText;
            saveText.setFont(font);
            saveText.setString("Gra zostala zapisana!");
            saveText.setCharacterSize(24);
            saveText.setFillColor(sf::Color::Green);
            centerText(saveText, 200.0f);

            window.draw(saveText);
            window.display();
            sf::sleep(sf::seconds(1.0f));
        }
    }

    void loadGame() {
        std::ifstream file("savegame.txt");
        if (file.is_open()) {
            file >> score >> level;
            float px, py;
            file >> px >> py;
            player.setPosition(px, py);

            int enemyCount;
            file >> enemyCount;
            enemies.clear();
            for (int i = 0; i < enemyCount; i++) {
                Enemy enemy;
                float ex, ey, vx, vy;
                int h;
                file >> ex >> ey >> vx >> vy >> h;
                enemy.sprite.setTexture((i % 2 == 0) ? enemyTexture1 : enemyTexture2);
                enemy.sprite.setScale(0.05f, 0.05f);
                enemy.sprite.setPosition(ex, ey);
                enemy.velocity = sf::Vector2f(vx, vy);
                enemy.health = h;
                enemies.push_back(enemy);
            }
            file.close();
        }
    }

    void shoot() {
        if (bulletClock.getElapsedTime().asSeconds() >= bulletCooldown) {
            Bullet bullet;
            bullet.shape.setSize(sf::Vector2f(5.0f, 15.0f));
            bullet.shape.setFillColor(sf::Color::Yellow);
            bullet.shape.setPosition(
                player.getPosition().x + player.getGlobalBounds().width / 2 - 2.5f,
                player.getPosition().y
            );
            bullet.speed = bulletSpeed;
            bullets.push_back(bullet);
            bulletClock.restart();
        }
    }

    void updateBullets() {
        for (size_t i = 0; i < bullets.size(); i++) {
            bullets[i].shape.move(0, -bullets[i].speed);

            for (size_t j = 0; j < enemies.size(); j++) {
                if (bullets[i].shape.getGlobalBounds().intersects(
                    enemies[j].sprite.getGlobalBounds())) {
                    createExplosion(enemies[j].sprite.getPosition().x, enemies[j].sprite.getPosition().y);
                    enemies.erase(enemies.begin() + j);
                    bullets.erase(bullets.begin() + i);
                    score += 10;
                    return;
                }
            }

            if (bullets[i].shape.getPosition().y < 0) {
                bullets.erase(bullets.begin() + i);
                break;
            }
        }
    }

    void spawnEnemy() {
        if (enemies.size() >= maxEnemiesOnScreen) {
            return;
        }
        Enemy enemy;
        enemy.sprite.setTexture((rand() % 2 == 0) ? enemyTexture1 : enemyTexture2);
        enemy.sprite.setScale(0.05f, 0.05f);
        enemy.sprite.setPosition(
            static_cast<float>(rand() % 750),
            -40.0f
        );
        enemy.velocity = sf::Vector2f(
            (rand() % 3 - 1) * (0.5f + level * 0.1f),
            enemySpeed + level * 0.2f
        );
        enemy.health = 1 + level / 3;
        enemies.push_back(enemy);
    }

    void updateEnemies() {
        for (size_t i = 0; i < enemies.size(); i++) {
            enemies[i].sprite.move(enemies[i].velocity);

            if (enemies[i].sprite.getPosition().x <= 0 ||
                enemies[i].sprite.getPosition().x >= 780) {
                enemies[i].velocity.x = -enemies[i].velocity.x;
            }

            if (enemies[i].sprite.getPosition().y > 600) {
                enemies.erase(enemies.begin() + i);
                break;
            }
        }
    }

    void checkCollisions() {
        for (const auto& enemy : enemies) {
            if (player.getGlobalBounds().intersects(
                enemy.sprite.getGlobalBounds())) {
                isPaused = true;
                gameOver = true;
                infoText.setString(
                    "Koniec gry! Punkty: " + std::to_string(score) +
                    "\nChcesz zagrac ponownie? (T/N)"
                );
                centerText(infoText, 300.0f);
            }
        }
    }

    void updateLevelTimer() {
        float timeLeft = levelDuration - levelClock.getElapsedTime().asSeconds();

        if (!isLevelComplete) {
            levelTimerText.setString("Czas: " + std::to_string(static_cast<int>(timeLeft)) + "s");
            levelTimerText.setPosition(
                window.getSize().x - levelTimerText.getGlobalBounds().width - 20.0f,
                10.0f
            );

            if (timeLeft <= 0) {
                isLevelComplete = true;
                levelCompleteClock.restart();
                levelCompleteText.setString("POZIOM " + std::to_string(level) + " UKONCZONY!");
                centerText(levelCompleteText, 250.0f);
                enemies.clear();
                bullets.clear();
            }
        }
        else {
            float transitionTimeLeft = 3.0f - levelCompleteClock.getElapsedTime().asSeconds();

            if (transitionTimeLeft > 0) {
                nextLevelText.setString("Nastepny poziom za: " + std::to_string(static_cast<int>(transitionTimeLeft + 1)));
                centerText(nextLevelText, 350.0f);
            }
            else {
                isLevelComplete = false;
                level++;
                levelClock.restart();
                enemySpeed += 0.2f;
                bullets.clear();
                enemies.clear();
            }
        }
    }

    void handleInput() {
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left) &&
            player.getPosition().x > 0) {
            player.move(-5.0f, 0.0f);
        }
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right) &&
            player.getPosition().x < 750) {
            player.move(5.0f, 0.0f);
        }

        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Space)) {
            shoot();
        }
    }

    void createExplosion(float x, float y) {
        Explosion explosion;
        explosion.sprite.setTexture(explosionTexture);
        explosion.sprite.setPosition(x, y);
        explosion.sprite.setScale(0.1f, 0.1f);
        explosions.push_back(explosion);
    }

    void updateExplosions() {
        for (size_t i = 0; i < explosions.size(); i++) {
            if (explosions[i].clock.getElapsedTime().asSeconds() > 0.2f) {
                explosions.erase(explosions.begin() + i);
                i--;
            }
        }
    }

    void renderExplosions() {
        for (const auto& explosion : explosions) {
            window.draw(explosion.sprite);
        }
    }

    void render() {
        window.clear(sf::Color(20, 20, 50));

        window.draw(background);

        if (showStartScreen) {
            window.draw(titleText);
            window.draw(startText);
            window.draw(authorText);
        }
        else if (isPaused) {
            if (gameOver) {
                window.draw(infoText);
            }
            else {
                window.draw(menuText);
            }
        }
        else if (showHelp) {
            window.draw(helpBackground);
            window.draw(helpText);
        }
        else if (isLevelComplete) {
            window.draw(levelCompleteText);
            window.draw(nextLevelText);
        }
        else {
            window.draw(player);

            for (const auto& enemy : enemies) {
                window.draw(enemy.sprite);
            }

            for (const auto& bullet : bullets) {
                window.draw(bullet.shape);
            }

            renderExplosions();

            infoText.setString("Punkty: " + std::to_string(score));
            centerText(infoText, 10.0f);
            window.draw(infoText);

            levelText.setString("Poziom: " + std::to_string(level));
            window.draw(levelText);

            window.draw(levelTimerText);
        }

        window.display();
    }

public:
    SpaceInvaders() {
        initWindow();
        initPlayer();
        initEnemies();
        initBackground();
        initFont();
        initStartScreen();
        srand(static_cast<unsigned>(time(nullptr)));
    }

    void restartGame() {
        score = 0;
        level = 1;
        enemies.clear();
        bullets.clear();
        explosions.clear();
        player.setPosition(375.0f, 500.0f);
        isPaused = false;
        gameOver = false;
        isLevelComplete = false;
        levelClock.restart();
        enemySpeed = 1.0f;
    }

    void run() {
        while (window.isOpen()) {
            sf::Event event;
            while (window.pollEvent(event)) {
                if (event.type == sf::Event::Closed) {
                    window.close();
                }

                if (event.type == sf::Event::KeyPressed) {
                    if (showStartScreen) {
                        if (event.key.code == sf::Keyboard::Return) {
                            showStartScreen = false;
                            levelClock.restart();
                        }
                        else if (event.key.code == sf::Keyboard::L) {
                            loadGame();
                            showStartScreen = false;
                        }
                    }
                    else if (event.key.code == sf::Keyboard::S && !showStartScreen && !isPaused) {
                        saveGame();
                    }
                    else if (event.key.code == sf::Keyboard::F1) {
                        showHelp = !showHelp;
                        if (!showHelp) {
                            isPaused = false;
                        }
                    }
                    else if (event.key.code == sf::Keyboard::M) {
                        showStartScreen = true;
                        isPaused = false;
                        gameOver = false;
                        isLevelComplete = false;
                        restartGame();
                    }
                    else if (isPaused) {
                        if (gameOver) {
                            if (event.key.code == sf::Keyboard::T) {
                                restartGame();
                            }
                            else if (event.key.code == sf::Keyboard::N) {
                                window.close();
                            }
                        }
                        else {
                            if (event.key.code == sf::Keyboard::T) {
                                window.close();
                            }
                            else if (event.key.code == sf::Keyboard::N) {
                                isPaused = false;
                            }
                        }
                    }
                    else if (event.key.code == sf::Keyboard::Escape) {
                        isPaused = true;
                    }
                }
            }

            if (!showStartScreen && !isPaused && !showHelp && !gameOver) {
                handleInput();
                updateBullets();
                updateEnemies();
                updateExplosions();
                checkCollisions();
                updateLevelTimer();

                if (transitionClock.getElapsedTime().asSeconds() >= enemySpawnInterval) {
                    spawnEnemy();
                    transitionClock.restart();
                }
            }

            render();
        }
    }
};

int main() {
    try {
        SpaceInvaders game;
        game.run();
    }
    catch (const std::exception& e) {
        std::cerr << "Błąd: " << e.what() << std::endl;
        return -1;
    }
    return 0;
}