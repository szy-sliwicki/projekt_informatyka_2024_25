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

// Struktura przechowująca dane o przeciwnikach
struct Enemy {
    sf::CircleShape shape;
    sf::Vector2f velocity;
};

// Klasa gry
class SpaceInvaders {
private:
    sf::RenderWindow window;
    sf::RectangleShape player;
    std::vector<Enemy> enemies;
    sf::Font font;
    sf::Text infoText;
    sf::Text helpText;
    sf::Text menuText;

    bool isPaused = false;
    bool showHelp = false;
    int score = 0;
    int level = 1;

    sf::Clock levelClock;
    const sf::Time levelDuration = sf::seconds(30.0f);

    void initWindow() {
        window.create(sf::VideoMode(800, 600), "Space Invaders", sf::Style::Close);
        window.setFramerateLimit(60);
    }

    void initPlayer() {
        player.setSize(sf::Vector2f(50.0f, 50.0f));
        player.setFillColor(sf::Color::Green);
        player.setPosition(375.0f, 500.0f);
    }

    void initFont() {
        if (!font.loadFromFile("arial.ttf")) {
            throw std::runtime_error("Could not load font.");
        }

        infoText.setFont(font);
        infoText.setCharacterSize(20);
        infoText.setPosition(10.0f, 10.0f);

        helpText.setFont(font);
        helpText.setCharacterSize(24);
        helpText.setPosition(100.0f, 100.0f);
        helpText.setFillColor(sf::Color::White);
        helpText.setString("Help:\n- Use Arrow keys to move\n- Avoid enemies\n- Press F1 for help\n- Press ESC to quit\n- Enjoy!");

        menuText.setFont(font);
        menuText.setCharacterSize(24);
        menuText.setFillColor(sf::Color::Yellow);
        menuText.setString("Are you sure you want to exit? Y/N");
        menuText.setPosition(200.0f, 300.0f);
    }

    void spawnEnemy() {
        Enemy enemy;
        enemy.shape.setRadius(20.0f);
        enemy.shape.setFillColor(sf::Color::Red);
        enemy.shape.setPosition(static_cast<float>(rand() % 750), -20.0f);
        enemy.velocity = sf::Vector2f(0.0f, 1.0f + level * 0.5f);
        enemies.push_back(enemy);
    }

    void updateEnemies() {
        for (auto& enemy : enemies) {
            enemy.shape.move(enemy.velocity);
        }

        // Usuń przeciwników poza ekranem
        enemies.erase(std::remove_if(enemies.begin(), enemies.end(), [](Enemy& e) {
            return e.shape.getPosition().y > 600;
            }), enemies.end());
    }

    void checkCollisions() {
        for (const auto& enemy : enemies) {
            if (player.getGlobalBounds().intersects(enemy.shape.getGlobalBounds())) {
                isPaused = true;
                infoText.setString("Game Over! Press R to restart.");
            }
        }
    }

    void saveState() {
        std::ofstream file("save.txt");
        if (file.is_open()) {
            file << score << " " << level << "\n";
            for (const auto& enemy : enemies) {
                file << enemy.shape.getPosition().x << " " << enemy.shape.getPosition().y << "\n";
            }
            file.close();
        }
    }

    void loadState() {
        std::ifstream file("save.txt");
        if (file.is_open()) {
            file >> score >> level;
            enemies.clear();
            float x, y;
            while (file >> x >> y) {
                Enemy enemy;
                enemy.shape.setRadius(20.0f);
                enemy.shape.setFillColor(sf::Color::Red);
                enemy.shape.setPosition(x, y);
                enemy.velocity = sf::Vector2f(0.0f, 1.0f + level * 0.5f);
                enemies.push_back(enemy);
            }
            file.close();
        }
    }

    void handleInput() {
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left) && player.getPosition().x > 0) {
            player.move(-5.0f, 0.0f);
        }
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right) && player.getPosition().x < 750) {
            player.move(5.0f, 0.0f);
        }
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Escape)) {
            isPaused = true;
        }
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::F1)) {
            showHelp = !showHelp;
        }
    }

    void updateLevel() {
        if (levelClock.getElapsedTime() >= levelDuration) {
            levelClock.restart();
            level++;
            score++;
        }
    }

    void render() {
        window.clear();

        if (isPaused) {
            window.draw(menuText);
        }
        else if (showHelp) {
            window.draw(helpText);
        }
        else {
            window.draw(player);
            for (const auto& enemy : enemies) {
                window.draw(enemy.shape);
            }

            infoText.setString("Score: " + std::to_string(score) + " Level: " + std::to_string(level));
            window.draw(infoText);
        }

        window.display();
    }

public:
    SpaceInvaders() {
        initWindow();
        initPlayer();
        initFont();
    }

    void run() {
        sf::Clock spawnClock;
        sf::Time spawnTimer = sf::seconds(1.0f);
        levelClock.restart();

        while (window.isOpen()) {
            sf::Event event;
            while (window.pollEvent(event)) {
                if (event.type == sf::Event::Closed) {
                    window.close();
                }

                if (isPaused && event.type == sf::Event::KeyPressed) {
                    if (event.key.code == sf::Keyboard::Y) {
                        saveState();
                        window.close();
                    }
                    else if (event.key.code == sf::Keyboard::N) {
                        isPaused = false;
                    }
                }

                if (showHelp && event.type == sf::Event::KeyPressed) {
                    if (event.key.code == sf::Keyboard::F1) {
                        showHelp = false;
                    }
                }
            }

            if (!isPaused && !showHelp) {
                handleInput();

                if (spawnClock.getElapsedTime() > spawnTimer) {
                    spawnEnemy();
                    spawnClock.restart();
                }

                updateEnemies();
                checkCollisions();
                updateLevel();
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
        std::cerr << "Error: " << e.what() << std::endl;
        return -1;
    }

    return 0;
}
