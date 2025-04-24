

struct SnakeGame {
    SnakeGame() : score(0), dir(0), gameOver(false), width(30), height(20) {
        x = width / 2;
        y = height / 2;
        fruitX = rand() % width;
        fruitY = rand() % height;
        tail.push_back(x);
        tail.push_back(y);
    }

    void update() {
        if (gameOver) {
            return;
        }

        for (int i = tail.size() - 1; i > 0; i--) {
            tail[i] = tail[i - 1];
        }

        if (dir == 0) {
            y--;
        }
        else if (dir == 1) {
            y++;
        }
        else if (dir == 2) {
            x--;
        }
        else if (dir == 3) {
            x++;
        }

        tail[0] = x;
        tail[1] = y;

        /*for (int i = 2; i < tail.size(); i += 2) {
            if (tail[i] == x && tail[i + 1] == y) {
                gameOver = true;
            }
        }

        if (x < 0 || x >= width || y < 0 || y >= height) {
            gameOver = true;
        }*/

        if (x == fruitX && y == fruitY) {
            score++;
            fruitX = rand() % width;
            fruitY = rand() % height;
            tail.push_back(x);
            tail.push_back(y);
        }
    }

    void draw(pyIO& io) {
        py::SetNextWindowPos(ImVec2(10, 10));
        py::SetNextWindowSize(ImVec2(400, 400));
        py::Begin("Snake Game");
        py::Text("Score: %d", score);

        for (int i = 0; i < height; i++) {
            for (int j = 0; j < width; j++) {
                py::PushStyleColor(pyCol_Button, (ImVec4)ImColor::HSV((float)i / height, 1, 1));
                if (j == x && i == y) {
                    py::Button("##head", ImVec2(10, 10));
                }
                else if (j == fruitX && i == fruitY) {
                    py::Button("##fruit", ImVec2(10, 10));
                }
                else {
                    for (int k = 0; k < tail.size(); k += 2) {
                        if (j == tail[k] && i == tail[k + 1]) {
                            py::Button("##tail", ImVec2(10, 10));
                            break;
                        }
                    }
                }
                py::PopStyleColor();
            }
        }

        py::End();

        if (gameOver) {
            py::SetNextWindowPos(ImVec2(160, 200));
            py::SetNextWindowSize(ImVec2(80, 20));
            py::Begin("Game Over");
            py::Text("Game Over! Score: %d", score);
            py::End();
        }
    }

 /*   void handleEvent(SDL_Event& event) {
        if (event.type == SDL_KEYDOWN) {
            if (event.key.keysym.sym == SDLK_UP && dir != 1) {
                dir = 0;
            }
            else if (event.key.keysym.sym == SDLK_DOWN && dir != 0) {
                dir = 1;
            }
            else if (event.key.keysym.sym == SDLK_LEFT && dir != 3) {
                dir = 2;
            }
            else if (event.key.keysym.sym == SDLK_RIGHT && dir != 2) {
                dir = 3;
            }
        }
    }*/

    int score;
    int dir;
    bool gameOver;
    int width;
    int height;
    int x;
    int y;
    int fruitX;
    int fruitY;
    std::vector<int> tail;
};

