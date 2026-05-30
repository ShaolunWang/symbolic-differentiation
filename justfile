conf:
	cmake -B build -D CMAKE_BUILD_TYPE=Debug -G Ninja
build:
	cmake --build build
clean:
	rm -rf build/* && rm -rf out*
test:
	./build/test/test_parser --gtest_color=yes
run:
	./build/ad 0 0
forward_dump:
	./build/ad 0 0
rev_dump:
	./build/ad 1 0 
forward_graph:
	./build/ad 0 1 > out.dot &&  dot -Tsvg out.dot > out.svg
rev_graph:
	./build/ad 1 1 > rev.dot &&  dot -Tsvg rev.dot > rev.svg

