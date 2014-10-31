project(reversi)

add_executable(reversi coordinate.cpp reversiboard.cpp server.cpp reversicompetitionagent.cpp reversihwagent.cpp ioreversi.cpp main.cpp)

install(TARGETS reversi RUNTIME DESTINATION bin)
