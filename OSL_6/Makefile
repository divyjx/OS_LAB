image_file=sample_pp3_5mb.ppm
all : part1 part2_1a part2_1b part2_2 part2_3
	
part1 : part1.cpp
	g++ $< -o $@
	./$@ $(image_file) output1.ppm

part2_1a : part2_1a.cpp
	g++ $< -o $@
	./$@ $(image_file) output2_1a.ppm

part2_1b : part2_1b.cpp
	g++ $< -o $@
	./$@ $(image_file) output2_1b.ppm

part2_2 : part2_2.cpp
	g++ $< -o $@
	./$@ $(image_file) output2_2.ppm

part2_3 : part2_3.cpp
	g++ $< -o $@
	./$@ $(image_file) output2_3.ppm

clean:
	rm -rf output* part1 part2_1a part2_1b part2_2 part2_3