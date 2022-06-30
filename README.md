# LuckyColorRecipeCalculator
A luckycolor recipe calculator for islet to get maximum protection.

# What is this?

Islet is a minecraft server that have been playing lately and it has a special armor system. In short, only leather armor could reduce the damage and the degree of the reduction depends on the manhattan distance between the color of the leather armor and a randomly generated color named "luckycolor".

To this end, inspired by [ANRAR4](https://github.com/ANRAR4)'s repo [ANRAR4/DyeLeatherArmor](https://github.com/ANRAR4/DyeLeatherArmor), I re-write the code that calculate the craftable colors in c++ and wrote a python script that helps to find the closest color to "luckycolor".

# How to use?

## I just want to calculat my luckycolor:
1. decompress the file ```./data/recipes.7z```, you will get ```recipes.json```. Put it in the current floder.
2. run ```python find_recipe.py```

## How can I get recipe.json from scratch:
1. compile  ```leatherColorCombinationsCalculator.cpp``` with  ```g++ -O3 -Wall leatherColorCombinationsCalculator.cpp -lpthread -o leatherColorCombinationsCalculator```

2. run ```./leatherColorCombinationsCalculator <numThreads>```, it will produce ```layer1-14.bin``` (the generated file is also provided in ./data/layers.7z)

3. run ```python generate_recipes.py``` to generate ```recipes.json```.
