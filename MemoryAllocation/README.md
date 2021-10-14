CS 342 2019 Fall Project 3

Nergiz Ünal 21301027

CODE EXPLANATION
● For this project, to allocate requested size of block, we added a header at the
very beginning of the block itself. This header holds the information about the
size of the block and the address of the next block. More clearly:
struct header
{
int size; → 8 byte
struct header*, next; → 8 byte
}
● In total, header’s size is 16 byte. Since we need to hold 16 byte for each
header, we need to allocate extra 16 additional byte for every requested
block.
● There is also one general header at the beginning of whole memory chunk for
indicating the situation of it

