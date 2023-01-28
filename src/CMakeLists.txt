target_sources( checkers-ai PRIVATE
Board.cpp
BoardTile.cpp
CheckersApp.cpp
cMain.cpp
Game.cpp
MiniMaxAI.cpp
Token.cpp
Tokens.cpp)

message(${PROJECT_SOURCE_DIR}/resources/test.png)
target_compile_definitions(checkers-ai PRIVATE
  RED_TOKEN_FILE="${PROJECT_SOURCE_DIR}/resources/token_red_small.png"
  RED_TOKEN_KING_FILE="${PROJECT_SOURCE_DIR}/resources/token_red_king_small.png"
WHITE_TOKEN_FILE="${PROJECT_SOURCE_DIR}/resources/token_white_small.png"
WHITE_TOKEN_KING_FILE="${PROJECT_SOURCE_DIR}/resources/token_white_king_small.png")
