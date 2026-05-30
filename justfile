conf:
	cmake -B build -D CMAKE_BUILD_TYPE=Debug -G Ninja
build:
	cmake --build build
clean:
	rm -rf build/* && rm -rf out*
test:
	./build/test/test_parser --gtest_color=yes
run:
	./build/sd 0 0
forward_dump:
	./build/sd 0 0
rev_dump:
	./build/sd 1 0 
forward_graph:
	./build/sd 0 1 > out.dot &&  dot -Tsvg out.dot > out.svg
rev_graph:
	./build/sd 1 1 > rev.dot &&  dot -Tsvg rev.dot > rev.svg

