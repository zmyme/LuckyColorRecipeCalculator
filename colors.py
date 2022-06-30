from base64 import decode, encode
from torch import full


mc_colors = [
    ['红色染料',   '#B02E26', 'red'],
    ['绿色染料',   '#5E7C16', 'green'],
    ['紫色染料',   '#8932B8', 'purple'],
    ['青色染料',   '#169C9C', 'cyan'],
    ['淡灰色染料', '#9D9D97', 'silver'],
    ['灰色染料',   '#474F52', 'gray'],
    ['粉红色染料', '#F38BAA', 'pink'],
    ['黄绿色染料', '#80C71F', 'lime'],
    ['黄色染料',   '#FED83D', 'yellow'],
    ['淡蓝色染料', '#3AB3DA', 'lightBlue'],
    ['品红色染料', '#C74EBD', 'magenta'],
    ['橙色染料',   '#F9801D', 'orange'],
    ['黑色染料',   '#1D1D21', 'black'],
    ['棕色染料',   '#835432', 'brown'],
    ['蓝色染料',   '#3C44AA', 'blue'],
    ['白色染料',   '#F9FFFE', 'white']
]

colornames = [
    "red", "green", "purple", "cyan", 
    "silver", "gray", "pink", "lime", 
    "yellow", "lightBlue", "magenta", "orange", 
    "black", "brown", "blue", "white"
]

def encode_color(r, g, b):
    color = '#{r}{g}{b}'.format(
        r = hex(r)[2:].rjust(2, '0'),
        g = hex(g)[2:].rjust(2, '0'),
        b = hex(b)[2:].rjust(2, '0'),
    )
    return color

def decode_color(color:str):
    color = color.strip('#')
    color = int(color, base=16)
    r = (color>>16) & 0xff
    g = (color>>8) & 0xff
    b = color & 0xff
    return [r, g, b]

def parse_mc_color(colors):
    parsed_color = {}
    for color in colors:
        name, value, key = color
        assert len(value) == 7
        r, g, b = decode_color(value)
        parsed_color[key] = {
            "name": name,
            "key": key,
            "value": [r, g, b]
        }
    return parsed_color

colors = parse_mc_color(mc_colors)

def calculate_fused_color(colors):
    if len(colors) == 0:
        return None
    # calculate average color
    avg_color = []
    num_colors = len(colors)
    for channel in zip(*colors):
        channel_avg = sum(channel) // num_colors
        avg_color.append(channel_avg)
    # calculate gain factor.
    avg_max = sum([max(x) for x in colors])/num_colors
    max_avg = max(avg_color)
    # print('avgcolor:', avg_color)
    # print('avgmax:', avg_max)
    # print('maxavg:', max_avg)
    gain_factor = avg_max / max_avg
    fused_color = [int(x*gain_factor) for x in avg_color]
    return fused_color
 
def get_color_by_recipe(recipe, current_color=None):
    color_list = []
    for key, count in recipe.items():
        if count > 0:
            color_list += [colors[key]['value']]*count
    if current_color is not None:
        color_list.append(current_color)
    return color_list



if __name__ == '__main__':
    # color = get_color_by_recipe({
    #     "purple": 1,
    #     "blue": 1,
    #     "white": 6
    # })
    # print('color:', color)
    # print(calculate_fused_color(color))

    color = [123, 59, 71]
    encoded = encode_color(*color)
    decoded = decode_color(encoded)
    print('color:', color)
    print('encoded:', encoded)
    print('decoded:', decoded)