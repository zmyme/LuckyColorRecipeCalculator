from utils import load_dict
from colors import colors, colornames, decode_color
import math

def calculate_leather_color(recipes):
    cache = {}
    for layer in recipes:
        for color in recipes[layer]:
            cache[color] = {
                "color": decode_color(color),
                "step": layer
            }
    return cache

def color_distance(c1, c2):
    return sum([abs(x - y) for x, y in zip(c1, c2)])

def find_closest_color(cache, color, max_step=-1):
    min_dist = 10000
    closest_color = None
    for name, this_color in cache.items():
        if max_step > 0 and int(this_color['step']) > max_step:
            continue
        this_dist = color_distance(color, this_color['color'])
        if this_dist < min_dist:
            min_dist = this_dist
            closest_color = name
    return closest_color, min_dist

def decode_recipe(encoded):
    encoded = int(encoded, base=16)
    counts = []
    while len(counts) < 16:
        counts.append(encoded%9)
        encoded //=9
    return {name:count for name, count in zip(colornames, counts) if count > 0}

def query_recipe(recipes, name):
    # search color layer.
    recipe = None
    for layer in recipes:
        recipe = recipes[layer].get(name, None)
        if recipe is not None:
            break
    if recipe is None:
        raise ValueError('Color not craftable.')
    base_color, recipe = recipe
    recipe = decode_recipe(recipe)
    if base_color == '#000000':
        return [recipe]
    else:
        steps = query_recipe(recipes, base_color)
        steps.append(recipe)
        return steps

def explain_recipe(steps):
    infos = []
    for i, step in enumerate(steps):
        info = ','.join(['{0}*{1}'.format(colors[name]['name'], count) for name, count in step.items()])
        infos.append(info)
    print('->'.join(infos))

def calculate_multipler(diff, enchant=False):
    diff = 256 if diff > 256 else diff
    multiplier  = None
    if enchant:
        multiplier = 1 - (0.50) * (256.0 - diff)*0.5 / 256.0
    else:
        multiplier = 1 - (0.50) * (256.0 - diff) / 256.0
    multiplier = multiplier * math.log(80) / math.log(125) # single cloth.
    return multiplier


def show_protection(diff):
    print('============= 减伤系数 =============')
    multiplier = calculate_multipler(diff, enchant=False)
    multiplier_full = multiplier**4
    multiplier_elytra = multiplier**3 * math.log(432) / math.log(125)
    info = '\n'.join([
        "无附魔情况下，单件衣服减伤系数为：{0}".format(multiplier),
        "无附魔情况下，全套衣服减伤系数为：{0}".format(multiplier_full),
        "无附魔情况下，三件衣服+鞘翅减伤系数为：{0}".format(multiplier_elytra),
    ])
    print(info)
    multiplier = calculate_multipler(diff, enchant=True)
    multiplier_full = multiplier**4
    multiplier_elytra = multiplier**3 * math.log(432) / math.log(125)
    info = '\n'.join([
        "有附魔情况下，单件衣服减伤系数为：{0}".format(multiplier),
        "有附魔情况下，全套衣服减伤系数为：{0}".format(multiplier_full),
        "有附魔情况下，三件衣服+鞘翅减伤系数为：{0}".format(multiplier_elytra),
    ])
    print(info)
    print('在有保护类附魔存在时，实际减伤表格如下所示。其中横排分别表示单件、全套、有鞘翅三种情况，纵列分别表示保护系数为0/1/2/3/4/5')
    print('(保护系数的计算：上限为5，其中保护四记为1，火焰、爆炸、弹射物保护对其对应的伤害时记为2，摔落保护对摔落保护记为3，每件衣服的保护系数可以直接相加且上限为5，超过5记为5)')
    multiplier_protection = [1 - x*4/25 for x in range(6)]
    multiplier_protect_enchant = [multiplier*this_protection for this_protection in multiplier_protection]
    multiplier_full_protect_enchant = [multiplier_full*this_protection for this_protection in multiplier_protection]
    multiplier_elytra_protect_enchant = [multiplier_elytra*this_protection for this_protection in multiplier_protection]
    info = '\n'.join([
        ' '.join(['%.4f']*6)%tuple(multiplier_protect_enchant),
        ' '.join(['%.4f']*6)%tuple(multiplier_full_protect_enchant),
        ' '.join(['%.4f']*6)%tuple(multiplier_elytra_protect_enchant)
    ])
    print(info)


if __name__ == '__main__':
    print('正在加载配方数据...')
    recipes = load_dict('./recipes.json')
    print('配方数据库加载完成！正在准备数据...')
    leather_color_cache = calculate_leather_color(recipes)
    print('数据准备完毕，共加载{0}种可合成颜色。'.format(len(leather_color_cache)))

    note = '\n'.join(
        [
        '请输入你的幸运色，用空格分隔。',
        '如你的幸运色为100， 200， 100, 请输入 100 200 100。',
        '默认所有颜色可用，若你希望限制合成的步骤数量，可以在幸运色后追加一个数字表示每种颜色的最大合成步骤',
        '如 100 200 100 2 表示给出的配方中最多进行两次合成。'
    ])
    print(note)

    while True:
        luckycolor = None
        max_step = -1
        while luckycolor is None:
            try:
                luckycolor_str = input('>>> ')
                luckycolor = luckycolor_str.split()
                luckycolor = [int(x) for x in luckycolor]
                if len(luckycolor) < 3 or len(luckycolor) > 4:
                    raise ValueError
                if len(luckycolor) == 4:
                    max_step = luckycolor[3]
                    luckycolor = luckycolor[:3]
            except Exception:
                print('幸运色颜色格式错误qwq')
                luckycolor = None

        print('正在寻找最接近的可合成颜色...')
        name, dist = find_closest_color(leather_color_cache, luckycolor, max_step=max_step)
        color = decode_color(name)
        print('最接近的可合成颜色为: {0}{1}，曼哈顿距离误差为：{2}'.format(
            name, tuple(color), dist
        ))

        show_protection(dist)

        print('查找配方中...')
        crafting_steps = query_recipe(recipes, name)
        print('合成配方如下：')
        explain_recipe(crafting_steps)


