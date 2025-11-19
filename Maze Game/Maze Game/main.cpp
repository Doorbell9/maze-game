#include <iostream>
#include <vector>
#include <stack>
#include <random>
#include <chrono>
#include <conio.h>
#include <windows.h>
using namespace std;    

class Door {
private:
	int x, y; //position of the door
	bool isOpen; //state of the door

public:
	//constructor that initializes the door's position and state
    Door(int posX, int posY) : x(posX), y(posY), isOpen(false) {}
	
    //getters for the coordinates and state of the door
    int getX() const { return x; }
    int getY() const { return y; }
    bool getIsOpen() const { return isOpen; }
	//setter for the state of the door
    void setIsOpen(bool open) { isOpen = open; }

    //function that generates a random math question 
    static bool askMathQuestion() {
        int num1 = rand() % 10 + 1; //picks 2 random numbers
        int num2 = rand() % 10 + 1;
        char operations[] = { '+', '-', '*' };
        char operation = operations[rand() % 3]; //picks a random operation
        int correctAnswer;
        //calculates the correct answer based on the operation
        switch (operation) {
        case '+': correctAnswer = num1 + num2; break;
        case '-': correctAnswer = num1 - num2; break;
        case '*': correctAnswer = num1 * num2; break;
        }
		
        //prompt the player to answer the question
        cout << "Solve to pass: " << num1 << " " << operation << " " << num2 << " = ?" << endl;
        int playerAnswer;
        cout << "Your answer: ";
        cin >> playerAnswer;

        //the door will only open if your answer matches the correct answer
        if (playerAnswer == correctAnswer) {
            cout << "Correct! The door opens." << endl;
            return true;
        }
        else {
            cout << "Wrong answer! The door remains closed." << endl;
            return false;
        }
    }
	//getter for the symbol of the door
    char getDoorSymbol() const {
        return 'D';
    }
};

class Enemy {
private:
    int x, y; //current position of the enemy
	const int dx[4] = { 0, 1, 0, -1 }; //possible moves along the x-axis
    const int dy[4] = { 1, 0, -1, 0 }; //possible moves along the y-axis
	mt19937 gen; //random number generator

public:
	//a constructor that initializes the enemy's starting position
    Enemy(int startX, int startY) : x(startX), y(startY) {}

	//getters for the x and y coordinates
    int getX() const { return x; }
    int getY() const { return y; }

	//method to move the enemy in the maze
    void move(const vector<std::vector<char>>& grid) {
        uniform_int_distribution<> dis(0, 3);

        //try to move the enemy up to 4 times
        for (int attempts = 0; attempts < 4; attempts++) {
            int direction = dis(gen); //pick a random direction
            int newX = x + dx[direction]; //calculate the new position
            int newY = y + dy[direction];

            //checks to see if the move is possible
            if (newX >= 0 && newX < grid.size() &&
                newY >= 0 && newY < grid[0].size() &&
                grid[newX][newY] == ' ') {
                x = newX; //update the coordinates
                y = newY;
				break; //exit the loop only if the move was successful
            }
        }
    }
	//getter for the symbol of the enemy
    char getEnemySymbol() const {
        return 'X';
    }
};

class Player {
private:
	int x, y; //current position of the player
	int currentLevel; //current level of the player
	int playerHealth = 3; //player's health
	int score = 0; //player's score
	bool hasPowerUp = false; //flag to check if the player has a power-up
    int invincibilityMoves = 0; // counter for invincibility moves

public:
	//constructor that initializes the player's position and level
    Player() : x(0), y(1), currentLevel(1) {}

	//function to reset the player's position, used when moving to the next level
    void resetPosition() {
        x = 0;
        y = 1;
    }

	//getters for the player's coordinates, level, health, and score
    int getX() const { return x; }
    int getY() const { return y; }
    int getCurrentLevel() const { return currentLevel; }
    int getPlayerHealth() const { return playerHealth; }
    int getScore() const { return score; }
    bool isInvincible() const { return invincibilityMoves > 0; } // Check if player is invincible

	//function to increase the level the player is on, called later when the player reaches the exit
    void levelUp() {
        currentLevel++;
        resetPosition();
    }

	//function to decrease the player's health, only takes damage if the player doesnt have an active powerup and is above 0 health
    void takeDamage() {
        if (!isInvincible() && playerHealth > 0) {
            playerHealth--;
        }
    }

    //function to collect items
    void collectItem(char itemType) {
        if (itemType == 'H') {
			playerHealth++; //if its a health potion, increase the player's health
        }
        else if (itemType == 'P') {
			hasPowerUp = true; //if its a power-up, set the flag to true
            invincibilityMoves = 20; //set invincibility moves to 20
        }
    }
	//function to add score
    void addScore(int additionalScore) { 
        score += additionalScore;
    }

	//the move function, used to move the player in the maze
    bool move(char direction, const vector<vector<char>>& grid, int height, int width, const vector<Enemy>& enemies, vector<Door>& doors) {
		//variables to store the new position
        int newX = x;
        int newY = y;

		//update the new position based on the direction
        switch (direction) {
        case 'w': newX--; break;
        case 's': newX++; break;
        case 'a': newY--; break;
        case 'd': newY++; break;
        }

		//check if the new position is within the maze and is not a wall
        if (newX >= 0 && newX <= height &&
            newY >= 0 && newY <= width &&
            grid[newX][newY] == ' ') {

            //check if there's an enemy at the new position
            for (const Enemy& enemy : enemies) {
                if (enemy.getX() == newX && enemy.getY() == newY) {
                    return false;  //don't move into the enemy's position
                }
            }

            //check if there's a door at the new position
            for (Door& door : doors) {
                if (door.getX() == newX && door.getY() == newY && !door.getIsOpen()) {
                    if (Door::askMathQuestion()) {
                        door.setIsOpen(true);
                    }
                    else {
                        return false;
                    }
                }
            }
            //if no enemy and no wall, move to new position
            x = newX;
            y = newY;

            //decrement invincibility moves if active
            if (invincibilityMoves > 0) {
                invincibilityMoves--;
            }

            return true;
        }
        return false;
    }
	//function to check if the player has reached the exit
    bool hasReachedExit(int exitX, int exitY) const {
        return (x == exitX && y == exitY);
    }

	//getter for the player's symbol
    char getPlayerSymbol() const {
        return 'O';
    }
};

class Items {
private:
	int x, y; //position of the item
    char type; // type of item,'H' for health potion, 'P' for power-up

public:
	//constructor that initializes the item's position and type
    Items(int startX, int startY, char itemType) : x(startX), y(startY), type(itemType) {}

	//getters for the coordinates and type of the item
    int getX() const { return x; }
    int getY() const { return y; }
    char getType() const { return type; }

	//getter for the symbol of the item
    char getSymbol() const {
        return type;
    }
};

class Maze {
private:
	int height; //height of the maze
	int width; //width of the maze
	const int dx[4] = { 0, 1, 0, -1 }; //possible moves along the x-axis
	const int dy[4] = { 1, 0, -1, 0 }; //possible moves along the y-axis
	vector<vector<char>> grid; //the maze grid                                
	vector<Enemy> enemies; //vector to store the enemies
	vector<Items> items; //vector to store the items
	vector<Door> doors; //vector to store the doors
	mt19937 rng; //random number generator

	//function to initialize the maze grid with walls
    void initializeGrid() {
        grid = vector<vector<char>>(height + 1, vector<char>(width + 1, '#'));
    }

public:
	//constructor that initializes the maze with a specific level
    Maze(int level) {
        //increase maze size with each level
        height = 8 + (level * 2);  //starting from 10x20, increasing by 2x4 each level
        width = 16 + (level * 4);
        
        unsigned seed = chrono::steady_clock::now().time_since_epoch().count();
        rng.seed(seed);

        initializeGrid();
    }

	//function to check if a position is within the maze
    bool isValid(int x, int y) {
        return x > 0 && x < height && y > 0 && y < width;
    }

	//getters for the enemies, items, and doors
    const vector<Enemy>& getEnemies() const {
        return enemies;
    }
    const vector<Items>& getItems() const {
        return items;
    }
    vector<Door>& getDoors() {
        return doors;
    }

	//function to spawn items in the maze
    void spawnItems(int level) {
        int numItems = level + 1; //number of items increases with level
        for (int i = 0; i < numItems; i++) {
            while (true) {
				//randomly choose a position for the item 
                int randX = 1 + (rng() % (height - 2));
                int randY = 1 + (rng() % (width - 2));
                char itemType = (rng() % 2 == 0) ? 'H' : 'P'; //randomly choose between health potion and power-up

                if (grid[randX][randY] == ' ' &&
                    !(randX == 1 && randY == 1) && //not at player start
                    !(randX == height - 1 && randY == width - 1)) { //not at exit
					items.push_back(Items(randX, randY, itemType)); //add the item to the vector
                    break;
                }
            }
        }
    }

	//function to remove an item from the maze
    void removeItem(int x, int y) {
        for (auto it = items.begin(); it != items.end(); ++it) {
            if (it->getX() == x && it->getY() == y) {
                items.erase(it);
                break;
            }
        }
    }

    //function to spawn doors in the maze
    void spawnDoors(int level) {
        int numDoors = level; //number of doors increases with level
        for (int i = 0; i < numDoors; i++) {
            while (true) {
				//randomly choose a position for the door
                int randX = 1 + (rng() % (height - 2));
                int randY = 1 + (rng() % (width - 2));

                if (grid[randX][randY] == ' ' &&
                    !(randX == 1 && randY == 1) && //not at player start
                    !(randX == height - 1 && randY == width - 1)) { //not at exit
					doors.push_back(Door(randX, randY)); //add the door to the vector
                    break;
                }
            }
        }
    }

	//function to generate the maze using the recursive backtracking algorithm
    void generateMaze() {
        initializeGrid();  //reset the grid before generating new maze
        recursiveBacktrack(1, 1);

        grid[0][1] = ' ';  //entrance
        grid[1][1] = ' ';
        grid[height - 1][width - 1] = ' ';
        grid[height][width - 1] = ' ';  //exit
    }

	//recursive function to carve out the paths in the maze
    void recursiveBacktrack(int x, int y) {
		grid[x][y] = ' '; //the current cell is marked as a path

        vector<int> directions = { 0, 1, 2, 3 };
		shuffle(directions.begin(), directions.end(), rng); //the possible directions are shuffled to ensure randomness

		//new positions are calculated based on the chosen direction
        for (int dir : directions) {
            int newX = x + (dx[dir] * 2);
            int newY = y + (dy[dir] * 2);

			//check if the new position is within the maze and is a wall
            if (isValid(newX, newY) && grid[newX][newY] == '#') {
                grid[x + dx[dir]][y + dy[dir]] = ' ';
                recursiveBacktrack(newX, newY);
            }
        }
    }

	//function to print the maze
    void printMaze(const Player &player) const {
        system("cls");
		//display the player's stats
        cout << "Level: " << player.getCurrentLevel() << "\n" << endl;
        cout << "Health: " << player.getPlayerHealth() << "\n" << endl;
        cout << "Score: " << player.getScore() << endl;

        for (int i = 0; i <= height; i++) {
            for (int j = 0; j <= width; j++) {
                if (i == player.getX() && j == player.getY()) {
                    cout << player.getPlayerSymbol() << ' ';
                    continue;
                }
                bool printed = false;
                //check for enemies
                for (const Enemy& enemy : enemies) {
                    if (enemy.getX() == i && enemy.getY() == j) {
                        cout << enemy.getEnemySymbol() << ' ';
                        printed = true;
                        break;
                    }
                }
                //check for items
                if (!printed) {
                    for (const Items& item : items) {
                        if (item.getX() == i && item.getY() == j) {
                            cout << item.getSymbol() << ' ';
                            printed = true;
                            break;
                        }
                    }
                }
				//check for doors
                if (!printed) {
                    for (const Door& door : doors) {
                        if (door.getX() == i && door.getY() == j) {
                            cout << door.getDoorSymbol() << ' ';
                            printed = true;
                            break;
                        }
                    }
                }
                if (!printed) {
                    cout << grid[i][j] << ' ';
                }
            }
            cout << endl;
        }
    }
	//getter for the maze grid
    const vector<vector<char>>& getGrid() const {
        return grid;
    }

	//getters for the maze dimensions and exit position
    int getHeight() const { return height; }
    int getWidth() const { return width; }
    int getExitX() const { return height; }
    int getExitY() const { return width - 1; }
 
	//function to spawn enemies in the maze
    void spawnEnemies(int level) {
        int numEnemies = level * 2; //increase number of enemies with each level

        for (int i = 0; i < numEnemies; i++) {
            while (true) {
				//randomly choose a position for the enemy
                int randX = 1 + (rng() % (height - 2));
                int randY = 1 + (rng() % (width - 2));

                if (grid[randX][randY] == ' ' &&
                    !(randX == 1 && randY == 1) && //not at player start
                    !(randX == height - 1 && randY == width - 1)) { //not at exit
					enemies.push_back(Enemy(randX, randY)); //add the enemy to the vector
                    break;
                }
            }
        }
    }

	//function to move the enemies in the maze
    void moveEnemies() {
        for (Enemy& enemy : enemies) {
            enemy.move(grid);
        }
    }

	//function to check for enemy collision with the player
    bool checkEnemyCollision(const Player& player) {
        for (const Enemy& enemy : enemies) {
            if (enemy.getX() == player.getX() && enemy.getY() == player.getY()) {
                return true;
            }
        }
        return false;
    }
};

//function to display the menu
void displayMenu() {
    cout << "Menu:\n";
    cout << "P - Play\n";
    cout << "I - Instructions\n";
    cout << "Q - Quit\n";
    cout << "Choose an option: ";
}

//function to display the instructions
void displayInstructions() {
    cout << "\nInstructions:\n";
    cout << "Use WASD keys to move through the maze.\n";
    cout << "Reach the exit to complete the level.\n";
    cout << "X are enemies, avoid them!\n";
	cout << "Items are scattered across the maze, pick them up to aid your journey.\n";
	cout << "P are power-ups, granting you temporary invincibility.\n";
	cout << "H are health potions, restoring your health by 1.\n";
	cout << "D are doors, solve the math question to open them.\n";
	cout << "You have 2 minutes per level. the quicker you complete it, the more points you earn.\n";
    cout << "points are awarded based on the level, time remaining, and health.\n";
	cout << "Good luck!\n";
    cout << "Press any key to return to the menu...\n";
	_getch(); //wait for user input
}

int main() {
    Player player;
	const int MAX_LEVEL = 5; //5 levels in total
	const int TIME_LIMIT = 120; //2 minutes per level 

    char choice;
	bool exitGame = false; //set to false initially to display the menu

	//while exitGame is false, display the menu
    while (!exitGame) {
        system("cls");
        displayMenu();
		choice = _getch(); //get user input

        switch (tolower(choice)) {
		case 'p': //pressing 'p' will start the game
            exitGame = true; //exit the menu loop to start the game
            break;
		case 'i': //pressing 'i' will display the instructions
            system("cls");
            displayInstructions();
            break;
		case 'q': //pressing 'q' will quit the game
            cout << "\nQuitting the game...\n";
            return 0;
		default: //if the input is invalid, display an error message and ask for input again
            cout << "\nInvalid choice. Please try again.\n";
            _getch();
            break;
        }
    }

	while (player.getCurrentLevel() <= MAX_LEVEL) { //while the player's current level is less than or equal to the max level, the game will continue
        Maze maze(player.getCurrentLevel());
		maze.generateMaze(); //generate the maze
		maze.spawnEnemies(player.getCurrentLevel()); //spawn enemies
		maze.spawnItems(player.getCurrentLevel()); //spawn items
		maze.spawnDoors(player.getCurrentLevel()); //spawn doors
        bool levelComplete = false;
		auto startTime = chrono::steady_clock::now(); //start the timer

        while (!levelComplete) {
            auto currentTime = chrono::steady_clock::now();
            auto elapsedTime = chrono::duration_cast<chrono::seconds>(currentTime - startTime).count();
            int remainingTime = TIME_LIMIT - elapsedTime;
			maze.printMaze(player); //print the maze with everything in it
			cout << "\nTime remaining: " << remainingTime << " seconds" << endl;  
            cout << "\nUse WASD to move (Q to quit): ";
            char move = _getch();

			//if the player presses 'q', the game will end
            if (move == 'q' || move == 'Q') {
                return 0;
            }
			//if the player's health reaches 0, the game will end
            if (player.getPlayerHealth() == 0) {
				cout << "You died! Game over!" << endl; 
                return 0;
            }

			//if the player runs out of time, the game will end
            if (remainingTime <= 0) {
                cout << "Time's up! Game over!" << endl;
                return 0;
            }


			//if the player moves, the player will move and the enemies will move
            if (move == 'w' || move == 'a' || move == 's' || move == 'd') {
                player.move(tolower(move), maze.getGrid(), maze.getHeight(), maze.getWidth(), maze.getEnemies(), maze.getDoors());
                maze.moveEnemies();
				if (maze.checkEnemyCollision(player)) { //check for enemy collision and take damage if true
                    player.takeDamage();
                }

				//check for item collection
                vector<Items> items = maze.getItems();
                for (size_t i = 0; i < items.size(); ) {
					//if the player 's position matches the item's position, collect the item
                    if (items[i].getX() == player.getX() && items[i].getY() == player.getY()) {
                        player.collectItem(items[i].getType());
						maze.removeItem(items[i].getX(), items[i].getY());
						items.erase(items.begin() + i); //remove the item from the vector once collected
                    }
                    else {
                        i++;
                    }
                }
				//check if the player has reached the exit
                if (player.hasReachedExit(maze.getExitX(), maze.getExitY())) {
					maze.printMaze(player); //print the maze again for the new level
					//alocate points based on level, time remaining, and health
					player.addScore(100 * player.getCurrentLevel());
                    player.addScore(10 * remainingTime);
                    player.addScore(30 * player.getPlayerHealth());  
					//if the player completes the last level, display a message and end the game
                    if (player.getCurrentLevel() == MAX_LEVEL) {
                        cout << "\nCongratulations! You've completed all levels!" << endl;
                        return 0;
                    }
					//if the game isnt over, display a message and move to the next level
                    else {
                        cout << "\nLevel " << player.getCurrentLevel() << " completed! Press any key for next level..." << endl;
                        _getch();
                        player.levelUp();
                        levelComplete = true;
                    }
                }
            }
        }
    }
    return 0;
}    