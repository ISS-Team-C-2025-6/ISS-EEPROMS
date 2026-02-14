#include <stdlib.h>
#include <stdio.h>
#include <string.h>

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

Result calculate_seu(char *c_page, char correct_pattern, int page_num)
{
    int bit_flip_count = 0; // bit flips counted
    int not_bit_flip_count = 0; //not bit flips counted

    char original = correct_pattern; // correct pattern
    size_t bf_size = 8; //size of flip values
    Bitflip *bitflips; //declare flip values pointer
    bitflips = (Bitflip*) calloc(bf_size, sizeof(Bitflip)); // allocate 8 wide array of characters to flip values

    char *ptr = c_page; //declare new pointer pointing to c_page necessary for arduino
    int val; //integer value of byte read before typecast to char
    int n; //number of bytes matched with sscanf
    int bytes_read = 0;

    //Explanation of sscanf: %i = integer, %n is special it means how many bytes were matched. 
    //so the c will scan for an integer until next wordbreak and then record that number and also how many characters were read.
    //pointer is address to where data is stored
    //* operator is dereference it means follow the pointer and retrieve the data
    //& operator is reference it means give the address of the data
    while (sscanf(ptr, "%i%n", &val, &n) == 1) //read a byte 
    {
        bytes_read++;
        ptr += n; //increment pointer so string "12345" turns into "2345"
        char b = (char)val; //typecast val to char

        if (b == correct_pattern)
        {
            not_bit_flip_count++;
        }
        else
        {
            bit_flip_count++;
            if (bit_flip_count > bf_size) // if our flip values array needs to be wider
            {
                bf_size *= 2; //double array size (realloc is expensive af so ideally we dont call it too much)
                bitflips = (Bitflip *) realloc(bitflips, bf_size * sizeof(Bitflip)); // domain expansion of array to 2x old size
            }
            bitflips[bit_flip_count - 1] = (Bitflip){original, b, page_num*64+bytes_read}; //set array yk yk
        }
    }
    Result result = {not_bit_flip_count, bit_flip_count, original, bitflips}; //initialize instance of struct result
    return result; //return very nice :)
}

// ONLY FOR TESTING PURPOSES NOT TO COPY INTO ARDUINO CODE 
// because raw c is not object oriented :(


void print_seu(Result result){
    Result results = result;
    printf("%i %i\n", results.not_bit_flip_count, results.bit_flip_count);
    for (int i = 0; i < results.bit_flip_count; i++) {
        printf("(%c => %c) at %i, ", results.bitflips[i].original_byte, results.bitflips[i].flipped_byte, results.bitflips[i].location);
    }
    printf("\n");
}

int main()
{
    Result test_8_not_bit_flip = calculate_seu("42 42 42 42 42 42 42 42 ", '*', 0);
    print_seu(test_8_not_bit_flip);
    free(test_8_not_bit_flip.bitflips);

    Result test_1_bit_flip = calculate_seu("42 42 43 42 42 42 42 42 ", '*', 1);
    print_seu(test_1_bit_flip);
    free(test_1_bit_flip.bitflips);

    Result test_8_bit_flip = calculate_seu("43 43 43 43 43 43 43 43 ", '*', 2);
    print_seu(test_8_bit_flip);
    free(test_8_bit_flip.bitflips);

    Result test_8_multiflavour_bit_flip = calculate_seu("43 45 46 47 48 49 50 51 ", '*', 3);
    print_seu(test_8_multiflavour_bit_flip);
    free(test_8_multiflavour_bit_flip.bitflips);

    Result test_many_multiflavour_bit_flip = calculate_seu("43 45 46 47 48 49 50 51 52 53 54 55 56 57 67 68 69 70", '*', 4);
    print_seu(test_many_multiflavour_bit_flip);
    free(test_many_multiflavour_bit_flip.bitflips);

    Result test_bit_flip_in_haystack = calculate_seu("42 42 42 42 42 42 42 42 42 42 42 42 42 42 42 42 42 42 42 42 42 42 42 42 42 42 42 42 42 42 42 42 42 42 42 42 42 42 42 42 42 42 42 42 42 42 42 42 42 42 42 42 42 42 42 42 50 42 42 42 42 42 42 42 ", '*', 5);
    print_seu(test_bit_flip_in_haystack);
    free(test_bit_flip_in_haystack.bitflips);

    // free(results.bitflips) IS VERY VERY IMPORTANT!!! 
    // if we get memory leak on iss i will cry :'(
}