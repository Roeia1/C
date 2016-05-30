TARGETS = DrawArrayDriver PitPlugger
C = gcc
FLAGS = -Wall
LOCATION = /cs/course/2014/slabc/public/ex2/inputOutput/
PITFILE = pit_radius_1.in
STONEFILE = stone_radius_1.in
EXPECTED = map_pit_1_stone_1.out
RESULT = my_map_pit_stone.out

DrawArrayDriver: DrawArrayDriver.o Draw2DCharArray.o
	$(C) $(FLAGS) Draw2DCharArray.c DrawArrayDriver.c -o DrawArrayDriver

PitPlugger: Draw2DCharArray.c PitPlugger.c RadiusReader.c
	$(C) $(FLAGS) Draw2DCharArray.c PitPlugger.c RadiusReader.c -o PitPlugger
	
all: $(TARGETS)

tar: 
	tar cvf ex2.tar Makefile PitPlugger.c RadiusReader.c Draw2DCharArray.c
		
PitPlugger.o: PitPlugger.c RadiusReader.h Draw2DCharArray.h
	$(C) $(FLAGS) -c PitPlugger.c

Draw2DCharArray.o: Draw2DCharArray.c Draw2DCharArray.h
	$(C) $(FLAGS) -c Draw2DCharArray.c
	
DrawArrayDriver.o: Draw2DCharArray.o DrawArrayDriver.c
	$(C) $(FLAGS) -c DrawArrayDriver.c
	
test1: PitPlugger
	rm $(RESULT) -f
	PitPlugger $(LOCATION)$(PITFILE) $(LOCATION)$(STONEFILE) > $(RESULT)
	diff $(RESULT) $(LOCATION)$(EXPECTED)
	
clean:
	rm -f *.o

.PHONY: all tar test1 clean