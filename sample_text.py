import json
from collections import defaultdict

SOURCE_FILE = "./text.txt"
RES_FILE = "./cmake-build-debug/probabilities.mrkv"
KEY_TERMS = 3

def get_word_pairs(source_file: str, key_terms: int) -> dict:
    with open(source_file, 'r') as f:
        words = f.read().split()

    res = defaultdict(list)

    for i in range(len(words) - key_terms):
        composite = " ".join(words[i : i + key_terms])
        res[composite].append(words[i + key_terms])

    return dict(res)

with open(RES_FILE, 'w') as f:
    words = get_word_pairs(SOURCE_FILE, KEY_TERMS)
    f.write(json.dumps(words))
