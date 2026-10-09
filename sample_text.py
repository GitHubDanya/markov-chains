import json
from collections import defaultdict

SOURCE_FILE = "./text.txt"
RES_FILE = "./probabilities.mrkv"


def get_word_pairs(source_file: str) -> dict:
    with open(source_file, 'r') as f:
        words = f.read().split()

    res = defaultdict(list)

    for i in range(len(words) - 2):
        composite = f"{words[i]} {words[i + 1]}"
        res[composite].append(words[i + 2])

    return dict(res)

with open(RES_FILE, 'w') as f:
    words = get_word_pairs(SOURCE_FILE)
    f.write(json.dumps(words))
