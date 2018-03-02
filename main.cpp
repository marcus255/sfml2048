#include <SFML/Graphics.hpp>
#include <array>
#include <stdlib.h>
#include <time.h>

const int size   = 4;
const int b_size = 64;
const int g_size = size * b_size;

class Grid {
public:
	std::array<std::array<unsigned, size>, size> array;
	std::array<std::array<sf::RectangleShape, size>, size> squares;
	std::array<std::array<sf::Text, size>, size> numbers;
	sf::RenderWindow& window;
	sf::Font font;
	sf::RectangleShape bg;
	sf::Text scoreString;
	int score = 0;
	int filled = 0;
	bool moved = false;
	Grid(sf::RenderWindow& w, int b_size) : window { w } {
		bg = sf::RectangleShape(sf::Vector2f(g_size, g_size));
		bg.setFillColor(sf::Color(255, 250, 100));
		if (!font.loadFromFile("arial.ttf")) { printf("Error loading font\n");}
		sf::RectangleShape square(sf::Vector2f(b_size - 2, b_size - 2));
		square.setFillColor(sf::Color(255, 200, 0));
		square.setOrigin((b_size - 2) / 2, (b_size - 2) / 2);
		sf::Text number = sf::Text("", font, b_size / 3);
		sf::FloatRect r = number.getLocalBounds();
		number.setColor(sf::Color::Black);
		scoreString.setFont(font);
		scoreString.setCharacterSize(b_size / 2);
		r = scoreString.getLocalBounds();
		scoreString.setOrigin(r.width / 2.0, r.height / 2.0);
		scoreString.setPosition(g_size / 2.0, g_size + b_size / 2);

		for (int i = 0; i < size; ++i)
			for (int j = 0; j < size; ++j){
				array[i][j] = 0;
				squares[i][j] = square;
				sf::Vector2f blockOrigin(b_size * j + b_size / 2, b_size * i + b_size / 2);
				squares[i][j].setPosition(blockOrigin);
				numbers[i][j] = number;
				numbers[i][j].setPosition(blockOrigin);
			}
	}

	int addBlock(){
		while (true) {
			int x = rand() % size;
			int y = rand() % size;
			if (array[x][y] == 0) {
				array[x][y] = 2;
				filled++;
				return filled;
			}
		}
	}

	void flipArray(){
		std::array<std::array<unsigned, size>, size> tmp_array = array;
		for (int i = 0; i < size; ++i)
			for (int j = 0; j < size; ++j)
				array[i][j] = tmp_array[j][i];	
	}

	void moveLeft(){
		for (auto& row : array){
			wipeEmptyFromVector(row);
			shrinkVector(row);
			wipeEmptyFromVector(row);
		}
	}

	void moveRight(){
		for (auto& row : array)
			std::reverse(std::begin(row), std::end(row));
		moveLeft();
		for (auto& row : array)
			std::reverse(std::begin(row), std::end(row));
	}

	void moveUp(){
		flipArray();
		moveLeft();
		flipArray();
	}

	void moveDown(){
		flipArray();
		moveRight();
		flipArray();
	}

	void shrinkVector(std::array<unsigned, size>& v){
		for (int i = 0; i < size - 1; ++i)
			if (v[i] != 0 && v[i] == v[i+1]){
				v[i] *= 2;
				score += v[i];
				v[i+1] = 0;
				filled -= 1;
				moved = true;
			}
	}

	void wipeEmptyFromVector(std::array<unsigned, size>& v){
		for (int i = 0; i < size - 1; ++i)
			for (int j = 0; j < size - 1; ++j)
				if (v[j] == 0 && v[j+1] != 0){
					[&v](int x, int y){int temp = v[x]; v[x] = v[y]; v[y] = temp;}(j, j+1);
					moved = true;
				}
	}

	bool isShrinkPossible(){
		for (auto row : array)
			for (int j = 0; j < size - 1; ++j){
				if (row[j] == row[j+1])
					return true;
			}
		flipArray();
		for (auto row : array)
			for (int j = 0; j < size - 1; ++j){
				if (row[j] == row[j+1]) {
					flipArray();
					return true;
				}
			}
		flipArray();
		return false;
	}

	void update(){
		for (int i = 0; i < size; ++i)
			for (int j = 0; j < size; ++j) {
				numbers[i][j].setString(std::to_string(array[i][j]));
				sf::FloatRect r = numbers[i][j].getLocalBounds();
				numbers[i][j].setOrigin(r.width / 2.0, r.height / 2.0);
			}
		scoreString.setString(std::to_string(score));
		sf::FloatRect r = scoreString.getLocalBounds();
		scoreString.setOrigin(r.width / 2, r.height / 2);
	}

	void draw(){
		window.clear();
		window.draw(bg);
		for (int i = 0; i < size; ++i)
			for (int j = 0; j < size; ++j)
				if (array[i][j] != 0){
					window.draw(squares[i][j]);
					window.draw(numbers[i][j]);
				}
		window.draw(scoreString);
	}
};

int main()
{
	bool gameover = false;
	srand (time(NULL));
	sf::RenderWindow window(sf::VideoMode(g_size, g_size + b_size), "SFML 2048");
	window.setFramerateLimit(10);
	Grid grid(window, b_size);
	grid.addBlock();
	grid.addBlock();
	grid.update();
	grid.draw();

	while (window.isOpen())
	{
		sf::Event event;
		while (window.pollEvent(event))
		{
			switch (event.type)
			{
				case sf::Event::Closed:
					window.close();
					break;

				case sf::Event::KeyPressed:
					switch (event.key.code)
					{
						case sf::Keyboard::Left:
							grid.moveLeft();
							break;
						case sf::Keyboard::Right:
							grid.moveRight();
							break;
						case sf::Keyboard::Up:
							grid.moveUp();
							break;
						case sf::Keyboard::Down:
							grid.moveDown();
							break;
						default:
							break;
					}
					if (grid.moved)
						grid.addBlock();
					grid.moved = false;
					grid.update();
					grid.draw();
					break;
				default:
					break;
			}
			if (grid.filled == size*size && !grid.isShrinkPossible()) {
				gameover = true;
				sf::Text end_str("GAME\nOVER", grid.font, g_size / 4);
				sf::FloatRect r = end_str.getLocalBounds();
				end_str.setColor(sf::Color::Red);
				end_str.setPosition((g_size - r.width) / 2, (g_size - r.height) / 2);
				window.draw(end_str);
				break;
			}
		}
		window.display();
	}

	return 0;
}