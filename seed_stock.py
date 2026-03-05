#!/usr/bin/env python3
import struct, sys

RECORD_FORMAT = '100sif'

products = [
    ("Dragonbone Sword",       850, 6.3),
    ("Shadowcat Pelt",         120, 1.8),
    ("Valyrian Steel Dagger",  999, 2.1),
    ("Obsidian Arrow Bundle",   75, 3.4),
    ("Ravens Scroll Case",      45, 0.6),
    ("Maesters Chain Link",    200, 0.3),
    ("Ironwood Shield",        310, 9.7),
    ("Citadel Candle",          60, 0.4),
    ("Amber Necklace",         430, 0.5),
    ("Direwolf Hide",          280, 4.2),
    ("Salted Mutton Barrel",    65, 18.0),
    ("Northern Mead Cask",      90, 7.5),
    ("Targaryen Silk Robe",    510, 1.1),
    ("Dragonglass Shard",      150, 0.9),
    ("Ravens Feather Quill",    25, 0.1),
    ("Weirwood Carving",       375, 5.8),
    ("Black Iron Shackles",    180, 8.6),
    ("Spiced Honey Jar",        40, 1.3),
    ("Bronze War Axe",         420, 7.2),
    ("Maesters Tome",          230, 2.7),
]

out = sys.argv[1] if len(sys.argv) > 1 else "stock.db"
with open(out, 'wb') as f:
    for name, amount, weight in products:
        name_padded = name.encode('utf-8')[:99].ljust(100, b'\x00')
        f.write(struct.pack(RECORD_FORMAT, name_padded, amount, weight))

print(f"Written {len(products)} products to {out}")