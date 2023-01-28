target_include_directories(checkers-ai PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
set(header
Board.h
BoardTile.h
CheckersApp.h
cMain.h
Game.h
MiniMaxAI.h
Token.h
Tokens.h)

# install(FILES ${headers} DESTINATION include/)
