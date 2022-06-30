import os
import struct
from utils import save_dict
from colors import encode_color, colornames, colors


def decode_color_from_bytes(buffer):
    color = struct.unpack('L', buffer)[0]
    r = (color>>16) & 0xff
    g = (color>>8) & 0xff
    b = color & 0xff
    return r, g, b

def decode_recipe_from_bytes(buffer):
    encoded = struct.unpack('Q', buffer)[0]
    # recipe = []
    # while len(recipe) < 16:
    #     recipe.append(encoded%9)
    #     encoded //=9
    # return {name:count for name, count in zip(colornames, recipe) if count > 0}
    return hex(encoded)[2:].rjust(16, '0')

def load_recipe(filepath):
    file = open(filepath, 'rb')
    while True:
        base_color = file.read(4)
        if base_color == b'':
            break
        recipe = file.read(8)
        if recipe == b'':
            raise ValueError
        crafted_color = file.read(4)
        
        base_color = decode_color_from_bytes(base_color)
        recipe = decode_recipe_from_bytes(recipe)
        crafted_color = decode_color_from_bytes(crafted_color)
        yield base_color, recipe, crafted_color

def convert_recipes(fmt='./layer{0}.bin', save_path='./recipes.json'):
    i = 1
    print('checking', fmt.format(i))
    recipe_db = {}
    counter = 0
    while os.path.exists(fmt.format(i)):
        path = fmt.format(i)
        print('loading ', path)
        recipes = load_recipe(path)
        this_converted_recipe = {}
        this_counter = 0
        for base_color, recipe, crafted_color in recipes:
            crafted_color = encode_color(*crafted_color)
            base_color = encode_color(*base_color)
            this_converted_recipe[crafted_color] = [base_color, recipe]
            this_counter += 1
        recipe_db[str(i)] = this_converted_recipe
        print('layer {0} has {1} colors.'.format(i, this_counter))
        counter += this_counter
        i += 1
    print('{0} colors loaded in total.'.format(counter))
    print('writing result to', save_path)
    save_dict(save_path, recipe_db, indent=None)
        

if __name__ == '__main__':
    convert_recipes()