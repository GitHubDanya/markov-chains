import json

SOURCE_FILE = "./text.txt"
RES_FILE = "./probabilities.mrkv"

def get_word_pairs(source_file: str):
    with open(SOURCE_FILE, 'r') as f:
        res = {}

        for line in f:
            words = line.split()
            for i in range(1, len(words) - 2):
                composite = words[i] + " " + words[i + 1]
                res[composite] = []

                if composite in res:
                    res[composite].append(words[i + 2])

    return res

with open(RES_FILE, 'w') as f:
    words = get_word_pairs(SOURCE_FILE)
    f.write(json.dumps(words))
