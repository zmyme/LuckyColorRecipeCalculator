import json

def save_dict(path, json_dict, indent=4, mute=True):
    if not mute:
        print('dumping json string...')
    text = json.dumps(json_dict, indent=indent, ensure_ascii=False)
    if not mute:
        print('saving to {0}...'.format(path))
    with open(path, 'w+', encoding='utf-8') as f:
        f.write(text)

def load_dict(path, mute=True):
    if not mute:
        print('reading file {0}...'.format(path))
    with open(path, 'r', encoding='utf-8') as f:
        text = f.read()
    if not mute:
        print('parsing json...')
    json_dict = json.loads(text)
    return json_dict