#!/bin/env python3
import pytmx
import sys
import pathlib

# Run with `./filename.py source-file.tmx`
FILE_PATH = pathlib.Path(sys.argv[1])

WIDTH = 15

LAYER_NAME = input("Enter the layer name: ")

data = pytmx.TiledMap(FILE_PATH)

# Used for pytmx GID lookup
tile_list = [i for i in data.gidmap]

for layer in data.visible_layers:
    if isinstance(layer, pytmx.TiledTileLayer) and layer.name.lower() == LAYER_NAME.lower():
        print(f"\nProcessing layer {layer.name}...")

        l = [tile_list[tile_id - 1] if tile_id != 0 else 0 for x, y, tile_id in layer]

        s = '\n'.join([', '.join([str(item) for item in row]) for row in zip(*(iter(l),) * WIDTH)])

        c = [[int(i) for i in j.split(',') if i != ''] for j in s.split('\n')]

        result = '\n'.join([', '.join([f'{(b-1):#0{4}x}' if b > 0 else '0xff' for b in a]) + ',' for a in c])[:-1]

        print(f"\nResult:\n\n{result}")
