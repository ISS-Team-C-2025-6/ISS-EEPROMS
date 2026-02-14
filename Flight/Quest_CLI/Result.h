#pragma once
typedef struct {
    char original_byte;
    char flipped_byte;
    int location;
} Bitflip;

typedef struct
{ // better naming conventions
    int not_bit_flip_count;
    int bit_flip_count;
    char original;
    Bitflip *bitflips;
} Result;

